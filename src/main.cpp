// Clock multiplier
// By Simon Juhl
// www.simonjuhl.net
// Circuit Diagram: www.github.com/simonjuha/ATTiny85-beat-clock-multiplier/blob/master/diagram.png
// This version uses ATtiny13a, but it should work with ATtiny85 as well (untested)
// No arduino libraries are used, only AVR libraries

/*
    PIN MAPPING:
    PB0: clock multiplier input (x4) <--
    PB1: clock multiplier input (x3) <--    
    PB2: clock out -->
    PB3: clock in  <--
    PB4: clock multiplier input (x2) <--
*/

#include <avr/io.h>
#include <avr/interrupt.h>
#define __AVR_ATtiny13a__ // use __AVR_ATtiny85__ for ATtiny85 (untested)

#if defined(__AVR_ATtiny13a__)
    // remeber to set fuses: lfuse: 0x7a, hfuse: 0xff when using ATtiny13a
    #define ISR_VECTOR TIM0_COMPA_vect
    #define F_CPU 9600000UL  // 9.6 MHz
#elif defined(__AVR_ATtiny85__)
    #define ISR_VECTOR TIMER0_COMPA_vect
    #define F_CPU 800000UL  // 8MHz or 16MHz
#endif

// Interrupt rate formula: (F_CPU) / (prescale * (1 + OCR0A))
#define INTERRUPT_RATE 20000 // 20 kHz
#define MS_TIMER_TICKS INTERRUPT_RATE / 1000

#define CLOCK_MULTIPLIER_4_PIN  PB0
#define CLOCK_MULTIPLIER_3_PIN  PB1
#define CLOCK_OUT_PIN           PB2
#define CLOCK_IN_PIN            PB3
#define CLOCK_MULTIPLIER_2_PIN  PB4

volatile static uint32_t timePassed_ms = 0; // Milliseconds since startup

uint32_t millis() {
    return timePassed_ms;
}

void init_beat_clock_multiplier(){

    /* -------- Pin setup -------- */
    DDRB |= (1 << CLOCK_OUT_PIN);           // Set clock out pin as output
    DDRB &= ~(1 << CLOCK_IN_PIN);           // Set clock in pin as input
    DDRB &= ~(1 << CLOCK_MULTIPLIER_2_PIN); // Set clock multiplier pins as input
    DDRB &= ~(1 << CLOCK_MULTIPLIER_3_PIN); // ...
    DDRB &= ~(1 << CLOCK_MULTIPLIER_4_PIN); // ...

    /* -------- Timer setup (Timer0) -------- */
    // Interrupt rate formula: (F_CPU) / (prescale * (1 + OCR0A))
    // 9.6 MHz / (8 * (1 + 57)) = 20.7 kHz
    TCCR0A =   (1<<WGM00) | (1<<WGM01);  // Set timer to CTC mode
    TCCR0B |=   (1<<WGM02);              // ...
    TCCR0B |= 1<<CS01;                   // Set prescale to 8
    #if defined(__AVR_ATtiny13a__)
        TIMSK0  = 1<<OCIE0A;             // Enable interrupt on compare match A
    #elif defined(__AVR_ATtiny85__)
        TIMSK  = 1<<OCIE0A;              // Enable interrupt on compare match A
    #endif
    OCR0A   = 57;       // Compare value
    sei();              // Enable interrupts
}

void tick_beat_clock_multiplier() {
    static bool last_clock_in_state = false;
    static uint32_t last_rise_time = 0;
    static uint32_t last_pulse_time = 0;
    static uint32_t pulse_interval = 0;
    static uint8_t pulse_length = 8;

    uint8_t multiplier = 1;

    // Read the state of the multiplier pins and calculate the multiplier
    multiplier *= (PINB & (1 << CLOCK_MULTIPLIER_2_PIN)) ? 2 : 1;
    multiplier *= (PINB & (1 << CLOCK_MULTIPLIER_3_PIN)) ? 3 : 1;
    multiplier *= (PINB & (1 << CLOCK_MULTIPLIER_4_PIN)) ? 4 : 1;

    // Read the state of the clock input pin
    bool clock_in_state = PINB & (1 << CLOCK_IN_PIN);

    // Handle rising edge of clock input
    if (clock_in_state && !last_clock_in_state) {
        if (last_pulse_time > 0) {
            pulse_interval = (millis() - last_pulse_time) / multiplier;
        }
        last_pulse_time = millis();
        last_rise_time = millis();
        PORTB |= (1 << CLOCK_OUT_PIN); // Set clock out pin high
    }

    // Handle clock pulse
    if ((millis() - last_rise_time) >= pulse_interval) {
        PORTB |= (1 << CLOCK_OUT_PIN); // Set clock out pin high
        last_rise_time = millis();
    }

    // Handle pulse length
    if ((millis() - last_rise_time) >= pulse_length) {
        PORTB &= ~(1 << CLOCK_OUT_PIN); // Set clock out pin low
    }

    // Save the current state as last state for the next iteration
    last_clock_in_state = clock_in_state;
}

int main() {
    init_beat_clock_multiplier(); // initiate the clock multiplier

    while(1) {
        tick_beat_clock_multiplier(); // tick the clock multiplier
        // ... interrupt will handle millis ...
    }

    return 0;
}

// regular interval interrupt
ISR(ISR_VECTOR){
    static uint8_t msCounter = 0; // Counter for milliseconds
    if(msCounter == MS_TIMER_TICKS) {
        msCounter = 0;
        timePassed_ms++;
    } else {
        msCounter++;
    }
}