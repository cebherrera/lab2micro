#include <avr/io.h>
#include <avr/interrupt.h>

volatile int counter = 0; // Variable global, debe ser volatile

int main(void)
{
  sei(); // Se establece el bit I - Global Interrupt Enable

  TCCR0B |= (1 << CS00 ); TCCR0B |= (1 << CS02 ) ; // CLK = CLK_IO[8 MHz]/1024 = 7812.5 HZ -> 1.28*10-4 s

  TCCR0A |= (1 << WGM01 ) ; // Modo CTC 

  //TCCR0A |= (1 << COM0A0 ) ; // Toggle OCOA on Compare Match (PIN B2)
  //DDRB = 0x04; // Se establece el PIN B2 como Salida

  OCR0A = 0xF0; // Valor que se compara con TCNT0

  DDRB = 0x08; // Se establece el PIN B3 como Salida

  TIMSK |= (1 << OCIE0A ) ; // TIFR |= (1 << OCF0A  ) ; // Se habilita la interrupcion por comparacion con OCR0A    

  int estado_LED = 0;

  // Logica Combinacional
  while (1) {
    switch (estado_LED){
    case 1:
        switch(counter){
          case 100:
            PORTB = 0x00; // PIN B3 como Sink (Se apaga LED)
            estado_LED = 0;
            counter = 0;
            break;
          default:
            // Led queda encendido
            break;
        }
        break;
    case 0:
        switch(counter){
          case 100:
            PORTB = 0x08; // PIN B3 como Source (Se enciende LED)
            estado_LED = 1;
            counter = 0;
            break;
          default:
            // Led queda apagado
            break;
        }
      break;
    }
  }
}

  // Logica Secuencial (Interrupciones)
  ISR ( TIMER0_COMPA_vect )
{
  counter = counter + 1; 
}
