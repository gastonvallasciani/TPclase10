/* Copyright 2017, Eric Pernia.
 * All rights reserved.
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
 * Date: 2017-04-14 
 */
 
/*==================[inlcusiones]============================================*/

#include "mefModoConfiguracion.h"   // <= su propio archivo de cabecera
#include "mefPuertas.h"
#include "sapi.h"                   // <= Biblioteca sAPI

/*==================[definiciones y macros]==================================*/

/*==================[definiciones de datos internos]=========================*/

/*==================[definiciones de datos externos]=========================*/

uint8_t eConfiguracion = 0;

uint32_t velocidadEntrePisos = 1; // En segundos, de 1 en adelante
uint32_t velocidadPuertas = 1;    // En segundos, de 1 en adelante
uint32_t cantidadDePisos = 5;     // De 1 a 20
uint32_t cantidadDeSubsuelos = 1; // De 0 a 5

extern uint8_t eAbrirPuertas;
/*
"1 - Configurar velocidad entre piso y piso."
"2 - Configurar velocidad de apertura o cerrado de puertas."
"3 - Configurar cantidad de pisos (1 a 20)."
"4 - Configurar cantidad de subsuelos (0 a 5)."
"5 - Salir del modo configuraci�n."
*/

/*==================[declaraciones de funciones internas]====================*/
static void configurarCantPisos(uartMap_t uart)
{
  int cant=0;
  char str[2]="00";
  char c;
  uint16_t nroPisos=0;
  
  do{
    cant=0;
    uartWriteString(uart,"Ingrese la cantidad de pisos (1-20)= ");
    while(!uartReadByte(uart,&c));
    while(c!='\r')
    {
      if((c>='0' && c<='9') && cant<2)
      {
        str[0]=str[1];
        str[1]=c;
        cant++;
        uartWriteByte(uart,c);
      }
      if(c=='\b' && cant>0)
      {
        --cant;
        str[1]=str[0];
        str[0]='0';
        uartWriteString(uart,"\b \b");
      }
      while(!uartReadByte(uart,&c));
    }
    nroPisos=(str[0]-'0')*10+str[1]-'0';
    if(0==nroPisos || 20<nroPisos)
    {
      uartWriteString(uart,"\b\b  \r");
      str[0]=str[1]='0';
      cant=0;
    }
    else
    {
      cantidadDePisos=nroPisos;
      uartWriteString(uart,"\n\r");
    }
  }while(0==nroPisos || 20<nroPisos);
  
}

static void configurarCantidadSubsuelos(uartMap_t uart)
{
  int cant=0;
  char str[1]="0";
  char c;
  uint16_t nroPisos=0;
  
  do{
    uartWriteString(uart,"Ingrese la cantidad de subsuelos (0-5)= ");
    while(!uartReadByte(uart,&c));
    while(c!='\r')
    {
      if((c>='0' && c<='9') && cant<1)
      {
        cant;
        str[cant++]=c;
        uartWriteByte(uart,c);
      }
      if(c=='\b' && cant>0)
      {
        str[--cant]='0';
        uartWriteString(uart,"\b \b");
      }
      while(!uartReadByte(uart,&c));
    }
    nroPisos=(str[0]-'0');
    if(5<nroPisos)
    {
      uartWriteString(uart,"\b \r");
      str[0]='0';
      cant=0;
    }
    else
    {
      cantidadDeSubsuelos=nroPisos;
      uartWriteString(uart,"\n\r");
    }
  }while(5<nroPisos);
}

static void configurarPeriodoAperturaCierre(uartMap_t uart)
{
  int cant=0;
  char str[1]="0";
  char c;
  uint16_t periodo=1;
  
  do{
    uartWriteString(uart,"Ingrese el tiempo de cierre o apertura de puerta (1s-5s)= ");
    while(!uartReadByte(uart,&c));
    while(c!='\r')
    {
      if((c>='0' && c<='9') && cant<1)
      {
        str[cant++]=c;
        uartWriteByte(uart,c);
      }
      if(c=='\b' && cant>0)
      {
        str[--cant]='0';
        uartWriteString(uart,"\b \b");
      }
      while(!uartReadByte(uart,&c));
    }
    periodo=(str[0]-'0');
    if(0==periodo || 5<periodo)
    {
      uartWriteString(uart,"\b \r");
      str[0]='0';
      cant=0;
    }
    else
    {
      velocidadPuertas=periodo;
      uartWriteString(uart,"\n\r");
    }
  }while(0==periodo || 5<periodo);
}

static void configurarPeriodoEntrePisos(uartMap_t uart)
{
  int cant=0;
  char str[1]="0";
  char c;
  uint16_t periodo=1;
  
  do{
    uartWriteString(uart,"Ingrese el tiempo de recorrido entre pisos (1s-5s)= ");
    while(!uartReadByte(uart,&c));
    while(c!='\r')
    {
      if((c>='0' && c<='9') && cant<1)
      {
        cant;
        str[cant++]=c;
        uartWriteByte(uart,c);
      }
      if(c=='\b' && cant>0)
      {
        str[--cant]='0';
        uartWriteString(uart,"\b \b");
      }
      while(!uartReadByte(uart,&c));
    }
    periodo=(str[0]-'0');
    if(0==periodo || 5<periodo)
    {
      uartWriteString(uart,"\b \r");
      str[0]='0';
      cant=0;
    }
    else
    {
      velocidadEntrePisos=periodo*1000;
      uartWriteString(uart,"\n\r");
    }
  }while(0==periodo || 5<periodo);
}

/*==================[definiciones de funciones internas]=====================*/

/*==================[definiciones de funciones externas]=====================*/

void secuenciaDeConfiguracion( void ){
   // ...
   eConfiguracion = 1;
   eAbrirPuertas = 1;
   uartWriteString(UART_USB,"Inicio de Configuraci'on\n\r");
   configurarCantPisos(UART_USB);
   configurarCantSubsuelos(UART_USB);
   configurarPeriodoEntrePisos(UART_USB);
   configurarPeriodoAperturaCierre(UART_USB);     
      
}

bool_t seCompletoLaConfiguracion( void ){

   uint8_t conf;
   
   uartWriteString(UART_USB,"?Se complet'o la configuraci'on? (s/n) ");
   uartReadByte(UART_USB,&conf);
   uartWtriteByte(UART_USB,conf);
   if (conf=='s' || conf=='S')
   {
     eConfiguracion=0;
     secuenciaDeCerradoDePuertas();
     return TRUE;
   }
   return FALSE;
}


// Funcion de test: Devuelve TRUE si se completo la configuracion
/*==================[fin del archivo]========================================*/
