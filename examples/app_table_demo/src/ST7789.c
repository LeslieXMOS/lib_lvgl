#include <stdio.h>
#include "spi.h"
#include "lvgl.h"
#include "src/drivers/display/st7789/lv_st7789.h"

// LCD Configuration
port_t p_io = XS1_PORT_4A;
uint32_t dc_pin = ST7789_DC_POS;
uint32_t rst_pin = ST7789_RST_POS;
uint32_t bl_pin = ST7789_BL_POS;
spi_master_t spi_ctx;
spi_master_device_t spi_dev;

void st7789_io(uint32_t pos, bool high) {
    int io_val = port_in(p_io);
    if (high) {
        io_val |= (1 << pos);
    } else {
        io_val &= ~(1 << pos);
    }
    port_out(p_io, io_val);
}

void st7789_dc(bool is_data) {
    st7789_io(dc_pin, is_data);     // 0: command, 1: data
}
void st7789_bl_enable(bool enable) {
    st7789_io(bl_pin, enable);      // BL active high
}
void st7789_rst() {
    st7789_io(rst_pin, false);      // RST active low
    lv_delay_ms(50);
    st7789_io(rst_pin, true);
    lv_delay_ms(50);
}

__attribute__(( fptrgroup("lv_lcd_send_cmd_cb") ))
static void send_cmd_st7789(lv_display_t *disp, const uint8_t *cmd, size_t cmd_size, const uint8_t *param, size_t param_size)
{
    LV_UNUSED(disp);

    st7789_dc(false);
    spi_master_start_transaction(&spi_dev);
    spi_master_transfer(&spi_dev, (uint8_t *)cmd, NULL, cmd_size);
    spi_master_end_transaction(&spi_dev);

    st7789_dc(true);
    spi_master_start_transaction(&spi_dev);
    spi_master_transfer(&spi_dev, (uint8_t *)param, NULL, param_size);
    spi_master_end_transaction(&spi_dev);
}

__attribute__(( fptrgroup("lv_lcd_send_color_cb") ))
static void send_color_st7789(lv_display_t *disp, const uint8_t *cmd, size_t cmd_size, uint8_t *param, size_t param_size)
{
    LV_UNUSED(disp);

    st7789_dc(false);
    spi_master_start_transaction(&spi_dev);
    spi_master_transfer(&spi_dev, (uint8_t *)cmd, NULL, cmd_size);
    spi_master_end_transaction(&spi_dev);

    st7789_dc(true);
    spi_master_start_transaction(&spi_dev);
    spi_master_transfer(&spi_dev, (uint8_t *)param, NULL, param_size);
    spi_master_end_transaction(&spi_dev);

    lv_display_flush_ready(disp);
}

/* Initialize LCD I/O bus, reset LCD */
lv_display_t* init_st7789(void)
{
    // SPI configuration
    port_t p_ss = ST7789_CS_PORT;
    port_t p_sclk = ST7789_SCK_PORT;
    port_t p_mosi = ST7789_MOSI_PORT;
    xclock_t cb = ST7789_CLKBLK;
    
    // Initialize the master device
    spi_master_init(&spi_ctx, cb, p_ss, p_sclk, p_mosi, 0);
    spi_master_device_init(
        &spi_dev, &spi_ctx,
        ST7789_CS_POS,
        SPI_MODE_0,
        spi_master_source_clock_ref,
        0,
        spi_master_sample_delay_0,
        0, 0 ,0 ,0 );

    port_enable(p_io);

    st7789_rst();
    st7789_bl_enable(true);

    /* Create the LVGL display object and the LCD display driver */
    lv_display_t *lcd_disp = lv_st7789_create(LCD_H_RES, LCD_V_RES, LV_LCD_FLAG_NONE, send_cmd_st7789, send_color_st7789);
    lv_st7789_set_gap(lcd_disp, (240-LCD_H_RES)/2, (320-LCD_V_RES)/2);
    lv_st7789_set_invert(lcd_disp, true);
    lv_display_set_rotation(lcd_disp, LV_DISPLAY_ROTATION_0);

    return lcd_disp;
}