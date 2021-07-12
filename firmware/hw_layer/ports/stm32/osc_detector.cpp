
#include "hal.h"
#include "efilib.h"

#ifdef ENABLE_AUTO_DETECT_HSE

static void useHsi() {
	// clear SW to use HSI
	RCC->CFGR &= ~RCC_CFGR_SW;
}

static void useHse() {
	// Switch to HSE clock
	RCC->CFGR |= RCC_CFGR_SW_HSE;
}

static uint32_t getOneCapture() {
	// wait for input capture
	while ((TIM5->SR & TIM_SR_CC4IF) == 0);

	// Return captured count
	return TIM5->CCR4;
}

static uint32_t getAverageLsiCounts() {
	// Burn one count
	getOneCapture();

	uint32_t lastCapture = getOneCapture();
	uint32_t sum = 0;

	for (size_t i = 0; i < 20; i++)
	{
		auto capture = getOneCapture();
		sum += (capture - lastCapture);
		lastCapture = capture;
	}

	return sum;
}

// This only works if you're using the PLL as the configured clock source!
static_assert(STM32_SW == RCC_CFGR_SW_PLL);

// These clocks must all be enabled for this to work
static_assert(STM32_HSI_ENABLED);
static_assert(STM32_LSI_ENABLED);
static_assert(STM32_HSE_ENABLED);

static void reprogramPll(uint8_t pllM) {
	// clear SW to use HSI
	RCC->CFGR &= ~RCC_CFGR_SW;

	// Stop the PLL
	RCC->CR &= ~RCC_CR_PLLON;

	// Mask out the old PLLM val
	RCC->PLLCFGR &= ~RCC_PLLCFGR_PLLM_Msk;

	// Stick in the new PLLM value
	RCC->PLLCFGR |= (pllM << RCC_PLLCFGR_PLLM_Pos) & RCC_PLLCFGR_PLLM_Msk;

	// Reenable PLL, wait for lock
	RCC->CR |= RCC_CR_PLLON;
	while (!(RCC->CR & RCC_CR_PLLRDY));

	// Switch clock source back to PLL
	RCC->CFGR |= RCC_CFGR_SW_PLL;
	while ((RCC->CFGR & RCC_CFGR_SWS) != (STM32_SW << 2));
}

extern "C" void __late_init() {
	// Turn on timer 5
	RCC->APB1ENR |= RCC_APB1ENR_TIM5EN;

	// Remap to connect LSI to input capture channel 4
	TIM5->OR = TIM_OR_TI4_RMP_0;

	// Enable capture on channel 4
	TIM5->CCMR2 = TIM_CCMR2_CC4S_0;
	TIM5->CCER = TIM_CCER_CC4E;

	// Start TIM5
	TIM5->CR1 |= TIM_CR1_CEN;

	// Use HSI
	useHsi();

	// Measure LSI against HSI
	auto hsiCounts = getAverageLsiCounts();

	useHse();

	// Measure LSI against HSE
	auto hseCounts = getAverageLsiCounts();

	// The external clocks's frequency is the ratio of the measured LSI speed, times HSI's speed (16MHz)
	float internalClockMhz = 16.0f * hseCounts / hsiCounts;

	uint8_t pllMValue = efiRound(internalClockMhz, 1);

	reprogramPll(pllMValue);
}

#endif // defined ENABLE_AUTO_DETECT_HSE
