#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include <driver/spi_master.h>
#include <driver/gpio.h>
#include "esp_log.h"

#include "sh1107.h"
#include "font8x8_basic.h"

#define TAG "SH1107"

#if 0
static const int GPIO_MOSI = 23;
static const int GPIO_SCLK = 18;
static const int GPIO_CS   = 14;
static const int GPIO_DC   = 27;
static const int GPIO_RESET= 33;
#endif

static const int SPI_Command_Mode = 0;
static const int SPI_Data_Mode = 1;
//static const int SPI_Frequency = 1000000;
static const int SPI_Frequency = 8000000;

void spi_master_init(SH1107_t * dev, int16_t GPIO_MOSI, int16_t GPIO_SCLK, int16_t GPIO_CS, int16_t GPIO_DC, int16_t GPIO_RESET)
{
	esp_err_t ret;
	ESP_LOGI(TAG, "GPIO_MOSI=%d", GPIO_MOSI);
	ESP_LOGI(TAG, "GPIO_SCLK=%d", GPIO_SCLK);
	ESP_LOGI(TAG, "GPIO_CS=%d", GPIO_CS);
	ESP_LOGI(TAG, "GPIO_DC=%d", GPIO_DC);
	ESP_LOGI(TAG, "GPIO_RESET=%d", GPIO_RESET);

	if (GPIO_CS >= 0) {
		ret = gpio_set_direction( GPIO_CS, GPIO_MODE_OUTPUT );
		ESP_LOGI(TAG, "gpio_set_direction(GPIO_CS)=%d",ret);
		assert(ret==ESP_OK);
		gpio_set_level( GPIO_CS, 1 );
	}

	if (GPIO_DC >= 0) {
		ret = gpio_set_direction( GPIO_DC, GPIO_MODE_OUTPUT );
		ESP_LOGI(TAG, "gpio_set_direction(GPIO_DC)=%d",ret);
		assert(ret==ESP_OK);
		gpio_set_level( GPIO_DC, 0 );
	}

	if (GPIO_RESET >= 0) {
		ret = gpio_set_direction( GPIO_RESET, GPIO_MODE_OUTPUT );
		ESP_LOGI(TAG, "gpio_set_direction(GPIO_RESET)=%d",ret);
		assert(ret==ESP_OK);
		gpio_set_level( GPIO_RESET, 0 );
		vTaskDelay( pdMS_TO_TICKS( 100 ) );
		gpio_set_level( GPIO_RESET, 1 );
	}

	spi_bus_config_t spi_bus_config = {
		.sclk_io_num = GPIO_SCLK,
		.mosi_io_num = GPIO_MOSI,
		.miso_io_num = -1,
		.quadwp_io_num = -1,
		.quadhd_io_num = -1,
	};

	ret = spi_bus_initialize( HSPI_HOST, &spi_bus_config, 1 );
	ESP_LOGI(TAG, "spi_bus_initialize=%d",ret);
	assert(ret==ESP_OK);

	spi_device_interface_config_t devcfg;
	memset( &devcfg, 0, sizeof( spi_device_interface_config_t ) );
	devcfg.clock_speed_hz = SPI_Frequency;
	devcfg.spics_io_num = GPIO_CS;
	devcfg.queue_size = 1;

	spi_device_handle_t handle;
	ret = spi_bus_add_device( HSPI_HOST, &devcfg, &handle);
	ESP_LOGI(TAG, "spi_bus_add_device=%d",ret);
	assert(ret==ESP_OK);
	dev->_dc = GPIO_DC;
	dev->_SPIHandle = handle;
}


bool spi_master_write_byte(spi_device_handle_t SPIHandle, const uint8_t* Data, size_t DataLength )
{
	spi_transaction_t SPITransaction;
	esp_err_t ret;

	if ( DataLength > 0 ) {
		memset( &SPITransaction, 0, sizeof( spi_transaction_t ) );
		SPITransaction.length = DataLength * 8;
		SPITransaction.tx_buffer = Data;
		ret = spi_device_transmit( SPIHandle, &SPITransaction );
		//ESP_LOGI(TAG, "spi_device_transmit=%d",ret);
		assert(ret==ESP_OK);
	}

	return true;
}

bool spi_master_write_command(SH1107_t * dev, uint8_t Command )
{
	//ESP_LOGI(TAG, "spi_master_write_command 0x%x",Command);
	static uint8_t CommandByte = 0;
	CommandByte = Command;
	//gpio_set_level( GPIO_DC, SPI_Command_Mode );
	gpio_set_level( dev->_dc, SPI_Command_Mode );
	return spi_master_write_byte( dev->_SPIHandle, &CommandByte, 1 );
}

bool spi_master_write_data(SH1107_t * dev, const uint8_t* Data, size_t DataLength )
{
	//ESP_LOGI(TAG, "spi_master_write_data 0x%x",Data[0]);
	//gpio_set_level( GPIO_DC, SPI_Data_Mode );
	gpio_set_level( dev->_dc, SPI_Data_Mode );
	return spi_master_write_byte( dev->_SPIHandle, Data, DataLength );
}

void sh1107_display_init(SH1107_t * dev, int width, int height)
{
	dev->_width = width;
	dev->_height = height;
	dev->_pages = height / 8;
	dev->_direction = DIRECTION0;

	spi_master_write_command(dev, 0xAE);	// Turn display off
	spi_master_write_command(dev, 0xDC);	// Set display start line
	spi_master_write_command(dev, 0x00);	// ...value
	spi_master_write_command(dev, 0x81);	// Set display contrast
	spi_master_write_command(dev, 0x2F);	// ...value
	spi_master_write_command(dev, 0x20);	// Set memory mode
	spi_master_write_command(dev, 0xA0);	// Non-rotated display
	spi_master_write_command(dev, 0xC0);	// Non-flipped vertical
	spi_master_write_command(dev, 0xA8);	// Set multiplex ratio
	spi_master_write_command(dev, 0x7F);	// ...value
	spi_master_write_command(dev, 0xD3);	// Set display offset to zero
	spi_master_write_command(dev, 0x60);	// ...value
	spi_master_write_command(dev, 0xD5);	// Set display clock divider
	spi_master_write_command(dev, 0x51);	// ...value
	spi_master_write_command(dev, 0xD9);	// Set pre-charge
	spi_master_write_command(dev, 0x22);	// ...value
	spi_master_write_command(dev, 0xDB);	// Set com detect
	spi_master_write_command(dev, 0x35);	// ...value
	spi_master_write_command(dev, 0xB0);	// Set page address
	spi_master_write_command(dev, 0xDA);	// Set com pins
	spi_master_write_command(dev, 0x12);	// ...value
	spi_master_write_command(dev, 0xA4);	// output ram to display
	spi_master_write_command(dev, 0xA6);	// Non-inverted display
//	spi_master_write_command(dev, 0xA7);	// Inverted display
	spi_master_write_command(dev, 0xAF);	// Turn display on
}

void sh1107_display_text(SH1107_t * dev, int row, int col, char * text, int text_len, bool invert)
{
	int _length = text_len;
	int _width = 0;
	int _height = 0;
	int _row = 0;
	int _col = 0;
	int _rowadd = 0;
	int _coladd = 0;
	if (dev->_direction == DIRECTION0) {
		_width = dev->_width / 8;
		_height = dev->_height / 8;
		_row = row;
		_col = col*8;
		_rowadd = 0;
		_coladd = 8;
	} else if (dev->_direction == DIRECTION90) {
		_width = dev->_height / 8;
		_height = dev->_width / 8;
		_row = col;
		_col = (dev->_width-8) - (row*8);
		_rowadd = 1;
		_coladd = 0;
	} else if (dev->_direction == DIRECTION180) {
		_width = dev->_width / 8;
		_height = dev->_height / 8;
		_row = _height - row - 1;
		_col = (dev->_width-8) - (col*8);
		_rowadd = 0;
		_coladd = -8;
	} else if (dev->_direction == DIRECTION270) {
		_width = dev->_height / 8;
		_height = dev->_width / 8;
		_row = (dev->_pages-1) - col;
		_col = row*8;
		_rowadd = -1;
		_coladd = 0;
	}
	if (row >= _height) return;
	if (col >= _width) return;
	if (col + text_len > _width) _length = _width - col;
	ESP_LOGD(TAG, "_direction=%d _width=%d _height=%d _length=%d _row=%d _col=%d", 
		dev->_direction, _width, _height, _length, _row, _col);

	//uint8_t seg = 0;
	uint8_t image[8];
	for (int i=0; i<_length; i++) {
		memcpy(image, font8x8_basic_tr[(uint8_t)text[i]], 8);
		if (invert) sh1107_display_invert(image, 8);
		sh1107_display_rotate(image, dev->_direction);
		sh1107_display_image(dev, _row, _col, image, 8);
		_row = _row + _rowadd;
		_col = _col + _coladd;
		//seg = seg + 8;
	}
}

void sh1107_display_image(SH1107_t * dev, int page, int col, uint8_t * images, int width)
{
	if (page >= dev->_pages) return;
	if (col >= dev->_width) return;

	uint8_t columLow = col & 0x0F;
	uint8_t columHigh = (col >> 4) & 0x0F;
	//ESP_LOGI(TAG, "page=%x columLow=%x columHigh=%x",page,columLow,columHigh);

	// Set Higher Column Start Address for Page Addressing Mode
	spi_master_write_command(dev, (0x10 + columHigh));
	// Set Lower Column Start Address for Page Addressing Mode
	spi_master_write_command(dev, (0x00 + columLow));
	// Set Page Start Address for Page Addressing Mode
	spi_master_write_command(dev, 0xB0 | page);

	spi_master_write_data(dev, images, width);

}

void sh1107_clear_screen(SH1107_t * dev, bool invert)
{
	uint8_t zero[64];
	if (invert) {
		memset(zero, 0xff, sizeof(zero));
	} else {
		memset(zero, 0x00, sizeof(zero));
	}
	for (int page = 0; page < dev->_pages; page++) {
		sh1107_display_image(dev, page, 0, zero, dev->_width);
	}
}

void sh1107_clear_line(SH1107_t * dev, int row, bool invert)
{
	char space[1];
	space[0] = 0x20;
	if (dev->_direction == DIRECTION0 || dev->_direction == DIRECTION90) {
		int _width = dev->_width / 8;
		for (int col=0;col<_width;col++) {
			sh1107_display_text(dev, row, col, space, 1, invert);
		}
	} else {
		int _width = dev->_height / 8;
		for (int col=0;col<_width;col++) {
			sh1107_display_text(dev, row, col, space, 1, invert);
		}
	}
}

void sh1107_display_contrast(SH1107_t * dev, int contrast) {
	int _contrast = contrast;
	if (contrast < 0x0) _contrast = 0;
	if (contrast > 0xFF) _contrast = 0xFF;

	spi_master_write_command(dev, 0x81);
	spi_master_write_command(dev, _contrast);
}

void sh1107_display_invert(uint8_t *buf, size_t blen)
{
	uint8_t wk;
	for(int i=0; i<blen; i++){
		wk = buf[i];
		buf[i] = ~wk;
	}
}


void sh1107_display_fadeout(SH1107_t * dev)
{
	uint8_t image[1];
	for(int page=0; page<dev->_pages; page++) {
		image[0] = 0xFF;
		for(int line=0; line<8; line++) {
			image[0] = image[0] << 1;
			for(int seg=0; seg<dev->_width; seg++) {
				sh1107_display_image(dev, page, seg, image, 1);
			}
		}
	}
}

void sh1107_display_direction(SH1107_t * dev, int dir) {
	dev->_direction = dir;
}

uint8_t rotate_byte(uint8_t ch1) {
	uint8_t ch2 = 0;
	for (int j=0;j<8;j++) {
		ch2 = (ch2 << 1) + (ch1 & 0x01);
		ch1 = ch1 >> 1;
	}
	return ch2;
}

void sh1107_display_rotate(uint8_t * buf, int dir)
{
	uint8_t wk[8];
	if (dir == DIRECTION0) return;
	for(int i=0; i<8; i++){
		wk[i] = buf[i];
		buf[i] = 0;
	}
	if (dir == DIRECTION90 || dir == DIRECTION270) {
		uint8_t wk2[8];
		uint8_t mask1 = 0x80;
		for(int i=0;i<8;i++) {
			wk2[i] = 0;
			ESP_LOGD(TAG, "wk[%d]=%x", i, wk[i]);
			uint8_t mask2 = 0x01;
			for(int j=0;j<8;j++) {
				if( (wk[j] & mask1) == mask1) wk2[i] = wk2[i] + mask2;
				mask2 = mask2 << 1;
			}
			mask1 = mask1 >> 1;
		}
		for(int i=0; i<8; i++){
			ESP_LOGD(TAG, "wk2[%d]=%x", i, wk2[i]);
		}

		for(int i=0;i<8;i++) {
			if (dir == DIRECTION90) {
				buf[i] = wk2[i];
			} else {
				buf[i] = rotate_byte(wk2[7-i]);
			}
		}
	} else if (dir == DIRECTION180) {
		for(int i=0;i<8;i++) {
			buf[i] = rotate_byte(wk[7-i]);
		}

	}
	return;

}

