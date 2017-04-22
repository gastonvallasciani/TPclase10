/*============================================================================
 * Licencia: 
 * Autor: 
 * Fecha: 
 *===========================================================================*/
/*============================================================================
/* Copyright 2016, Eric Pernia.
 * All rights reserved.
 *
 * This file is part sAPI library for microcontrollers.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 */
 
/*
 * Date: 2016-07-28
 */
 



/*==================[inlcusiones]============================================*/

#include "stdint.h"
#include "driverDisplay.h"   // <= su propio archivo de cabecera
#include "sapi.h"       // <= Biblioteca sAPI

/*==================[definiciones y macros]==================================*/

/*==================[definiciones de datos internos]=========================*/

/*==================[definiciones y macros]==================================*/

/*==================[definiciones de datos internos]=========================*/


// Configuraci�n de pines del display
/*static uint8_t display7SegmentosPines[8] = {
   GPIO5, // a
   GPIO7, // b
   GPIO6, // c
   GPIO1, // d
   GPIO2, // e
   GPIO4, // f
   GPIO3, // g
   GPIO8  // h = dp
};
*/
// Configuraci�n de pines del display
static uint8_t display7SegmentosPines[8] = {
   GPIO0, // a
   GPIO1, // b
   GPIO2, // c
   GPIO3, // d
   GPIO4, // e
   GPIO5, // f
   GPIO6, // g
   GPIO7  // h = dp
};
static uint8_t display4DigitosPines[4] = {
   LCD1, // Digito 1
   LCD2, // Digito 2
   LCD3, // Digito 3
   LCD4 // Digito 4
};
// Tavla de simbolos formados con los segmentos 
/*
------------+------+---------
  Segmentos | HEX  | Simbolo
------------+------+---------
   hgfedcba |      |
 0b00111111 | 0x0F |   0
 0b00000110 | 0x00 |   1
 0b01011011 | 0x00 |   2
 0b01001111 | 0x00 |   3
 0b01100110 | 0x00 |   4
 0b01101101 | 0x00 |   5
 0b01111101 | 0x00 |   6
 0b00000111 | 0x00 |   7
 0b01111111 | 0x00 |   8
 0b01101111 | 0x00 |   9

 0b01011111 | 0x00 |   a
 0b01111100 | 0x00 |   b
 0b01011000 | 0x00 |   c
 0b01011110 | 0x00 |   d
 0b01111011 | 0x00 |   e
 0b01110001 | 0x00 |   F

 0b01110111 | 0x00 |   A
 0b00111001 | 0x00 |   C
 0b01111001 | 0x00 |   E
 0b01110110 | 0x00 |   H
 0b00011110 | 0x00 |   J
 0b00111000 | 0x00 |   L
 0b01110011 | 0x00 |   P
 0b00111110 | 0x00 |   U

 0b10000000 | 0x00 |   .

             a
           -----
       f /     / b
        /  g  /
        -----
    e /     / c
     /  d  /
     -----    O h = dp (decimal point).

*/
static uint8_t display7SegmentosValores[27] = {
   0b00111111, // 0
   0b00000110, // 1
   0b01011011, // 2
   0b01001111, // 3
   0b01100110, // 4
   0b01101101, // 5
   0b01111101, // 6
   0b00000111, // 7
   0b01111111, // 8
   0b01101111, // 9

   0b01011111, // a
   0b01111100, // b
   0b01011000, // c
   0b01011110, // d
   0b01111011, // e
   0b01110001, // f

   0b01110111, // A
   0b00111001, // C
   0b01111001, // E
   0b01110110, // H
   0b00011110, // J
   0b00111000, // L
   0b01110011, // P
   0b00111110, // U

   0b10000000, // .

   0b00000000,  // display off
   0b01000000  // MENOS (-)
};

static uint16_t digitoDisplay[4]={NULO,NULO,NULO,NULO};
static uint16_t currDigit;
static delay_t delayRefresco;
/*==================[definiciones de datos externos]=========================*/

/*==================[declaraciones de funciones internas]====================*/
static void selectDigit(uint8_t d);
static void display7SegmentosTestearPines( void );
static void display7SegmentosConfigurarPines( void );
static void display7SegmentosMostrar( uint8_t symbolIndex );

/*==================[declaraciones de funciones externas]====================*/
void driverDisplayShf(uint16_t digito,uint16_t vDisplay[4]);
void driverDisplayLimpiar(uint16_t vDisplay[4] );
void driverDisplayMostrar(uint16_t vDisplay[4]);
void driverDisplayInit( void );
void driverDisplayActualizar( void );

/*==================[definiciones de funciones internas]=====================*/
static void selectDigit(uint8_t d)
{
  int i;
  
  for(i=0;i<4;i++)
    gpioWrite(display4DigitosPines[i],OFF);
  gpioWrite(display4DigitosPines[d],ON);
}

static void display7SegmentosTestearPines( void ){
   /*
   -------+------------+-----------+--------------------------
     GPIO | Valor BIN  | Valor HEX | Segmento encendido
   -------+------------+-----------+--------------------------
    GPIO1 | 0b00000001 |   0x01    | Enciende el segmento 'd'.
    GPIO2 | 0b00000010 |   0x02    | Enciende el segmento 'e'.
    GPIO3 | 0b00000100 |   0x04    | Enciende el segmento 'g'.
    GPIO4 | 0b00001000 |   0x08    | Enciende el segmento 'f'.
    GPIO5 | 0b00010000 |   0x10    | Enciende el segmento 'a'.
    GPIO6 | 0b00100000 |   0x20    | Enciende el segmento 'c'.
    GPIO7 | 0b01000000 |   0x40    | Enciende el segmento 'b'.
    GPIO8 | 0b10000000 |   0x80    | Enciende el segmento 'h' = dp (punto decimal).
   -------+------------+-----------+--------------------------
                a
              -----
          f /     / b
           /  g  /
           -----
       e /     / c
        /  d  /
        -----    O h = dp

   */
   uint8_t i = 0;
   for(i=0;i<=7;i++){
      gpioWrite( display7SegmentosPines[i], OFF  );
      if( i == 0 )
         gpioWrite( LEDB, ON );
      delay(1000);
      gpioWrite( display7SegmentosPines[i], ON );
      if( i == 0 )
         gpioWrite( LEDB, OFF );
   }
}

// Configuraci�n de pines para el display 7 segmentos
static void display7SegmentosConfigurarPines( void ){   
   uint8_t i = 0;
   for(i=0;i<=7;i++)
      gpioConfig( display7SegmentosPines[i], GPIO_OUTPUT );
   for(i=0;i<4;i++){
      gpioConfig( display4DigitosPines[i], GPIO_OUTPUT );
//      gpioConfig( display4DigitosPines[i],OFF);
   }      
}

// Mostrar dato en el display 7 segmentos
static void display7SegmentosMostrar( uint8_t symbolIndex ){
   uint8_t i = 0;
   for( i=0; i<=7; i++ )
      gpioWrite( display7SegmentosPines[i], ~display7SegmentosValores[symbolIndex] & (1<<i) );
}


/*==================[definiciones de funciones externas]=====================*/
/*
* driverDisplayShf ingresa un caracter por la derecha del display y corre los*
* caracteres del display un lugar a la derecha                               *
*/
void driverDisplayShf(uint16_t digito, uint16_t vDisplay[4])
{
  int i;
  
  for(i=1;i<4;i++)
  {
    //copiar vDisplay[i] en [i-1]
  }
//vDisplay[3] asignarle el d'igito;
}

/*
* driverDisplayMostrar copia la instancia vDisplay en la variable digitoDisplay *
* que es la variable que tiene los caracteres a mostrar                         *
* Tenemos dos instancias vDisplay; una para el teclado y otra para el ascensor  *
*/
void driverDisplayMostrar(uint16_t *vDisplay)
{
  int i;
  for(i=0;i<4;i++)
  {
    //copiar vDisplay[i] en digitoDisplay[i]
  } 
}

/*
* driverDisplayLimpiar apaga todos los d'igitos de la instancia vDisplay en     *
*/
void driverDisplayLimpiar(uint16_t vDisplay[4] )
{
  int i;

  for(i=0;i<4;i++)
    vDisplay[i]=NULO;
}


/*
* driverDisplayInit inicializa el display apagando los d'igitos, configura los  *
* pines y el periodo de refresco
*/
void driverDisplayInit( void )
{

  currDigit=0;
  digitoDisplay[0]=digitoDisplay[1]=digitoDisplay[2]=digitoDisplay[3]=NULO;
  display7SegmentosConfigurarPines();
  delayConfig(&delayRefresco,5);
}

void driverDisplayActualizar( void )
{
  if (delayRead(&delayRefresco))
  {
    //seleccionar el currDigit del display
    //mostrar digitoDisplay[currDigit] en el display (usar la funcion de Eric display7SegmentosMostrar
    // incrementar el currDigit m'odulo 4
  }
}

/*==================[fin del archivo]========================================*/