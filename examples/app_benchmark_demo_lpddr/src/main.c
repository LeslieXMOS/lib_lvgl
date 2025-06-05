#include <stdio.h>
#include <xcore/channel.h>
#include "lvgl.h"
#include "xcore/parallel.h"
#include "xcore/select.h"
#include "xcore/hwtimer.h"
#include "examples/lv_examples.h"
#include "demos/lv_demos.h"
#include "lvgl_task.h"
#include "ST7789.h"
#include "CST816S.h"
#include "spi_task.h"

chanend_t c_driver_spi;

__attribute__(( fptrgroup("lvgl_driver_init") ))
lvgl_drivers_t* lvgl_driver_init(void) {
    lvgl_drivers_t* drivers = lv_malloc(sizeof(lvgl_drivers_t));
    st7789_config_t disp_config = {
        .width = 240,
        .height = 280,
        .c_spi = c_driver_spi,
        .dc_port = XS1_PORT_4A,
        .dc_pin = 0,
        .rst_port = XS1_PORT_4A,
        .rst_pin = 1,
        .bl_port = XS1_PORT_4A,
        .bl_pin = 2
    };
    cst816s_config_t touch_config = {
        .scl_port = XS1_PORT_4B,
        .scl_pin = 0,
        .sda_port = XS1_PORT_4B,
        .sda_pin = 1,
        .rst_port = XS1_PORT_4C,
        .rst_pin = 0
    };

    drivers->lcd_disp = init_st7789(&disp_config);
    drivers->input_dev = init_cst816s(&touch_config);

    return drivers;
}

__attribute__(( fptrgroup("lvgl_ui_init") ))
void lvgl_ui_init(lvgl_drivers_t* drivers)
{
    LV_UNUSED(drivers);
    lv_demo_benchmark();
}

void main_tile0(chanend_t c)
{
}

void main_tile1(chanend_t c)
{
    channel_t c_spi = chan_alloc();

    spi_task_config_t spi_task_config = {
        .sck_port = XS1_PORT_1O,
        .mosi_port = XS1_PORT_1M,
        .cs_port = XS1_PORT_32A,
        .cs_pin = 0,
        .spi_cb = XS1_CLKBLK_1,
        .c_control = c_spi.end_a
    };
    c_driver_spi = c_spi.end_b;
    lvgl_task_config_t lvgl_task_config = {
        .buf_size = 1,  // using 1/10 of the screen size as buff
        .buf2 = true,   // enable buf2
        .driver_init = lvgl_driver_init,
        .ui_init = lvgl_ui_init
    };

    PAR_JOBS(
        PJOB(spi_master_task, (&spi_task_config)),
        PJOB(lvgl_task, (&lvgl_task_config))
    );
}