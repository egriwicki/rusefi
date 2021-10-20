#pragma once

#include "engine_ptr.h"
#include "ac_control_generated.h"

class AcState final : public ac_control_s {
public:
	// Returns true if AC is currently active
	bool updateAc(DECLARE_ENGINE_PARAMETER_SIGNATURE);

private:
	bool getAcState(DECLARE_ENGINE_PARAMETER_SIGNATURE);
};
