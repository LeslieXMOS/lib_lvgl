#include "spi_task.h"
#include <stdint.h>
#include "xcore/parallel.h"
#include "xcore/select.h"
#include "xcore/hwtimer.h"
#include "spi.h"
#include <string.h>

#define SPI_BUF_SIZE    256
static uint8_t spi_out_buffer[SPI_BUF_SIZE];     // Buffer used for xfer lpddr data
static uint8_t spi_in_buffer[SPI_BUF_SIZE];     // Buffer used for xfer lpddr data

void spi_task_master_send(chanend_t c_control, uint8_t* data_out, uint8_t* data_in, size_t len, finish_cb_t finish_cb, void* user_data) {
    chan_out_word(c_control, (uint32_t)get_local_tile_id());
    chan_out_word(c_control, (uint32_t)data_out);
    chan_out_word(c_control, (uint32_t)data_in);
    chan_out_word(c_control, len);
    chan_out_word(c_control, (uint32_t)finish_cb);
    chan_out_word(c_control, (uint32_t)user_data);
    if (finish_cb == NULL) {
        chan_in_word(c_control);
    }
}

void spi_master_task(const spi_task_config_t* config) {
    spi_master_t spi_ctx;
    spi_master_device_t spi_dev;
    // Initialize the master device
    spi_master_init(&spi_ctx, config->spi_cb, config->cs_port, config->sck_port, config->mosi_port, 0);
    spi_master_device_init(
        &spi_dev, &spi_ctx,
        config->cs_pin,
        SPI_MODE_0,
        spi_master_source_clock_ref,
        0,
        spi_master_sample_delay_0,
        0, 0 ,0 ,0 );
    
    uint32_t tile_id = get_local_tile_id();

    SELECT_RES(
        CASE_THEN(config->c_control, xfer_handler))
    {
    xfer_handler: {
        uint32_t caller_tile_id = chan_in_word(config->c_control);
        if (caller_tile_id == tile_id) {
            uint8_t* data_out = (uint8_t*)chan_in_word(config->c_control);
            uint8_t* data_in = (uint8_t*)chan_in_word(config->c_control);
            size_t len = chan_in_word(config->c_control);
            __attribute__((fptrgroup("spi_task_callback")))
            finish_cb_t callback = (finish_cb_t)chan_in_word(config->c_control);
            void* user_data = (void*)chan_in_word(config->c_control);
            spi_master_start_transaction(&spi_dev);
            if ((uint32_t)data_out > XS1_EXTMEM_BASE && (uint32_t)data_out < XS1_EXTMEM_BASE+XS1_EXTMEM_SIZE) {
                for(int i = 0; i < len; i+=SPI_BUF_SIZE) {
                    size_t new_size = (len-i) <= SPI_BUF_SIZE ? len-i : SPI_BUF_SIZE;
                    memcpy(spi_out_buffer, &data_out[i], new_size);
                    spi_master_transfer(&spi_dev, spi_out_buffer, spi_in_buffer, new_size);
                    if (data_in)
                        memcpy(&data_in[i], spi_in_buffer, new_size);
                }
            } else {
                spi_master_transfer(&spi_dev, data_out, data_in, len);
            }
            spi_master_end_transaction(&spi_dev);
            if (callback) {
                callback(data_in, len, user_data);
            } else {
                chan_out_word(config->c_control, 1);
            }
        }
        continue;
    }
    }
}