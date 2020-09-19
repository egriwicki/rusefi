#include "start_stop.h"
#include "engine.h"

EXTERN_ENGINE;

ButtonDebounce startStopButtonDebounce;

void initStartStopButton(DECLARE_ENGINE_PARAMETER_SIGNATURE) {
	startStopButtonDebounce.init((CONFIG(startCrankingDuration)*1000), &CONFIG(startStopButtonPin), &CONFIG(startStopButtonMode));
}
