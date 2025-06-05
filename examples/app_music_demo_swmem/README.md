LVGL Table on XCORE.AI
======================

This example application demonstrates how to utilize lib_lvgl to run lv_demo_music with spi in seperate core and large const array in software define memory (QSPI flash) on XK-EVK-XU316 with Waveshare's 1.69inch Touch LCD Module.

## Get Started
1. Configure Project through CMake 
    ```bash
    # if using Windows
    cmake -G Ninja -B build
    # else
    cmake -B build
    ```

2. Build the firmware
    ```bash
    # if using Windows
    ninja -C build
    # else
    xmake -C build
    ```

3. Run the firmware on XK-EVK-XU316 with xrun
    ```bash
    cd bin
    xobjdump --strip app_music_demo_swmem.xe
    xobjdump --split app_music_demo_swmem.xb
    xflash --reverse --write-all image_n0c1.swmem --target XCORE-AI-EXPLORER
    cd ..
    xrun --xscope .\bin\app_music_demo_swmem.xe
    ```

## Hardware Configuration

Connect XK-EVK-XU316 and Waveshare's 1.69inch Touch LCD Module according to the following table.

|XK-EVK-XU316|Touch LCD Module|
|------------|----------------|
|J10 (PWR)|VCC|
|J10 (GND)|GND|
|J10 (36)|LCD_DIN|
|J10 (38)|LCD_CLK|
|J13 (49)|LCD_CS|
|J10 (02)|LCD_DC|
|J10 (03)|LCD_RST|
|J10 (08)|LCD_BL|
|J10 (05)|TP_SDA|
|J10 (04)|TP_SCL|
|J11 (14)|TP_RST|
|NC|TP_IRQ|