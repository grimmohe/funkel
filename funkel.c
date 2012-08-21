#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#define TIMER_INIT_VAL    (255 - 15)    // 1ms
#define X_RES             16

unsigned char led[16];
unsigned long int rotation_time = 1;
unsigned long int time_count = 1;


void init_timer0() {
    TCCR0 = 0x00;       // Stop Timer/Counter0
    TCNT0 = TIMER_INIT_VAL;   // initial value
    TIMSK = _BV(TOIE0); // Overflow Interrupt Enable
    TCCR0 = (1 << CS00) | (1 << CS02);        // 16.000.000/1.024
}

void init_interupt() {
	// disable alternate functions
	TCCR1A &= ~((1 << COM1B0) | (1 << COM1B1));

	GIMSK |= (1<<PCIE0);
	GIMSK |= (1<<INT0);      // turn on interrupts!
	MCUCR |= (1<<ISC00) | (1<<ISC01);

}

/*
 * timer
 */
ISR(TIMER0_OVF0_vect) {
    time_count++;

    TCNT0 = TIMER_INIT_VAL;  // Clear Time/Counter0
}

/*
 * rising edge trigger
 */
ISR (IO_PINS_vect) {
	if (time_count > 2) {
		rotation_time = time_count;

		time_count = 1;
	}
}

void set_leds() {

	PORTA = (led[0] << PA0)
			| (led[1] << PA1)
			| (led[2] << PA2)
			| (led[3] << PA3)
	        | (led[4] << PA4)
	        | (led[5] << PA5)
	        | (led[6] << PA6)
	        | (led[7] << PA7);

	PORTB |= (1 << PB4);
	PORTB = 0xff ^ ((1 << PB4) | (1 << PB5));

	PORTA = (led[8] << PA0)
				| (led[9] << PA1)
				| (led[10] << PA2)
				| (led[11] << PA3)
		        | (led[12] << PA4)
		        | (led[13] << PA5)
		        | (led[14] << PA6)
		        | (led[15] << PA7);

	PORTB |= (1 << PB5);
	PORTB = 0xff ^ ((1 << PB4) | (1 << PB5));

}

void refresh (unsigned char current_x) {

	unsigned long the_one_on = current_x;

	for (int i = 0; i < 16; i++) {
		if (the_one_on == i) {
			led[i] = 1;
		}
		else {
			led[i] = 0;
		}
	}

	set_leds();

}

int main() {
    init_interupt();
    init_timer0();

    DDRA  = 0xFF;
    PORTA = 0xFF;
    DDRB  = (1 << PB4) | (1 << PB5);
    PORTB = 0xFF;

    sei();

    unsigned long last_x = X_RES + 1;
    unsigned long current_x;

    while (1) {
    	current_x = X_RES * time_count / rotation_time;

    	if (current_x == last_x) {
    		asm("sleep"::);
    	}
    	else {
			refresh(current_x);
    	}
    }

    return 0;
}
