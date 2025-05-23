LVGL Table on XCORE.AI
======================

This example application demonstrates how to utilize lib_lvgl to run lv_example_anim_3 with spi in seperate core on XK-EVK-XU316 with Waveshare's 1.69inch Touch LCD Module.

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

3. Run the firmware on XK-EVK-XU316
    ```bash
    xrun --xscope .\bin\app_cubic_bezier_spi_task.xe
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