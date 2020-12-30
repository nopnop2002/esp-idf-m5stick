#ifndef MAIN_SH1107_H_
#define MAIN_SH1107_H_

#include "driver/spi_master.h"

#define DIRECTION0		0
#define DIRECTION90		1
#define DIRECTION180	2
#define DIRECTION270	3

typedef struct {
	int _width;
	int _height;
	int _pages;
	int _direction;
	int16_t _dc;
	spi_device_handle_t _SPIHandle;
} SH1107_t;

//void spi_master_init(SH1107_t * dev);
void spi_master_init(SH1107_t * dev, int16_t GPIO_MOSI, int16_t GPIO_SCLK, int16_t GPIO_CS, int16_t GPIO_DC, int16_t GPIO_RESET);
bool spi_master_write_byte(spi_device_handle_t SPIHandle, const uint8_t* Data, size_t DataLength );
bool spi_master_write_command(SH1107_t * dev, uint8_t Command );
bool spi_master_write_data(SH1107_t * dev, const uint8_t* Data, size_t DataLength );
void sh1107_display_init(SH1107_t * dev, int width, int height);
void sh1107_display_text(SH1107_t * dev, int row, int col, char * text, int text_len, bool invert);
void sh1107_display_image(SH1107_t * dev, int page, int seg, uint8_t * images, int width);
void sh1107_clear_screen(SH1107_t * dev, bool invert);
void sh1107_clear_line(SH1107_t * dev, int page, bool invert);
void sh1107_display_contrast(SH1107_t * dev, int contrast);
void sh1107_display_invert(uint8_t *buf, size_t blen);
void sh1107_display_fadeout(SH1107_t * dev);
void sh1107_display_direction(SH1107_t * dev, int dir);
uint8_t rotate_byte(uint8_t ch1);
void sh1107_display_rotate(uint8_t * buf, int dir);

#endif /* MAIN_SH1107_H_ */
