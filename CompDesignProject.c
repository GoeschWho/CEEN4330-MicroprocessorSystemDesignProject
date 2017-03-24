/* Auth: Megan Bird
 * File: CompDesignProject.c
 * Course: CEEN-4330 � Microprocessor System Design � University of Nebraska-Lincoln
 * Lab: Project
 * Date: 3/14/2017
 * Desc: 8051 program with ROM, RAM, and I/O
 */
 
#include <REGX55.H>
#include <stdio.h>

// P1
sbit P1_0 = P1^0;
sbit P1_1 = P1^1;

// P3
sbit P3_0 = P3^0;
sbit P3_1 = P3^1;
sbit P3_2 = P3^2;
sbit P3_3 = P3^3;
sbit P3_4 = P3^4;
sbit P3_5 = P3^5;
sbit P3_6 = P3^6;
sbit P3_7 = P3^7;

// Keypad
sbit kCol1  = P1^7;
sbit kCol2  = P1^6;
sbit kCol3  = P1^5;
sbit kCol4  = P1^4;
sbit kRow1  = P1^3;
sbit kRow2  = P1^2;
sbit kRow3  = P1^1;
sbit kRow4  = P1^0;
sbit cs_keypad = P3^1;		// latch
sbit oe_keypad = P3^2;

// Seven-Segment Display
sfr sevenSegPort = 0x90;
sbit cs_sevenSeg = P3^0;	// latch

// LCD
sbit cs_lcd = P3^3;				// latch
sbit rs_lcd = P3^4;				// reg select
sbit e_lcd  = P3^5;				// enable
// rw tied to ground to always enable read
sfr lcdPort = 0x90;

// Mission Control
enum {ctrl_off,
			ctrl_ss_latch,
			ctrl_kp_oe,
			ctrl_kp_oelatch,
			ctrl_lcd_cmd_start,
			ctrl_lcd_cmd_finish,
			ctrl_lcd_data_start,
			ctrl_lcd_data_finish };
			

// ======================= prototypes =========================== //

typedef unsigned char byte;

struct keypad_data {
	unsigned char k1;
	unsigned char k2;
	unsigned char k3;
	unsigned char k4;
	unsigned char k5;
	unsigned char k6;
	unsigned char k7;
	unsigned char k8;
	unsigned char k9;
	unsigned char k0;
	unsigned char kA;
	unsigned char kB;
	unsigned char kC;
	unsigned char kD;
	unsigned char kstar;
	unsigned char kpound;
};

void latchSevenSeg( void );
void latchKeypad( void );
void outputSevenSeg( char character );
struct keypad_data getKeysPressed( void );
void displayKeyPressed( struct keypad_data keypad );
void lcdCmd( byte cmd );
void lcdData( byte dat );
void lcdInit( void );
void lcdClear( void );
void lcdChar( byte character );
void lcdString( volatile char *string );
void lcdLine( int line );
void missionControl( int dec );
void msDelay( unsigned msecs );

// ======================== main ================================ //

void main(void) {
	
	struct keypad_data keypad;
	
	char string1[] = "Hello world`";
	char string2[] = "M&M's`";
	
	cs_keypad = 0;
	cs_sevenSeg = 0;
	cs_lcd = 0;
	
	lcdInit();
	
	lcdLine(1);
	lcdString( &string1 );
	lcdLine(2);
	lcdString( &string2 );
	
	while (1) {
		keypad = getKeysPressed();
		displayKeyPressed( keypad );
	}

	while(1); // Stay off the streets
	
} // end main()

// ======================= functions ============================ //

void latchSevenSeg( void ) {
	
//	cs_sevenSeg = 1;
//	cs_sevenSeg = 0;
	
	missionControl( ctrl_ss_latch );
	missionControl( ctrl_off );
	
} // end latchSevenSeg()

// -------------------------------------------------------------- //

void latchKeypad( void ) {
	
//	cs_keypad = 1;
//	cs_keypad = 0;
	
	missionControl( ctrl_kp_oelatch );
	missionControl( ctrl_kp_oe );
	
} // end latchKeypad()

// -------------------------------------------------------------- //

void latchLCD( void ) {
	
	cs_lcd = 1;
	cs_lcd = 0;
	
} // end latchLCD()

// -------------------------------------------------------------- //

void outputSevenSeg( char character ) {
	
	switch( character ) {
		case '0': sevenSegPort = ~0x3F; break;
		case '1':	sevenSegPort = ~0x06; break;
		case '2': sevenSegPort = ~0x5B; break;
		case '3': sevenSegPort = ~0x4F; break;
		case '4': sevenSegPort = ~0x66; break;
		case '5': sevenSegPort = ~0x6D; break;
		case '6': sevenSegPort = ~0x7D; break;
		case '7': sevenSegPort = ~0x07; break;
		case '8': sevenSegPort = ~0x7F; break;
		case '9': sevenSegPort = ~0x67; break;
		case 'A': sevenSegPort = ~0x77; break;
		case 'b': sevenSegPort = ~0x7C; break;
		case 'C': sevenSegPort = ~0x39; break;
		case 'd': sevenSegPort = ~0x5E; break;
		case 'E': sevenSegPort = ~0x79; break;
		case 'F': sevenSegPort = ~0x71; break;
		case '-': sevenSegPort = 0xBF; 	break;
		case '.': sevenSegPort = 0x7F;	break;
		case 'o':	sevenSegPort = 0xFF;  break;  // off
		default:  sevenSegPort = 0xAA; 					// invalid
	}
	
	latchSevenSeg();
	
} // end outputSevenSeg()

// -------------------------------------------------------------- //

struct keypad_data getKeysPressed( void ) {
	
	struct keypad_data keypad;
	
	// enable keypad latch output
	//oe_keypad = 0;
	missionControl( ctrl_kp_oe );
	
	// Set keypad columns as outputs & rows as inputs
	kCol1 = 0;
	kCol2 = 0;
	kCol3 = 0;
	kCol4 = 0;
	kRow1 = 1;
	kRow2 = 1;
	kRow3 = 1;
	kRow4 = 1;
	
	// scan by columns

	// column 1
	kCol1 = 0;
	kCol2 = 1;
	kCol3 = 1;
	kCol4 = 1;
	latchKeypad();
	
	if (kRow1 == 0) {
		keypad.k1 = 1;
	}
	else {
		keypad.k1 = 0;
	}
	if (kRow2 == 0) {
		keypad.k4 = 1;
	}
	else {
		keypad.k4 = 0;
	}
	if (kRow3 == 0) {
		keypad.k7 = 1;
	}
	else {
		keypad.k7 = 0;
	}
	if (kRow4 == 0) {
		keypad.kstar = 1;
	}
	else {
		keypad.kstar = 0;
	}
	kCol1 = 1;
	
	// column 2
	kCol2 = 0;
	latchKeypad();
	
	if (kRow1 == 0) {
		keypad.k2 = 1;
	}
	else {
		keypad.k2 = 0;
	}
	if (kRow2 == 0) {
		keypad.k5 = 1;
	}
	else {
		keypad.k5 = 0;
	}
	if (kRow3 == 0) {
		keypad.k8 = 1;
	}
	else {
		keypad.k8 = 0;
	}
	if (kRow4 == 0) {
		keypad.k0 = 1;
	}
	else {
		keypad.k0 = 0;
	}
	kCol2 = 1;
	
	// column 3
	kCol3 = 0;
	latchKeypad();
	
	if (kRow1 == 0) {
		keypad.k3 = 1;
	}
	else {
		keypad.k3 = 0;
	}
	if (kRow2 == 0) {
		keypad.k6 = 1;
	}
	else {
		keypad.k6 = 0;
	}
	if (kRow3 == 0) {
		keypad.k9 = 1;
	}
	else {
		keypad.k9 = 0;
	}
	if (kRow4 == 0) {
		keypad.kpound = 1;
	}
	else {
		keypad.kpound = 0;
	}
	kCol3 = 1;
	
	// column 4
	kCol4 = 0;
	latchKeypad();
	
	if (kRow1 == 0) {
		keypad.kA = 1;
	}
	else {
		keypad.kA = 0;
	}
	if (kRow2 == 0) {
		keypad.kB = 1;
	}
	else {
		keypad.kB = 0;
	}
	if (kRow3 == 0) {
		keypad.kC = 1;
	}
	else {
		keypad.kC = 0;
	}
	if (kRow4 == 0) {
		keypad.kD = 1;
	}
	else {
		keypad.kD = 0;
	}
	kCol4 = 1;
	latchKeypad();
	
	// disable keypad latch output
	//oe_keypad = 1;
	missionControl( ctrl_off );
	
	return keypad; 
	
} // end getKeysPressed()

// -------------------------------------------------------------- //

void displayKeyPressed( struct keypad_data keypad ) {
	
	if ( keypad.k1 == 1 ) {
		outputSevenSeg('1');
	}
	else if ( keypad.k2 == 1 ) {
		outputSevenSeg('2');
	}
	else if ( keypad.k3 == 1 ) {
		outputSevenSeg('3');
	}
	else if ( keypad.k4 == 1 ) {
		outputSevenSeg('4');
	}
	else if ( keypad.k5 == 1 ) {
		outputSevenSeg('5');
	}
	else if ( keypad.k6 == 1 ) {
		outputSevenSeg('6');
	}
	else if ( keypad.k7 == 1 ) {
		outputSevenSeg('7');
	}
	else if ( keypad.k8 == 1 ) {
		outputSevenSeg('8');
	}
	else if ( keypad.k9 == 1 ) {
		outputSevenSeg('9');
	}
	else if ( keypad.kA == 1 ) {
		outputSevenSeg('A');
	}
	else if ( keypad.k0 == 1 ) {
		outputSevenSeg('0');
	}
	else if ( keypad.kB == 1 ) {
		outputSevenSeg('b');
	}
	else if ( keypad.kC == 1 ) {
		outputSevenSeg('C');
	}
	else if ( keypad.kD == 1 ) {
		outputSevenSeg('d');
	}
	else if ( keypad.kstar == 1 ) {
		outputSevenSeg('.');
	}
	else if ( keypad.kpound == 1 ) {
		outputSevenSeg('-');
	}
	else {
		outputSevenSeg('o');
	}
	
} // end displayKeyPressed()
	
// -------------------------------------------------------------- //

void lcdCmd( byte cmd ) {
	
	//rs_lcd = 0;			// reg select low for command
	// --- RW tied low for write --- //
	//e_lcd = 1;			// E high for pulse
	
	missionControl( ctrl_lcd_cmd_start );
	
	lcdPort = cmd;
	//latchLCD();
	
	msDelay(1);			// Need Tpw > 140 ns
	//e_lcd = 0;			// E low to end pulse
	
	missionControl( ctrl_lcd_cmd_finish );
	
} // end lcdCmd()

// -------------------------------------------------------------- //

void lcdData( byte dat ) {
	
	//rs_lcd = 1;			// reg select high for data
	// --- RW tied low for write --- //
	//e_lcd = 1;			// E high for pulse
	
	missionControl( ctrl_lcd_data_start );
	
	lcdPort = dat;
	//latchLCD();
	
	msDelay(1);			// Need Tpw > 140 ns
	//e_lcd = 0;			// E low to end pulse
	
	missionControl( ctrl_lcd_data_finish );
	
} // end lcdData()

// -------------------------------------------------------------- //

void lcdInit( void ) {	
	
	msDelay(50);
	lcdCmd(0x38);			// Function set
	msDelay(5);
	lcdCmd(0x38);			// Function set
	msDelay(1);
	lcdCmd(0x0F);			// Display ON/OFF control
	msDelay(1);
	lcdCmd(0x01);			// Clear display
	msDelay(2);
	
} // end lcdInit()

// -------------------------------------------------------------- //

void lcdClear( void ) {
	
	lcdCmd(0x01);			// Clear display
	msDelay(2);
	
} // end lcdClear()

// -------------------------------------------------------------- //

void lcdChar( byte character ) {
	
	lcdData(character);		// send character
	
} // end lcdChar()

// -------------------------------------------------------------- //

void lcdString( volatile char *string ) {
	
	int i = 0;
	while( string[i] != '`') {
		lcdChar( string[i] );
		i++;
	}
	
} // end lcdString()

// -------------------------------------------------------------- //

void lcdLine( int line ) {
	
	switch (line) {
		case 1: {
			lcdCmd(0x80);
			break;
		}
		case 2: {
			lcdCmd(0xC0);
			break;
		}
		case 3: {
			lcdCmd(0x94);
			break;
		}
		case 4: {
			lcdCmd(0xD4);
			break;
		}
		default:
			lcdCmd(0x80);
		
	} // end switch
	
} // end lcdLine()

// -------------------------------------------------------------- //

void missionControl( int dec ) {
	
	switch ( dec ) {
		
		// The MSB is changed first to avoid false triggers of the LCD enable
		// line as the enable line never goes high when the MSB is low.
		case ctrl_off: {
			P3_2 = 0;
			P3_1 = 0;
			P3_0 = 0;
			break;
		}
		case ctrl_ss_latch: {
			P3_2 = 0;
			P3_1 = 0;
			P3_0 = 1;
			break;
		}
		case ctrl_kp_oe: {
			P3_2 = 0;
			P3_1 = 1;
			P3_0 = 0;
			break;
		}
		case ctrl_kp_oelatch: {
			P3_2 = 0;
			P3_1 = 1;
			P3_0 = 1;
			break;
		}
		case ctrl_lcd_cmd_start: {
			P3_2 = 1;
			P3_1 = 0;
			P3_0 = 0;
			break;
		}
		case ctrl_lcd_cmd_finish: {
			P3_2 = 1;
			P3_1 = 0;
			P3_0 = 1;
			break;
		}
		case ctrl_lcd_data_start: {
			P3_2 = 1;
			P3_1 = 1;
			P3_0 = 0;
			break;
		}
		case ctrl_lcd_data_finish: {
			P3_2 = 1;
			P3_1 = 1;
			P3_0 = 1;
			break;
		}		
	} // end switch
	
} // end missionControl()

// -------------------------------------------------------------- //

void msDelay( unsigned msecs ) {
	
	unsigned i;
	unsigned char j;
	
	msecs = msecs * 3.23;
	
	for(i=0;i<msecs;i++){
		for(j=0;j<100;j++);
	}
	
} // end msDelay()

// -------------------------------------------------------------- //

