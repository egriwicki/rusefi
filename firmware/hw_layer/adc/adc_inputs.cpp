/**
 * @file	adc_inputs.cpp
 * @brief	Low level ADC code
 *
 * rusEfi uses two ADC devices on the same 16 pins at the moment. Two ADC devices are used in orde to distinguish between
 * fast and slow devices. The idea is that but only having few channels in 'fast' mode we can sample those faster?
 *
 * At the moment rusEfi does not allow to have more than 16 ADC channels combined. At the moment there is no flexibility to use
 * any ADC pins, only the hardcoded choice of 16 pins.
 *
 * Slow ADC group is used for IAT, CLT, AFR, VBATT etc - this one is currently sampled at 500Hz
 *
 * Fast ADC group is used for MAP, MAF HIP - this one is currently sampled at 10KHz
 *  We need frequent MAP for map_averaging.cpp
 *
 * 10KHz equals one measurement every 3.6 degrees at 6000 RPM
 *
 * @date Jan 14, 2013
 * @author Andrey Belomutskiy, (c) 2012-2020
 */

#include "pch.h"

float __attribute__((weak)) getAnalogInputDividerCoefficient(adc_channel_e) {
    return engineConfiguration->analogInputDividerCoefficient;
}

#if HAL_USE_ADC

#include "adc_subscription.h"
#include "AdcConfiguration.h"
#include "mpu_util.h"
#include "periodic_thread_controller.h"
#include "protected_gpio.h"

/* Depth of the conversion buffer, channels are sampled X times each.*/
#ifndef ADC_BUF_DEPTH_FAST
#define ADC_BUF_DEPTH_FAST      4
#endif

static NO_CACHE adcsample_t slowAdcSamples[SLOW_ADC_CHANNEL_COUNT];

static adc_channel_mode_e adcHwChannelMode[HW_MAX_ADC_INDEX];

// Board voltage, with divider coefficient accounted for
float getVoltageDivided(const char *msg, adc_channel_e hwChannel) {
	return getVoltage(msg, hwChannel) * getAnalogInputDividerCoefficient(hwChannel);
}

// voltage in MCU universe, from zero to VDD
float getVoltage(const char *msg, adc_channel_e hwChannel) {
	return adcToVolts(getAdcValue(msg, hwChannel));
}

#if EFI_USE_FAST_ADC
AdcDevice::AdcDevice(ADCConversionGroup* p_hwConfig, adcsample_t *p_buf) {
	this->hwConfig = p_hwConfig;
	this->samples = p_buf;

	hwConfig->sqr1 = 0;
	hwConfig->sqr2 = 0;
	hwConfig->sqr3 = 0;
#if ADC_MAX_CHANNELS_COUNT > 16
	hwConfig->sqr4 = 0;
	hwConfig->sqr5 = 0;
#endif /* ADC_MAX_CHANNELS_COUNT */
	memset(internalAdcIndexByHardwareIndex, 0xFF, sizeof(internalAdcIndexByHardwareIndex));
}

#endif // EFI_USE_FAST_ADC

// is there a reason to have this configurable at runtime?
#ifndef ADC_FAST_DEVICE
#define ADC_FAST_DEVICE ADCD2
#endif /* ADC_FAST_DEVICE */

static uint32_t slowAdcCounter = 0;

// todo: move this flag to Engine god object
static int adcDebugReporting = false;

#if EFI_USE_FAST_ADC

// See https://github.com/rusefi/rusefi/issues/976 for discussion on this value
#ifndef ADC_SAMPLING_FAST
#define ADC_SAMPLING_FAST ADC_SAMPLE_28
#endif

static void fastAdcDoneCB(ADCDriver *adcp) {
	// State may not be complete if we get a callback for "half done"
	if (adcp->state == ADC_COMPLETE) {
		onFastAdcComplete(adcp->samples);
	}
}

static volatile adcerror_t fastAdcLastError;

static void fastAdcErrorCB(ADCDriver *, adcerror_t err)
{
	fastAdcLastError = err;
}

static ADCConversionGroup adcgrpcfgFast = {
	.circular			= FALSE,
	.num_channels		= 0,
	.end_cb				= fastAdcDoneCB,
	.error_cb			= fastAdcErrorCB,
	/* HW dependent part.*/
	.cr1				= 0,
	.cr2				= ADC_CR2_SWSTART,
		/**
		 * here we configure all possible channels for fast mode. Some channels would not actually
         * be used hopefully that's fine to configure all possible channels.
		 *
		 */
	// sample times for channels 10...18
	.smpr1 =
		ADC_SMPR1_SMP_AN10(ADC_SAMPLING_FAST) |
		ADC_SMPR1_SMP_AN11(ADC_SAMPLING_FAST) |
		ADC_SMPR1_SMP_AN12(ADC_SAMPLING_FAST) |
		ADC_SMPR1_SMP_AN13(ADC_SAMPLING_FAST) |
		ADC_SMPR1_SMP_AN14(ADC_SAMPLING_FAST) |
		ADC_SMPR1_SMP_AN15(ADC_SAMPLING_FAST),
	// In this field must be specified the sample times for channels 0...9
	.smpr2 =
		ADC_SMPR2_SMP_AN0(ADC_SAMPLING_FAST) |
		ADC_SMPR2_SMP_AN1(ADC_SAMPLING_FAST) |
		ADC_SMPR2_SMP_AN2(ADC_SAMPLING_FAST) |
		ADC_SMPR2_SMP_AN3(ADC_SAMPLING_FAST) |
		ADC_SMPR2_SMP_AN4(ADC_SAMPLING_FAST) |
		ADC_SMPR2_SMP_AN5(ADC_SAMPLING_FAST) |
		ADC_SMPR2_SMP_AN6(ADC_SAMPLING_FAST) |
		ADC_SMPR2_SMP_AN7(ADC_SAMPLING_FAST) |
		ADC_SMPR2_SMP_AN8(ADC_SAMPLING_FAST) |
		ADC_SMPR2_SMP_AN9(ADC_SAMPLING_FAST),
	.htr				= 0,
	.ltr				= 0,
	.sqr1				= 0, // Conversion group sequence 13...16 + sequence length
	.sqr2				= 0, // Conversion group sequence 7...12
	.sqr3				= 0, // Conversion group sequence 1...6
#if ADC_MAX_CHANNELS_COUNT > 16
	.sqr4				= 0, // Conversion group sequence 19...24
	.sqr5				= 0  // Conversion group sequence 25...30
#endif /* ADC_MAX_CHANNELS_COUNT */
};

static NO_CACHE adcsample_t fastAdcSampleBuf[ADC_BUF_DEPTH_FAST * ADC_MAX_CHANNELS_COUNT];
AdcDevice fastAdc(&adcgrpcfgFast, fastAdcSampleBuf);

static void fastAdcTrigger(GPTDriver*) {
#if EFI_INTERNAL_ADC
	/*
	 * Starts an asynchronous ADC conversion operation, the conversion
	 * will be executed in parallel to the current PWM cycle and will
	 * terminate before the next PWM cycle.
	 */
	chSysLockFromISR();
	if ((ADC_FAST_DEVICE.state != ADC_READY) &&
		(ADC_FAST_DEVICE.state != ADC_COMPLETE) &&
		(ADC_FAST_DEVICE.state != ADC_ERROR)) {
		engine->outputChannels.fastAdcErrorsCount++;
		// todo: when? why? criticalError("ADC fast not ready?");
		// see notes at https://github.com/rusefi/rusefi/issues/6399
	} else {
		adcStartConversionI(&ADC_FAST_DEVICE, &adcgrpcfgFast, fastAdc.samples, ADC_BUF_DEPTH_FAST);
		fastAdc.conversionCount++;
	}
	chSysUnlockFromISR();
#endif /* EFI_INTERNAL_ADC */
}
#endif // EFI_USE_FAST_ADC

static float mcuTemperature;

float getMCUInternalTemperature() {
	return mcuTemperature;
}

int getInternalAdcValue(const char *msg, adc_channel_e hwChannel) {
	if (!isAdcChannelValid(hwChannel)) {
		warning(ObdCode::CUSTOM_OBD_ANALOG_INPUT_NOT_CONFIGURED, "ADC: %s input is not configured", msg);
		return -1;
	}

#if EFI_USE_FAST_ADC
	if (adcHwChannelMode[hwChannel] == ADC_FAST) {
		/* todo if ADC_BUF_DEPTH_FAST EQ 1
		 * return fastAdc.samples[internalIndex]; */
		return fastAdc.getAvgAdcValue(hwChannel, ADC_BUF_DEPTH_FAST);
	}
#endif // EFI_USE_FAST_ADC

	return slowAdcSamples[hwChannel - EFI_ADC_0];
}

#if EFI_USE_FAST_ADC
static GPTConfig fast_adc_config = {
	.frequency = GPT_FREQ_FAST,
	.callback = fastAdcTrigger,
	.cr2 = 0,
	.dier = 0,
};
#endif /* EFI_USE_FAST_ADC */

adc_channel_mode_e getAdcMode(adc_channel_e hwChannel) {
	return adcHwChannelMode[hwChannel];
}

#if EFI_USE_FAST_ADC

int AdcDevice::size() const {
	return channelCount;
}

void AdcDevice::init(void) {
	hwConfig->num_channels = size();
	/* driver does this internally */
	//hwConfig->sqr1 += ADC_SQR1_NUM_CH(size());
}

void AdcDevice::enableChannel(adc_channel_e hwChannel) {
	if ((channelCount + 1) >= ADC_MAX_CHANNELS_COUNT) {
		criticalError("Too many ADC channels configured");
		return;
	}

	int logicChannel = channelCount++;

	/* TODO: following is correct for STM32 ADC1/2.
	 * ADC3 has another input to gpio mapping
	 * and should be handled separately */
	size_t channelAdcIndex = hwChannel - EFI_ADC_0;

	internalAdcIndexByHardwareIndex[hwChannel] = logicChannel;
	if (logicChannel < 6) {
		hwConfig->sqr3 |= channelAdcIndex << (5 * logicChannel);
	} else if (logicChannel < 12) {
		hwConfig->sqr2 |= channelAdcIndex << (5 * (logicChannel - 6));
	} else if (logicChannel < 18) {
		hwConfig->sqr1 |= channelAdcIndex << (5 * (logicChannel - 12));
	}
#if ADC_MAX_CHANNELS_COUNT > 16
	else if (logicChannel < 24) {
		hwConfig->sqr4 |= channelAdcIndex << (5 * (logicChannel - 18));
	}
	else if (logicChannel < 30) {
		hwConfig->sqr5 |= channelAdcIndex << (5 * (logicChannel - 24));
	}
#endif /* ADC_MAX_CHANNELS_COUNT */
}

adcsample_t AdcDevice::getAvgAdcValue(adc_channel_e hwChannel, size_t bufDepth) {
	uint32_t result = 0;
	int numChannels = size();
	int index = fastAdc.internalAdcIndexByHardwareIndex[hwChannel];

	for (size_t i = 0; i < bufDepth; i++) {
		adcsample_t sample = samples[index];
//		if (sample > 0x1FFF) {
//			// 12bit ADC expected right now, make this configurable one day
//			criticalError("fast ADC unexpected sample %d", sample);
//		} else
		if (sample > ADC_MAX_VALUE) {
			if (!engineConfiguration->skipADC12bitAssert) {
				criticalError("fast ADC unexpected sample %d. Please report and use skipADC12bitAssert to disable", sample);
			}
			engine->outputChannels.unexpectedAdcSample = sample;
			// sad hack which works around https://github.com/rusefi/rusefi/issues/6376 which we do not understand
			sample = sample & ADC_MAX_VALUE;
			engine->outputChannels.adc13bitCounter++;
		}
		result += sample;
		index += numChannels;
	}

	// this truncation is guaranteed to not be lossy - the average can't be larger than adcsample_t
	return static_cast<adcsample_t>(result / bufDepth);
}

adc_channel_e AdcDevice::getAdcChannelByInternalIndex(int hwChannel) const {
	for (size_t idx = EFI_ADC_0; idx < EFI_ADC_TOTAL_CHANNELS; idx++) {
		if (internalAdcIndexByHardwareIndex[idx] == hwChannel) {
			return (adc_channel_e)idx;
		}
	}
	return EFI_ADC_NONE;
}

FastAdcToken AdcDevice::getAdcChannelToken(adc_channel_e hwChannel) {
	return fastAdc.internalAdcIndexByHardwareIndex[hwChannel];
}

#endif // EFI_USE_FAST_ADC

static void printAdcValue(int channel) {
	/* Do this check before conversion to adc_channel_e that is uint8_t based */
	if ((channel < EFI_ADC_NONE) || (channel >= EFI_ADC_TOTAL_CHANNELS)) {
		efiPrintf("Invalid ADC channel %d", channel);
		return;
	}
	int value = getAdcValue("print", (adc_channel_e)channel);
	float volts = adcToVoltsDivided(value, (adc_channel_e)channel);
	efiPrintf("adc %d voltage : %.3f", channel, volts);
}

static uint32_t slowAdcConversionCount = 0;
static uint32_t slowAdcErrorsCount = 0;

static void printAdcChannedReport(const char *prefix, int internalIndex, adc_channel_e hwChannel)
{
	if (isAdcChannelValid(hwChannel)) {
		ioportid_t port = getAdcChannelPort("print", hwChannel);
		int pin = getAdcChannelPin(hwChannel);
		int adcValue = getAdcValue("print", hwChannel);
		float volts = getVoltage("print", hwChannel);
		float voltsDivided = getVoltageDivided("print", hwChannel);
		/* Human index starts from 1 */
		efiPrintf(" %s ch[%2d] @ %s%d ADC%d 12bit=%4d %.3fV (input %.3fV)",
			prefix, internalIndex, portname(port), pin,
			/* TODO: */ hwChannel - EFI_ADC_0 + 1,
			adcValue, volts, voltsDivided);
	}
}

void printFullAdcReport(void) {
#if EFI_USE_FAST_ADC
	efiPrintf("fast %d samples", fastAdc.conversionCount);

	for (int internalIndex = 0; internalIndex < fastAdc.size(); internalIndex++) {
		adc_channel_e hwChannel = fastAdc.getAdcChannelByInternalIndex(internalIndex);

		printAdcChannedReport("F", internalIndex, hwChannel);
	}
#endif // EFI_USE_FAST_ADC
	efiPrintf("slow %d samples", slowAdcConversionCount);

	/* we assume that all slow ADC channels are enabled */
	for (int internalIndex = 0; internalIndex < ADC_MAX_CHANNELS_COUNT; internalIndex++) {
		adc_channel_e hwChannel = static_cast<adc_channel_e>(internalIndex + EFI_ADC_0);

		printAdcChannedReport("S", internalIndex, hwChannel);
	}
}

static void setAdcDebugReporting(int value) {
	adcDebugReporting = value;
	efiPrintf("adcDebug=%d", adcDebugReporting);
}

void waitForSlowAdc(uint32_t lastAdcCounter) {
	// we use slowAdcCounter instead of slowAdc.conversionCount because we need ADC_COMPLETE state
	// todo: use sync.objects?
	while (slowAdcCounter <= lastAdcCounter) {
		chThdSleepMilliseconds(1);
	}
}

class SlowAdcController : public PeriodicController<UTILITY_THREAD_STACK_SIZE> {
public:
	SlowAdcController()
		: PeriodicController("ADC", PRIO_ADC, SLOW_ADC_RATE)
	{
	}

	void PeriodicTask(efitick_t nowNt) override {
		{
			ScopePerf perf(PE::AdcConversionSlow);

			slowAdcConversionCount++;
			if (!readSlowAnalogInputs(slowAdcSamples)) {
				slowAdcErrorsCount++;
				return;
			}

			// Ask the port to sample the MCU temperature
			mcuTemperature = getMcuTemperature();
		}

		{
			ScopePerf perf(PE::AdcProcessSlow);

			slowAdcCounter++;

			AdcSubscription::UpdateSubscribers(nowNt);

			protectedGpio_check(nowNt);
		}
	}
};

void addChannel(const char*, adc_channel_e hwChannel, adc_channel_mode_e mode) {
	if (!isAdcChannelValid(hwChannel)) {
		return;
	}

#if EFI_USE_FAST_ADC
	if (mode == ADC_FAST) {
		fastAdc.enableChannel(hwChannel);
	}
#endif

	adcHwChannelMode[hwChannel] = mode;
	// Nothing to do for slow channels, input is mapped to analog in init_sensors.cpp
}

void removeChannel(const char*, adc_channel_e hwChannel) {
	if (!isAdcChannelValid(hwChannel)) {
		return;
	}
#if EFI_USE_FAST_ADC
	if (adcHwChannelMode[hwChannel] == ADC_FAST) {
		/* TODO: */
		//fastAdc.disableChannel(hwChannel);
	}
#endif

	adcHwChannelMode[hwChannel] = ADC_OFF;
}

// Weak link a stub so that every board doesn't have to implement this function
__attribute__((weak)) void setAdcChannelOverrides() { }

static void configureInputs() {
	memset(adcHwChannelMode, 0, sizeof(adcHwChannelMode));

	/**
	 * order of analog channels here is totally random and has no meaning
	 * we also have some weird implementation with internal indices - that all has no meaning, it's just a random implementation
	 * which does not mean anything.
	 */

	addChannel("MAP", engineConfiguration->map.sensor.hwChannel, ADC_FAST);

	addChannel("HIP9011", engineConfiguration->hipOutputChannel, ADC_FAST);

	// not currently used	addChannel("Vref", engineConfiguration->vRefAdcChannel, ADC_SLOW);

	addChannel("AUXF#1", engineConfiguration->auxFastSensor1_adcChannel, ADC_FAST);

	setAdcChannelOverrides();
}

static SlowAdcController slowAdcController;

void initAdcInputs() {
	efiPrintf("initAdcInputs()");

	configureInputs();

	// migrate to 'enable adcdebug'
	addConsoleActionI("adcdebug", &setAdcDebugReporting);

#if EFI_INTERNAL_ADC
	portInitAdc();

	// Start the slow ADC thread
	slowAdcController.start();

#if EFI_USE_FAST_ADC
	fastAdc.init();

	gptStart(EFI_INTERNAL_FAST_ADC_GPT, &fast_adc_config);
	gptStartContinuous(EFI_INTERNAL_FAST_ADC_GPT, GPT_PERIOD_FAST);
#endif // EFI_USE_FAST_ADC

	addConsoleActionI("adc", (VoidInt) printAdcValue);
#else
	efiPrintf("ADC disabled");
#endif
}

void printFullAdcReportIfNeeded(void) {
	if (!adcDebugReporting)
		return;
	printFullAdcReport();
}

#else /* not HAL_USE_ADC */

__attribute__((weak)) float getVoltageDivided(const char*, adc_channel_e) {
	return 0;
}

// voltage in MCU universe, from zero to VDD
__attribute__((weak)) float getVoltage(const char*, adc_channel_e) {
	return 0;
}

#endif
