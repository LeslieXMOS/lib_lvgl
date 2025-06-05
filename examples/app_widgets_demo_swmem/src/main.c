#include <stdio.h>
#include <xcore/channel.h>
#include "lvgl.h"
#include "xcore/parallel.h"
#include "xcore/select.h"
#include "xcore/hwtimer.h"
#include <xcore/swmem_fill.h>
#include "flash_server.h"
#include "examples/lv_examples.h"
#include "demos/lv_demos.h"
#include "lvgl_task.h"
#include "ST7789.h"
#include "CST816S.h"
#include "spi_task.h"

chanend_t c_driver_spi;

// We must initialise this to a value such that it is not memset to zero during C runtime startup
#define SWMEM_ADDRESS_UNINITIALISED 0xffffffff
volatile unsigned int __swmem_address = SWMEM_ADDRESS_UNINITIALISED;

DECLARE_JOB(swmem_handler, (swmem_fill_t, chanend_t))
void swmem_handler(swmem_fill_t fill_handle, chanend_t c_flash)
{
    if (__swmem_address == SWMEM_ADDRESS_UNINITIALISED)
    {
        __swmem_address = 0;
    }

    fill_slot_t address = 0;
    swmem_fill_buffer_t buf;
    while (1)
    {
        address = swmem_fill_in_address(fill_handle);
        unsigned int * buf_ptr = (unsigned int *) buf;

        chan_out_word(c_flash, FLASH_READ_XIP_DIFF_TILE);
        chan_out_word(c_flash, (address - (void *)XS1_SWMEM_BASE + __swmem_address));// >> 2
        chan_out_word(c_flash, SWMEM_FILL_SIZE_WORDS*4);
        for (int i = 0; i < SWMEM_FILL_SIZE_WORDS; ++i) {
            buf_ptr[i] = chanend_in_word(c_flash);
        }
        chanend_check_end_token(c_flash);

        swmem_fill_populate_from_buffer(fill_handle, address, buf);
    }
}

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
    lv_demo_widgets();
}

void main_tile0(chanend_t c)
{
    chanend_t c_flash[1];
    flash_t flash_headers[1];

    c_flash[0] = c;

    fl_QuadDeviceSpec flash_spec[1] = {
        FL_QUADDEVICE_DEFAULT
    };

    fl_QSPIPorts qspi = {
        PORT_SQI_CS,
        PORT_SQI_SCLK,
        PORT_SQI_SIO,
        XS1_CLKBLK_2
    };

    // #define PATTERN_WORDS 8
    // static unsigned read_data_check[PATTERN_WORDS] = {
    //     0x0f0f00ff,
    //     0x0f0f0f0f,
    //     0x00ff00ff,
    //     0x00ff00ff,
    //     0x08cef731,
    //     0x08cef731,
    //     0x639c639c,
    //     0x639c639c
    // };
    // unsigned char flash_sector_buffer[4096];
    // printf("%d\n", fl_connectToDevice(&qspi, flash_spec, 1));
    // unsigned* sector_buffer_word = (unsigned*)flash_sector_buffer;
    // const unsigned page_size = fl_getPageSize();
    // const unsigned sector_size = fl_getDataSectorSize(0);
    // for (int j = 0; j < sector_size/page_size; ++j) {
    //     fl_readDataPage(j, &(flash_sector_buffer[j*page_size]));
    // }
    // fl_eraseDataSector(0);                        
    // for (int j = 0; j < PATTERN_WORDS*4; j+=4) {
    //     sector_buffer_word[j/4] = read_data_check[j/4];
    //     printf("%x\n", sector_buffer_word[j]);
    //     flash_sector_buffer[j] = (((flash_sector_buffer[j] << 4) & 0xf0) | ((flash_sector_buffer[j] >> 4) & 0x0f));
    //     flash_sector_buffer[j+1] = (((flash_sector_buffer[j+1] << 4) & 0xf0) | ((flash_sector_buffer[j+1] >> 4) & 0x0f));
    //     flash_sector_buffer[j+2] = (((flash_sector_buffer[j+2] << 4) & 0xf0) | ((flash_sector_buffer[j+2] >> 4) & 0x0f));
    //     flash_sector_buffer[j+3] = (((flash_sector_buffer[j+3] << 4) & 0xf0) | ((flash_sector_buffer[j+3] >> 4) & 0x0f));
    // }
    // printf("\n");
    // for (int j = 0; j < sector_size/page_size; ++j) {
    //     fl_writeDataPage(j, &(flash_sector_buffer[j*page_size]));
    // }
    // memset(sector_buffer_word, 0, sizeof(flash_sector_buffer));
    // fl_readData(0, PATTERN_WORDS*4, flash_sector_buffer);
    // for (int j = 0; j < PATTERN_WORDS; j++) {
    //     printf("%x\n", sector_buffer_word[j]);
    // }


    PAR_JOBS(
        PJOB(flash_server, (c_flash, flash_headers, 1, &qspi, flash_spec, 1))
    );
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

    swmem_fill_t fill_handle = swmem_fill_get();

    PAR_JOBS(
        PJOB(swmem_handler, (fill_handle, c)),
        PJOB(spi_master_task, (&spi_task_config)),
        PJOB(lvgl_task, (&lvgl_task_config))
    );
}