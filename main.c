#include "bme68x.h"
#include "bme68x_params.h"

#include "functions.h"

int main(void) {
    bme68x_t dev[BME68X_NUMOF];
    for (unsigned i = 0; i < BME68X_NUMOF; i++) {
        printf("Initialize BME68X sensor %u ... ", i);
        if (bme68x_init(&dev[i], &bme68x_params[i]) != BME68X_OK) {
            puts("failed.");
            dev[i].sensor.chip_id = 0;
        } else {
            puts("OK.");
            printf("Self test ... "); fflush(stdout);
            int res = bme68x_self_test(&dev[i]);
            if (res != BME68X_OK) {
                puts("failed.");
            } else {
                puts("OK.");
            }
        }
    }
    for (unsigned i = 0, j = 0; i < BME68X_NUMOF; i++) {
        printf(">>> Sensor %d: ", i);
        if (dev[i].sensor.chip_id == BME68X_CHIP_ID) {
            switch (j++ % 3) {
            case 0:
                puts("forced mode.");
                bme68x_forced_mode(&dev[i]);
                break;
            case 1:
                puts("sequential mode.");
                bme68x_sequential_mode(&dev[i]);
                break;
            case 2:
                puts("parallel mode.");
                bme68x_parallel_mode(&dev[i]);
                break;
            }
        } else {
            puts("not initialized.");
        }
    }
    return 0;
}
