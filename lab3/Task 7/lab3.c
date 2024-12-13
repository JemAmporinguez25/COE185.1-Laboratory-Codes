#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "lcd.h"  // Declarations of the LCD functions

// Define encoder pins and buzzer pin
#define ENCODER_A PC1
#define ENCODER_B PC5
#define BUZZER_PIN PB4

// Define global variables
volatile int16_t count = 0;           // Count value that will increase/decrease
volatile int16_t last_multiple_of_8 = 0;  // Track last multiple of 8 to detect changes
volatile uint8_t changed = 0;         // Flag to indicate state change
uint8_t old_state = 0;
uint8_t new_state = 0;
uint16_t frequency[8] = { 262, 294, 330, 349, 392, 440, 494, 523 }; // Frequency array
volatile uint16_t isr_count = 0;        // Track number of times ISR needs to run
volatile uint16_t isr_target_count = 0; // Target ISR runs for the tone duration

// Define duration in seconds
#define variable_delay_us  1000000  // Set duration in seconds

// Function prototypes
void timer1_init();
void setup();
void play_note(uint16_t freq);
void display_count(int16_t value);
void play_note_based_on_count();

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

    _delay_ms(1000);  // Delay for 1 second to display the splash screen
    lcd_writecommand(0x01);  // Clear the screen

    // Set encoder pins as inputs with pull-ups
    PORTC |= (1 << ENCODER_A) | (1 << ENCODER_B);

    // Set buzzer pin as output
    DDRB |= (1 << BUZZER_PIN);

    // Initialize encoder state
    old_state = ((PINC & (1 << ENCODER_A)) >> ENCODER_A) | ((PINC & (1 << ENCODER_B)) >> (ENCODER_B - 1));

    // Enable pin change interrupt on Port C
    PCICR |= (1 << PCIE1); // Enable pin change interrupt for Port C
    PCMSK1 |= (1 << PCINT9) | (1 << PCINT13); // Enable PC1 and PC5 in PCMSK1

    // Initialize TIMER1
    timer1_init();

    // Enable global interrupts
    sei();
}

// Initialize TIMER1 for CTC mode without starting the timer
void timer1_init() {
    TCCR1A = 0;               // Set to normal mode
    TCCR1B = (1 << WGM12);    // Set CTC mode (Clear Timer on Compare Match)
    TIMSK1 = (1 << OCIE1A);   // Enable the Output Compare A Match Interrupt
}

// Play a tone based on the given frequency
void play_note(uint16_t freq) {
    OCR1A = (16000000 / (2 * freq)) - 1; // Set the count for the required frequency

    // Calculate the target ISR count for the tone duration (in seconds)
    isr_target_count = (variable_delay_us/1000000  * 2 * freq); // Frequency times two transitions per cycle times duration
    isr_count = 0; // Reset the ISR count

    // Start TIMER1 by setting the prescaler to 1
    TCCR1B |= (1 << CS10);
}

// ISR for pin change interrupt - handles encoder changes
ISR(PCINT1_vect) {
    // Read the new state of the encoder pins
    new_state = ((PINC & (1 << ENCODER_A)) >> ENCODER_A) | ((PINC & (1 << ENCODER_B)) >> (ENCODER_B - 1));
    
    // Check if state has changed and determine rotation direction
    if (new_state != old_state) {
        if ((old_state == 0b00 && new_state == 0b01) ||
            (old_state == 0b01 && new_state == 0b11) ||
            (old_state == 0b11 && new_state == 0b10) ||
            (old_state == 0b10 && new_state == 0b00)) {
            count++;  // Clockwise rotation
        } else {
            count--;  // Counterclockwise rotation
        }
        old_state = new_state;
        changed = 1; // Set the flag to indicate that count has changed
    }
}

// ISR for TIMER1 Compare Match to toggle buzzer
ISR(TIMER1_COMPA_vect) {
    PORTB ^= (1 << BUZZER_PIN); // Toggle buzzer pin PB4 to create the square wave
    isr_count++;

    // Stop TIMER1 after reaching the target count for the tone duration
    if (isr_count >= isr_target_count) {
        TCCR1B &= ~(1 << CS10); // Stop TIMER1 by clearing the prescaler bits
        isr_count = 0;          // Reset count
    }
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

// Function to play note based on count value
void play_note_based_on_count() {
    int16_t multiple_of_8 = (count / 8) * 8;

    if (multiple_of_8 != last_multiple_of_8) { // New multiple of 8 detected
        last_multiple_of_8 = multiple_of_8;
        uint8_t note_index = ((count < 0 ? -count : count) % 64) / 8;
        play_note(frequency[note_index]);
    }
}

// Main loop to update LCD and manage tone playback
void loop() {
    if (changed) {  // Only if the count has changed
        display_count(count);        // Display count on LCD
        play_note_based_on_count();  // Check and play note if count is multiple of 8
        changed = 0;                 // Reset changed flag after processing
    }
}

int main(void) {
    setup(); // Initialize setup
    while (1) {
        loop(); // Continuously run loop
    }
}
