#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#include "bme68x.h"
#include "common.h"

#include "periph/i2c.h"
#include "ztimer.h"
#define SENSOR_BUS I2C_DEV(1)

/******************************************************************************/
/*!                Static variable definition                                 */
static uint8_t dev_addr;

/******************************************************************************/
/*!                User interface functions                                   */

BME68X_INTF_RET_TYPE bme68x_i2c_read(uint8_t reg_addr, uint8_t *reg_data, uint32_t len, void *intf_ptr)
{
    uint8_t dev_addr = *(uint8_t*)intf_ptr;
    int error = i2c_read_regs(SENSOR_BUS, dev_addr, reg_addr, reg_data, len, 0);
/*
    printf("bme68x_i2c_read %d:0x%02x reg:0x%02x len:%d\n", bus, SENSOR_ADDR, reg, len);
    printf("[ ");
    for(int i=0;i<len;i++){ printf("0x%02x ", bufp[i]); }
    printf("] error:%d\n", error);
*/
    return error;
}

BME68X_INTF_RET_TYPE bme68x_i2c_write(uint8_t reg_addr, const uint8_t *reg_data, uint32_t len, void *intf_ptr)
{
    uint8_t dev_addr = *(uint8_t*)intf_ptr;
    int error = i2c_write_regs(SENSOR_BUS, dev_addr, reg_addr, (uint8_t *)reg_data, len, 0);
/*
    printf("bme68x_i2c_read %d:0x%02x reg:0x%02x len:%d\n", bus, SENSOR_ADDR, reg, len);
    printf("[ ");
    for(int i=0;i<len;i++){ printf("0x%02x ", bufp[i]); }
    printf("] error:%d\n", error);
*/
    return error;
}

BME68X_INTF_RET_TYPE bme68x_spi_read(uint8_t reg_addr, uint8_t *reg_data, uint32_t len, void *intf_ptr)
{
    (void)reg_addr;
    (void)reg_data;
    (void)len;
    (void)intf_ptr;
    return -1;
}

BME68X_INTF_RET_TYPE bme68x_spi_write(uint8_t reg_addr, const uint8_t *reg_data, uint32_t len, void *intf_ptr)
{
    (void)reg_addr;
    (void)reg_data;
    (void)len;
    (void)intf_ptr;
    return -1;
}

void bme68x_delay_us(uint32_t period, void *intf_ptr)
{
    (void)intf_ptr;
    ztimer_sleep(ZTIMER_USEC, period);
}

void bme68x_check_rslt(const char api_name[], int8_t rslt)
{
    switch (rslt)
    {
        case BME68X_OK:

            /* Do nothing */
            break;
        case BME68X_E_NULL_PTR:
            printf("API name [%s]  Error [%d] : Null pointer\r\n", api_name, rslt);
            break;
        case BME68X_E_COM_FAIL:
            printf("API name [%s]  Error [%d] : Communication failure\r\n", api_name, rslt);
            break;
        case BME68X_E_INVALID_LENGTH:
            printf("API name [%s]  Error [%d] : Incorrect length parameter\r\n", api_name, rslt);
            break;
        case BME68X_E_DEV_NOT_FOUND:
            printf("API name [%s]  Error [%d] : Device not found\r\n", api_name, rslt);
            break;
        case BME68X_E_SELF_TEST:
            printf("API name [%s]  Error [%d] : Self test error\r\n", api_name, rslt);
            break;
        case BME68X_W_NO_NEW_DATA:
            printf("API name [%s]  Warning [%d] : No new data found\r\n", api_name, rslt);
            break;
        default:
            printf("API name [%s]  Error [%d] : Unknown error code\r\n", api_name, rslt);
            break;
    }
}

int8_t bme68x_interface_init(struct bme68x_dev *bme, uint8_t intf)
{
    int8_t rslt = BME68X_OK;
    /* Bus configuration : I2C */
    if (intf == BME68X_I2C_INTF)
    {
        printf("I2C Interface\n");
        dev_addr = BME68X_I2C_ADDR_LOW;
        bme->read = bme68x_i2c_read;
        bme->write = bme68x_i2c_write;
        bme->intf = BME68X_I2C_INTF;
    }
    /* Bus configuration : SPI */
    else if (intf == BME68X_SPI_INTF)
    {
        printf("SPI Interface UNSUPPORTED\n");
        rslt = BME68X_E_NULL_PTR;
    }

    bme->delay_us = bme68x_delay_us;
    bme->intf_ptr = &dev_addr;
    bme->amb_temp = 25; /* The ambient temperature in deg C is used for defining the heater temperature */
    return rslt;
}
