#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "lcd.h"  // Declarations of the LCD functions

// Define encoder pins and buzzer pin
#define ENCODER_A PC1
#define ENCODER_B PC5
#define BUZZER_PIN PB4

// Define global variables
volatile int16_t count = 0;  // Count value that will increase/decrease
uint8_t old_state = 0;
uint8_t new_state = 0;
volatile uint8_t play_note_flag = 0;
uint16_t frequency[8] = { 262, 294, 330, 349, 392, 440, 494, 523 }; // Frequency array

// Function to initialize the LCD, encoder, and timer
void setup() {
    // Initialize LCD
    lcd_init();
    lcd_stringout("Lab 3 - Your Name");
    _delay_ms(1000);
    lcd_writecommand(0x01); // Clear LCD display

    // Set encoder pins as inputs with pull-ups
    PORTC |= (1 << ENCODER_A) | (1 << ENCODER_B);

    // Set buzzer pin as output
    DDRB |= (1 << BUZZER_PIN);

    // Initialize encoder state
    old_state = ((PINC & (1 << ENCODER_A)) >> ENCODER_A) | ((PINC & (1 << ENCODER_B)) >> (ENCODER_B - 1));

    // Enable pin change interrupt on encoder pins
    PCICR |= (1 << PCIE1); // Enable pin change interrupt for Port C
    PCMSK1 |= (1 << ENCODER_A) | (1 << ENCODER_B); // Mask for encoder pins

    // Configure Timer1 for tone generation (CTC mode)
    TCCR1A = 0;
    TCCR1B = 0;

    // Enable global interrupts
    sei();
}

// Timer setup to play a specific frequency
void setup_timer1(uint16_t freq) {
    OCR1A = (16000000 / (2 * freq)) - 1; // Set compare match register based on frequency
    TCCR1B = (1 << WGM12) | (1 << CS10); // CTC mode, no prescaling
    TIMSK1 |= (1 << OCIE1A); // Enable Timer1 compare match interrupt
}

// ISR for pin change interrupt - detects encoder changes and updates LCD and note
ISR(PCINT1_vect) {
    // Read the new state of the encoder pins
    new_state = ((PINC & (1 << ENCODER_A)) >> ENCODER_A) | ((PINC & (1 << ENCODER_B)) >> (ENCODER_B - 1));
    if (new_state != old_state) {
        // Determine rotation direction based on state transition
        if ((old_state == 0b00 && new_state == 0b01) ||
            (old_state == 0b01 && new_state == 0b11) ||
            (old_state == 0b11 && new_state == 0b10) ||
            (old_state == 0b10 && new_state == 0b00)) {
            count++;
        } else {
            count--;
        }
        old_state = new_state;

        // Display the updated count
        lcd_writecommand(0x01); // Clear the LCD each time
        lcd_moveto(0, 0);
        if (count < 0) {
            lcd_writedata('-');
            lcd_writedata('0' + (-count / 10) % 10);
            lcd_writedata('0' + (-count % 10));
        } else {
            lcd_writedata('0' + (count / 10) % 10);
            lcd_writedata('0' + (count % 10));
        }

        // Check if note should be played
        if (count % 8 == 0) {
            uint8_t note_index = ((count < 0 ? -count : count) % 64) / 8;
            setup_timer1(frequency[note_index]); // Set Timer1 for frequency
            play_note_flag = 1;
        }
    }
}

// ISR for Timer1 to generate square wave for buzzer
ISR(TIMER1_COMPA_vect) {
    if (play_note_flag) {
        PORTB ^= (1 << BUZZER_PIN); // Toggle buzzer pin
    }
}

// Timer1 overflow interrupt to turn off the buzzer after a delay
ISR(TIMER1_OVF_vect) {
    TCCR1B = 0; // Stop Timer1
    PORTB &= ~(1 << BUZZER_PIN); // Ensure buzzer is off
    play_note_flag = 0; // Reset the flag
}

int main(void) {
    setup(); // Initialize setup
    while (1) {
        // Main loop is empty as everything is interrupt-driven
    }
}
