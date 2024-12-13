#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include "lcd.h"

int main(void) {
    uint8_t new_state, old_state;
    int16_t count = 0;    // Count to display
    uint8_t a, b;

    // Initialize DDR and PORT registers and LCD
    lcd_init();  // Initialize the LCD

    // Enable pull-up resistors for encoder inputs on PC1 and PC5
    DDRC &= ~((1 << PC1) | (1 << PC5)); // Set PC1 and PC5 as input
    PORTC |= (1 << PC1) | (1 << PC5);   // Enable pull-up resistors

    // Splash screen
    lcd_moveto(0, 0);
    lcd_stringout("Jemuel Amporinguez");
    _delay_ms(1000);
    lcd_writecommand(0x01);  // Clear screen

    // Initialize the old state by reading the initial encoder state
    old_state = ((PINC & (1 << PC1)) >> PC1) | (((PINC & (1 << PC5)) >> PC5) << 1);

    // Main loop
    while (1) {
        // Read the input bits from the PINC register
        uint8_t pinC = PINC;

        // Mask to get the "A" and "B" values
        a = (pinC & (1 << PC1)) >> PC1;  // Read state of PC1
        b = (pinC & (1 << PC5)) >> PC5;  // Read state of PC5

        // Update new_state with current encoder position (00, 01, 10, 11)
        new_state = (a << 1) | b;

        // Check for state change to track rotation
        if (new_state != old_state) {
            // Determine rotation direction based on quadrature sequence
            if ((old_state == 0 && new_state == 1) ||
                (old_state == 1 && new_state == 3) ||
                (old_state == 3 && new_state == 2) ||
                (old_state == 2 && new_state == 0)) {
                count++; // Clockwise rotation
            } else if ((old_state == 0 && new_state == 2) ||
                       (old_state == 2 && new_state == 3) ||
                       (old_state == 3 && new_state == 1) ||
                       (old_state == 1 && new_state == 0)) {
                count--; // Counterclockwise rotation
            }

            // Update old_state to the current state
            old_state = new_state;

            // Display count and A/B state on the LCD
            char buf[16];
            snprintf(buf, 16, "A=%d B=%d", a, b);
            lcd_moveto(0, 0);
            lcd_stringout(buf);

            _delay_ms(200);  // Delay to avoid display flickering
        }
    }
}
