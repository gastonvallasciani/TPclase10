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
#include "sapi.h"                   // <= Biblioteca sAPI

/*==================[definiciones y macros]==================================*/

/*==================[definiciones de datos internos]=========================*/

/*==================[definiciones de datos externos]=========================*/

bool_t modoConfiguracion = FALSE;

uint32_t velocidadEntrePisos = 1; // En segundos, de 1 en adelante
uint32_t velocidadPuertas = 1;    // En segundos, de 1 en adelante
uint32_t cantidadDePisos = 5;     // De 1 a 20
uint32_t cantidadDeSubsuelos = 1; // De 0 a 5

/*
"1 - Configurar velocidad entre piso y piso."
"2 - Configurar velocidad de apertura o cerrado de puertas."
"3 - Configurar cantidad de pisos (1 a 20)."
"4 - Configurar cantidad de subsuelos (0 a 5)."
"5 - Salir del modo configuración."
*/

/*==================[declaraciones de funciones internas]====================*/

/*==================[definiciones de funciones internas]=====================*/

/*==================[definiciones de funciones externas]=====================*/

void secuenciaDeConfiguracion( void ){
   // ...
   modoConfiguracion = FALSE;
}

// Funcion de test: Devuelve TRUE si se completo la configuracion
bool_t seCompletoLaConfiguracion( void ){
   bool_t retVal = FALSE;
   
   return retVal;
}

/*==================[fin del archivo]========================================*/
