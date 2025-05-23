#include <stdio.h>
#include "i2c.h"
#include "i2c_reg.h"
#include "CST816S.h"

#define CST816S_ADDRESS 0x15

#define GESTURE_ID  0x01
#define FINGER_NUM  0x02
#define XPOSH       0x03
#define XPOSL       0x04
#define YPOSH       0x05
#define YPOSL       0x06
#define BPC0H       0xB0
#define BPC0L       0xB1
#define BPC1H       0xB2
#define BPC1L       0xB3
#define CHIP_ID     0xA7
#define PROJ_ID     0xA8
#define FW_VER      0xA9
#define MOTION_MASK         0xEC
#define IRQ_PULSE_WIDTH     0xED
#define NOR_SCAN_PER        0xEE
#define MOTION_S1_ANGLE     0xEF
#define LP_SCAN_RAW_1H      0xF0
#define LP_SCAN_RAW_1L      0xF1
#define LP_SCAN_RAW_2H      0xF2
#define LP_SCAN_RAW_2L      0xF3
#define LP_AUTO_WAKE_TIME   0xF4
#define LP_SCAN_TH          0xF5
#define LP_SCAN_WIN         0xF6
#define LP_SCAN_FREQ        0xF7
#define LP_SCAN_IDAC        0xF8
#define AUTO_SLEEP_TIME     0xF9
#define IRQ_CTL             0xFA
#define AUTO_RESET          0xFB
#define LONG_PRESS_TIME     0xFC
#define IOCTL               0xFD
#define DIS_AUTO_SLEEP      0xFE

typedef struct {
    i2c_master_t i2c_ctx;
    port_t rst_port;
    uint32_t rst_mask;
} cst816s_handler_t;

inline bool read_regbuf(
    i2c_master_t *ctx,
    uint8_t device_addr,
    uint8_t reg,
    uint8_t *buf,
    size_t reg_size)
{
    size_t bytes_sent = 0;
    i2c_res_t res;

    res = i2c_master_write(ctx, device_addr, &reg, 1, &bytes_sent, 0);
    if (bytes_sent != 1) {
        i2c_master_stop_bit_send(ctx);
        return false;
    }
    res = i2c_master_read(ctx, device_addr, buf, reg_size, 1);
    if (res == I2C_NACK) {
        return false;
    } else {
        return true;
    }
}


__attribute__(( fptrgroup("lv_indev_read_cb") ))
void touchscreen_read_cb(lv_indev_t* indev, lv_indev_data_t* data)
{
    uint8_t data_raw[8];
    i2c_master_t* i2c_ctx = &(((cst816s_handler_t*)lv_indev_get_driver_data(indev))->i2c_ctx);
    bool status = read_regbuf(i2c_ctx, CST816S_ADDRESS, 0x02, data_raw, 5);
    LV_UNUSED(status);
    if (data_raw[0]) {
        data->point.x = (data_raw[1] << 8 | data_raw[2]) & 0x0FFF;
        data->point.y = (data_raw[3] << 8 | data_raw[4]) & 0x0FFF;
        data->state = LV_INDEV_STATE_PRESSED;
    } else {
        data->state = LV_INDEV_STATE_RELEASED;
    }
}

void reset_cst816s(cst816s_handler_t* handler) {
    int io_val = port_in(handler->rst_port);
    io_val &= ~(handler->rst_mask);  // RST active low
    port_out(handler->rst_port, io_val);
    lv_delay_ms(50);
    io_val = port_in(handler->rst_port) | handler->rst_mask;
    port_out(handler->rst_port, io_val);
    lv_delay_ms(50);
}

/* Initialize Touchscreen I/O bus, reset controller */
lv_indev_t* init_cst816s(const cst816s_config_t* config)
{
    cst816s_handler_t* handler = lv_malloc(sizeof(cst816s_handler_t));
    LV_ASSERT_MSG(handler, "CST816S handler malloc failed\n");

    uint32_t scl_mask, sda_mask;
    scl_mask = ~(1 << config->scl_pin);
    if (config->scl_port == config->sda_port) {
        LV_ASSERT_MSG(config->scl_pin != config->sda_pin, "CST816S I2C's SDA and SCL shouldn't be same pin\n");
        scl_mask &= ~(1 << config->sda_pin);
        sda_mask = scl_mask;
    } else {
        sda_mask = ~(1 << config->sda_pin);
    }

    i2c_master_init(&(handler->i2c_ctx),
        config->scl_port, config->scl_pin, scl_mask,
        config->sda_port, config->sda_pin, sda_mask,
        400);

    port_enable(config->rst_port);
    handler->rst_port = config->rst_port;
    handler->rst_mask = (1 << config->rst_pin);
    reset_cst816s(handler);

    uint8_t tp_version_info[3];
    if (read_regbuf(&(handler->i2c_ctx), CST816S_ADDRESS, CHIP_ID, tp_version_info, 3)) {
        LV_LOG_INFO("CST816S INFO:\n    Chip ID: 0x%02X\n    Proj ID: 0x%02X\n    FW Ver: 0x%02X\n", tp_version_info[0], tp_version_info[1], tp_version_info[2]);
        write_reg(&(handler->i2c_ctx), CST816S_ADDRESS, DIS_AUTO_SLEEP, 0x01);
    } else {
        LV_LOG_ERROR("CST816S communication failed\n");
    }

    lv_indev_t* touchdev = lv_indev_create();
    lv_indev_set_driver_data(touchdev, (void*)handler);
    lv_indev_set_type(touchdev, LV_INDEV_TYPE_POINTER);
    lv_indev_set_read_cb(touchdev, touchscreen_read_cb);

    return touchdev;
}