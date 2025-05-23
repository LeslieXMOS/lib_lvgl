#include "ST7789.h"
#include <stdio.h>
#include "spi.h"
#include "src/drivers/display/st7789/lv_st7789.h"
#include "spi_task.h"

typedef struct {
    spi_master_t spi_ctx;
    spi_master_device_t spi_dev;
    chanend_t c_spi;
    port_t dc_port;
    uint32_t dc_mask;
    port_t rst_port;
    uint32_t rst_mask;
    port_t bl_port;
    uint32_t bl_mask;
} st7789_handler_t;

st7789_handler_t *handler_temp;

void st7789_io(port_t p, uint32_t mask, bool high) {
    int io_val = port_in(p);
    if (high) {
        io_val |= mask;
    } else {
        io_val &= ~mask;
    }
    port_out(p, io_val);
}

void st7789_dc(st7789_handler_t* handler, bool is_data) {
    st7789_io(handler->dc_port, handler->dc_mask, is_data);  // 0: command, 1: data
}
void st7789_bl_enable(st7789_handler_t* handler, bool enable) {
    st7789_io(handler->bl_port, handler->bl_mask, enable);   // BL active high
}
void st7789_rst(st7789_handler_t* handler) {
    st7789_io(handler->rst_port, handler->rst_mask, false);  // RST active low
    lv_delay_ms(50);
    st7789_io(handler->rst_port, handler->rst_mask, true);
    lv_delay_ms(50);
}

__attribute__((fptrgroup("spi_task_callback")))
void spi_task_finish_cb(uint8_t* data_in, size_t len, void* user_data) {
    LV_UNUSED(data_in);
    LV_UNUSED(len);
    lv_display_flush_ready((lv_display_t*)user_data);
}

__attribute__(( fptrgroup("lv_lcd_send_cmd_cb") ))
void send_cmd_st7789(lv_display_t* disp, const uint8_t *cmd, size_t cmd_size, const uint8_t *param, size_t param_size)
{
    st7789_handler_t* handler = (st7789_handler_t*)lv_display_get_user_data(disp);
    if (handler == NULL) handler = handler_temp;
    spi_master_device_t* spi_dev = &(handler->spi_dev);

    st7789_dc(handler, false);
    if (handler->c_spi) {
        spi_task_master_send(handler->c_spi, (uint8_t*)cmd, NULL, cmd_size, NULL, NULL);
    } else {
        spi_master_start_transaction(spi_dev);
        spi_master_transfer(spi_dev, (uint8_t *)cmd, NULL, cmd_size);
        spi_master_end_transaction(spi_dev);
    }

    st7789_dc(handler, true);
    if (handler->c_spi) {
        spi_task_master_send(handler->c_spi, (uint8_t*)param, NULL, param_size, NULL, NULL);
    } else {
        spi_master_start_transaction(spi_dev);
        spi_master_transfer(spi_dev, (uint8_t *)param, NULL, param_size);
        spi_master_end_transaction(spi_dev);
    }
}

__attribute__(( fptrgroup("lv_lcd_send_color_cb") ))
void send_color_st7789(lv_display_t *disp, const uint8_t *cmd, size_t cmd_size, uint8_t *param, size_t param_size)
{
    st7789_handler_t* handler = (st7789_handler_t*)lv_display_get_user_data(disp);
    if (handler == NULL) handler = handler_temp;
    spi_master_device_t* spi_dev = &(handler->spi_dev);

    st7789_dc(handler, false);
    if (handler->c_spi) {
        spi_task_master_send(handler->c_spi, (uint8_t*)cmd, NULL, cmd_size, NULL, NULL);
    } else {
        spi_master_start_transaction(spi_dev);
        spi_master_transfer(spi_dev, (uint8_t *)cmd, NULL, cmd_size);
        spi_master_end_transaction(spi_dev);
    }

    st7789_dc(handler, true);
    if (handler->c_spi) {
        spi_task_master_send(handler->c_spi, (uint8_t*)param, NULL, param_size, spi_task_finish_cb, (void*)disp);
    } else {
        spi_master_start_transaction(spi_dev);
        spi_master_transfer(spi_dev, (uint8_t *)param, NULL, param_size);
        spi_master_end_transaction(spi_dev);
        lv_display_flush_ready(disp);
    }
}

/* Initialize LCD I/O bus, reset LCD */
lv_display_t* init_st7789(const st7789_config_t* config)
{
    st7789_handler_t* handler = lv_malloc(sizeof(st7789_handler_t));
    LV_ASSERT_MSG(handler, "ST7789 handler malloc failed\n");
    
    if (config->c_spi) {
        handler->c_spi = config->c_spi;
    } else {
        // Initialize the master device
        spi_master_init(&(handler->spi_ctx), config->spi_cb, config->cs_port, config->sck_port, config->mosi_port, 0);
        spi_master_device_init(
            &(handler->spi_dev), &(handler->spi_ctx),
            config->cs_pin,
            SPI_MODE_0,
            spi_master_source_clock_ref,
            0,
            spi_master_sample_delay_0,
            0, 0 ,0 ,0 );
    }

    port_enable(config->dc_port);
    if (config->rst_port != config->dc_port) {
        port_enable(config->rst_port);
    }
    if (config->bl_port != config->rst_port && config->bl_port != config->dc_port) {
        port_enable(config->bl_port);
    }
    handler->dc_port = config->dc_port;
    handler->dc_mask = 1 << config->dc_pin;
    handler->rst_port = config->rst_port;
    handler->rst_mask = 1 << config->rst_pin;
    handler->bl_port = config->bl_port;
    handler->bl_mask = 1 << config->bl_pin;

    st7789_rst(handler);
    st7789_bl_enable(handler, true);

    LV_ASSERT_MSG(config->width <= 240, "ST7789 screen width cannot larger than 240\n");
    LV_ASSERT_MSG(config->height <= 320, "ST7789 screen height cannot larger than 320\n");

    /* Create the LVGL display object and the LCD display driver */
    handler_temp = handler;
    lv_display_t *lcd_disp = lv_st7789_create(config->width, config->height, LV_LCD_FLAG_NONE, send_cmd_st7789, send_color_st7789);
    lv_display_set_user_data(lcd_disp, (void*)handler);
    lv_st7789_set_gap(lcd_disp, (240-config->width)/2, (320-config->height)/2);
    lv_st7789_set_invert(lcd_disp, true);
    lv_display_set_rotation(lcd_disp, LV_DISPLAY_ROTATION_0);

    return lcd_disp;
}