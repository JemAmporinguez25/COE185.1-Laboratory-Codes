/********************************************
*
*  Name:
*  Email:
*  Section:
*  Assignment: Lab 2 - Up/Down counter on LCD display
*
********************************************/

#include <avr/io.h>
#include <util/delay.h>
#include "lcd.h"

int main(void) {

    // Initialize the LCD
    lcd_init();  // Initialize the LCD display

    // Clear the display
    lcd_writecommand(0x01);  // Clear display command

    // Set the cursor to the first row, first column
    lcd_moveto(0, 0);  // Move cursor to the first row, first column (row 0, column 0)

    // Write "M", "S", and "U" using their corresponding hex values
    lcd_writedata(0x4D);  // 0x4D corresponds to 'M'
    lcd_writedata(0x53);  // 0x53 corresponds to 'S'
    lcd_writedata(0x55);  // 0x55 corresponds to 'U'

    while (1) {
        // Infinite loop to keep the LCD displaying the characters
        _delay_ms(500);  // Add a delay
    }

    return 0;  // This is never reached
}
