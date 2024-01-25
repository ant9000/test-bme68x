#include <stdio.h>

#include "bme68x.h"
#include "ztimer.h"
#include "functions.h"

/*
 * Macro definition for valid new data (0x80) AND
 * heater stability (0x10) AND gas resistance (0x20) values
 */
#define BME68X_VALID_DATA  UINT8_C(0xB0)

int bme68x_forced_mode(bme68x_t *dev)
{
    struct bme68x_dev *bme = &BME68X_SENSOR(dev);
    int8_t rslt;
    struct bme68x_data data;
    uint32_t del_period;
    uint32_t time_ms = 0;
    uint8_t n_fields;
    uint16_t sample_count = 1;

    dev->config.op_mode = BME68X_FORCED_MODE;
    dev->config.sensors.filter = BME68X_FILTER_OFF;
    dev->config.sensors.odr = BME68X_ODR_NONE;
    dev->config.sensors.os_hum = BME68X_OS_16X;
    dev->config.sensors.os_pres = BME68X_OS_1X;
    dev->config.sensors.os_temp = BME68X_OS_2X;
    dev->config.heater.enable = BME68X_ENABLE;
    dev->config.heater.heatr_temp = 300;
    dev->config.heater.heatr_dur = 100;
    rslt = bme68x_apply_config(dev);
    bme68x_check_rslt("bme68x_apply_config", rslt);

    printf("Sample, TimeStamp(ms), Temperature(deg C), Pressure(Pa), Humidity(%%), Gas resistance(ohm), Status\n");

    while (sample_count <= SAMPLE_COUNT)
    {
        rslt = bme68x_start_measure(dev);
        bme68x_check_rslt("bme68x_start_measure", rslt);

        /* Calculate delay period in microseconds */
        del_period = bme68x_get_meas_dur(dev->config.op_mode, &dev->config.sensors, bme) + (dev->config.heater.heatr_dur * 1000);
        bme->delay_us(del_period, bme->intf_ptr);

        time_ms = ztimer_now(ZTIMER_MSEC);

        /* Check if rslt == BME68X_OK, report or handle if otherwise */
        rslt = bme68x_get_data(dev->config.op_mode, &data, &n_fields, bme);
        bme68x_check_rslt("bme68x_get_data", rslt);

        if (n_fields)
        {
#ifdef BME68X_USE_FPU
            printf("%u, %lu, %.2f, %.2f, %.2f, %.2f, 0x%x\n",
                   sample_count,
                   (long unsigned int)time_ms,
                   data.temperature,
                   data.pressure,
                   data.humidity,
                   data.gas_resistance,
                   data.status);
#else
            printf("%u, %lu, %d, %lu, %lu, %lu, 0x%x\n",
                   sample_count,
                   (long unsigned int)time_ms,
                   (data.temperature / 100),
                   (long unsigned int)data.pressure,
                   (long unsigned int)(data.humidity / 1000),
                   (long unsigned int)data.gas_resistance,
                   data.status);
#endif
            sample_count++;
        }
    }

    return rslt;
}

int bme68x_sequential_mode(bme68x_t *dev)
{
    struct bme68x_dev *bme = &BME68X_SENSOR(dev);
    int8_t rslt;
    struct bme68x_data data[3];
    uint32_t del_period;
    uint32_t time_ms = 0;
    uint8_t n_fields;
    uint16_t sample_count = 1;

    /* Heater temperature in degree Celsius */
    uint16_t temp_prof[10] = { 200, 240, 280, 320, 360, 360, 320, 280, 240, 200 };

    /* Heating duration in milliseconds */
    uint16_t dur_prof[10] = { 100, 100, 100, 100, 100, 100, 100, 100, 100, 100 };

    dev->config.op_mode = BME68X_SEQUENTIAL_MODE;
    dev->config.sensors.filter = BME68X_FILTER_OFF;
    dev->config.sensors.odr = BME68X_ODR_NONE; /* This parameter defines the sleep duration after each profile */
    dev->config.sensors.os_hum = BME68X_OS_16X;
    dev->config.sensors.os_pres = BME68X_OS_1X;
    dev->config.sensors.os_temp = BME68X_OS_2X;
    dev->config.heater.enable = BME68X_ENABLE;
    dev->config.heater.heatr_temp_prof = temp_prof;
    dev->config.heater.heatr_dur_prof = dur_prof;
    dev->config.heater.profile_len = 10;
    rslt = bme68x_apply_config(dev);
    bme68x_check_rslt("bme68x_apply_config", rslt);

    /* Check if rslt == BME68X_OK, report or handle if otherwise */
    rslt = bme68x_start_measure(dev);
    bme68x_check_rslt("bme68x_start_measure", rslt);

    /* Check if rslt == BME68X_OK, report or handle if otherwise */
    printf(
        "Sample, TimeStamp(ms), Temperature(deg C), Pressure(Pa), Humidity(%%), Gas resistance(ohm), Status, Profile index, Measurement index\n");
    while (sample_count <= SAMPLE_COUNT)
    {
        /* Calculate delay period in microseconds */
        del_period = bme68x_get_meas_dur(dev->config.op_mode, &dev->config.sensors, bme) + (dev->config.heater.heatr_dur_prof[0] * 1000);
        bme->delay_us(del_period, bme->intf_ptr);

        time_ms = ztimer_now(ZTIMER_MSEC);

        rslt = bme68x_get_data(dev->config.op_mode, data, &n_fields, bme);
        bme68x_check_rslt("bme68x_get_data", rslt);

        /* Check if rslt == BME68X_OK, report or handle if otherwise */
        for (uint8_t i = 0; i < n_fields; i++)
        {
#ifdef BME68X_USE_FPU
            printf("%u, %lu, %.2f, %.2f, %.2f, %.2f, 0x%x, %d, %d\n",
                   sample_count,
                   (long unsigned int)time_ms + (i * (del_period / 2000)),
                   data[i].temperature,
                   data[i].pressure,
                   data[i].humidity,
                   data[i].gas_resistance,
                   data[i].status,
                   data[i].gas_index,
                   data[i].meas_index);
#else
            printf("%u, %lu, %d, %lu, %lu, %lu, 0x%x, %d, %d\n",
                   sample_count,
                   (long unsigned int)time_ms + (i * (del_period / 2000)),
                   (data[i].temperature / 100),
                   (long unsigned int)data[i].pressure,
                   (long unsigned int)(data[i].humidity / 1000),
                   (long unsigned int)data[i].gas_resistance,
                   data[i].status,
                   data[i].gas_index,
                   data[i].meas_index);
#endif
            sample_count++;
        }
    }

    return 0;
}

int bme68x_parallel_mode(bme68x_t *dev)
{
    struct bme68x_dev *bme = &BME68X_SENSOR(dev);
    int8_t rslt;
    struct bme68x_data data[3];
    uint32_t del_period;
    uint8_t n_fields;
    uint32_t time_ms = 0;
    uint16_t sample_count = 1;

    /* Heater temperature in degree Celsius */
    uint16_t temp_prof[10] = { 320, 100, 100, 100, 200, 200, 200, 320, 320, 320 };

    /* Multiplier to the shared heater duration */
    uint16_t mul_prof[10] = { 5, 2, 10, 30, 5, 5, 5, 5, 5, 5 };

    dev->config.op_mode = BME68X_PARALLEL_MODE;
    dev->config.sensors.filter = BME68X_FILTER_OFF;
    dev->config.sensors.odr = BME68X_ODR_NONE;
    dev->config.sensors.os_hum = BME68X_OS_1X;
    dev->config.sensors.os_pres = BME68X_OS_16X;
    dev->config.sensors.os_temp = BME68X_OS_2X;
    dev->config.heater.enable = BME68X_ENABLE;
    dev->config.heater.heatr_temp_prof = temp_prof;
    dev->config.heater.heatr_dur_prof = mul_prof;
    dev->config.heater.profile_len = 10;
    /* Shared heating duration in milliseconds */
    dev->config.heater.shared_heatr_dur = (uint16_t)(140 - (bme68x_get_meas_dur(dev->config.op_mode, &dev->config.sensors, bme) / 1000));

    rslt = bme68x_apply_config(dev);
    bme68x_check_rslt("bme68x_apply_config", rslt);

    /* Check if rslt == BME68X_OK, report or handle if otherwise */
    rslt = bme68x_start_measure(dev);
    bme68x_check_rslt("bme68x_start_measure", rslt);

    printf(
        "Print parallel mode data if mask for new data(0x80), gas measurement(0x20) and heater stability(0x10) are set\n\n");

    /* Check if rslt == BME68X_OK, report or handle if otherwise */
    printf(
        "Sample, TimeStamp(ms), Temperature(deg C), Pressure(Pa), Humidity(%%), Gas resistance(ohm), Status, Gas index, Meas index\n");
    while (sample_count <= SAMPLE_COUNT)
    {
        /* Calculate delay period in microseconds */
        del_period = bme68x_get_meas_dur(dev->config.op_mode, &dev->config.sensors, bme) + (dev->config.heater.shared_heatr_dur * 1000);
        bme->delay_us(del_period, bme->intf_ptr);

        time_ms = ztimer_now(ZTIMER_MSEC);

        rslt = bme68x_get_data(dev->config.op_mode, data, &n_fields, bme);
        bme68x_check_rslt("bme68x_get_data", rslt);

        /* Check if rslt == BME68X_OK, report or handle if otherwise */
        for (uint8_t i = 0; i < n_fields; i++)
        {
            if (data[i].status == BME68X_VALID_DATA)
            {
#ifdef BME68X_USE_FPU
                printf("%u, %lu, %.2f, %.2f, %.2f, %.2f, 0x%x, %d, %d\n",
                       sample_count,
                       (long unsigned int)time_ms,
                       data[i].temperature,
                       data[i].pressure,
                       data[i].humidity,
                       data[i].gas_resistance,
                       data[i].status,
                       data[i].gas_index,
                       data[i].meas_index);
#else
                printf("%u, %lu, %d, %lu, %lu, %lu, 0x%x, %d, %d\n",
                       sample_count,
                       (long unsigned int)time_ms,
                       (data[i].temperature / 100),
                       (long unsigned int)data[i].pressure,
                       (long unsigned int)(data[i].humidity / 1000),
                       (long unsigned int)data[i].gas_resistance,
                       data[i].status,
                       data[i].gas_index,
                       data[i].meas_index);
#endif
                sample_count++;
            }
        }
    }

    return 0;
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
