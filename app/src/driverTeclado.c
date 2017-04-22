/*============================================================================
 * Licencia: 
 * Autor: 
 * Fecha: 
 *===========================================================================*/



/*==================[inlcusiones]============================================*/
#include "stdint.h"
#include "driverTeclado.h"   // <= su propio archivo de cabecera
#include "sapi.h"       // <= Biblioteca sAPI
#include "driverDisplay.h"
#include "mefAscensor.h"

/*==================[definiciones y macros]==================================*/

/*==================[definiciones de datos internos]=========================*/
// Guarda la ultima tecla apretada
static uint16_t key = 0;
static uint16_t signo=0;
static uint16_t vDisplay[4]={NULO,NULO,NULO,NULO};
/* Pines del teclado matricial */

// Pines conectados a las Filas --> Salidas (MODO = OUTPUT)
static uint8_t keypadRowPins[4] = {
   RS232_TXD, // Row 0
   CAN_RD,    // R1
   CAN_TD,    // R2
   T_COL1     // R3
};

// Pines conectados a las Columnas --> Entradas con pull-up (MODO = INPUT_PULLUP)
static uint8_t keypadColPins[4] = {
   T_FIL0,    // Column 0
   T_FIL3,    // C1
   T_FIL2,    // C2
   T_COL0     // C3
};



/*==================[definiciones de datos externos]=========================*/
uint16_t tecla=0;
static int16_t piso=0;
uint16_t eMostrarPiso=0;

// Vector para mostrar tecla presionada por UART
uint16_t asciiKeypadKeys[16] = {
                                '1', '2', '3', 'A',
                                '4', '5', '6', 'B',
                                '7', '8', '9', 'C',
                                '*', '0', '#', 'D'
                               };

// Vector para mostrar tecla presionada en el display 7 segmentos
uint16_t keypadKeys[16] = {
                               1,    2,    3, 0x0a,
                               4,    5,    6, 0x0b,
                               7,    8,    9, 0x0c,
                            0x0e,    0, 0x0f, 0x0d
                          };

/*==================[declaraciones de funciones internas]====================*/
static void configurarTecladoMatricial( void );
static bool_t keyPressed( void );                
static bool_t readKey( void );
/*==================[declaraciones de funciones externas]====================*/
bool_t driverTecladoPisoRdy( void );
void driverTecladoInit( void );

/*==================[funcion principal]======================================*/

/*==================[definiciones de funciones internas]=====================*/
static void configurarTecladoMatricial( void ){
   
   uint8_t i = 0;
   
   // Configure Rows as Outputs
   for( i=0; i<4; i++ ){
      gpioConfig( keypadRowPins[i], GPIO_OUTPUT );
   }

   // Configure Columns as Inputs with pull-up resistors enable
   for( i=0; i<4; i++ ){
      gpioConfig( keypadColPins[i], GPIO_INPUT_PULLUP );
   }
}


/* Devuelve TRUE si hay alguna tecla presionada o FALSE (0) en caso contrario.
 * Si hay tecla presionada guarda el valor en la variable key.
 * El valor es un numero de indice entre 0 y 15 */
static bool_t keyPressed( void ){

   bool_t retVal = FALSE;

   uint16_t r = 0; // Rows
   uint16_t c = 0; // Columns

   // Poner todas las filas en estado BAJO
   for( r=0; r<4; r++ ){
	  gpioWrite( keypadRowPins[r], LOW );
   }

   // Chequear todas las columnas buscando si hay alguna tecla presionada
   for( c=0; c<4; c++ ){

      // Si leo un estado BAJO en una columna entonces puede haber una tecla presionada
      if( !gpioRead( keypadColPins[c] ) ){

//         delay( 50 ); // Anti-rebotes de 50 ms

         // Poner todas las filas en estado ALTO excepto la primera
         for( r=1; r<4; r++ ){
            gpioWrite( keypadRowPins[r], HIGH );
         }

         // Buscar que tecla esta presionada
         for( r=0; r<4; r++ ){

            // Poner la Fila[r-1] en estado ALTO y la Fila[r] en estado BAJO
            if( r>0 ){ // Exceptua el indice negativo en el array
               gpioWrite( keypadRowPins[r-1], HIGH );
            }
            gpioWrite( keypadRowPins[r], LOW );

            // Chequear la Columna[c] en Fila[r] para buscar si la tecla esta presionada
            // Si dicha tecla esta oresionada (en estado BAJO) entonces retorna
            // graba la tecla en key y retorna TRUE
            if( !gpioRead( keypadColPins[c] ) ){
               retVal = TRUE;
               key = r * 4 + c;
               /*
                  Formula de las teclas de Teclado Matricial (Keypad)
                  de 4 filas (rows) * 5 columnas (columns)

                     c0 c1 c2 c3 c4
                  r0  0  1  2  3  4
                  r1  5  6  7  8  9   Si se presiona la tecla r[i] c[j]:
                  r2 10 11 12 13 14   valor = (i) * cantidadDeColumnas + (j)
                  r3 15 16 17 18 19
               */
               return retVal;
            }
         }

      }
   }
   return retVal;
}

// ReadKey lee una tecla del teclado matricial realizando la tarea de anti-rebote.
// Devuelve true si una tecla fue presiona y liberada
//////////////////////////////////////////////////////////////////////////////
//                               Mirar en detalle  
//////////////////////////////////////////////////////////////////////////////
static bool_t readKey( void )
{
  static enum{Reposo, Debounce, Keypressed} estado=Reposo;
  static delay_t delayDebounce;
  
  switch(estado)
  {
  case Reposo:
    if(keyPressed()){
      delayConfig(&delayDebounce, 100);
      tecla=key; //key es actualizada por keyPressed
      estado=Debounce;
    }
    return FALSE;
  case Debounce:
    if (delayRead(&delayDebounce))
      if(keyPressed() && key==tecla)
        estado=Keypressed;
      else
        estado=Reposo;
    return FALSE;
  case Keypressed:
    if(!keyPressed())
    {
      estado=Reposo;
      return TRUE;
    }
    return FALSE;
  default:
    estado=Reposo;
    return FALSE;
  }	
}
/*==================[definiciones de funciones externas]=====================*/
void driverTecladoActualizar( void )
{
  static enum{Digito1,Digito2oLetra,Letra,Guardarpiso} estado=Digito1;
  uint16_t digito;
  static delay_t delayTemporizador;
  static char string[20];
  

  switch (estado)
  {
  case Digito1:
    if (readKey())
    {
      //limpiar vDisplay
      //Mostrar vDisplay
      digito=keypadKeys[tecla];
      switch ( digito )
      {
      case 0:
      case 1:
      case 2:
      case 3:
      case 4:
      case 5:
      case 6:
      case 7:
      case 8:
      case 9:
        //poner signo en cero
        //piso asignarle el digito leido del teclado
        //hacer un shift de vDisplay para ingresar el digito por la derecha
        //configurar el temporizador en 3000ms para que si no ingresa una tecla en 3s vuelve aqu'i
        //cambiar al estado Digito2oLetra
        break;
      case 0xe:  // el asterisco del teclado es el signo menos
        //poner el piso en cero y signo en 1
        // driverDisplayShf(MENOS,vDisplay); hacer un shift del signo menos
        // mostrar vDisplay
        // configurar temporizador en 3000ms para que si no ingresa una tecla en 3s vuelve aqui;
        // ir al estado Digito2oLetra
        break;
      }
    }
    break;
  case Digito2oLetra:
    if (readKey())
    {
      digito=keypadKeys[tecla];
      switch ( digito )
      {
      case 0:
      case 1:
      case 2:
      case 3:
      case 4:
      case 5:
      case 6:
      case 7:
      case 8:
      case 9:
        //piso=piso*10+digito;
        // si el signo==1 negar piso
        // shiftear el digito en vDisplay
        // configurar el temporizador en 3000ms
        // cambiar al estado Letra
        break;
      case 0xa:  //ingresa la A
        estado=Guardarpiso;
        break;
      case 0xb:  //ingresa B para cancelar el piso
        // poner piso en cero
        // poner signo en cero
        // limpiar vDisplay
        // mostrar vDisplay
        // ir al estado Digito1
        break;
      }
      return;
    }
    if (delayRead(&delayTemporizador)) // si pasaron los 3s vuelvo a Digito1
    {
      // poner piso y signo en cero
      // limpiar vDisplay
      // mostrar vDisplay
      // cambiar al estado Digito1
    }
    break;
  case Letra:
    if(readKey())
    {
      digito=keypadKeys[tecla];
      if(0xa==digito)
      {
        estado=Guardarpiso;
        return;
      }
    }
    if(delayRead(&delayTemporizador)||0x0b==digito)
    {
      //Si se le acaba el tiempo o ingresa una B para cancelar
      // cambiar al estado Digito1 como en los otros casos
    }
    break;
  case Guardarpiso:
    if(99==piso || (piso>=-vCantidadSubsuelos && piso<=vCantidadPisos))
    {
      mefAscensorInsertar(piso);
    }
    else  //el piso ingresado est'a fuera de rango y el ascensor recibe el evento eMostrarPiso para mostrar su vDisplay
      eMostrarPiso=1;
    signo=piso=0;
    estado=Digito1;
    break;
  default:
    piso=0;
    signo=0;
    driverDisplayLimpiar(vDisplay);
    driverDisplayMostrar(vDisplay);
    break;
  }
}

void driverTecladoInit( void )
{
  configurarTecladoMatricial();
  piso=0;
  signo=0;
}
/*==================[fin del archivo]========================================*/
/*
    itoa(piso,string,10);
    uartWriteString(UART_USB,string);    
    uartWriteString(UART_USB,"\n\r");
*/