/*============================================================================
 * License: 
 * Author: 
 * Date: 
 *===========================================================================*/

#ifndef _TECLADO_MATRICIAL_H_
#define _TECLADO_MATRICIAL_H_

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

void configurarTecladoMatricial( void );
bool_t leerTecladoMatricial( void ); 
uint16_t key_pressedHex( void);
uint16_t key_pressedAscii( void);

/*==================[c++]====================================================*/
#ifdef __cplusplus
}
#endif

/*==================[end of file]============================================*/
#endif /* _TTECLADO_MATRICIAL_H_ */
