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

#include "mefPuertas.h"   // <= su propio archivo de cabecera
#include "sapi.h"         // <= Biblioteca sAPI

/*==================[definiciones y macros]==================================*/

#define INDICADOR_PUERTA_ABIERTA      LEDG // puerta abierta.
#define INDICADOR_ALARMA              LEDR // alarma puerta abierta.
#define INDICADOR_PUERTA_ABRIENDOSE   LED1 // puerta abri�ndose.
#define INDICADOR_PUERTA_CERRANDOSE   LED2 // puerta cerr�ndose.

/*==================[definiciones de datos internos]=========================*/

static delay_t base1seg;
static bool_t alarmaOn=FALSE;

/*==================[definiciones de datos externos]=========================*/

extern uint8_t eConfiguracion;
extern uint8_t eAperturaPuertas;

extern uint32_t velocidadPuertas;   // En segundos, de 1 en adelante

estadoMefPuertas_t estadoMefPuertas;
uint8_t	sPuertaCerrada=0;
/*==================[declaraciones de funciones internas]====================*/

/*==================[definiciones de funciones internas]=====================*/

/*==================[definiciones de funciones externas]=====================*/

void secuenciaDeAperturaDePuertas( void ){/*
   //gpioWrite( INDICADOR_PUERTA_ABRIENDOSE, ON );
   gpioWrite( LEDG, ON );
   delay( velocidadPuertas*1000 );
   //gpioWrite( INDICADOR_PUERTA_ABIERTA, ON );
   //gpioWrite( INDICADOR_PUERTA_ABRIENDOSE, OFF );
   gpioWrite( LEDG, OFF );*/
   uartWriteString( UART_USB, "Puertas abiertas\r\n" );
   eAperturaPuertas=1; 
}

void secuenciaDeCierreDePuertas( void ){/*
   //gpioWrite( INDICADOR_PUERTA_ABRIENDOSE, ON );
   gpioWrite( LEDG, ON );
   delay( velocidadPuertas*1000 );
   //gpioWrite( INDICADOR_PUERTA_ABIERTA, ON );
   //gpioWrite( INDICADOR_PUERTA_ABRIENDOSE, OFF );
   gpioWrite( LEDG, OFF );*/
   uartWriteString( UART_USB, "Cerrando abiertas\r\n" );
   sPuertaCerrada=0; 
}

// Inicializar la MEF de puertas
void puertasInicializarMEF( estadoMefPuertas_t estadoInicial ){
  estadoMefPuertas = PUERTA_CERRADA;
}

// Actualizar la MEF de puertas
void puertasActualizarMEF( void ){
  static bool_t flagPuertaCerrada=FALSE;
  static bool_t flagPuertaAbierta=FALSE;
  static bool_t flagAbriendoPuerta=FALSE;
  static bool_t flagCerrandoPuerta=FALSE;
  static bool_t flagAlarmaPuertaAbierta=FALSE;
  static uint32_t temporizador1=1;
  
  switch(estadoMefPuertas)
  {
  case PUERTA_CERRADA:
    if(flagPuertaCerrada==FALSE)
    {
      flagPuertaCerrada==TRUE;
      sPuertaCerrada=1;
    }
    if(eAperturaPuertas)
    {
      estadoMefPuertas=ABRIENDO_PUERTA;
    }
    if(estadoMefPuertas!=PUERTA_CERRADA)
    {
      flagPuertaCerrada=FALSE;
      temporizador1=velocidadPuertas;
      delayConfig(&base1seg,1000);
      delayRead(&base1seg);
      sPuertaCerrada=0;
    }
    break;
  case ABRIENDO_PUERTA:
    if(flagAbriendoPuerta==FALSE)
    {
      flagAbriendoPuerta==TRUE;
      gpioWrite(INDICADOR_PUERTA_ABRIENDOSE,ON);
    }
    if(delayRead(&base1seg))
    {
      temporizador1--;
    }
    if(temporizador1==0)
      estadoMefPuertas=PUERTA_ABIERTA;
      
    if(estadoMefPuertas!=ABRIENDO_PUERTA)
    {
      flagAbriendoPuerta=FALSE;
      gpioWrite(INDICADOR_PUERTA_ABRIENDOSE,OFF);
    }
    break;
  case PUERTA_ABIERTA:
    if(flagPuertaAbierta==FALSE)
    {
      flagPuertaAbierta==TRUE;
      gpioWrite(INDICADOR_PUERTA_ABIERTA,ON);
    }
    if(!eConfiguracion)
    {
      if(delayRead(&base1seg))
      {
       temporizador1--;
      }
      if(temporizador1==0)
      {
        if(!gpioRead(TEC1))
          estadoMefPuertas=ALARMA_PUERTA_ABIERTA;
        else
          estadoMefPuertas=CERRANDO_PUERTA;
      }
      if(estadoMefPuertas!=PUERTA_ABIERTA)
      {
        flagPuertaAbierta=FALSE;
        gpioWrite(INDICADOR_PUERTA_ABIERTA,OFF);
      }
    }
    break;
  case CERRANDO_PUERTA:
    if(flagCerrandoPuerta==FALSE)
    {
      flagCerrandoPuerta==TRUE;
      gpioWrite(INDICADOR_PUERTA_CERRANDOSE,ON);
      temporizador1=velocidadPuertas;
      delayConfig(&base1seg,1000);
      delayRead(&base1seg);
    }
    if(delayRead(&base1seg))
    {
      temporizador1--;
    }
    if(!gpioRead(TEC1))
      estadoMefPuertas=ABRIENDO_PUERTA;
    else if(temporizador1==0)
      estadoMefPuertas=PUERTA_CERRADA;
      
    if(estadoMefPuertas!=CERRANDO_PUERTA)
    {
      flagCerrandoPuerta=FALSE;
      gpioWrite(INDICADOR_PUERTA_CERRANDOSE,OFF);
    }
    break;
  case ALARMA_PUERTA_ABIERTA:
    if(flagAlarmaPuertaAbierta==FALSE)
    {
      flagAlarmaPuertaAbierta==TRUE;
      gpioWrite(INDICADOR_ALARMA,ON);
      alarmaOn=TRUE;
      delayConfig(&base1seg,1000);
      delayRead(&base1seg);
      temporizador1=2;
    }
    if(delayRead(&base1seg))
    {
      if(alarmaOn)
      {
        gpioWrite(INDICADOR_ALARMA,OFF);
        alarmaOn=FALSE;
      }
      else
      {
        gpioWrite(INDICADOR_ALARMA,ON);
        alarmaOn=TRUE;
      }
    }
    if(gpioRead(TEC1))
      estadoMefPuertas=CERRANDO_PUERTA;
      
    if(estadoMefPuertas!=ALARMA_PUERTA_ABIERTA)
    {
      flagPuertaAbierta=FALSE;
      gpioWrite(INDICADOR_ALARMA,OFF);
    }
    break;
  }
  
}

/*==================[fin del archivo]========================================*/
