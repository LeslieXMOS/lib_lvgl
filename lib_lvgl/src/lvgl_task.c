#include "lvgl_task.h"
#include <stdint.h>
#include "xcore/parallel.h"
#include "xcore/select.h"
#include "xcore/hwtimer.h"

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

void lvgl_task(const lvgl_task_config_t* config) {
    /* Initialize LVGL */
    lv_init();
    lv_tick_set_cb(lvgl_tick_get_cb);

    lvgl_drivers_t* drivers = config->driver_init();

    /* Allocate draw buffers on the heap */
    lv_color_t * buf1 = NULL;
    lv_color_t * buf2 = NULL;
    
    uint32_t buf_size = config->buf_size * lv_display_get_horizontal_resolution(drivers->lcd_disp) * lv_display_get_vertical_resolution(drivers->lcd_disp) / 10 * lv_color_format_get_size(lv_display_get_color_format(drivers->lcd_disp));
    
    buf1 = lv_malloc(buf_size);
    if(buf1 == NULL) {
        LV_LOG_ERROR("display draw buffer malloc failed");
        return;
    }

    if (config->buf2) {
        buf2 = lv_malloc(buf_size);
        if(buf2 == NULL) {
            LV_LOG_ERROR("display buffer malloc failed");
            lv_free(buf1);
            return;
        }
    }
    
    lv_display_set_buffers(drivers->lcd_disp, buf1, buf2, buf_size, LV_DISPLAY_RENDER_MODE_PARTIAL);

    config->ui_init(drivers);

    while (1) {
        lv_timer_handler();
    }
}