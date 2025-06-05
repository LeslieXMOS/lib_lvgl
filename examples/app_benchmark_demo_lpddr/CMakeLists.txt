# cmake
cmake_minimum_required(VERSION 3.21)
include($ENV{XMOS_CMAKE_PATH}/xcommon.cmake)
project(app_benchmark_demo_lpddr)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# root
set(XMOS_SANDBOX_DIR ${CMAKE_CURRENT_LIST_DIR}/../../../libraries)

# target
set(APP_HW_TARGET XCORE-AI-EXPLORER)
set(APP_XSCOPE_SRCS basic.xscope)

# includes
list(APPEND APP_INCLUDES src)

# flags
list(APPEND APP_COMPILER_FLAGS
  -Os
  -g
  -report
  -Wall
  -fxscope
  -mcmodel=large
  -Wno-xcore-fptrgroup
  -lquadflash
)

# Dependencies
set(APP_DEPENDENT_MODULES
    "lib_lvgl"
    "i2c"
    "spi"
)

set(XMOS_DEP_DIR_lib_lvgl ${CMAKE_CURRENT_LIST_DIR}/../../)

# I2C and SPI
set(XMOS_DEP_DIR_i2c ${XMOS_SANDBOX_DIR}/fwk_io/modules)
set(XMOS_DEP_DIR_spi ${XMOS_SANDBOX_DIR}/fwk_io/modules)
if(NOT EXISTS ${XMOS_SANDBOX_DIR}/fwk_io)
    include(FetchContent)
    FetchContent_Declare(
        fwk_io
        GIT_REPOSITORY git@github.com:xmos/fwk_io
        GIT_TAG feature/xcommon_cmake
        SOURCE_DIR ${XMOS_SANDBOX_DIR}/fwk_io
    )
    FetchContent_Populate(fwk_io)
endif()

XMOS_REGISTER_APP()