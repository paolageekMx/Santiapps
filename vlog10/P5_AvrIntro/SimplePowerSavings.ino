#include <avr/wdt.h>            // library for default watchdog functions
#include <avr/interrupt.h>      // library for interrupts handling
#include <avr/sleep.h>          // library for sleep
#include <avr/power.h>          // library for power control

int nbr_remaining; // how many times remain to sleep before wake up
#define led 13

// interrupt raised by the wdt firing (when the wdt fires during sleep, this function will be executed).  remember that interrupts are disabled in ISR functions
ISR(WDT_vect){// not hanging, just waiting
        wdt_reset();// reset the watchdog
}

// function to configure the wdt: let it sleep 8 seconds before firing.  when firing, configure it for resuming program execution
void configure_wdt(void){
  cli();                           // disable interrupts for changing the registers
  MCUSR = 0;                       // reset status register flags
                                   // Put timer in interrupt-only mode:                                       
  WDTCSR |= 0b00011000;            // Set WDCE (5th from left) and WDE (4th from left) to enter config mode,
                                   // using bitwise OR assignment (leaves other bits unchanged).
  WDTCSR =  0b01000000 | 0b100001; // set WDIE: interrupt enabled = seconds
                                   // clr WDE: reset disabled
                                   // and set delay interval (right side of bar) to 8 seconds
  sei();                           // re-enable interrupts
  // reminder of the definitions for the time before firing
  // delay interval patterns:
  //  16 ms:     0b000000
  //  500 ms:    0b000101
  //  1 second:  0b000110
  //  2 seconds: 0b000111
  //  4 seconds: 0b100000
  //  8 seconds: 0b100001
}

void sleep(int ncycles) {// Put the Arduino to deep sleep. Only an interrupt can wake it up.  
  nbr_remaining = ncycles; // defines how many cycles should sleep
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);// Set sleep to full power down.  Only external interrupts or the wdt timer can wake the CPU!
  power_adc_disable();  // Turn off the ADC while asleep.
 
  while (nbr_remaining > 0){ // while some cycles left, sleep!
    sleep_mode();  // Enable sleep and enter sleep mode.
    // CPU is now asleep. program execution completely halts!  Once awake, execution will resume at this point if the wdt is configured for resume rather than restart
    sleep_disable();  // When awake, disable sleep mode
    nbr_remaining = nbr_remaining - 1;   // we have slept one time more 
  } 
  power_all_enable();  // put everything on again
}

void setup(){  
  pinMode(led, OUTPUT);
  digitalWrite(led, LOW); //turn off led13
  delay(1000);
  configure_wdt();  // configure the watchdog.  wdt is set to bite, unless you pet it...

  digitalWrite(led, HIGH);     // blink twice
  delay(500);               
  digitalWrite(led, LOW);   
  delay(500); 
  digitalWrite(led, HIGH);   
  delay(500);               
  digitalWrite(led, LOW);   
  delay(500); 

}

void loop(){
  sleep(5);  // sleep for a given number of cycles (here, 5 * 8 seconds) in lowest power mode
  // usefull stuff should be done here before next long sleep, like Call wifi-startup() & wifi-getpost() and blink 3x before going back to sleep
  digitalWrite(led, HIGH);   
  delay(500);               
  digitalWrite(led, LOW);   
  delay(500); 
  digitalWrite(led, HIGH);   
  delay(500);               
  digitalWrite(led, LOW);   
  delay(500); 
  digitalWrite(led, HIGH);   
  delay(500);               
  digitalWrite(led, LOW);   
  delay(500); 
}