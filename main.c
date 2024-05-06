//Project Name : Automatic Battery Charger (ABC)
//Developer : Ahmad Zamani
//Gmail : ahmadzamani9831

#define F_CPU 8000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <stdlib.h>
#include "LCD.h"

void ADC_Init(){
	DDRC &= ~(1<<0);
	DDRC &= ~(1<<1);
	DDRC &= ~(1<<2);
	ADCSRA = 0b10000111;
	ADMUX = 0b01000000;
}


float Read_V(){
	int ADCL_V,ADCH_V;
	float Value_V;
	ADMUX = 0b01000000;
	ADCSRA |= (1<<ADSC);
	while ( ADIF == 0 );
	ADCL_V = (int)ADCL;
	ADCH_V = (int)ADCH * 256;
	ADCH_V = ADCL_V + ADCH_V ;
	Value_V = ADCH_V * 15 / 1023;
	return (Value_V);
}

float Read_A(){
	int ADCL_A,ADCH_A;
	float Value_A;
	ADMUX = 0b01000001;
	ADCSRA |= (1<<ADSC);
	while ( ADIF == 0 );
	ADCL_A = (int)ADCL;
	ADCH_A = (int)ADCH * 256;
	ADCH_A = ADCL_A + ADCH_A ;
	Value_A = ADCH_A * 50 / 1023;
	return (Value_A);
}

float Read_T(){
	int ADCL_T,ADCH_T;
	float Value_T;
	ADMUX = 0b01000010;
	ADCSRA |= (1<<ADSC);
	while ( ADIF == 0 );
	ADCL_T = (int)ADCL;
	ADCH_T = (int)ADCH * 256;
	ADCH_T = ADCL_T + ADCH_T ;
	Value_T = ADCH_T * 500 / 1023;
	return (Value_T);
}

int main(void) {
	LCD_Init();
	ADC_Init();
	DDRD = 0b11000000;
	float VBAT,ABAT,TBAT;
	char SVBAT[5],SABAT[5],STBAT[5];
	while (1) {
		VBAT = Read_V();
		dtostrf(VBAT,5,2,SVBAT);
		TBAT = Read_T();
		dtostrf(TBAT,5,2,STBAT);
		if (VBAT>13.7) {
			LCD_String_xy(0,0,"BAT is full");
			LCD_String_xy(1,0,"V:");
			LCD_String_xy(1,2,SVBAT);
			_delay_ms(50);
			LCD_Clear();
		}
		else if (8<=VBAT&&VBAT<=13.7) {
			if (TBAT>=60) {
				PORTD |= (1<<6);
				LCD_String_xy(0,0,"BAT is hot!");
				LCD_String_xy(1,0,"T:");
				LCD_String_xy(1,2,STBAT);
				_delay_ms(50);
				LCD_Clear();
				PORTD &= ~(1<<6);
			}
			else if (40<TBAT&&TBAT<60) {
				PORTD |= (1<<6);
				PORTD |= (1<<7);
				LCD_String_xy(1,0,"V:");
				LCD_String_xy(1,2,SVBAT);
				for (int X=0;X<28;X++) {
					ABAT = Read_A() * 10;
					dtostrf(ABAT,5,2,SABAT);
					LCD_String_xy(1,10,"A:");
					LCD_String_xy(1,12,SABAT);
					LCD_String_xy(0,0,"BAT is charging");
					_delay_ms(500);
					LCD_String_xy(0,0,"BAT is hot!    ");
					_delay_ms(500);
				}
				PORTD &= ~(1<<6);
				PORTD &= ~(1<<7);
				_delay_ms(2000);
				LCD_Clear();
			}
			else if (40>=TBAT) {
				PORTD |= (1<<7);
				LCD_String_xy(0,0,"BAT is charging");
				LCD_String_xy(1,0,"V:");
				LCD_String_xy(1,2,SVBAT);
				for (int Y=0;Y<56;Y++) {
					ABAT = Read_A() * 10;
					dtostrf(ABAT,5,2,SABAT);
					LCD_String_xy(1,10,"A:");
					LCD_String_xy(1,12,SABAT);
					_delay_ms(500);
				}
				PORTD &= ~(1<<7);
				_delay_ms(2000);
				LCD_Clear();
			}
		}
		else if (0.02<VBAT&&VBAT<8) {
			LCD_String_xy(0,0,"BAT is broken");
			LCD_String_xy(1,0,"V:");
			LCD_String_xy(1,2,SVBAT);
			_delay_ms(50);
			LCD_Clear();
		}
		else if (VBAT<=0.02) {
			LCD_String_xy(0,0,"BAT is");
			LCD_String_xy(1,0,"not connected");
			_delay_ms(50);
			LCD_Clear();
		}
	}
}
