#ifndef S1D13700_h
#define S1D13700_h

#include <inttypes.h>
#include "Print.h"

// Definitions for Epson S1D13700 LCD controller

// Commands
#define SYSTEM_SET		0x40
#define POWER_SAVE		0x53
#define DISP_ON			0x59
#define DISP_OFF		0x58
#define SCROLL			0x44
#define CSRFORM			0x5D
#define CGRAM_ADR		0x5C
#define CSRDIR_RIGHT	0x4C
#define CSRDIR_LEFT		0x4D
#define CSRDIR_UP		0x4E
#define CSRDIR_DOWN		0x4F
#define HDOT_SCR		0x5A
#define OVLAY			0x5B
#define CSRW			0x46
#define CSRR			0x47
#define MEMWRITE		0x42
#define MEMREAD			0x43
#define GRAYSCALE		0x60

// Screen properties, Crystalfontz CFAG320240CX
#define LCDWIDTH		320
#define LCDHEIGHT		240

// Screen addressing definitions
#define SAD1_ADDR	 	0x0000
#define SAD1_1		   	0x00
#define SAD1_2		  	0x00
#define SL1			0xEF //239 lines
#define SAD1_LEN	  	0x2580 //(320 x 240 / 8 = 9600 = 0x2580)
#define SAD2_ADDR		0x2580
#define SAD2_1		  	0x25
#define SAD2_2		  	0x80
#define SL2			0xEF
#define SAD2_LEN	  	0x2580
#define SAD3_ADDR		0x4B00
#define SAD3_1		  	0x4B
#define SAD3_2		  	0x00
#define SAD4_ADDR		0x0000 // no layer 4
#define SAD4_1		  	0x00
#define SAD4_2		  	0x00

//functions for splitting hex
#define HINIBBLE(b) (((b) & 0xFF00) >> 8) //thanks, wiki! (Article on Nibbles)
#define LONIBBLE(b) ((b) & 0x00FF) //thanks, wiki! (Article on Nibbles)


struct TPositionS1D13700
{
    uint8_t x;
    uint8_t y;
};

class S1D13700 : public Print {
public:
	S1D13700(uint8_t res, uint8_t a0, uint8_t rw, uint8_t enable, uint8_t cs,
		uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3,
		uint8_t d4, uint8_t d5, uint8_t d6, uint8_t d7);
	S1D13700(uint8_t res, uint8_t a0, uint8_t enable, uint8_t cs,
		uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3,
		uint8_t d4, uint8_t d5, uint8_t d6, uint8_t d7);

	void init(uint8_t res, uint8_t a0, uint8_t rw, uint8_t enable, uint8_t cs,
		uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3,
		uint8_t d4, uint8_t d5, uint8_t d6, uint8_t d7);

	void begin();

    void switchLayer(uint8_t);
    void clearTextLayer();
    void clearLayer2();
    void clearLayer3();

    //These don't actually exist yet.
	void noDisplay();
	void display();
	void noBlink();
	void blink();
	void noCursor();
	void cursor();
	void scrollDisplayLeft();
	void scrollDisplayRight();
	void leftToRight();
	void rightToLeft();
	void autoscroll();
	void noAutoscroll();

    
	void createChar(uint8_t, uint8_t[]);
	void setCursor(uint8_t, uint8_t);
    
    //drawing commands
    void setMemPosition(uint8_t, uint8_t);
    void setPosition(uint8_t, uint8_t);
    void setPixel(uint8_t, uint8_t, uint8_t);

    uint8_t readByte(uint8_t xPos, uint8_t yPos);
    
    virtual void write(uint8_t);
	void command(uint8_t);
    uint8_t read();
    
private:
	void send(uint8_t, uint8_t);
	void write8bits(uint8_t);

	void reset();
    void pulseEnable();

    TPositionS1D13700 _cursorPos; //position of cursor, of course!
    
    uint8_t _current_layer; //1, 2 or 3.
    uint16_t _current_layer_mempos; //memory position of current layer
	uint8_t _res_pin; // LOW: Active. HIGH: Reset.
	uint8_t _a0_pin; // LOW: command.  HIGH: character.
	uint8_t _cs_pin; // LOW: select chip.  HIGH: bus will ignore commands.
	uint8_t _rw_pin; // LOW: write to LCD.  HIGH: read from LCD.
	uint8_t _enable_pin; // activated by a HIGH pulse.
	uint8_t _data_pins[8]; //8 data lines, for 8 bit mode. (Chip doesn't support 4 bit mode.)

	uint8_t _displayfunction;
	uint8_t _displaycontrol;
	uint8_t _displaymode;

	uint8_t _initialized;

	uint8_t _numlines,_currline;
};

#endif

