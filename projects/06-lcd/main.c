/*
 * ---------------------------------------------------------------------
 * Author:      Tomas Fryza
 *              Dept. of Radio Electronics, Brno Univ. of Technology
 * Created:     2018-10-16
 * Last update: 2019-10-25
 * Platform:    ATmega328P, 16 MHz, AVR 8-bit Toolchain 3.6.2
 * ---------------------------------------------------------------------
 * Description:
 *    Decimal counter with data output on LCD display.
 * 
 * Note:
 *    Modified version of Peter Fleury's LCD library with R/W pin 
 *    connected to GND. Newline symbol "\n" is not implemented, use
 *    lcd_gotoxy() function instead.
 */

/* Includes ----------------------------------------------------------*/
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdlib.h>             // itoa() function
#include "timer.h"
#include "lcd.h"
#include <string.h>

uint8_t temp10;
uint8_t vyber;
char desitkova [4];
char sestnactkova[3];
char dvojkova[9];
char delka[2];
char lcd_user_symbols[16] = {
  0x0C,
  0x0D,
  0x05,
  0x1F,
  0x05,
  0x04,
  0x0A,
  0x0A,
  0x0E,
  0x11,
  0x1B,
  0x11,
  0x15,
  0x0E,
  0x0E,
  0x0E
};
/* Typedef -----------------------------------------------------------*/
/* Define ------------------------------------------------------------*/
/* Variables ---------------------------------------------------------*/
/* Function prototypes -----------------------------------------------*/
void bargr(uint8_t value)
    {
        for(uint8_t i = 1; i <= 16; i++)    
        {
            lcd_gotoxy(i - 1,0);
            lcd_gotoxy(i - 1,1);
            
        
        if(value >= ((255*i)/16)-1)
        {
        lcd_putc(0x00);
        }
        }
        if(value ==0)
        {
            
            lcd_gotoxy(0,1);
            lcd_puts("                ");
            lcd_gotoxy(10,0);
            lcd_puts("   ");
            
        }
        if (value == 255){
            vyber = 1;
            lcd_gotoxy(0,1);
            lcd_puts("                ");
            lcd_gotoxy(0,0);
            lcd_puts("Counter:");
            lcd_gotoxy(6,1);
            lcd_puts("0b");
            lcd_gotoxy(1,1);
            lcd_puts("0x");
        }
    }
void pocitani()
{
    lcd_gotoxy(3,1);
    lcd_puts(sestnactkova);
    int size = strlen(dvojkova);

    /*itoa(size, delka, 10);                ---------    zobrazeni delky aktualniho cisla v bin soustave v pravem hornim rohu
    lcd_gotoxy(15, 0);
    lcd_puts(delka);
    */
    
    lcd_gotoxy(16-size,1);
    lcd_puts(dvojkova);
    lcd_gotoxy(10,0);
    lcd_puts(desitkova);
    if(temp10 ==0)
        {
            
            lcd_gotoxy(10,0);
            lcd_puts("   ");
            lcd_gotoxy(8,1);
            lcd_puts("00000000");
            lcd_gotoxy(3,1);
            lcd_puts("  ");
            
        }
    if (temp10 == 255){
            vyber = 0;
            lcd_clrscr();
            lcd_gotoxy(8,0);
            lcd_putc(0x00);
        }
}
/* Functions ---------------------------------------------------------*/
/**
 *  Brief:  Main program. Shows decimal values ​​on LCD display.
 *  Input:  None
 *  Return: None
 */

int main(void)
{
    /* LCD display
     * TODO: See Peter Fleury's online manual for LCD library 
     * http://homepage.hispeed.ch/peterfleury/avr-software.html
     * Initialize display and test different types of cursor */
    lcd_init(LCD_DISP_ON);



    // Set pointer to beginning of CG RAM memory
    lcd_command(1<<LCD_CGRAM);
    // Store new characters line by line
    for(int i = 0; i<16; i++)
    {
    lcd_data(lcd_user_symbols[i]);
    }

    // Store new characters line by line
    
    // Clear display and set cursor to home position
    lcd_clrscr();
    vyber = 0;
    // Display first user - defined character
    lcd_gotoxy(15,0);
    lcd_putc(0x00);

    // Display string without auto linefeed

   
    

    /*lcd_gotoxy(8,1);
    lcd_puts("00000000");
    */
    // TODO: Display variable value in decimal, binary, and hexadecimal
    //----------------------------------------------------------------------------------------------------
    
    
    
    
    
    //----------------------------------------------------------------------------------------------------
    /* Timer1
     * TODO: Configure Timer1 clock source and enable overflow 
     *       interrupt */
    TIM_config_prescaler(TIM1, TIM_PRESC_8);
    TIM_config_interrupt(TIM1, TIM_OVERFLOW_ENABLE);
    /* TODO: Design at least two user characters and store them in 
     *       the display memory */

    // Enables interrupts by setting the global interrupt mask
    sei();

    // Infinite loop
    for (;;) {
    }

    // Will never reach this
    return (0);
}

/**
 *  Brief: Timer1 overflow interrupt routine. Increment counter value.
 */
ISR(TIMER1_OVF_vect)
{
    temp10++;
    
    itoa(temp10, desitkova, 10);
    itoa(temp10, sestnactkova, 16);
    itoa(temp10, dvojkova, 2);

    
    
    if(vyber == 0) {
    bargr(temp10);
    }
    else 
    {
    pocitani();  
    }
   
}