# cmake
cmake_minimum_required(VERSION 3.21)
include($ENV{XMOS_CMAKE_PATH}/xcommon.cmake)
project(app_table_demo)

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

  -DLCD_H_RES=240
  -DLCD_V_RES=280

  -DST7789_IO_PORT=XS1_PORT_4A
  -DST7789_DC_POS=0
  -DST7789_RST_POS=1
  -DST7789_BL_POS=2
  -DST7789_CS_PORT=XS1_PORT_32A
  -DST7789_CS_POS=0
  -DST7789_SCK_PORT=XS1_PORT_1O
  -DST7789_MOSI_PORT=XS1_PORT_1M
  -DST7789_CLKBLK=XS1_CLKBLK_1

  -DCST816S_IO_PORT=XS1_PORT_4C
  -DCST816S_RST_POS=0
  -DCST816S_I2C_SCL_PORT=XS1_PORT_4B
  -DCST816S_I2C_SCL_POS=0
  -DCST816S_I2C_SCL_MASK=0b1100
  -DCST816S_I2C_SDA_PORT=XS1_PORT_4B
  -DCST816S_I2C_SDA_POS=1
  -DCST816S_I2C_SDA_MASK=0b1100
)

# Dependencies
set(APP_DEPENDENT_MODULES
    "LeslieXMOS/lib_lvgl"
    "i2c"
    "spi"
)

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