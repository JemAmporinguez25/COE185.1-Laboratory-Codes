/********************************************
*
*  Name: Jemuel Amporinguez
*  Email: jemuel.amporinguez@g.msuiit.edu.ph
*  Section: m98
*  Assignment: Lab 2 - Up/Down counter on LCD display
*
********************************************/

#include <avr/io.h>
#include <util/delay.h>
#include "lcd.h"

// Define the states
enum states { UP, DOWN, PAUSE };

// Function to initialize ADC
void init_adc(void) {
    ADMUX = (1 << REFS0);  // Use AVcc as reference, ADC0 as input
    ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);  // Enable ADC, prescaler 128
}

// Function to read ADC value
uint16_t read_adc(void) {
    ADCSRA |= (1 << ADSC);    // Start conversion
    while (ADCSRA & (1 << ADSC)); // Wait for completion
    return ADC;
}

int main(void) {
    // Initialize variables
    enum states current_state = UP;  // Initial state set to UP
    unsigned char count = 0;  // Counter variable
    unsigned char last_count = 255;  // Last count value for comparison (invalid initially)
    uint16_t timer = 0;  // Timer to track 500ms intervals (10 x 50ms)

    // Initialize the LCD
    lcd_init();
    lcd_writecommand(0x01);    // Clear display
    _delay_ms(2);              // Delay for clear display to complete

    // Initialize ADC for button reading
    init_adc();

    // Move cursor to the home position and display initial count
    lcd_writecommand(2);  // Move cursor to row 0, column 0
    lcd_writedata(count + '0');  // Display the initial count

    // Loop forever
    while (1) {
        // Check button state every 50ms
        uint16_t button_value = read_adc();
        
        // Check buttons and update the current state based on ADC values
        if (button_value > 50 && button_value < 150) {
            current_state = UP;  // Up button pressed
        } else if (button_value > 150 && button_value < 300) {
            current_state = DOWN;  // Down button pressed
        } else if (button_value > 500 && button_value < 750) {
            current_state = PAUSE;  // Pause button pressed
        }

        // Timer logic to control counting every 500ms (10 x 50ms)
        if (++timer >= 10) {  // 500 ms has passed (10 x 50 ms)
            timer = 0;  // Reset the timer

            // State machine logic for counting
            if (current_state == UP) {
                count = (count + 1) % 10;  // Increment count (wrap from 9 to 0)
            } else if (current_state == DOWN) {
                count = (count == 0) ? 9 : count - 1;  // Decrement count (wrap from 0 to 9)
            }

            // Only update the display if the count has changed
            if (count != last_count) {
                last_count = count;  // Update the last count value for the next iteration

                // Move the cursor to home position and display the new count value
                lcd_writecommand(2);  // Move cursor to row 0, column 0
                lcd_writedata(count + '0');  // Display the new count value
            }
        }

        // Short delay for frequent checks (50ms)
        _delay_ms(50);
    }

    return 0;  // This is never reached
}
