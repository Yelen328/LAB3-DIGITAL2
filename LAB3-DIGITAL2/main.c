/*
 * LAB3-DIGITAL2.c
 *
 * Created: 3/2/2026 01:00:43
 * Author : yelen
 */ 

#define  F_CPU	16000000
#include "util/delay.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <stdint.h>
#include "SPI/SPI.h"
#include "UART/UART.h"

uint8_t valorSPI	=0;
uint8_t valorSPI2	=0;
uint8_t entero=0;
uint8_t decimal=0;


//PROTOTIPOS DE FUNCIÓN:
void RefreshPORT(uint8_t VALOR);
void setup();
void decimales(uint8_t VOLTAJE);

void setup(){
	//SALIDAS
	//puerto de salida para leds (8 leds)
	DDRC |=(1<<DDC0);
	
	DDRD |= (1<<DDD2)|(1<<DDD3)|(1<<DDD4)|(1<<DDD5)|(1<<DDD6)|(1<<DDD7);	//declarar el puerto D como salidas
	PORTD = 0x00;	//Estado inicial apagado
	
	//B2=MSS para seleccionar el esclavo=1
	//Declarar los bits 0 y 1 del puerto B como salida
	DDRB |= (1<<DDB0)|(1<<DDB1)|(1<<DDB2);	
	PORTB = 0x00;	//Inicialmente apagado
	
	
	SPI_INIT(SPI_MASTER_OSC_DIV128, SPI_DATA_ORDER_MSB, SPI_CLOCK_IDLE_LOW, SPI_CLOCK_FIRST_EDGE );
	INIT_UART(103);
}

int main(void)
{
    setup();
	_delay_ms(1000);
    while (1) 
    {
		PORTB &= ~(1<<PORTB2);	//Slave select=0;
		
		SPI_WRITE(0);
		//SPI_WRITE(0x00);
		
		valorSPI=SPI_READ();	//Lee la información del SPI
		
		
		
		PORTC|= (1<<PORTC0);	//Slave select = 1
		
		SPI_WRITE(1);
		
		//SPI_WRITE(0x00);
		valorSPI2=SPI_READ();	//Lee la información del SPI
		//RefreshPORT(valorSPI2);	//Una vez leído, lo carga al puerto
		
		SPI_WRITE(caracter);
		PORTB|= (1<<PORTB2);	//Slave select = 1
		
		
		char buffer[32];

		decimales(valorSPI);
		sprintf(buffer, "S1: %d.%02dV\r\n", entero, decimal);
		writeString(buffer);

		decimales(valorSPI2);
		sprintf(buffer, "S2: %d.%02dV\r\n", entero, decimal);
		writeString(buffer);
		
		
		
		_delay_ms(250);
		
    }
}


void RefreshPORT(uint8_t VALOR){
	PORTD = (PORTD & 0x03) | ((VALOR << 2) & 0xFC);	// Muestra los primeros 6 bits en D2-D7
	PORTB = (PORTB & 0xFC) | ((VALOR >> 6) & 0x03);	// Muestra lo otros 2 bits en B0 y B1
}

void decimales(uint8_t VOLTAJE){
	float conversion=((VOLTAJE*5.0f)/255.0f);
	entero = (uint8_t)conversion;
	float parte_decimal = conversion - (float)entero;
	decimal = (uint8_t)(parte_decimal * 100.0f);
}

ISR (USART_RX_vect){
	uint8_t caracter = UDR0;
	//WriteChar(caracter);	//Envía de vuelta el mismo carácter resibido	
	RefreshPORT(caracter);

}