#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

#include "sh1107.h"
//#include "font8x8_basic.h"

#define tag "SH1107"

#define GPIO_MOSI  23
#define GPIO_SCLK  18
#define GPIO_CS    14
#define GPIO_DC    27
#define GPIO_RESET 33

void app_main(void)
{
	SH1107_t dev;
	spi_master_init(&dev, GPIO_MOSI, GPIO_SCLK, GPIO_CS, GPIO_DC, GPIO_RESET);
	sh1107_display_init(&dev, 64, 128);
	sh1107_display_contrast(&dev, 0xff);
	
	for (int i=0;i<2;i++) {
		sh1107_clear_screen(&dev, false);
		if (i == 0) sh1107_display_direction(&dev, DIRECTION0);
		if (i == 1) sh1107_display_direction(&dev, DIRECTION180);
		sh1107_display_text(&dev, 0, 0, "M5 Stick", 8, false);
		sh1107_display_text(&dev, 1, 0, "64x128  ", 8, false);
		sh1107_display_text(&dev, 2, 0, "ABCDEFGH", 8, false);
		sh1107_display_text(&dev, 3, 0, "abcdefgh", 8, false);
		sh1107_display_text(&dev, 4, 0, "01234567", 8, false);
		sh1107_display_text(&dev, 5, 0, "Hello   ", 8, false);
		sh1107_display_text(&dev, 6, 0, "World!! ", 8, false);

		sh1107_display_text(&dev, 8, 0, "M5 Stick", 8, true);
		sh1107_display_text(&dev, 9, 0, "64x128  ", 8, true);
		sh1107_display_text(&dev, 10, 0, "ABCDEFGH", 8, true);
		sh1107_display_text(&dev, 11, 0, "abcdefgh", 8, true);
		sh1107_display_text(&dev, 12, 0, "01234567", 8, true);
		sh1107_display_text(&dev, 13, 0, "Hello   ", 8, true);
		sh1107_display_text(&dev, 14, 0, "World!! ", 8, true);
		vTaskDelay(5000 / portTICK_PERIOD_MS);
	}

	for (int i=0;i<2;i++) {
		sh1107_clear_screen(&dev, false);
		if (i == 0) sh1107_display_direction(&dev, DIRECTION90);
		if (i == 1) sh1107_display_direction(&dev, DIRECTION270);
		sh1107_display_text(&dev, 0, 0, "M5 Stick", 8, false);
		sh1107_display_text(&dev, 1, 0, "64x128  ", 8, false);
		sh1107_display_text(&dev, 2, 0, "ABCDEFGH", 8, false);
		sh1107_display_text(&dev, 3, 0, "abcdefgh", 8, false);
		sh1107_display_text(&dev, 4, 0, "01234567", 8, false);
		sh1107_display_text(&dev, 5, 0, "Hello   ", 8, false);
		sh1107_display_text(&dev, 6, 0, "World!! ", 8, false);


		sh1107_display_text(&dev, 0, 8, "M5 Stick", 8, true);
		sh1107_display_text(&dev, 1, 8, "64x128  ", 8, true);
		sh1107_display_text(&dev, 2, 8, "ABCDEFGH", 8, true);
		sh1107_display_text(&dev, 3, 8, "abcdefgh", 8, true);
		sh1107_display_text(&dev, 4, 8, "01234567", 8, true);
		sh1107_display_text(&dev, 5, 8, "Hello   ", 8, true);
		sh1107_display_text(&dev, 6, 8, "World!! ", 8, true);

		vTaskDelay(5000 / portTICK_PERIOD_MS);
	}

	// Invert
	sh1107_clear_screen(&dev, true);
	sh1107_display_direction(&dev, DIRECTION0);
	sh1107_display_contrast(&dev, 0xff);
	int center = 7;
	sh1107_display_text(&dev, center, 0, "  Good  ", 8, true);
	sh1107_display_text(&dev, center+1, 0, "  Bye!! ", 8, true);
	vTaskDelay(5000 / portTICK_PERIOD_MS);

#if 1
	// Fade Out
	sh1107_display_fadeout(&dev);
#endif
	
#if 0
	// Contrast Change
	for(int contrast=0xff;contrast>0;contrast=contrast-0x20) {
		sh1107_display_contrast(&dev, contrast);
		vTaskDelay(40);
	}
#endif

	// Restart module
#if 0
	for (int i = 10; i >= 0; i--) {
		printf("Restarting in %d seconds...\n", i);
		vTaskDelay(1000 / portTICK_PERIOD_MS);
	}
	printf("Restarting now.\n");
	fflush(stdout);
#endif
	esp_restart();
}
