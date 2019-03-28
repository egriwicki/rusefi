/**
 *
 * TLE8888 driver
 *
 * at the moment only Enable Outputs is implemented - this command is needed to get parallel control pins
 *
 * @date Mar 25, 2019
 * @author Andrey Belomutskiy, (c) 2012-2019
 */

#include "engine.h"
#include "tle8888.h"
#include "hardware.h"

#if EFI_TLE8888 || defined(__DOXYGEN__)

/**
 * 15.2 SPI Frame Definition
 *
 */
#define Cmd_write_access 1
#define CmdOE 0x1C
#define DATA_OE_ENABLE 0b00000010

static unsigned char tx_buff[2];
static unsigned char rx_buff[2];

extern TunerStudioOutputChannels tsOutputChannels;

EXTERN_ENGINE;

static SPIDriver *driver;

static OutputPin tle8888Cs;

static SPIConfig spiConfig = { /* end_cb */ NULL,
	/* HW dependent part.*/
	/* ssport */ NULL, /* sspad */ 0, /* cr1 */ SPI_CR1_MSTR | SPI_CR1_CPHA, /* cr2*/ 0 };

void initTle8888(DECLARE_ENGINE_PARAMETER_SIGNATURE) {
	if (engineConfiguration->tle8888_cs == GPIO_UNASSIGNED) {
		return;
	}
	// SPI pins are enabled in initSpiModules()
	tle8888Cs.initPin("tle8888 CS", engineConfiguration->tle8888_cs,
			&engineConfiguration->tle8888_csPinMode);

	spiConfig.cr1 += getSpiPrescaler(_150KHz, engineConfiguration->tle8888spiDevice);

	driver = getSpiDevice(engineConfiguration->tle8888spiDevice);
	if (driver == NULL) {
		// error already reported
		return;
	}
	// todo: reuse initSpiCs method?
	spiConfig.ssport = getHwPort("tle8888", engineConfiguration->tle8888_cs);
	spiConfig.sspad = getHwPin("tle8888", engineConfiguration->tle8888_cs);

	spiStart(driver, &spiConfig);

	tx_buff[0] = Cmd_write_access + CmdOE;
	tx_buff[1] = DATA_OE_ENABLE;
	// todo: extract 'sendSync' method?
	spiSelect(driver);
	spiSend(driver, 2, tx_buff);
	if (engineConfiguration->debugMode == DBG_TLE8888) {
		tsOutputChannels.debugIntField1 = 2;
	}
	/**
	 * 15.1 SPI Protocol
	 *
	 * after a read or write command: the address and content of the selected register
	 * is transmitted with the next SPI transmission (for not existing addresses or
	 * wrong access mode the data is always �0�)
	 */

	spiReceive(driver, 2, rx_buff);

	if (engineConfiguration->debugMode == DBG_TLE8888) {
		tsOutputChannels.debugIntField2 = (rx_buff[1] << 8) + rx_buff[0];
	}

	spiUnselect(driver);
}

#endif /* EFI_TLE8888 */
