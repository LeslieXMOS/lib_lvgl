#ifndef SPI_TASK_H_
#define SPI_TASK_H_

#if defined(__cplusplus) || defined(__XC__)
extern "C" {
#endif

#include <platform.h>
#include <xcore/port.h>
#include <xcore/channel.h>
#include "xcore/parallel.h"

typedef struct {
    port_t sck_port;    // This need to be a 1 bit port
    port_t mosi_port;   // This need to be a 1 bit port
    port_t cs_port;
    uint8_t cs_pin;     // Pin position in the port
    xclock_t spi_cb;
    chanend_t c_control;
} spi_task_config_t;

typedef void(*finish_cb_t)(uint8_t* data_in, size_t len, void*);

void spi_task_master_send(chanend_t c_control, uint8_t* data_out, uint8_t* data_in, size_t len, finish_cb_t finish_cb, void* user_data);

DECLARE_JOB(spi_master_task, (const spi_task_config_t*));
void spi_master_task(const spi_task_config_t* config);

#if defined(__cplusplus) || defined(__XC__)
}
#endif

#endif /* SPI_TASK_H_ */