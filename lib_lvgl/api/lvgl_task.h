#ifndef LVGL_TASK_H_
#define LVGL_TASK_H_

#if defined(__cplusplus) || defined(__XC__)
extern "C" {
#endif

#include "lvgl.h"
#include "xcore/parallel.h"

typedef struct {
    lv_display_t* lcd_disp;
    lv_indev_t* input_dev;
} lvgl_drivers_t;

typedef struct {
    // Buffer size
    size_t buf_size;    // Range 1-10, 1: 1/10 screen size buffer, 10: 10/10 screen size buffer
    bool buf2;          // True: create buf2; False: do not create buf2
    // Driver init callback
    __attribute__(( fptrgroup("lvgl_driver_init") ))
    lvgl_drivers_t* (*driver_init)(void);
    // UI init callback
    __attribute__(( fptrgroup("lvgl_ui_init") ))
    void (*ui_init)(lvgl_drivers_t*);
} lvgl_task_config_t;

DECLARE_JOB(lvgl_task, (const lvgl_task_config_t*));
void lvgl_task(const lvgl_task_config_t* config);

#if defined(__cplusplus) || defined(__XC__)
}
#endif

#endif /* LVGL_TASK_H_ */