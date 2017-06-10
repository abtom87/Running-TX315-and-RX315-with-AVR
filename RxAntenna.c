#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#define FOSC            16000000UL
#define baud_rate       4800
#define baud            FOSC/16/baud_rate-1
#define SYNC            0xAA
#define RX_ADDR         0x21
#define LED_ON          0x11

char data=0;

typedef enum state_t{LOW,HIGH}state;

void usart_init(unsigned int b_rate)
{
    UBRR0H=0x00;
    UBRR0L=0x00;
    UCSR0B=0x00;
    UCSR0C=0x00;
    UDR0  =0x00;
    
    UBRR0H  =(unsigned char)b_rate>>8;
    UBRR0L  =(unsigned char)b_rate;

    UCSR0B = ( (1<<RXCIE0) | (1<<RXEN0)  | (1<<TXEN0)    );
    UCSR0C = ( (1<<UCSZ00) | (1 << UCSZ01) );
}
void Transmit(unsigned char c)
{
   while( !(UCSR0A & (1<<UDRE0) ) );
     UDR0=c;
}


void set_Debug_LED()
{
    PORTC |= (1<<PC0);
}
void reset_Debug_LED()
{
    PORTC &= ~(1<<PC0);
}

void send_debug_msg(char *str)
{
   Transmit(*str);
   while( *(++str)!=0)
      Transmit(*str);

}

void check_valid_payload()
{
   
   if(data==RX_ADDR)
   { 
      send_debug_msg("---ADDR OK ---");
      _delay_ms(10);
      
      if(data==LED_ON)
      {
         set_Debug_LED();
         send_debug_msg("---LED ON---");
         _delay_ms(50);
      }                        
                      
      if(data==LED_OFF)
      {
      
         reset_Debug_LED();
         send_debug_msg("---LED OFF ---");
         _delay_ms(50);
         
      }
   }
   
   
   
}
int main (void)
{
   DDRC  = 0xFF;
   PORTC = 0xFE;

   sei();
   usart_init(baud);
   
   while(1)
   {
      check_valid_payload();
     _delay_ms(5); 
   }
   return 0;
}

ISR(USART_RX_vect )
{
   data=UDR0;
}





