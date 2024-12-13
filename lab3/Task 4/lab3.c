#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "lcd.h"  // Declarations of the LCD functions

// Define encoder pins and buzzer pin
#define ENCODER_A PC1
#define ENCODER_B PC5
#define BUZZER_PIN PB4

// Define global variables
volatile int16_t count = 0; // Count value that will increase/decrease
uint8_t old_state = 0;
uint8_t new_state = 0;
uint8_t changed = 0; // Flag to indicate state change
uint16_t frequency[8] = { 262, 294, 330, 349, 392, 440, 494, 523 }; // Frequency array

// Function to initialize the LCD, encoder, and timer
void setup() {
    // Initialize LCD
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

    //_delay_ms(3000);  // Delay for 1 second to display the splash screen
    lcd_writecommand(0x01);  // Clear the screen

    // Set encoder pins as inputs with pull-ups
    PORTC |= (1 << ENCODER_A) | (1 << ENCODER_B);

    // Set buzzer pin as output
    DDRB |= (1 << BUZZER_PIN);

    // Initialize encoder state
    old_state = ((PINC & (1 << ENCODER_A)) >> ENCODER_A) | ((PINC & (1 << ENCODER_B)) >> (ENCODER_B - 1));

    // Enable pin change interrupt on encoder pins
    PCICR |= (1 << PCIE1); // Enable pin change interrupt for Port C
    PCMSK1 |= (1 << ENCODER_A) | (1 << ENCODER_B); // Mask for encoder pins

    // Configure Timer1 for tone generation
    TCCR1A = 0; // Normal mode
    TCCR1B = 0; // No clock source initially

    // Enable global interrupts
    sei();
}

// Timer setup to play a specific frequency
void setup_timer1(uint16_t freq) {
    OCR1A = (16000000 / (2 * freq)) - 1; // Set compare match register based on frequency
    TCCR1B = (1 << WGM12) | (1 << CS10); // CTC mode, no prescaling
    TIMSK1 |= (1 << OCIE1A); // Enable Timer1 compare match interrupt
}

// ISR for pin change interrupt - detects encoder changes
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
        changed = 1;
    }
}

// ISR for Timer1 to generate square wave for buzzer
ISR(TIMER1_COMPA_vect) {
    PORTB ^= (1 << BUZZER_PIN); // Toggle buzzer pin
}

// Helper function to display integer count on LCD
void display_count(int16_t value) {
    lcd_writecommand(0x01); // Clear the LCD each time
    lcd_moveto(0, 0);
    if (value < 0) {
        lcd_writedata('-');
        value = -value;
    }

    // Convert integer to characters and display
    if (value >= 100) lcd_writedata('0' + (value / 100) % 10);
    if (value >= 10)  lcd_writedata('0' + (value / 10) % 10);
    lcd_writedata('0' + (value % 10));
}

// Function to play note based on encoder count
void play_note_based_on_count() {
    if (count % 8 == 0) {
        uint8_t note_index = ((count < 0 ? -count : count) % 64) / 8; // Manual absolute value calculation
        setup_timer1(frequency[note_index]); // Set Timer1 for frequency
        _delay_ms(1000); // Play tone for 1 second
        TCCR1B = 0; // Stop Timer1 after tone
    }
}

// Main loop to update LCD and check for state changes
void loop() {
    if (changed) {
        display_count(count); // Display count on LCD
        play_note_based_on_count(); // Check and play note if count is multiple of 8
        changed = 0; // Reset changed flag
    }
}

int main(void) {
    setup(); // Initialize setup
    while (1) {
        loop(); // Continuously run loop
    }
}
