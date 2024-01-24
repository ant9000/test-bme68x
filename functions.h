#include "bme68x.h"

#define SAMPLE_COUNT  UINT8_C(20)

int bme68x_forced_mode(bme68x_t *dev);
int bme68x_sequential_mode(bme68x_t *dev);
int bme68x_parallel_mode(bme68x_t *dev);
void bme68x_check_rslt(const char api_name[], int8_t rslt);
