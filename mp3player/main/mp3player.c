#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "sdkconfig.h"

#include "odroid_display.h"
#include "odroid_sdcard.h"

#include "minimp3_ex.h"

#include "driver/i2s.h"

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

    // Created with audacity, raw wave file, mono, 8 bit, no header
    FILE* s = fopen("/sdcard/tetris.raw", "r");
    char tone;

    puts("Reading buffer");
    // read file in 64 KB Buffer, make sure Buffer is in internal RAM
    int buffersize = 100 * 1024;
    char *soundfile = heap_caps_malloc(buffersize, MALLOC_CAP_DMA);

    // For now just use use external RAM (512 KB)
    //int buffersize = 512 * 1024;
    //char *soundfile = heap_caps_malloc(buffersize, MALLOC_CAP_SPIRAM);

    if (soundfile) {
        tone = fgetc(s);
        int i = 0;
        while (tone != EOF && i < buffersize) {
            soundfile[i++] = tone;
            tone = fgetc(s);
        }

        int i2s_num = 0;
	    i2s_config_t i2s_config = {
            .mode = I2S_MODE_MASTER | I2S_MODE_TX | I2S_MODE_DAC_BUILT_IN ,
            .sample_rate = 16000,
            .bits_per_sample = 8,
	        .communication_format = I2S_COMM_FORMAT_I2S_MSB,
	        .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,
	        .intr_alloc_flags = 0,
	        .dma_buf_count = 8,
	        .dma_buf_len = 64,
	        .use_apll = false,
	    };
	    //install and start i2s driver
	    i2s_driver_install(i2s_num, &i2s_config, 0, NULL);
	    //init DAC pad
	    i2s_set_dac_mode(I2S_DAC_CHANNEL_BOTH_EN);

        i2s_set_clk(0, 44100, 16, 1);
	size_t bytes_written;
        i2s_write(0, soundfile, buffersize, &bytes_written, portMAX_DELAY);

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
    // Stacksize is 8 KB (8192), our Core is 1 which is the APP CPU, which is completely free
    // Core 0 (PRO CPU) is tasked by default with managment (WiFi, bluetooth...)
    xTaskCreatePinnedToCore(&mp3playermain, "mp3player_main", 8192, NULL, 5, NULL, 1);
}

