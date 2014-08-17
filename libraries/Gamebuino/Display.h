/*
 * (C) Copyright 2014 Aurélien Rodot. All rights reserved.
 *
 * This file is part of the Gamebuino Library (http://gamebuino.com)
 *
 * The Gamebuino Library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>
 */

#ifndef DISPLAY_H
#define	DISPLAY_H

#include <Arduino.h>
#include <avr/pgmspace.h>
#include <SPI.h>

#include "settings.c"

//colors
#define BLACK 1
#define WHITE 0

//for extended bitmap function :
#define NOROT 0
#define ROTCCW 1
#define ROT180 2
#define ROTCW 3
#define NOFLIP 0
#define FLIPH 1
#define FLIPV 2
#define FLIPVH 3

#if (DISPLAY_ROT == NO_ROT)||(DISPLAY_ROT == ROT180) //landscape mode
	#define LCDWIDTH 84
	#define LCDHEIGHT 48
#else //portrait mode
	#define LCDWIDTH 48
	#define LCDHEIGHT 84
#endif
#define LCDHEIGHT_NOROT 48
#define LCDWIDTH_NOROT 84
	
/*#if TINY_FONT == 0
	#include "font5x7.c"
	#define FONTWIDTH 6
	#define FONTHEIGHT 8
#else
	#include "font3x5.c"
	#define FONTWIDTH 4
	#define FONTHEIGHT 6
#endif*/

#define swap(a, b) { int8_t t = a; a = b; b = t; }

#define PCD8544_POWERDOWN 0x04
#define PCD8544_ENTRYMODE 0x02
#define PCD8544_EXTENDEDINSTRUCTION 0x01

#define PCD8544_DISPLAYBLANK 0x0
#define PCD8544_DISPLAYNORMAL 0x4
#define PCD8544_DISPLAYALLON 0x1
#define PCD8544_DISPLAYINVERTED 0x5

// H = 0
#define PCD8544_FUNCTIONSET 0x20
#define PCD8544_DISPLAYCONTROL 0x08
#define PCD8544_SETYADDR 0x40
#define PCD8544_SETXADDR 0x80
#define PCD8544_SETXADDR 0x80

// H = 1
#define PCD8544_SETTEMP 0x04
#define PCD8544_SETBIAS 0x10
#define PCD8544_SETVOP 0x80

extern uint8_t _displayBuffer[];

class Display : public Print {
public:
    void begin(int8_t SCLK, int8_t DIN, int8_t DC, int8_t CS, int8_t RST);

    void command(uint8_t c);
    void data(uint8_t c);

    void setContrast(uint8_t val);
    void clear(void);
    void update();

	void setColor(int8_t c);
	void setColor(int8_t c, int8_t bg);
    inline void drawPixel(int8_t x, int8_t y);
    inline uint8_t getPixel(int8_t x, int8_t y);

    void drawLine(int8_t x0, int8_t y0, int8_t x1, int8_t y1);
    void drawFastVLine(int8_t x, int8_t y, int8_t h);
    void drawFastHLine(int8_t x, int8_t y, int8_t w);
    void drawRect(int8_t x, int8_t y, int8_t w, int8_t h);
    void fillRect(int8_t x, int8_t y, int8_t w, int8_t h);
    void fillScreen(uint8_t color);

    void drawCircle(int8_t x0, int8_t y0, int8_t r);
    void drawCircleHelper(int8_t x0, int8_t y0, int8_t r, uint8_t cornername);
    void fillCircle(int8_t x0, int8_t y0, int8_t r);
    void fillCircleHelper(int8_t x0, int8_t y0, int8_t r, uint8_t cornername, int8_t delta);

    void drawTriangle(int8_t x0, int8_t y0, int8_t x1, int8_t y1, int8_t x2, int8_t y2);
    void fillTriangle(int8_t x0, int8_t y0, int8_t x1, int8_t y1, int8_t x2, int8_t y2);
    void drawRoundRect(int8_t x0, int8_t y0, int8_t w, int8_t h, int8_t radius);
    void fillRoundRect(int8_t x0, int8_t y0, int8_t w, int8_t h, int8_t radius);
    
    void drawBitmap(int8_t x, int8_t y, const uint8_t *bitmap);
    void drawBitmap(int8_t x, int8_t y, const uint8_t *bitmap, uint8_t rotation, uint8_t flip);
	
	void setFont(const uint8_t* f);
	uint8_t fontWidth, fontHeight;
    void drawChar(int8_t x, int8_t y, unsigned char c, uint8_t size);

    virtual size_t write(uint8_t);
	
	boolean persistence; //disable clean() at each frame if true
    boolean textWrap; // If set, 'wrap' text at right edge of 
    uint8_t fontSize;
    int8_t cursorX, cursorY;
	byte contrast;

private:
    int8_t sclk, din, dc, cs, rst;
    volatile uint8_t *mosiport, *clkport, *csport, *dcport;
    uint8_t mosipinmask, clkpinmask, cspinmask, dcpinmask;
	
	uint8_t *font;
	uint8_t color, bgcolor;
};

inline void Display::drawPixel(int8_t x, int8_t y) {
    if ((x < 0) || (x >= LCDWIDTH) || (y < 0) || (y >= LCDHEIGHT))
        return;

#if DISPLAY_ROT == NOROT
    if (color)
        _displayBuffer[x + (y / 8) * LCDWIDTH_NOROT] |= _BV(y % 8);
    else
        _displayBuffer[x + (y / 8) * LCDWIDTH_NOROT] &= ~_BV(y % 8);
#elif DISPLAY_ROT == ROTCCW
	if (color)
        _displayBuffer[LCDHEIGHT - y - 1 + (x / 8) * LCDWIDTH_NOROT] |= _BV(x % 8);
    else
        _displayBuffer[LCDHEIGHT - y - 1 + (x / 8) * LCDWIDTH_NOROT] &= ~_BV(x % 8);
#elif DISPLAY_ROT == ROT180
	if (color)
        _displayBuffer[LCDWIDTH - x - 1 + ((LCDHEIGHT - y - 1) / 8) * LCDWIDTH_NOROT] |= _BV((LCDHEIGHT - y - 1) % 8);
    else
        _displayBuffer[LCDWIDTH - x - 1 + ((LCDHEIGHT - y - 1) / 8) * LCDWIDTH_NOROT] &= ~_BV((LCDHEIGHT - y - 1) % 8);
#elif DISPLAY_ROT == ROTCW
	if (color)
        _displayBuffer[y + ((LCDWIDTH - x - 1) / 8) * LCDWIDTH_NOROT] |= _BV((LCDWIDTH - x -1) % 8);
    else
        _displayBuffer[y + ((LCDWIDTH - x - 1) / 8) * LCDWIDTH_NOROT] &= ~_BV((LCDWIDTH - x - 1) % 8);
#endif
}

inline uint8_t Display::getPixel(int8_t x, int8_t y) {
    if ((x < 0) || (x >= LCDWIDTH) || (y < 0) || (y >= LCDHEIGHT))
        return 0;

    return (_displayBuffer[x + (y / 8) * LCDWIDTH] >> (y % 8)) & 0x1;
}


#endif	/* DISPLAY_H */

