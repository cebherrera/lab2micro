#include <avr/io.h>
#include <avr/interrupt.h>

volatile int counter = 0; // Variable global, debe ser volatile

int main(void)
{
  sei(); // Se establece el bit I - Global Interrupt Enable

  // Configuracion de Registros

  TCCR0B |= (1 << CS00 ) | (1 << CS02 ) ; // CLK = CLK_IO[8 MHz]/1024 = 7812.5 HZ -> 1.28*10-4 s (Periodo del Reloj)

  TCCR0A |= (1 << WGM01 ) ; // Modo CTC 

  OCR0A |= 0xF0; // Valor que se compara con TCNT0

  DDRB |= (1 << DDB3 ) | (1 << DDB2 ) | (1 << DDB1 ) | (1 << DDB0 ); // Se establece el PIN B0, B1, B2 y B3 como Salida

  TIMSK |= (1 << OCIE0A ) ; // TIFR |= (1 << OCF0A  ) ; // Se habilita la interrupcion por comparacion con OCR0A    

  MCUCR |= (1 << ISC01 ) | (1 << ISC11 ) ; //Se configura la interrupcion INT0 y INT1 por flanco decreciente

  GIMSK |= (1 << INT1 ) | (1 << INT0 ) ; //Se habilita la interrupcion por INT0 y INT1

  int estado_LED = 0;

  // Logica Combinacional
  while (1) {
    switch (estado_LED){
    case 1:
        switch(counter){
          case 100:
            PORTB &= ~(1 << PORTB0); // PIN B3 como Sink (Se apaga LED)
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
            PORTB = (1 << PORTB0 ); // PIN B3 como Source (Se enciende LED)
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


/*
  ISR ( INT0_vect )
{
  boton = 1; 
}

  ISR ( INT1_vect )
{
  boton = 1; 
}
*/