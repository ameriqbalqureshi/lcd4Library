/*
 * lcd8.c
 *
 *  Created on: Jan 21, 2023
 *      Author: iqbal
 *      Library for HD44780 Character LCD
 *      4-Bit Mode
 */
#include "lcd4.h"
#include <stdio.h>
GPIO_TypeDef	*lcdPort;
uint16_t		lcdRsPin;
uint16_t		lcdEnPin;
uint16_t		lcdDataPin[4];

void lcdSetup(GPIO_TypeDef *PORT, uint16_t RS, uint16_t En, uint16_t D4,uint16_t D5,uint16_t D6,uint16_t D7 )
{
// Assign Pin values to global variables
	lcdPort=PORT;
	lcdRsPin=RS;
	lcdEnPin=En;
	lcdDataPin[0]=D4;
	lcdDataPin[1]=D5;
	lcdDataPin[2]=D6;
	lcdDataPin[3]=D7;

}

void lcdEnable()
{
	// Give a small 0 to 1 and 1 to 0 pulse on Enable pin to transfer port data
	HAL_GPIO_WritePin(lcdPort, lcdEnPin, GPIO_PIN_SET);
	HAL_Delay(1);
	HAL_GPIO_WritePin(lcdPort, lcdEnPin, GPIO_PIN_RESET);

}

void lcdWrite(uint8_t data)
//	Writes one byte of data to the LCD Pins
// in 4 bit mode write upper nibble first then lower nibble
{
	lcdWrite4Bit(data >> 4);	// Send upper Nibble
	lcdWrite4Bit(data & 0x0F);
}

void lcdWrite4Bit(uint8_t data) // 0000 1010
{
	for(int i=0;i<4;i++)
	{
		if(data & (1<<i))
			HAL_GPIO_WritePin(lcdPort, lcdDataPin[i], GPIO_PIN_SET);
		else
			HAL_GPIO_WritePin(lcdPort, lcdDataPin[i], GPIO_PIN_RESET);
	}
	lcdEnable();
}

void lcdCommand(uint8_t command)
{
//	Sends Command data to LCD
	HAL_GPIO_WritePin(lcdPort, lcdRsPin, GPIO_PIN_RESET);	// Set RS to 0 for commands
	lcdWrite(command);
}
void lcdInit()
{
	lcdCommand(0x02); 	//  0 0 1 DL N F — — 0x30= 0010 0000 DL=0 means 4 Bit Mode
	HAL_Delay(10);
	lcdCommand(0x02); 	//  0 0 1 DL N F — — 0x30= 0010 0000 DL=0 means 4 Bit Mode
	HAL_Delay(10);
	lcdCommand(0x02); 	//  0 0 1 DL N F — — 0x30= 0010 0000 DL=0 means 4 Bit Mode
	HAL_Delay(10);
	lcdCommand(0x28);
	HAL_Delay(10);
	lcdCommand(0x0E);	// 0 0 0 0 1 D C B		0x0E= 0000 1110 D=1 Display ON, C=1 Cursor ON
	HAL_Delay(10);

}

void lcdChar(uint8_t ch)
{
//	Display One Byte of Data to LCD
	HAL_GPIO_WritePin(lcdPort, lcdRsPin, GPIO_PIN_SET);	// Set RS Pin 1 for Data
	lcdWrite(ch);
}

void lcdString(char * string)
{
//	Display a String of characters
	while(*string)
		lcdChar(*string++);
}

void lcdWriteInt(char * format, uint32_t number )
{
//	Display integer numbers format is standard c printf() like %d etc
	char buffer[20];
	sprintf(buffer,format,number);
	lcdString(buffer);
}

void lcdWriteFloat(char * format, double number )
{
//	Display a floating point number
	char buffer[20];
	sprintf(buffer,format,number);
	lcdString(buffer);
}

void lcdSetCursor(uint8_t row, uint8_t col)
{
//	Sets the cursor position
	HAL_GPIO_WritePin(lcdPort, lcdRsPin, GPIO_PIN_RESET);	// RS=0;
	switch (row)
	{
	case 0:
		lcdWrite(0x80+col); break;
	case 1:
		lcdWrite(0xC0+col); break;
	case 2:
		lcdWrite(0x80+0x14+col); break;
	case 3:
		lcdWrite(0xC0+0x14+col); break;
	}
}


