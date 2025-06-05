#include <stdio.h>
#include <print.h>
#include <stdint.h>
#include <platform.h>
#include <assert.h>
#include <xcore/channel.h>
#include <xcore/select.h>
#include "flash_server.h"

#include "fast_flash.h"

#ifndef LOAD_WEIGHTS_MAX_BLOCKS
    #define LOAD_WEIGHTS_MAX_BLOCKS 1
#endif

#define TMP_BUF_SIZE_IN_BYTES  1024

#define VERSION_MAJOR 1
#define VERSION_MINOR 2
#define VERSION_LITTLE_ENDING (VERSION_MAJOR |\
                               (VERSION_MINOR << 8) |\
                               ((VERSION_MAJOR^0xff) << 16) |\
                               ((VERSION_MINOR^0xff) << 24))

static int flash_version_check(fl_QSPIPorts *qspi) {
    uint32_t tmp[2];
    fast_flash_read(qspi, /*unsigned addr*/32, /*unsigned word_count*/1, /*unsigned read_data[]*/(unsigned *)tmp, /*chanend ?c_data_out*/ 0);

    return tmp[0] ^ VERSION_LITTLE_ENDING;
}

static int flash_server_operate(chanend_t c_flash, flash_t *headers, fl_QSPIPorts *qspi) {
    int address, bytes;
    flash_command_t cmd;
    cmd = chan_in_word(c_flash);
    switch (cmd)
    {
    case FLASH_READ_PARAMETERS: {
        // Set not parallel mode
        chan_out_word(c_flash, 0);
        address = chan_in_word(c_flash);
        bytes   = chan_in_word(c_flash);
        address = headers->parameters_start + address;
        fast_flash_read(qspi, address, bytes/4, /*not using this arg*/NULL, c_flash);
        break;
    }
    case FLASH_READ_PARAMETERS_ASYNC: {
        int N;
        int target_address[LOAD_WEIGHTS_MAX_BLOCKS];
        int size_in_bytes[LOAD_WEIGHTS_MAX_BLOCKS];
        address = chan_in_word(c_flash);
        N       = chan_in_word(c_flash);
        assert(N <= LOAD_WEIGHTS_MAX_BLOCKS);
        for(int i = 0; i < N; i++) {
            size_in_bytes[i] = chan_in_word(c_flash);
            target_address[i] = chan_in_word(c_flash);
        }
        address = headers->parameters_start + address;
        for(int i = 0; i < N; i++) {
            fast_flash_read(qspi, address, size_in_bytes[i]/4, (unsigned *)(target_address[i])
                , /*When zero, data is directly written into target address*/0);
            address += size_in_bytes[i];
        }
        chanend_out_end_token(c_flash);
        break;
    }
    case FLASH_READ_XIP: {
        int target_address;
        address = chan_in_word(c_flash);
        bytes =  chan_in_word(c_flash);
        target_address = chan_in_word(c_flash);
        fast_flash_xip(qspi, address, bytes/4, (unsigned *)(target_address), NULL);
        chanend_out_end_token(c_flash);
        break;
    }
    case FLASH_READ_XIP_DIFF_TILE: {
        int target_address;
        address = chan_in_word(c_flash);
        bytes =  chan_in_word(c_flash);
        fast_flash_xip(qspi, address, bytes/4, NULL, c_flash);
        break;
    }
    // case FLASH_SERVER_INIT:
    case FLASH_SERVER_QUIT:
        return 0;
    default:
        break;
    }
    return 1;
}


void flash_server(chanend_t c_flash[], flash_t headers[], int n_flash,
                  fl_QSPIPorts *qspi, fl_QuadDeviceSpec flash_spec[],
                  int n_flash_spec) {
    int res;
    assert((res = fl_connectToDevice(qspi, flash_spec, n_flash_spec)) == 0);
    assert((res = fast_flash_init(qspi)) == 0);
    // assert((res = flash_version_check(qspi)) == 0);
    
    fast_flash_read(qspi, /*unsigned addr*/36, /*unsigned word_count*/(n_flash * sizeof(flash_t))/4, /*unsigned read_data[]*/(unsigned*)headers, /*chanend ?c_data_out*/ 0);
    
    if (n_flash == 1) {
        while(flash_server_operate(c_flash[0], &headers[0], qspi)) {
            ;
        }
    } else if (n_flash == 2) {  // This is a bit unpleasant
        SELECT_RES(
            CASE_THEN(c_flash[0], channel0),
            CASE_THEN(c_flash[1], channel1)
            )
        {
        channel0:
            if (flash_server_operate(c_flash[0], &headers[0], qspi)) {
                SELECT_CONTINUE_NO_RESET;
            } else {
                break;
            }
        channel1:
            if (flash_server_operate(c_flash[1], &headers[1], qspi)) {
                SELECT_CONTINUE_NO_RESET;
            } else {
                break;
            }
        }
    } else {
        assert(n_flash);
    }
}