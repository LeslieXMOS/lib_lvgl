#include <stdio.h>
#include <xcore/channel.h>
#include "lvgl.h"
#include "xcore/parallel.h"
#include "xcore/select.h"
#include "xcore/hwtimer.h"
#include "ST7789.h"
#include "CST816S.h"
#include "examples/lv_examples.h"
#include "demos/lv_demos.h"

void lvgl_ui(lv_display_t *disp)
{
    lv_example_table_2();
    lv_example_msgbox_2();
}

__attribute__(( fptrgroup("lv_tick_get_cb") ))
uint32_t lvgl_tick_get_cb(void) {
    static uint32_t tick_ms = 0;
    static uint32_t last_call_time = 0;
    if (last_call_time == 0) {
        last_call_time = get_reference_time();
        return 0;
    } else {
        uint32_t now = get_reference_time();
        if ((now - last_call_time) >= 100000) {
            tick_ms += (now - last_call_time) / 100000;
            last_call_time = now;
        }
        return tick_ms;
    }
}

DECLARE_JOB(lvgl_task, (void));
void lvgl_task(void)
{
    /* Initialize LVGL */
    lv_init();
    lv_tick_set_cb(lvgl_tick_get_cb);

    lv_display_t *lcd_disp = init_st7789();
    lv_indev_t* touchdev = init_cst816s();

    /* Allocate draw buffers on the heap. In this example we use two partial buffers of 1/10th size of the screen */
    lv_color_t * buf1 = NULL;
    lv_color_t * buf2 = NULL;
    uint32_t buf_size = LCD_H_RES * LCD_V_RES / 10 * lv_color_format_get_size(lv_display_get_color_format(lcd_disp));

    buf1 = lv_malloc(buf_size);
    if(buf1 == NULL) {
        LV_LOG_ERROR("display draw buffer malloc failed");
        return;
    }

    buf2 = lv_malloc(buf_size);
    if(buf2 == NULL) {
        LV_LOG_ERROR("display buffer malloc failed");
        lv_free(buf1);
        return;
    }
    
    lv_display_set_buffers(lcd_disp, buf1, buf2, buf_size, LV_DISPLAY_RENDER_MODE_PARTIAL);

    lvgl_ui(lcd_disp);

    while (1) {
        lv_timer_handler();
    }
}

void main_tile0(chanend_t c)
{
}

void main_tile1(chanend_t c)
{
    PAR_JOBS(
        PJOB(lvgl_task, ())
    );
}