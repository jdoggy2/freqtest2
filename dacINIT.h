#ifndef DACINIT_H
#define DACINIT_H

#include "ti_msp_dl_config.h"
#include <stdint.h>

#define DAC80508_REG_NOP      0x00
#define DAC80508_REG_DEVID    0x01
#define DAC80508_REG_SYNC     0x02
#define DAC80508_REG_CONFIG   0x03
#define DAC80508_REG_GAIN     0x04
#define DAC80508_REG_TRIGGER  0x05
#define DAC80508_REG_BRDCAST  0x06
#define DAC80508_REG_STATUS   0x07
#define DAC80508_REG_DAC0     0x08
#define DAC80508_REG_DAC1     0x09
#define DAC80508_REG_DAC2     0x0A
#define DAC80508_REG_DAC3     0x0B
#define DAC80508_REG_DAC4     0x0C
#define DAC80508_REG_DAC5     0x0D
#define DAC80508_REG_DAC6     0x0E
#define DAC80508_REG_DAC7     0x0F

#define dacSOFTRESET_VAL  0x000Au

#define extLedON()  DL_GPIO_setPins  (gpioLED2_PORT, gpioLED2_LED2_PIN)
#define extLedOFF() DL_GPIO_clearPins(gpioLED2_PORT, gpioLED2_LED2_PIN)
#define extLedTOGGLE() DL_GPIO_togglePins(gpioLED2_PORT, gpioLED2_LED2_PIN)

/* CLR — PB7 */
#define DAC_CLR_HIGH() DL_GPIO_setPins  (gpioCLR_PORT, gpioCLR_clrPIN_PIN)
#define DAC_CLR_LOW()  DL_GPIO_clearPins(gpioCLR_PORT, gpioCLR_clrPIN_PIN)

/* LED — PA0 */
#define ledON()  DL_GPIO_setPins  (gpioLED_PORT, gpioLED_LED1_PIN)
#define ledOFF() DL_GPIO_clearPins(gpioLED_PORT, gpioLED_LED1_PIN)

/*old byte shift 
static inline void DAC80508_WRITE(uint8_t addr, uint16_t data)
{
    while (DL_SPI_isBusy(mySPI_INST)) {}
    dbg_spiStat0 = SPI1->STAT;
    DL_SPI_transmitData8(mySPI_INST,  addr        & 0x1Fu);
    DL_SPI_transmitData8(mySPI_INST, (data >> 8)  & 0xFFu);
    DL_SPI_transmitData8(mySPI_INST,  data        & 0xFFu);
    while (DL_SPI_isBusy(mySPI_INST)) {}
    dbg_spiDone  = SPI1->STAT;
    dbg_writeCount++;
}
*/
/* Drive PB6 low/high directly via GPIOB registers */

#define _CS_LOW()  (GPIOB->DOUTCLR31_0 = (1u << 6))
#define _CS_HIGH() (GPIOB->DOUTSET31_0 = (1u << 6))

/*
static inline void DAC80508_WRITE(uint8_t addr, uint16_t data)
{
    while (DL_SPI_isBusy(mySPI_INST)) {}
    _CS_LOW();
    DL_SPI_transmitData8(mySPI_INST,  addr        & 0x1Fu);
    DL_SPI_transmitData8(mySPI_INST, (data >> 8)  & 0xFFu);
    DL_SPI_transmitData8(mySPI_INST,  data        & 0xFFu);
    while (DL_SPI_isBusy(mySPI_INST)) {}
    _CS_HIGH();
}
*/
static inline void DAC80508_WRITE(uint8_t addr, uint16_t data)
{
    while (DL_SPI_isBusy(mySPI_INST)) {}
    DL_SPI_transmitData8(mySPI_INST,  addr        & 0x1Fu);
    DL_SPI_transmitData8(mySPI_INST, (data >> 8)  & 0xFFu);
    DL_SPI_transmitData8(mySPI_INST,  data        & 0xFFu);
    while (DL_SPI_isBusy(mySPI_INST)) {}
 //   dbg_writeCount++;
}
static inline void DAC80508_write_reg(uint8_t reg, uint16_t val)
{
    DAC80508_WRITE(reg, val);
}

static inline void DAC80508_soft_reset(void)
{
    DAC80508_WRITE(DAC80508_REG_TRIGGER, dacSOFTRESET_VAL);
}

static inline void dacSetChannel(uint8_t ch, uint16_t code)
{
    DAC80508_WRITE((uint8_t)(DAC80508_REG_DAC0 + (ch & 0x07u)), code);
}

#endif