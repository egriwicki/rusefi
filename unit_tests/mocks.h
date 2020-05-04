#include "electronic_throttle.h"
#include "dc_motor.h"
#include "table_helper.h"
#include "pwm_generator_logic.h"

#include "gmock/gmock.h"

class MockEtb : public IEtbController {
public:
	// IEtbController mocks
	MOCK_METHOD(void, reset, (), ());
	MOCK_METHOD(void, start, (), (override));
	MOCK_METHOD(void, init, (DcMotor* motor, int ownIndex, pid_s* pidParameters, const ValueProvider3D* pedalMap), (override));
	MOCK_METHOD(void, setIdlePosition, (percent_t pos), (override));
	MOCK_METHOD(void, autoCalibrateTps, (), (override));

	// ClosedLoopController mocks
	MOCK_METHOD(expected<percent_t>, getSetpoint, (), (const, override));
	MOCK_METHOD(expected<percent_t>, observePlant, (), (const, override));
	MOCK_METHOD(expected<percent_t>, getOpenLoop, (percent_t setpoint), (const, override));
	MOCK_METHOD(expected<percent_t>, getClosedLoop, (percent_t setpoint, percent_t observation), (override));
	MOCK_METHOD(void, setOutput, (expected<percent_t> outputValue), (override));
};

class MockMotor : public DcMotor {
public:
	MOCK_METHOD(bool, set, (float duty), (override));
	MOCK_METHOD(float, get, (), (const, override));
	MOCK_METHOD(void, enable, (), (override));
	MOCK_METHOD(void, disable, (), (override));
	MOCK_METHOD(bool, isOpenDirection, (), (const, override));
};

class MockVp3d : public ValueProvider3D {
public:
	MOCK_METHOD(float, getValue, (float xRpm, float y), (const, override));
};

class MockPwm : public SimplePwm {
public:
	MOCK_METHOD(void, setSimplePwmDutyCycle, (float dutyCycle), (override));
};
