#ifndef CST816S_H_
#define CST816S_H_

#if defined(__cplusplus) || defined(__XC__)
extern "C" {
#endif

#include "lvgl.h"

void touchscreen_read_cb(lv_indev_t * indev, lv_indev_data_t* data);
void reset_cst816s(void);
lv_indev_t* init_cst816s(void);

#if defined(__cplusplus) || defined(__XC__)
}
#endif

#endif /* CST816S_H_ */