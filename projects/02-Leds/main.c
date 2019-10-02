/*
 * ---------------------------------------------------------------------
 * Author:      Tomas Fryza
 *              Dept. of Radio Electronics, Brno Univ. of Technology
 * Created:     2018-09-27
 * Last update: 2019-09-26
 * Platform:    ATmega328P, AVR 8-bit Toolchain 3.6.2
 * ---------------------------------------------------------------------
 * Description: Blink a LED with delay function.
 * TODO: Verify functionality of LED blinker.
 * NOTE: Macro _BV converts a bit number into a byte value (see AVR Libc
 * Reference Manual).
 */

/* Includes ----------------------------------------------------------*/
#include <avr/io.h>
#include <util/delay.h>

/* Typedef -----------------------------------------------------------*/
/* Define ------------------------------------------------------------*/
#define LED_GREEN   PB5
#define LED_2   PB0
#define SWITCH   PD2
#define BLINK_DELAY 250

/* Variables ---------------------------------------------------------*/
/* Function prototypes -----------------------------------------------*/

/* Functions ---------------------------------------------------------*/
/**
  * Brief:  Main program. Blink a LED with delay function.
  * Input:  None
  * Return: None
  */
int main(void)
{
    /* Set output pin */
    DDRB |= _BV(LED_GREEN);  
    DDRB |= _BV(LED_2);       /* DDRB = DDRB or (0010 0000) */
    DDRD &= ~_BV(SWITCH);
    PORTD |= _BV(SWITCH);

    /* Turn LED off */
    PORTB &= ~_BV(LED_GREEN);       /* PORTB = PORTB and (0010 0000) */
    PORTB &= ~_BV(LED_2);

    /* Infinite loop */
    for (;;)
    {
        /* Invert LED and delay */
        if(bit_is_clear(PIND, SWITCH))
        {
    
        PORTB ^= _BV(LED_GREEN);    /* PORTB = PORTB xor (0010 0000) */
        PORTB ^= _BV(LED_2);
        _delay_ms(BLINK_DELAY);     /* Wait for several milisecs */

        }
        PORTB ^= _BV(LED_GREEN);
        _delay_ms(BLINK_DELAY);
    }

    return (0);
}
