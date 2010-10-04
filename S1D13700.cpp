#include "S1D13700.h"

#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include "WProgram.h"

//============================================================================
const unsigned char SplashText_Screen[30][41]=
	{"                                        ",  // 0
	"                                        ",  // 1
	"                                        ",  // 2
	"                                        ",  // 3
	"                                        ",  // 4
	"                                        ",  // 5
	"                                        ",  // 6
	"                                        ",  // 7
	"                                        ",  // 8
	"                                        ",  // 9
	"                                        ",  // 10
	"                                        ",  // 11
	"                                        ",  // 12
	"                                        ",  // 13
	"                                        ",  // 14
	"                                        ",  // 15
	"                                        ",  // 16
	"                                        ",  // 17
	"                                        ",  // 18
	"                                        ",  // 19
	"                               Built-in ",  // 20
	"                               5x7 font ",  // 21
	"                               on a     ",  // 22
	"                               separate ",  // 23
	"                               text     ",  // 24
	"                               layer.   ",  // 25
	"                               Can be   ",  // 26
	"                               combined ",  // 27
	"                               using AND",  // 28
	"                               OR or XOR"}; // 29

void S1D13700::clear_text_layer() {

    //position cursor at beginning of text layer
    command(CSRW);
	write(SAD1_2);
	write(SAD1_1);

    //clearing loop
    for(int i = 0; i < SAD1_LEN; i++) {

        //loop through and write a space character to every location in text layer
		command(MEMWRITE);
		write(0x20);
	}
}

void S1D13700::clear_layer_2() {

    //position cursor at beginning of second display block
	command(CSRW);
	write(SAD2_2);
	write(SAD2_1);

    //clear loop
	for(int i = 0; i < SAD2_LEN; i++) {

        //write 0x00 to all locations in the second display buffer
		command(MEMWRITE);
		write(0x00);
	}
}

void S1D13700::clear_layer_3() {

    //position cursor at beginning of second display block
	command(CSRW);
	write(SAD3_2);
	write(SAD3_1);

    //clear loop
	for(int i = 0; i < SAD2_LEN; i++) {
        //write 0x00 to all locations in the second display buffer
		command(MEMWRITE);
		write(0x00);
	}
}
//MY STUFF!!!!

// When the display powers up, it is configured as follows:
//
// 1. Display clear
// 2. Function set: 
//    DL = 1; 8-bit interface data 
//    N = 0; 1-line display 
//    F = 0; 5x8 dot character font 
// 3. Display on/off control: 
//    D = 0; Display off 
//    C = 0; Cursor off 
//    B = 0; Blinking off 
// 4. Entry mode set: 
//    I/D = 1; Increment by 1 
//    S = 0; No shift 
//
// Note, however, that resetting the Arduino doesn't reset the LCD, so we
// can't assume that its in that state when a sketch starts (and the
// LiquidCrystal constructor is called).

S1D13700::S1D13700(uint8_t res, uint8_t a0, uint8_t rw, uint8_t enable, uint8_t cs,
	uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3,
	uint8_t d4, uint8_t d5, uint8_t d6, uint8_t d7)
{
	init(res, a0, rw, enable, cs, d0, d1, d2, d3, d4, d5, d6, d7);
}

//we're not using RW here, just writing.
S1D13700::S1D13700(uint8_t res, uint8_t a0, uint8_t enable, uint8_t cs,
	uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3,
	uint8_t d4, uint8_t d5, uint8_t d6, uint8_t d7)
{
	init(res, a0, 255, enable, cs, d0, d1, d2, d3, d4, d5, d6, d7);
}

void S1D13700::init(uint8_t res, uint8_t a0, uint8_t rw, uint8_t enable, uint8_t cs,
	uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3,
	uint8_t d4, uint8_t d5, uint8_t d6, uint8_t d7)
{

	//tackle all pin assignments
	_res_pin = res;
	_a0_pin = a0;
	_rw_pin = rw;
	_cs_pin = cs;

	_enable_pin = enable;

	_data_pins[0] = d0;
	_data_pins[1] = d1;
	_data_pins[2] = d2;
	_data_pins[3] = d3; 
	_data_pins[4] = d4;
	_data_pins[5] = d5;
	_data_pins[6] = d6;
	_data_pins[7] = d7; 

	pinMode(_res_pin, OUTPUT);
	pinMode(_a0_pin, OUTPUT);
	pinMode(_cs_pin, OUTPUT);

    // we can save 1 pin by not using RW. Indicate by passing 255 instead of pin#
	if (_rw_pin != 255) { 
		pinMode(_rw_pin, OUTPUT);
	}
	pinMode(_enable_pin, OUTPUT);
}

void S1D13700::begin() {

	int i;

    // according to datasheet, we need at least 40ms after power rises above 2.7V
    // before sending commands. Arduino can turn on way befer 4.5V so we'll wait 50
	delayMicroseconds(50000); 

    // Now we pull both A0 and R/W low to begin commands
	digitalWrite(_a0_pin, LOW);
	digitalWrite(_enable_pin, LOW);

	if (_rw_pin != 255) { 
		digitalWrite(_rw_pin, LOW);
	}

	write8bits(0x00); // not sure if I should do this in "reset" or not

	//now that we have all data lines at 0 and all control lines properly set, reset.
	reset();

//SYSTEM_SET:
	command(SYSTEM_SET); //system set
	write(0x30); // first register, single panel drive
	write(0x87); //FX - Horizontal character size register (8 pixels)
	write(0x07); //FY - Vertical character size register (8 pixels)
	write(0x27); //C/R - character bytes per row
	write(0x42); //TC/R - total character bytes per row (CHECK CALCULATIONS)
	write(0xEF); //L/F - frame height in lines
	write(0x28); //APL - horizontal address range
	write(0x00); //APH - also horizontal address range!

	// Scroll setup, of course:
	command(SCROLL);
	write(SAD1_2); //screen block 1 start address
	write(SAD1_1); // screen block 1 start address
	write(SL1); // screen block 1 size
	write(SAD2_2);
	write(SAD2_1);
	write(SL2);
	write(SAD3_2);
	write(SAD3_1);
	write(SAD4_2); //these are zero, as we're not using a 4th display area
	write(SAD4_1);

    //HDOT_SCR:
	command(HDOT_SCR);
	write(0x00); //NO SCROLL

    //OVERLAY:
	command(OVLAY);
	write(0x01);	// SB1 = Graphics
					// SB3 = Text
					// Exculsive-OR
					
	command(DISP_OFF);
	write(0x56); // P1 -- flash cursor!!! yes, fucking FINALLY
    // write(0x54); //cursor off

    //wipe all layers in use
    clear_text_layer();
    clear_layer_2();
    clear_layer_3();
    
	command(CSRW);
	write(SAD1_2);
	write(SAD1_1);

    //CSR_FORM:
	command(CSRFORM);
	write(0x04);
	write(0x86);
	
	//turn the display on!
	command(DISP_ON);
	write(0x56); // P1 -- flash cursor!!! yes, fucking FINALLY
	
	//set the cursor direction to "right"
	command(CSRDIR_RIGHT);

    //write "epson" to the screen
	command(MEMWRITE);
	write(0x20);
	write(0x45);
	write(0x50);
	write(0x53);
	write(0x4F);
	write(0x4E);	
}


/*********** drawing commands ********/

// the most basic function, set a single pixel
// void S1D13700::setpixel(uint8_t x, uint8_t y, uint8_t color) {
//     if ((x >= LCDWIDTH) || (y >= LCDHEIGHT))
//         return;

//     // x is which column
//     if (color) 
//         buffer[x+ (y/8) * LCDWIDTH] |= _BV(7-(y%8));  
//     else
//         buffer[x+ (y/8) * LCDWIDTH] &= ~_BV(7-(y%8)); 
// }



/*********** mid level commands, for sending data/cmds */

inline void S1D13700::command(uint8_t value) {
	send(value, HIGH);
}

inline void S1D13700::write(uint8_t value) {
	send(value, LOW);
}

/************ low level data pushing commands **********/

// write either command or data, with automatic 4/8-bit selection
void S1D13700::send(uint8_t value, uint8_t mode) {
	digitalWrite(_a0_pin, mode);

// if there is a RW pin indicated, set it low to Write
	if (_rw_pin != 255) { 
		digitalWrite(_rw_pin, LOW);
	}

	write8bits(value);  
}

void S1D13700::reset() {
	digitalWrite(_res_pin, LOW);
	delayMicroseconds(5000);
	digitalWrite(_res_pin, HIGH);
}

void S1D13700::pulseEnable(void) {
	digitalWrite(_enable_pin, LOW);
	delayMicroseconds(1);
	digitalWrite(_enable_pin, HIGH);
	delayMicroseconds(1);    // enable pulse must be >450ns
	digitalWrite(_enable_pin, LOW);
	delayMicroseconds(10);   // commands need > 37us to settle
}

void S1D13700::write8bits(uint8_t value) {
	for (int i = 0; i < 8; i++) {
		pinMode(_data_pins[i], OUTPUT);
		digitalWrite(_data_pins[i], (value >> i) & 0x01);
	}

	pulseEnable();
}

