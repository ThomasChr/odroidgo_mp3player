#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
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
	ili9341_init();
	ili9341_clear(0xffff);
	puts("LCD init done");

	uint16_t* buffer;
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

	/************ MP3LIB ************/
	
	/************ /MP3LIB ************/


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
}

void app_main()
{
	// Stacksize is 8 KB (8192)
    	xTaskCreatePinnedToCore(&mp3playermain, "mp3player_main", 8192, NULL, 5, NULL, 0);
}

