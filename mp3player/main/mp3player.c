#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "driver/dac.h"
#include "sdkconfig.h"

#include "odroid_display.h"
#include "odroid_sdcard.h"

#include "minimp3_ex.h"

#define LED 2

void mp3playermain()
{
    /************ SDCARD ************/
    odroid_sdcard_open("/sdcard");
    puts("SDCard init done");

    FILE* f = fopen("/sdcard/test.txt", "r");
    if (f != NULL) {
        puts("Reading test.txt:");
        char line[64];
        fgets(line, sizeof(line), f);
        fclose(f);
        puts(line);
    } else {
        puts("File test.txt not found");
    }
    /************ /SDCARD ************/

    /************ DISPLAY ************/
    uint16_t* buffer;

    /* odroid_display.c */
    ili9341_init();
    ili9341_clear(0xffff);
    puts("Display init done");

    // 320 x 240 Pixel, 16bit each, Color: RRRRRGGG GGGBBBBB (RGB565)
    buffer = malloc(320*240*2);
    memset(buffer, 0, 320*240*2);

    puts("Drawing");
    // blue rectangle, 50x50 pixel, left upper corner
    for (short x = 0; x < 50; x++) {
        for (short y = 0; y < 50; y++) {
            buffer[x*320+y] = 0xFF00;
        }
    }

    ili9341_write_frame(buffer);
    puts("Drawing done");
    /************ /DISPLAY ************/

    /************ SOUND ************/
    puts("Preparing sound");
    dac_channel_t dac_channel1 = DAC_CHANNEL_1;
    dac_channel_t dac_channel2 = DAC_CHANNEL_2;
    gpio_num_t dac_gpio1 = 25;
    gpio_num_t dac_gpio2 = 26;

    gpio_pad_select_gpio(dac_gpio1);
    gpio_set_direction(dac_gpio1, GPIO_MODE_OUTPUT);
    gpio_set_level(dac_gpio1, 1);

    gpio_pad_select_gpio(dac_gpio2);
    gpio_set_direction(dac_gpio2, GPIO_MODE_OUTPUT);
    gpio_set_level(dac_gpio2, 1);

    dac_output_enable(dac_channel1);
    dac_output_enable(dac_channel2);

    // differential DAC, we set DAC2 to 0 volt:
    dac_output_voltage(dac_channel2, 0);

    // Created with audacity, raw wave file, mono, 8 bit, no header
    FILE* s = fopen("/sdcard/tetris.raw", "r");
    char tone;

    puts("Reading buffer");
    // read file in 64 KB Buffer, make sure Buffer is in internal RAM
    //int buffersize = 64 * 1024;
    //char *soundfile = heap_caps_malloc(buffersize, MALLOC_CAP_DMA);

    // For now just use use external RAM
    int buffersize = 2 * 1024 * 1024;
    char *soundfile = heap_caps_malloc(buffersize, MALLOC_CAP_SPIRAM);

    if (soundfile) {
        tone = fgetc(s);
        int i = 0;
        while (tone != EOF && i < buffersize) {
            soundfile[i++] = tone;
            tone = fgetc(s);
        }

        puts("Starting sound");
        for (int i; i <= buffersize; i++) {
            dac_output_voltage(dac_channel1, soundfile[i]);

            // delay
            int sleep = 0;
            while (sleep <= 255) {
                sleep++;
            }
        }
    } else {
        puts("Could not allocate buffer for sound");
    }
    puts("Sound done");
    /************ /SOUND ************/

    /************ MP3LIB ************/

    /************ /MP3LIB ************/

    /************ BLINK ************/
    puts("Starting blink");
    // make LED Output
    gpio_pad_select_gpio(LED);
    gpio_set_direction(LED, GPIO_MODE_OUTPUT);

    while (1) {
            gpio_set_level(LED, 0);
            vTaskDelay(1000 / portTICK_PERIOD_MS);
            gpio_set_level(LED, 1);
            vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
    /************ /BLINK ************/
}

void app_main()
{
    // Stacksize is 8 KB (8192), Core is 1
    xTaskCreatePinnedToCore(&mp3playermain, "mp3player_main", 8192, NULL, 5, NULL, 1);
}

