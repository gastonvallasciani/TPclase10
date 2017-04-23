/*============================================================================
 * Description: Debounce
 * Author: Gastón Vallasciani
 * Date:  08/04/2017
 *===========================================================================*/

#ifndef _DEBOUNCE_H_
#define _DEBOUNCE_H_

/*==================[inclusiones]=============================================*/

#include "sapi.h"

/*==================[c++]====================================================*/

#ifdef __cplusplus
extern "C" {
#endif

/*==================[macros]=================================================*/

/*==================[tipos de datos declarados por el usuario]===============*/

/*==================[declaraciones de datos externos]========================*/

/*==================[declaraciones de funciones externas]====================*/
void Ini_AntirreboteMEF( void );
uint8_t Act_AntirreboteMEF( uint8_t button );

/*==================[c++]====================================================*/
#ifdef __cplusplus
}
#endif

/*==================[end of file]============================================*/
#endif /* _DEBOUNCE_H_ */
