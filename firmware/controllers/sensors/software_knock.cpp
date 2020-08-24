
#include "global.h"
#include "hal.h"
#include "engine.h"
#include "biquad.h"
#include "perf_trace.h"

#include "software_knock.h"

EXTERN_ENGINE;


adcsample_t sampleBuffer[8000];
Biquad knockFilter;

static volatile bool knockIsSampling = false;
static volatile bool knockNeedsProcess = false;
static volatile size_t sampleCount = 0;

static void completionCallback(ADCDriver*, adcsample_t*, size_t) {
	knockNeedsProcess = true;
}

static void errorCallback(ADCDriver*, adcerror_t err) {
}

static ADCConversionGroup adcConvGroup = { FALSE, 1, &completionCallback, &errorCallback,
	0,
	ADC_CR2_SWSTART,
	ADC_SMPR1_SMP_AN14(ADC_SAMPLE_84), // sample times for channels 10...18
	0,

	0,	// htr
	0,	// ltr

	0,	// sqr1
	0,	// sqr2
	ADC_SQR3_SQ1_N(ADC_CHANNEL_IN14)	// knock 1 - pin PF4
};

void startKnockSampling(uint8_t cylinderIndex) {
	if (cylinderIndex == 2) {
		return;
	}

	// Cancel if ADC isn't ready
	if (!((ADCD3.state == ADC_READY) ||
			(ADCD3.state == ADC_COMPLETE) ||
			(ADCD3.state == ADC_ERROR))) {
		return;
	}

	// If there's pending processing, skip
	if (knockNeedsProcess) {
		return;
	}

	// Sample for 60 degrees
	float samplingSeconds = ENGINE(rpmCalculator).oneDegreeUs * 45 * 1e-6;
	constexpr int sampleRate = 217000;
	sampleCount = 0xFFFFFFFE & static_cast<size_t>(clampF(100, samplingSeconds * sampleRate, efi::size(sampleBuffer)));

	adcStartConversionI(&ADCD3, &adcConvGroup, sampleBuffer, sampleCount);
}

void initSoftwareKnock() {
	knockFilter.configureBandpass(217000, CONFIG(knockBandCustom), 3);
	adcStart(&ADCD3, nullptr);
}

void processLastKnockEvent() {
	if (!knockNeedsProcess) {
		return;
	}

	ScopePerf perf(PE::SoftwareKnockProcess);

	float sum = 0;
	float sumSq = 0;

	constexpr float ratio = 3.3f / 4095.0f;

	knockFilter.reset();

	// Compute the sum and sum of squares
	for (size_t i = 0; i < sampleCount; i++)
	{
		float volts = ratio * (sampleBuffer[i] - 2048);

		float filtered = knockFilter.filter(volts);

		sumSq += filtered * filtered;
	}

	// mean of squares (not yet root)
	float meanSquares = sumSq / sampleCount;

	// RMS
	float db = 10 * log10(meanSquares);

	tsOutputChannels.knockLevel = db;

	knockNeedsProcess = false;
}
