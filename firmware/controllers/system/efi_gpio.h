/**
 * @file	efi_gpio.h
 * @brief	EFI-related GPIO code
 *
 *
 * @date Sep 26, 2014
 * @author Andrey Belomutskiy, (c) 2012-2020
 */

#pragma once

#include "global.h"
#include "io_pins.h"
#include "engine_configuration.h"
#include "smart_gpio.h"

void initPrimaryPins(Logging *sharedLogger);
void initOutputPins(DECLARE_ENGINE_PARAMETER_SIGNATURE);

#if EFI_GPIO_HARDWARE
void turnAllPinsOff(void);
#else /* EFI_GPIO_HARDWARE */
#define turnAllPinsOff() {}
#endif /* EFI_GPIO_HARDWARE */

// Used if you want a function to be virtual only for unit testing purposes
#if EFI_UNIT_TEST
#define TEST_VIRTUAL virtual
#else
#define TEST_VIRTUAL
#endif

#ifdef __cplusplus
/**
 * @brief   Single output pin reference and state
 */
class OutputPin {
public:
	OutputPin();
	/**
	 * initializes pin & registers it in pin repository
	 * outputMode being a pointer allow us to change configuration (for example invert logical pin) in configuration and get resuts applied
	 * away, or at least I hope that's why
	 */
	void initPin(const char *msg, brain_pin_e brainPin, const pin_output_mode_e *outputMode);
	void initPin(const char *msg, brain_pin_e &brainPin, const pin_output_mode_e *outputMode);
	/**
	 * same as above, with DEFAULT_OUTPUT mode
	 */
	void initPin(const char *msg, brain_pin_e &brainPin);
	/**
	 * dissociates pin from this output and un-registers it in pin repository
	 */
	void unregisterOutput(brain_pin_e oldPin);

	bool isInitialized();

	bool getAndSet(int logicValue);
	TEST_VIRTUAL void setValue(int logicValue);
	void toggle();
	bool getLogicValue() const;


#if EFI_GPIO_HARDWARE
	pin_output_mode_e outputMode;
	// 4 byte pointer is a bit of a memory waste here
	const pin_output_mode_e *modePtr = nullptr;
	brain_pin_e brainPin;
	brain_pin_e *brainPinPtr = nullptr;
	#if (BOARD_EXT_GPIOCHIPS > 0)
		/* used for external pins */
		bool ext;
	#endif
#endif /* EFI_GPIO_HARDWARE */
	int8_t currentLogicValue = INITIAL_PIN_STATE;
	/**
	 * we track current pin status so that we do not touch the actual hardware if we want to write new pin bit
	 * which is same as current pin value. This maybe helps in case of status leds, but maybe it's a total over-engineering
	 */
	static void updateConfigurationList();
	void updateConfiguration();
	const char* m_msg;
private:
	// todo: inline this method?
	void setDefaultPinState(const pin_output_mode_e *defaultState);

	OutputPin *nextOutput = nullptr;
	static OutputPin* s_firstOutput;
};


class NamedOutputPin : public OutputPin {
public:
	NamedOutputPin();
	explicit NamedOutputPin(const char *name);
	void setHigh();
	void setLow();
	const char *getName() const;
	const char *getShortName() const;
	/**
	 * @return true if pin was stopped
	 */
	bool stop();
	// todo: char pointer is a bit of a memory waste here, we can reduce RAM usage by software-based getName() method
	const char *name;
	/**
	 * rusEfi Engine Sniffer protocol uses these short names to reduce bytes usage
	 */
	const char *shortName = NULL;
};

class InjectorOutputPin final : public NamedOutputPin {
public:
	InjectorOutputPin();
	void reset();

	void open(efitick_t nowNt DECLARE_ENGINE_PARAMETER_SUFFIX);
	void close(efitick_t nowNt DECLARE_ENGINE_PARAMETER_SUFFIX);

	int8_t getOverlappingCounter() const { return overlappingCounter; }

	// todo: re-implement this injectorIndex via address manipulation to reduce memory usage?
	int8_t injectorIndex;

private:
	int8_t overlappingCounter;
};

class IgnitionOutputPin : public NamedOutputPin {
public:
	IgnitionOutputPin();
	void reset();
	int signalFallSparkId;
	bool outOfOrder; // https://sourceforge.net/p/rusefi/tickets/319/
};

class EnginePins {
public:
	EnginePins();
	void startPins();
	void reset();
	bool stopPins();
	void unregisterPins();
	void startInjectionPins();
	void startIgnitionPins();
	void startAuxValves();
	void stopInjectionPins();
	void stopIgnitionPins();
	OutputPin mainRelay;

	// this one cranks engine
	OutputPin starterControl;
	// this one prevents driver from cranknig engine
	OutputPin starterRelayDisable;

	OutputPin fanRelay;
	// see acRelayPin
	OutputPin acRelay;
	OutputPin fuelPumpRelay;
	OutputPin o2heater;
	/**
	 * brain board RED LED by default
	 */
	OutputPin errorLedPin;
	OutputPin communicationLedPin; // blue LED on brain board by default
	OutputPin warningLedPin; // orange LED on brain board by default
	OutputPin runningLedPin; // green LED on brain board by default

	OutputPin debugTriggerSync;
	OutputPin boostPin;
	OutputPin idleSolenoidPin;
	OutputPin secondIdleSolenoidPin;
	OutputPin alternatorPin;
	/**
	 * this one is usually on the gauge cluster, not on the ECU
	 */
	OutputPin checkEnginePin;

	NamedOutputPin tachOut;

	OutputPin fsioOutputs[FSIO_COMMAND_COUNT];
	OutputPin triggerDecoderErrorPin;
	OutputPin hipCs;
	OutputPin sdCsPin;
	OutputPin accelerometerCs;

	InjectorOutputPin injectors[INJECTION_PIN_COUNT];
	IgnitionOutputPin coils[IGNITION_PIN_COUNT];
	NamedOutputPin auxValve[AUX_DIGITAL_VALVE_COUNT];
};

#endif /* __cplusplus */

/**
 * it's a macro to be sure that stack is not used
 * @return 0 for OM_DEFAULT and OM_OPENDRAIN
 */

#define getElectricalValue0(mode) ((mode) == OM_INVERTED || (mode) == OM_OPENDRAIN_INVERTED)

/**
 * it's a macro to be sure that stack is not used
 * @return 1 for OM_DEFAULT and OM_OPENDRAIN
 */
#define getElectricalValue1(mode) ((mode) == OM_DEFAULT || (mode) == OM_OPENDRAIN)

#define getElectricalValue(logicalValue, mode) \
	(logicalValue ? getElectricalValue1(mode) : getElectricalValue0(mode))

#if EFI_GPIO_HARDWARE

EXTERNC ioportmask_t getHwPin(const char *msg, brain_pin_e brainPin);
EXTERNC ioportid_t getHwPort(const char *msg, brain_pin_e brainPin);
const char *portname(ioportid_t GPIOx);

#endif /* EFI_GPIO_HARDWARE */

void printSpiConfig(Logging *logging, const char *msg, spi_device_e device);
brain_pin_e parseBrainPin(const char *str);
const char *hwPortname(brain_pin_e brainPin);
