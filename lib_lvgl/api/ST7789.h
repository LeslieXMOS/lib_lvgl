#ifndef ST7789_H_
#define ST7789_H_

#if defined(__cplusplus) || defined(__XC__)
extern "C" {
#endif

#include "lvgl.h"
#include <platform.h>
#include <xcore/port.h>
#include <xcore/channel.h>

typedef struct {
    // LCD size
    size_t width;
    size_t height;
    // SPI Configuration
    port_t sck_port;    // This need to be a 1 bit port
    port_t mosi_port;   // This need to be a 1 bit port
    port_t cs_port;
    uint8_t cs_pin;    // Pin position in the port
    xclock_t spi_cb;
    chanend_t c_spi;
    // IO Configuration
    port_t dc_port;
    uint8_t dc_pin;
    port_t rst_port;
    uint8_t rst_pin;
    port_t bl_port;
    uint8_t bl_pin;
} st7789_config_t;

lv_display_t* init_st7789(const st7789_config_t* config);

#if defined(__cplusplus) || defined(__XC__)
}
#endif

#endif /* ST7789_H_ */