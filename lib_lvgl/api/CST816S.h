#ifndef CST816S_H_
#define CST816S_H_

#if defined(__cplusplus) || defined(__XC__)
extern "C" {
#endif

#include "lvgl.h"
#include <platform.h>
#include <xcore/port.h>

typedef struct {
    port_t scl_port;
    uint8_t scl_pin;
    port_t sda_port;
    uint8_t sda_pin;
    port_t rst_port;
    uint8_t rst_pin;
} cst816s_config_t;

lv_indev_t* init_cst816s(const cst816s_config_t* config);

#if defined(__cplusplus) || defined(__XC__)
}
#endif

#endif /* CST816S_H_ */