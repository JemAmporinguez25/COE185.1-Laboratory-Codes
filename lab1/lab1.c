/********************************************
*
*  Name: Jemuel Amporinguez
*  Email: jemuel.amporinguez@g.msuiit.edu.ph
*  Lab section: M98
*  Assignment: Lab 1 - Arduino Input and Output
*
********************************************/

#include <avr/io.h>
#include <util/delay.h>

#define DOT_LENGTH  250         /* Define the length of a "dot" time in msec */
#define DASH_LENGTH  500    /* Define the length of a "dot" time in msec */

void dot(void);
void dash(void);
void makeOutput(char);
char checkInput(char);

int main(void)
{
    DDRD |= (1 << DDD2);   // Set PD2 as output
    PORTB |= (1 << PB5) | (1 << PB4) | (1 << PB3);
    // Enable pull-up resistors for the buttons on Port B
    //PORTB |= (1 << PB0) | (1 << PB1) | (1 << PB2) | (1 << PB3); 
   
    // Initialize appropriate DDR registers
	
    // Initialize the LED output to 0
	
    // Enable the pull-up resistors for the 
    // 3 button inputs 
	

    // Loop forever
   // while (1) {                 
    
	//  Use "if" statements and the checkInput()
	//  function to determine if a button
	//  is being pressed and then output
	//  the correct dot/dash sequence by
	//  calling the dot() and dash(), and 
	//  using appropriate delay functions
   while (1) {
 
    if (checkInput(PB3) != 0) {// Is button 1 pressed?
      

        
    dash(); 
    _delay_ms(DASH_LENGTH); 
    dash();
    _delay_ms(DASH_LENGTH); 
    makeOutput(0);
    _delay_ms(DASH_LENGTH*2);
    }
    if (checkInput(PB4) != 0) {   
      
    
    dot();
    _delay_ms(DOT_LENGTH);   
    dot();
    _delay_ms(DOT_LENGTH); 
    dot();
    _delay_ms(DASH_LENGTH*2); 
    

    }
    if  (checkInput(PB5) != 0){ // Changed from 2 to 3 for pin 11
      
     
    dot();
    _delay_ms(DOT_LENGTH); 
    dot();
    _delay_ms(DOT_LENGTH); 
    dash();
    _delay_ms(DASH_LENGTH*2);
     
    
    }
   }
   
    return 0;   /* never reached */
}

/*
  dot() - Makes the output LED blink a "dot".

  Write code to generate a dot by using the makeOutput function
  to turn the output ON and OFF with appropriate delays.
  Be sure you don't forget to also delay an
  appropriate time after you turn the output off.
*/
void dot()
{
    makeOutput(1);
    _delay_ms(DOT_LENGTH);
    makeOutput(0);

}

/*
  dash() - Makes the output LED blink a "dash".

  Write code to generate a dash by using the makeOutput function
  to turn the output ON and OFF with appropriate delays.
  Be sure you don't forget to also delay an
  appropriate time after you turn the output off.
*/
void dash()
{
    makeOutput(1);
    _delay_ms(DASH_LENGTH);
    makeOutput(0);
    
}

/*
  makeOutput() - Changes the output bit (Group D, bit 2) to either
  a zero or one, based on the input argument "value".
  
  If the argument is zero, turn the output OFF,
  otherwise turn the output ON. 
  
  Do not use any delays here.  Just use bit-wise operations
  to make the appropriate PORT bit turn on or off.
*/
void makeOutput(char value) {
    if (value == 0) {
       PORTD &= ~(1 << PD2); // Clear PD2 (set to 0)
    } else {
        PORTD |= (1 << PD2);  // Set PD2 (set to 1)
    }
}


char checkInput(char bit)
{
  if ((PINB & (1 << bit))){
    return 0;
  }
  else{
    return 1;
  }
}