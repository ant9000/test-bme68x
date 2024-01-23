#include "bme68x.h"
#define BME68X_PARAMS_I2C                    \
{                                            \
        .ifsel          = BME68X_I2C_INTF,   \
        .intf.i2c.dev   = ACME1_I2C_DEV,     \
        .intf.i2c.addr  = BME68X_I2C_ADDR_1, \
},                                           \
{                                            \
        .ifsel          = BME68X_I2C_INTF,   \
        .intf.i2c.dev   = ACME1_I2C_DEV,     \
        .intf.i2c.addr  = BME68X_I2C_ADDR_2, \
}
#include "bme68x_params.h"
#include "common.h"

int main(void) {
    bme68x_t dev[BME68X_NUMOF];

    for (unsigned i = 0; i < BME68X_NUMOF; i++) {
        printf("Initialize BME68X sensor %u ... ", i);
        if (bme68x_init(&dev[i], &bme68x_params[i]) != BME68X_OK) {
            puts("failed");
            return -1;
        }
        else {
            puts("OK");
        }
    }

    //bme68x_forced_mode();
    //bme68x_sequential_mode();
    return 0;
}
