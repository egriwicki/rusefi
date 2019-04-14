/*
 * @file smart_gpio.cpp
 *
 * @date Apr 13, 2019
 * @author Andrey Belomutskiy, (c) 2012-2019
 */

#include "global.h"

#if EFI_PROD_CODE
#include "smart_gpio.h"
#include "efi_gpio.h"
#include "engine_configuration.h"
#include "hardware.h"
#include "gpio_ext.h"
#include "drivers/gpio/tle6240.h"
#include "drivers/gpio/mc33972.h"
#include "drivers/gpio/tle8888.h"

EXTERN_CONFIG;

#if (BOARD_TLE6240_COUNT > 0)
	extern struct tle6240_config tle6240;
#endif

#if (BOARD_MC33972_COUNT > 0)
	extern struct mc33972_config mc33972;
#endif

void initSmartGpio() {
	int ret;

#if (BOARD_TLE6240_COUNT > 0)
	tle6240.spi_bus = getSpiDevice(engineConfiguration->tle6240spiDevice);
	ret = tle6240_add(0, &tle6240);
	if (ret < 0)
#endif /* (BOARD_TLE6240_COUNT > 0) */
		/* whenever chip is disabled or error returned - occupy its gpio range */
		gpiochip_use_gpio_base(TLE6240_OUTPUTS);

#if (BOARD_MC33972_COUNT > 0)
	mc33972.spi_bus = getSpiDevice(engineConfiguration->mc33972spiDevice);
	// todo: propogate 'basePinOffset' parameter
	ret = mc33972_add(0, &mc33972);
	if (ret < 0)
#endif /* (BOARD_MC33972_COUNT > 0) */
		/* whenever chip is disabled or error returned - occupy its gpio range */
		gpiochip_use_gpio_base(MC33972_INPUTS);

#if (BOARD_TLE8888_COUNT > 0)
	if (engineConfiguration->tle8888_cs != GPIO_UNASSIGNED) {
		static OutputPin tle8888Cs;
//		// SPI pins are enabled in initSpiModules()
		tle8888Cs.initPin("tle8888 CS", engineConfiguration->tle8888_cs,
					&engineConfiguration->tle8888_csPinMode);
	}

	ret = initTle8888(PASS_ENGINE_PARAMETER_SIGNATURE);
	if (ret < 0)
#endif /* (BOARD_TLE6240_COUNT > 0) */
		/* whenever chip is disabled or error returned - occupy its gpio range */
		gpiochip_use_gpio_base(TLE8888_OUTPUTS);

#if (BOARD_EXT_GPIOCHIPS > 0)
	/* external chip init */
	gpiochips_init();
#endif
}

#endif /* EFI_PROD_CODE */
