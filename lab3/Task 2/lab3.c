/********************************************
 *
 *  Name: Jemuel Amporinguez
 *  Email: jemuel.amporinguez@g.msuiit.edu.ph
 *  Section:
 *  Assignment: Lab 3 - Rotary Encoder
 *
 ********************************************/

#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <avr/interrupt.h>
#include <string.h>
#include "lcd.h"

void play_note(uint16_t);
void variable_delay_us(int16_t);

// Frequencies for natural notes from middle C (C4)
// up one octave to C5.
uint16_t frequency[8] = { 262, 294, 330, 349, 392, 440, 494, 523 };

int main(void) {
    uint8_t new_state, old_state;
    uint8_t changed = 0;  // Flag for state change
    int16_t count = 0;    // Count to display
    uint8_t a, b;

    // Initialize DDR and PORT registers and LCD
    lcd_init();  // Initialize the LCD

    // Write a splash screen to the LCD
    lcd_moveto(0, 0);  // Move to the first row
    lcd_stringout("Jemuel Amporinguez");  // Display your name

    // Display the birthdate with a formatted date message
    char date[30];
    int month = 12, day = 6, year = 2001;
    char *dayofweek = "Thursday";
    snprintf(date, sizeof(date), "Date is %d/%d/%d = %s", month, day, year, dayofweek);

    lcd_moveto(1, (16 - strlen(date)) / 2);  // Center on the second row
    lcd_stringout(date);

    _delay_ms(1000);  // Delay for 1 second to display the splash screen
    lcd_writecommand(0x01);  // Clear the screen

    // Read the A and B inputs to determine the initial state.
    if (!b && !a)
        old_state = 0;
    else if (!b && a)
        old_state = 1;
    else if (b && !a)
        old_state = 2;
    else
        old_state = 3;

    new_state = old_state;

    while (1) {  // Loop forever
        // Read the input bits and determine A and B.

        if (old_state == 0) {
            // Handle A and B inputs for state 0
        }
        else if (old_state == 1) {
            // Handle A and B inputs for state 1
        }
        else if (old_state == 2) {
            // Handle A and B inputs for state 2
        }
        else {   // old_state = 3
            // Handle A and B inputs for state 3
        }

        // If state changed, update the value of old_state,
        // and set a flag that the state has changed.
        if (new_state != old_state) {
            changed = 1;
            old_state = new_state;
        }

        if (changed) { // Did state change?
            changed = 0;  // Reset changed flag

            // Output count to LCD

            // Do we play a note?
            if ((count % 8) == 0) {
                // Determine which note (0-7) to play

                // Find the frequency of the note

                // Call play_note and pass it the frequency
            }
        }
    }
}

/*
  Play a tone at the frequency specified for one second
*/
void play_note(uint16_t freq)
{
    uint32_t period = 1000000 / freq;  // Period of note in microseconds

    while (freq--) {
        PORTB |= (1 << PB4);  // Buzzer output high
        variable_delay_us(period / 2);  // Delay for half the period
        PORTB &= ~(1 << PB4);  // Buzzer output low
        variable_delay_us(period / 2);  // Delay for half the period
    }
}

/*
    variable_delay_us - Delay a variable number of microseconds
*/
void variable_delay_us(int delay)
{
    int i = (delay + 5) / 10;

    while (i--)
        _delay_us(10);
}

ISR(PCINT1_vect)
{
    // In Task 7, add code to read the encoder inputs and determine the new
    // count value.
}

void timer1_init()
{
    // In Task 8, add code to initialize TIMER1, but don't start it counting
}

ISR(TIMER1_COMPA_vect)
{
    // In Task 8, add code to change the output bit to the buzzer, and to turn
    // off the timer after enough periods of the signal have gone by.
}
