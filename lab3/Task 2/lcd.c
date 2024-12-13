#include <avr/io.h>
#include <util/delay.h>

#include "lcd.h"                // Declarations of the LCD functions

/* This function not declared in lcd.h since
   should only be used by the routines in this file. */
void lcd_writenibble(unsigned char);

/* Define a couple of masks for the bits in Port B and Port D */
#define DATA_BITS ((1 << PD7)|(1 << PD6)|(1 << PD5)|(1 << PD4)) // DATA BITS = 0b11110000
#define CTRL_BITS ((1 << PB1)|(1 << PB0))

/*
  lcd_init - Do various things to initialize the LCD display
*/
void lcd_init(void)
{
    /* ??? */                   // Set the DDR register bits for ports B and D
                                // Take care not to affect any unnecessary bits
      DDRB |= (1 << DDB0); //for output
      DDRB |= (1 << DDB1); // for ouput

      DDRD |= (1 << DDD7); // for ouput
      DDRD |= (1 << DDD6); // for ouput
      DDRD |= (1 << DDD5); // for ouput
      DDRD |= (1 << DDD4); // for ouput

    _delay_ms(15);              // Delay at least 15ms

    lcd_writenibble(0x30);      // Use lcd_writenibble to send 0b0011
    _delay_ms(5);               // Delay at least 4msec

    lcd_writenibble(0x30);      // Use lcd_writenibble to send 0b0011
    _delay_us(120);             // Delay at least 100usec

    lcd_writenibble(0x30);      // Use lcd_writenibble to send 0b0011, no delay needed

    lcd_writenibble(0x20);      // Use lcd_writenibble to send 0b0010
    _delay_ms(2);               // Delay at least 2ms
    
    lcd_writecommand(0x28);     // Function Set: 4-bit interface, 2 lines

    lcd_writecommand(0x0f);     // Display and cursor on
}

/*
  lcd_moveto - Move the cursor to the row and column given by the arguments.
  Row is 0 or 1, column is 0 - 15.
*/
void lcd_moveto(unsigned char row, unsigned char col)
{
    unsigned char pos;
    if(row == 0) {
        pos = 0x80 + col;       // 1st row locations start at 0x80
    }
    else {
        pos = 0xc0 + col;       // 2nd row locations start at 0xc0
    }
    lcd_writecommand(pos);      // Send command
}

/*
  lcd_stringout - Print the contents of the character string "str"
  at the current cursor position.
*/
void lcd_stringout(char *str)
{
    int i = 0;
    while (str[i] != '\0') {    // Loop until next charater is NULL byte
        lcd_writedata(str[i]);  // Send the character
        i++;
    }
}

/*
  lcd_writecommand - Output a byte to the LCD command register.
*/
void lcd_writecommand(unsigned char cmd)
{
    /* Clear PB0 to 0 for a command transfer */
    PORTB &= ~CTRL_BITS; // Ensure both RS and E are cleared for command mode
    
    /* Call lcd_writenibble to send UPPER four bits of "cmd" argument */
    lcd_writenibble(cmd >> 4);
    /* Call lcd_writenibble to send LOWER four bits of "cmd" argument */
    lcd_writenibble(cmd);
    /* Delay 2ms */
    _delay_ms(2); 
}

/*
  lcd_writedata - Output a byte to the LCD data register
*/

// deal with sending data and commands to the LCD 
// by making use of the low level functions
// These functions transfer one byte each time they are called
void lcd_writedata(unsigned char dat)
{
    /* Set PB0 to 1 for a data transfer */
    PORTB = (PORTB & ~CTRL_BITS) | (1 << PB0); // ~CTRL_BITS (00)| 1 (PBO), ENABLE  =0
    /* Call lcd_writenibble to send UPPER four bits of "dat" argument */
    lcd_writenibble(dat >> 4);  // Send upper 4 bits of dat   shift right (7-4) to (3-0), 7-4 becomes 0000
    /* Call lcd_writenibble to send LOWER four bits of "dat" argument */
    lcd_writenibble(dat);       // Send lower 4 bits of data
    /* Delay 2ms */
    _delay_ms(2);     
}

/*
  lcd_writenibble - Output the UPPER four bits of "lcdbits" to the LCD
*/

//  will handle the changing of the bits in the I/O ports connected to LCD. 
//  Most of the “bit fiddling” will be done in this function. 
//  The primary task will be sending a 4-bit “nibble” to the LCD 
//  and creating the Enable signal transition from 0 to 1 back to 0. 
void lcd_writenibble(unsigned char lcdbits)
{
    /* Load PORTD, bits 7-4 with bits 7-4 of "lcdbits" */
    PORTD = (PORTD & ~DATA_BITS) | (lcdbits << 4); // ~DATABITS = 0b00001111 | shifted 4 bit (7-4)

    /* Make E signal (PB1) go to 1 and back to 0 */

    PORTB |= (1 << PB1);  // Set E (PB1) to 1
    PORTB |= (1 << PB1);        // Make E longer, task 3
    
    //Add enough delay that the E signal is in the 1 state for at least 230ns.
    PORTB &= ~(1 << PB1); // Clear E (PB1) to 0
    _delay_us(1);         // Small delay
}