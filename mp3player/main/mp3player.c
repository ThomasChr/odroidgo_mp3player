#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "sdkconfig.h"

#define LED 2

void mp3playermain()
{
	printf("Blink online!");
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

