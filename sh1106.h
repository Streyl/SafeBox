  
#ifndef _SH1106_H_
#define _SH1106_H_

#include <avr/pgmspace.h>

#define VCCSTATE SH1106_SWITCHCAPVCC
#define WIDTH   128
#define HEIGHT   64
#define NUM_PAGE  8


#define OLED_RST  46 
#define OLED_DC   47
#define OLED_CS  53
#define SPI_MOSI 51     /* connect to the DIN pin of OLED */
#define SPI_SCK  52     /* connect to the CLK pin of OLED */

void SH1106_begin();
void SH1106_display(uint8_t* buffer);
void SH1106_clear(uint8_t* buffer);
void SH1106_pixel(int x,int y,char color, uint8_t* buffer);
void SH1106_bitmap(uint8_t x,uint8_t y,const uint8_t *pBmp, uint8_t chWidth, uint8_t chHeight, uint8_t* buffer);
void SH1106_char1616(uint8_t x,uint8_t y,uint8_t chChar, uint8_t* buffer);
void SH1106_char3216(uint8_t x, uint8_t y, uint8_t chChar, uint8_t* buffer);
void SH1106_string(uint8_t x, uint8_t y, const char *pString, uint8_t Size, uint8_t Mode, uint8_t* buffer);
void SPIWrite(uint8_t *buffer, int bufferLength);
void command(uint8_t cmd);

extern const uint8_t Font1612[11][32];
extern const uint8_t Font3216[11][64];
extern const uint8_t Signal816[16];
extern const uint8_t Msg816[16];
extern const uint8_t Bat816[16];
extern const uint8_t Bluetooth88[8];
extern const uint8_t GPRS88[8];
extern const uint8_t Alarm88[8];
extern const uint8_t Waveshare12864[1024];


#endif
