/********************************************
*
*  Name: Jemuel Amporinguez
*  Email: jemuel.amporinguez@g.msuiit.edu.ph
*  Section:
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
    while(1){
        lcd_write(1);
    }
}
