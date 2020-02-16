#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

#include "sh1107.h"
#include "font8x8_basic.h"

#define tag "SH1107"

void app_main(void)
{
	SH1107_t dev;
	int center, bottom;
	char lineChar[20];

	spi_master_init(&dev);
	spi_init(&dev, 64, 128);

	clear_screen(&dev, false);
	display_contrast(&dev, 0xff);
	display_text(&dev, 0, "M5 Stick", 8, false);
	display_text(&dev, 1, "64x128  ", 8, false);
	display_text(&dev, 2, "ABCDEFGH", 8, false);
	display_text(&dev, 3, "abcdefgh", 8, false);
	display_text(&dev, 4, "01234567", 8, false);
	display_text(&dev, 5, "Hello   ", 8, false);
	display_text(&dev, 6, "World!! ", 8, false);

	display_text(&dev, 8, "M5 Stick", 8, true);
	display_text(&dev, 9, "64x128  ", 8, true);
	display_text(&dev, 10, "ABCDEFGH", 8, true);
	display_text(&dev, 11, "abcdefgh", 8, true);
	display_text(&dev, 12, "01234567", 8, true);
	display_text(&dev, 13, "Hello   ", 8, true);
	display_text(&dev, 14, "World!! ", 8, true);
	clear_line(&dev, 15, true);
	vTaskDelay(3000 / portTICK_PERIOD_MS);
	
	// Display Count Down
	uint8_t image[24];
	memset(image, 0, sizeof(image));
	center = 7;
	display_image(&dev, center-1, 20, image, sizeof(image));
	display_image(&dev, center, 20, image, sizeof(image));
	display_image(&dev, center+1, 20, image, sizeof(image));
	for(int font=0x39;font>0x30;font--) {
		memset(image, 0, sizeof(image));
		display_image(&dev, center, 28, image, 8);
		memcpy(image, font8x8_basic_tr[font], 8);
		display_image(&dev, center, 28, image, 8);
		vTaskDelay(1000 / portTICK_PERIOD_MS);
	}

	// Scroll Up
	bottom = 16;
	clear_screen(&dev, false);
	display_contrast(&dev, 0xff);
	display_text(&dev, 0, "ScrollUP", 8, true);
	software_scroll(&dev, 15, 1);
	for (int line=0;line<bottom+10;line++) {
		lineChar[0] = 0x01;
		sprintf(&lineChar[1], "Line %02d", line);
		scroll_text(&dev, lineChar, strlen(lineChar), false);
		vTaskDelay(500 / portTICK_PERIOD_MS);
	}
	vTaskDelay(3000 / portTICK_PERIOD_MS);

	// Scroll Down
	clear_screen(&dev, false);
	display_contrast(&dev, 0xff);
	display_text(&dev, 0, "ScrollDN", 8, true);
	software_scroll(&dev, 1, 15);
	for (int line=0;line<bottom+10;line++) {
		lineChar[0] = 0x02;
		sprintf(&lineChar[1], "Line %02d", line);
		scroll_text(&dev, lineChar, strlen(lineChar), false);
		vTaskDelay(500 / portTICK_PERIOD_MS);
	}
	vTaskDelay(3000 / portTICK_PERIOD_MS);

	// Page Down
	clear_screen(&dev, false);
	display_contrast(&dev, 0xff);
	display_text(&dev, 0, "PageDOWN", 8, true);
	software_scroll(&dev, 1, 15);
	for (int line=0;line<bottom+10;line++) {
		if ( (line % 15) == 0) scroll_clear(&dev);
		lineChar[0] = 0x02;
		sprintf(&lineChar[1], "Line %02d", line);
		scroll_text(&dev, lineChar, strlen(lineChar), false);
		vTaskDelay(500 / portTICK_PERIOD_MS);
	}
	vTaskDelay(3000 / portTICK_PERIOD_MS);
	
	// Invert
	clear_screen(&dev, true);
	display_contrast(&dev, 0xff);
	display_text(&dev, center, "  Good  ", 8, true);
	display_text(&dev, center+1, "  Bye!! ", 8, true);
	vTaskDelay(5000 / portTICK_PERIOD_MS);

	// Fade Out
	display_fadeout(&dev);
	
#if 0
	// Fade Out
	for(int contrast=0xff;contrast>0;contrast=contrast-0x20) {
		display_contrast(&dev, contrast);
		vTaskDelay(40);
	}
#endif
}
