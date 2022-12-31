#include "pch.h"

#include "adc_subscription.h"
#include "functional_sensor.h"
#include "redundant_sensor.h"
#include "proxy_sensor.h"
#include "linear_func.h"
#include "tps.h"
#include "auto_generated_sensor.h"

#ifndef MAX_TPS_PPS_DISCREPANCY
#define MAX_TPS_PPS_DISCREPANCY 5.0f
#endif

#define TPS_ADC_UNITS_PER_VOLT 200

struct TpsConfig {
	adc_channel_e channel;
	float closed;
	float open;
	float min;
	float max;
};

class FuncSensPair {
public:
	FuncSensPair(float divideInput, SensorType type)
		: m_func(divideInput)
		, m_sens(type, MS2NT(10))
	{
		m_sens.setFunction(m_func);
	}

	bool init(const TpsConfig& cfg) {
		// If the configuration was invalid, don't continue to configure the sensor
		if (!configure(cfg)) {
			return false;
		}

		AdcSubscription::SubscribeSensor(m_sens, cfg.channel, TPS_ADC_UNITS_PER_VOLT);

		return m_sens.Register();
	}

	void deinit() {
		AdcSubscription::UnsubscribeSensor(m_sens);
	}

	SensorType type() const {
		return m_sens.type();
	}

	const char* name() const {
		return m_sens.getSensorName();
	}

private:
	bool configure(const TpsConfig& cfg) {
		// Only configure if we have a channel
		if (!isAdcChannelValid(cfg.channel)) {
#if EFI_UNIT_TEST
	printf("Configured NO hardware %s\n", name());
#endif
			return false;
		}

		float scaledClosed = cfg.closed / m_func.getDivideInput();
		float scaledOpen = cfg.open / m_func.getDivideInput();

		float split = absF(scaledOpen - scaledClosed);

		// If the voltage for closed vs. open is very near, something is wrong with your calibration
		if (split < 0.5f) {
			firmwareError(OBD_TPS_Configuration, "\"%s\" problem: open %.2f/closed %.2f cal values are too close together. Check your calibration and wiring!", name(),
					cfg.open,
					cfg.closed);
			return false;
		}

		m_func.configure(
			cfg.closed, 0,
			cfg.open, 100,
			cfg.min, cfg.max
		);

#if EFI_UNIT_TEST
	printf("Configured YES %s\n", name());
#endif
		return true;
	}

	LinearFunc m_func;
	FunctionalSensor m_sens;
};

struct RedundantPair {
public:
	RedundantPair(FuncSensPair& pri, FuncSensPair& sec, SensorType outputType)
		: m_pri(pri)
		, m_sec(sec)
		, m_redund(outputType, m_pri.type(), m_sec.type())
	{
	}

	void init(const TpsConfig& primary, const TpsConfig& secondary, bool averageSensors, float secondaryMaximum) {
		bool hasFirst = m_pri.init(primary);
		if (!hasFirst) {
			// no input if we have no first channel
			return;
		}

		// Check that the primary and secondary aren't too close together - if so, the user may have done
		// an unsafe thing where they wired a single sensor to both inputs. Don't do that!
		bool hasBothSensors = isAdcChannelValid(primary.channel) && isAdcChannelValid(secondary.channel);
		bool tooCloseClosed = absF(primary.closed - secondary.closed) < 0.2f;
		bool tooCloseOpen = absF(primary.open - secondary.open) < 0.2f;

		if (hasBothSensors && tooCloseClosed && tooCloseOpen) {
			firmwareError(OBD_TPS_Configuration, "Configuration for redundant pair %s/%s are too similar - did you wire one sensor to both inputs...?", m_pri.name(), m_sec.name());
			return;
		}

		bool hasSecond = m_sec.init(secondary);
		if (secondaryMaximum < 20) {
			// don't allow <20% partial redundancy
			warning(CUSTOM_INVALID_TPS_SETTING, "Configuration for partial redundant switch-over too low: %.1f %s", secondaryMaximum, m_redund.getSensorName());
			secondaryMaximum = 20;
		}

		m_redund.configure(MAX_TPS_PPS_DISCREPANCY, !hasSecond, averageSensors, secondaryMaximum);
#if EFI_UNIT_TEST
        printf("init m_redund.Register() %s\n", getSensorType(m_redund.type()));
#endif // EFI_UNIT_TEST
		m_redund.Register();
	}

	void deinit() {
		m_pri.deinit();
		m_sec.deinit();

		m_redund.unregister();
	}

private:
	FuncSensPair& m_pri;
	FuncSensPair& m_sec;

	RedundantSensor m_redund;
};

// Throttle sensors and redundancy
static FuncSensPair throttle1Primary(TPS_TS_CONVERSION, SensorType::Tps1Primary);
static FuncSensPair throttle1Secondary(TPS_TS_CONVERSION, SensorType::Tps1Secondary);
static RedundantPair throttle1(throttle1Primary, throttle1Secondary, SensorType::Tps1);
static FuncSensPair throttle2Primary(TPS_TS_CONVERSION, SensorType::Tps2Primary);
static FuncSensPair throttle2Secondary(TPS_TS_CONVERSION, SensorType::Tps2Secondary);
static RedundantPair throttle2(throttle2Primary, throttle2Secondary, SensorType::Tps2);

// Pedal sensors and redundancy
static FuncSensPair pedalPrimary(1, SensorType::AcceleratorPedalPrimary);
static FuncSensPair pedalSecondary(1, SensorType::AcceleratorPedalSecondary);
static RedundantPair pedal(pedalPrimary, pedalSecondary, SensorType::AcceleratorPedal);

// This sensor indicates the driver's throttle intent - Pedal if we have one, TPS if not.
static ProxySensor driverIntent(SensorType::DriverThrottleIntent);

// These sensors are TPS-like, so handle them in here too
static FuncSensPair wastegate(PACK_MULT_VOLTAGE, SensorType::WastegatePosition);
static FuncSensPair idlePos(PACK_MULT_VOLTAGE, SensorType::IdlePosition);

void initTps() {
	efiAssertVoid(OBD_PCM_Processor_Fault, engineConfiguration != nullptr, "null engineConfiguration");
	percent_t min = engineConfiguration->tpsErrorDetectionTooLow;
	percent_t max = engineConfiguration->tpsErrorDetectionTooHigh;

	if (!engineConfiguration->consumeObdSensors) {
		// Throttle sensors
		bool averageThrottleSensors = !engineConfiguration->tpsSecondaryMonitors;
		float throttleSecondaryMaximum = engineConfiguration->tpsSecondaryMaximum;
		throttle1.init(
			{ engineConfiguration->tps1_1AdcChannel, (float)engineConfiguration->tpsMin, (float)engineConfiguration->tpsMax, min, max },
			{ engineConfiguration->tps1_2AdcChannel, (float)engineConfiguration->tps1SecondaryMin, (float)engineConfiguration->tps1SecondaryMax, min, max },
			averageThrottleSensors, throttleSecondaryMaximum
		);
		throttle2.init(
			{ engineConfiguration->tps2_1AdcChannel, (float)engineConfiguration->tps2Min, (float)engineConfiguration->tps2Max, min, max },
			{ engineConfiguration->tps2_2AdcChannel, (float)engineConfiguration->tps2SecondaryMin, (float)engineConfiguration->tps2SecondaryMax, min, max },
			averageThrottleSensors, throttleSecondaryMaximum
		);

		// Pedal sensors
		bool averagePedalSensors = !engineConfiguration->ppsSecondaryMonitors;
		float pedalSecondaryMaximum = engineConfiguration->ppsSecondaryMaximum;
		pedal.init(
			{ engineConfiguration->throttlePedalPositionAdcChannel, engineConfiguration->throttlePedalUpVoltage, engineConfiguration->throttlePedalWOTVoltage, min, max },
			{ engineConfiguration->throttlePedalPositionSecondAdcChannel, engineConfiguration->throttlePedalSecondaryUpVoltage, engineConfiguration->throttlePedalSecondaryWOTVoltage, min, max },
			averagePedalSensors, pedalSecondaryMaximum
		);

		// Other TPS-like sensors
		wastegate.init({ engineConfiguration->wastegatePositionSensor, (float)engineConfiguration->wastegatePositionMin, (float)engineConfiguration->wastegatePositionMax, min, max });
		idlePos.init({ engineConfiguration->idlePositionSensor, (float)engineConfiguration->idlePositionMin, (float)engineConfiguration->idlePositionMax, min, max });
	}

	// Route the pedal or TPS to driverIntent as appropriate
	if (isAdcChannelValid(engineConfiguration->throttlePedalPositionAdcChannel)) {
		driverIntent.setProxiedSensor(SensorType::AcceleratorPedal);
	} else {
		driverIntent.setProxiedSensor(SensorType::Tps1);
	}

	driverIntent.Register();
}

void deinitTps() {
	throttle1.deinit();
	throttle2.deinit();

	pedal.deinit();

	wastegate.deinit();
	idlePos.deinit();
}
