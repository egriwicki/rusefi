
#include "global.h"
#include "engine.h"

#include "gppwm_channel.h"
#include "pwm_generator_logic.h"

EXTERN_ENGINE;

static GppwmChannel channels[GPPWM_CHANNELS];
static OutputPin pins[GPPWM_CHANNELS];
static SimplePwm outputs[GPPWM_CHANNELS];

static gppwm_Map3D_t table1("GPPWM 1");
static gppwm_Map3D_t table2("GPPWM 2");
static gppwm_Map3D_t table3("GPPWM 3");
static gppwm_Map3D_t table4("GPPWM 4");

static gppwm_Map3D_t* tables[] = {
	&table1,
	&table2,
	&table3,
	&table4,
};

void initGpPwm(DECLARE_ENGINE_PARAMETER_SIGNATURE) {
	for (size_t i = 0; i < efi::size(channels); i++) {
		auto& cfg = CONFIG(gppwm)[i];

		// If no pin, don't enable this channel.
		if (cfg.pin == GPIO_UNASSIGNED) continue;

		// Determine frequency and whether PWM is enabled
		float freq = cfg.pwmFrequency;
		bool usePwm = freq > 0;

		// Setup pin & pwm
		pins[i].initPin("gp pwm", cfg.pin);
		startSimplePwm(&outputs[i], "gp pwm", &engine->executor, &pins[i], freq, 0);

		// Set up this channel's lookup table
		tables[i]->init(cfg.table, cfg.loadBins, cfg.rpmBins);

		// Finally configure the channel
		INJECT_ENGINE_REFERENCE(&channels[i]);
		channels[i].init(usePwm, &outputs[i], &pins[i], tables[i], &cfg);
	}
}

void updateGppwm() {
	for (size_t i = 0; i < efi::size(channels); i++) {
		channels[i].update();
	}
}
