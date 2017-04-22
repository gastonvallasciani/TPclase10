/*============================================================================
 * Licencia: 
 * Autor: 
 * Fecha: 
 *===========================================================================*/



/*==================[inlcusiones]============================================*/

#include "stdint.h"
#include "mefAscensor.h"   // <= su propio archivo de cabecera
#include "driverDisplay.h" //Interfaz con el Display
#include "driverTeclado.h" //Interfaz con el Teclado
#include "sapi.h"       // <= Biblioteca sAPI

/*==================[definiciones y macros]==================================*/

/*==================[definiciones de datos internos]=========================*/
static uint16_t vPeriodoEntrePiso=2000;
static uint16_t vPeriodoAperturaPuerta=1000;
static uint16_t vPeriodoCierrePuerta=1000;
static uint16_t vPeriodoIngresoEgreso=5000;
static uint16_t vPeriodoEsperaEnDestino=5000;
static uint16_t vDisplay[4]={NULO,NULO,NULO,NULO};
static int16_t  vPisoCorriente=0;
static int16_t  vPisoDestino=0;
static uint16_t eAbrirPuertas=0;
static uint16_t eCerrarPuertas=0;
static uint16_t sPuertaCerrada=0;
static uint16_t sPuertaAbierta=0;
static delay_t delayEntrePiso;
static delay_t delayEsperaEnDestino;

static struct cola
{
  int16_t  pendiente[16];
  uint16_t cant;
  uint16_t ultimo;
  uint16_t primero;
} vColaDePisos={.cant=0,.ultimo=0,.primero=0}; 
/*==================[definiciones de datos externos]=========================*/
uint16_t vCantidadPisos=20;
uint16_t vCantidadSubsuelos=5;

/*==================[declaraciones de funciones internas]====================*/
static void configurarCantPisos(uartMap_t uart);
static void configurarCantidadSubsuelos(uartMap_t uart);
static void configurarPeriodoAperturaCierre(uartMap_t uart);
static void configurarPeriodoEntrePisos(uartMap_t uart);

/*==================[declaraciones de funciones externas]====================*/
void mefAscensorInsertar(uint16_t piso);

/*==================[funcion principal]======================================*/

/*==================[definiciones de funciones internas]=====================*/
static bool_t mefAscensorObtener(uint16_t *piso)
{
	if (vColaDePisos.cant>0)
	{
	  vColaDePisos.cant--;
	  *piso=vColaDePisos.pendiente[vColaDePisos.primero++];
	  vColaDePisos.primero&=0x0f;
	  return TRUE;  
	}
	return FALSE;
}

static mefAscensorPutPiso(int16_t vPisoCorriente)
{
        if (vPisoCorriente<0)
        {
         if(vPisoCorriente<-vCantidadSubsuelos)
         {
           uartWriteString(UART_USB,"valor invalido del subsuelo corriente\n\n");
           while(1);
         }
         vDisplay[3]=(-vPisoCorriente);
         vDisplay[2]=MENOS;
        }
        else if (vPisoCorriente==0)
        {
          vDisplay[3]=CHAR_b;
          vDisplay[2]=CHAR_P;
        }
        else
        {
         if(vPisoCorriente>vCantidadPisos)
         {
           uartWriteString(UART_USB,"valor invalido del piso corriente\n\n");
           while(1);
         }
         vDisplay[3]=vPisoCorriente%10;
         if (!(vDisplay[2]=vPisoCorriente/10))
           vDisplay[2]=NULO;
        }
}

static void configurarCantPisos(uartMap_t uart)
{
  int cant=0;
  char str[2]="00";
  char c;
  uint16_t nroPisos=0;
  
  do{
    cant=0;
    uartWriteString(uart,"Ingrese la cantidad de pisos (1-98)= ");
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
    if(0==nroPisos || 98<nroPisos)
    {
      uartWriteString(uart,"\b\b  \r");
      str[0]=str[1]='0';
      cant=0;
    }
    else
    {
      vCantidadPisos=nroPisos;
      uartWriteString(uart,"\n\r");
    }
  }while(0==nroPisos || 98<nroPisos);
  
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
      vCantidadSubsuelos=nroPisos;
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
      vPeriodoAperturaPuerta=periodo*1000;
      vPeriodoCierrePuerta=periodo*1000;
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
      vPeriodoEntrePiso=periodo*1000;
      uartWriteString(uart,"\n\r");
    }
  }while(0==periodo || 5<periodo);
}

/*==================[definiciones de funciones externas]=====================*/

void mefAscensorInsertar(uint16_t piso)
{
	if (vColaDePisos.cant<16)
	{
	  vColaDePisos.cant++;
	  vColaDePisos.pendiente[vColaDePisos.ultimo++]=piso;
	  vColaDePisos.ultimo&=0x0f;  
	}
}
/*==================[fin del archivo]========================================*/
void mefAscensorInit( void )
{
  driverDisplayLimpiar(vDisplay);
  vDisplay[2]=CHAR_P;
  vDisplay[3]=CHAR_b;
  driverDisplayMostrar(vDisplay);  //Muestra Pb en el display
  vPisoCorriente=0;
  vPisoDestino=0;
  eAbrirPuertas=0;
  eCerrarPuertas=0;
  vColaDePisos.ultimo=0;
  vColaDePisos.primero=0;
  vColaDePisos.cant=0;
  uartConfig(UART_USB,115200);
}

void mefAscensorActualizar( void )
{
  static enum{EnPlantaBaja,Subiendo,Bajando,
              EsperaCierreBajando,EsperaCierreSubiendo,
              Parado,ParadoPuertaAbierta,YendoPlantaBaja,
              SubiendoaPlantaBaja,BajandoaPlantaBaja,
              ModoConfiguracion} estado=EnPlantaBaja;

  if (eMostrarPiso)
  {
    eMostrarPiso=0;
    driverDisplayMostrar(vDisplay);
  }

  switch (estado)
  {
  case EnPlantaBaja:
    if(sPuertaCerrada)
      if (mefAscensorObtener(&vPisoDestino))
      {
        if (99==vPisoDestino)
        {
          estado=ModoConfiguracion;
        }
        else if(vPisoCorriente>vPisoDestino)
        {
          vDisplay[0]=CHAR_b;
          vDisplay[1]=NULO;
          driverDisplayMostrar(vDisplay);
          delayConfig(&delayEntrePiso,vPeriodoEntrePiso);
          estado=Bajando;
          eCerrarPuertas=1;
        }
        else if (vPisoCorriente<vPisoDestino)
        {
          vDisplay[0]=CHAR_S;
          vDisplay[1]=NULO;
          driverDisplayMostrar(vDisplay);
          delayConfig(&delayEntrePiso,vPeriodoEntrePiso);
          estado=Subiendo;
          eCerrarPuertas=1;
        }
        else
        {
           eAbrirPuertas=1;
           driverDisplayMostrar(vDisplay);
        }
      }
    break;
  case Bajando:
    if(delayRead(&delayEntrePiso))
    {
      int i;
      
      vPisoCorriente--;
      if(vPisoCorriente==vPisoDestino)
      {
        eAbrirPuertas=1;
        if(0==vPisoCorriente)
          estado=EnPlantaBaja;
        else
        {
          estado=Parado;
          delayConfig(&delayEsperaEnDestino,vPeriodoEsperaEnDestino);
        }
        vDisplay[0]=NULO;
        mefAscensorPutPiso(vPisoCorriente);
      }
      mefAscensorPutPiso(vPisoCorriente);
      driverDisplayMostrar(vDisplay);
    }
    break;
  case Subiendo:
    if(delayRead(&delayEntrePiso))
    {
      vPisoCorriente++;
      if(vPisoCorriente==vPisoDestino)
      {
        eAbrirPuertas=1;
        if(vPisoCorriente==0)
          estado=EnPlantaBaja;
        else
        {
          estado=Parado;
          delayConfig(&delayEsperaEnDestino,vPeriodoEsperaEnDestino);
        }
        vDisplay[0]=NULO;
      }
      mefAscensorPutPiso(vPisoCorriente);
      driverDisplayMostrar(vDisplay);
    }
    break;
  case Parado:
    if(sPuertaAbierta)
      eCerrarPuertas=1;   
    else if(sPuertaCerrada)
      if (mefAscensorObtener(&vPisoDestino))
        if(vPisoCorriente>vPisoDestino)
        {
          vDisplay[0]=CHAR_b;
          vDisplay[1]=NULO;
          driverDisplayMostrar(vDisplay);
          delayConfig(&delayEntrePiso,vPeriodoEntrePiso);
          estado=Bajando;
        }
        else if (vPisoCorriente<vPisoDestino)
        {
          vDisplay[0]=CHAR_S;
          vDisplay[1]=NULO;
          driverDisplayMostrar(vDisplay);
          delayConfig(&delayEntrePiso,vPeriodoEntrePiso);
          estado=Subiendo;
        }
        else
          eAbrirPuertas=1;
      else if (delayRead(&delayEsperaEnDestino))
      {
        if(vPisoCorriente==0)
          estado=EnPlantaBaja;
        else
        {
          vPisoDestino=0;
          estado=YendoPlantaBaja;
        }
      }
      
    break;
  case YendoPlantaBaja:
    delayConfig(&delayEntrePiso,vPeriodoEntrePiso);
    if(vPisoCorriente==vPisoDestino)
    {
      estado=EnPlantaBaja;
    }
    else if (vPisoCorriente>vPisoDestino)
    {
      vDisplay[0]=CHAR_b;
      vDisplay[1]=NULO;
      driverDisplayMostrar(vDisplay);
      estado=Bajando;
    }
    else if (vPisoCorriente<vPisoDestino)
    {
      vDisplay[0]=CHAR_S;
      vDisplay[1]=NULO;
      driverDisplayMostrar(vDisplay);
      estado=Subiendo;
    }
    break;
  case ModoConfiguracion:
    configurarCantPisos(UART_USB);
    configurarCantidadSubsuelos(UART_USB);
    configurarPeriodoAperturaCierre(UART_USB);
    configurarPeriodoEntrePisos(UART_USB);
    estado=EnPlantaBaja;
    driverDisplayLimpiar(vDisplay);
    vDisplay[0]=CHAR_P;
    vDisplay[1]=CHAR_b;
    driverDisplayMostrar(vDisplay);
    break;
  }
}

void mefManejoPuertasActualizar( void )
{
  static enum{PuertaCerrada,AbriendoPuerta,PuertaAbierta,CerrandoPuerta,AlarmaPuertaAbierta} estado=PuertaCerrada;
  static bool_t ledEncendido=FALSE;
  static delay_t delayTemporizador;
  static delay_t delayTemporizador1;
  static delay_t delayLed;
  
  switch(estado)
  {
  case PuertaAbierta:
    if(delayRead(&delayTemporizador))
    {
      eCerrarPuertas=0;
      delayConfig(&delayTemporizador,vPeriodoCierrePuerta);
      gpioWrite(LEDG,OFF);
      gpioWrite(LED1,ON);
      estado=CerrandoPuerta;
      sPuertaAbierta=0;
    }
    break;
  case CerrandoPuerta:
    if(!gpioRead(TEC1))
    {
      delayConfig(&delayLed,500);
      delayConfig(&delayTemporizador1,2000);
      ledEncendido=1;
      gpioWrite(LED1,OFF);
      gpioWrite(LEDR,ON);
      estado=AlarmaPuertaAbierta;
    }
    else if (delayRead(&delayTemporizador))
    {
      gpioWrite(LED1,OFF);
      estado=PuertaCerrada;
      sPuertaCerrada=1;
    }
    break;
  case AlarmaPuertaAbierta:
    if(delayRead(&delayTemporizador1))
    {
      if(ledEncendido)
      {
        gpioWrite(LEDR,OFF);
        ledEncendido=FALSE;
      }
      delayConfig(&delayTemporizador,vPeriodoCierrePuerta);
      gpioWrite(LED1,ON);
      estado=CerrandoPuerta;
    }
    else if(delayRead(&delayLed))
    {
      if(ledEncendido)
      {
        gpioWrite(LEDR,OFF);
        ledEncendido=FALSE;
      }
      else
      {
        gpioWrite(LEDR,ON);
        ledEncendido=TRUE;
      }
    }
    break;
  case PuertaCerrada:
    if(eAbrirPuertas)
    {
      eAbrirPuertas=0;
      estado=AbriendoPuerta;
      gpioWrite(LED2,ON);
      delayConfig(&delayTemporizador,vPeriodoAperturaPuerta);
      sPuertaCerrada=0;
    }
    break;
  case AbriendoPuerta:
    if(delayRead(&delayTemporizador))
    {
      sPuertaAbierta=1;
      gpioWrite(LED2,OFF);
      gpioWrite(LEDG,ON);
      estado=PuertaAbierta;
      delayConfig(&delayTemporizador,vPeriodoIngresoEgreso);
    }
  }
}

void mefManejoPuertasInit( void )
{
  gpioConfig(LEDR,GPIO_OUTPUT);
  gpioConfig(LEDG,GPIO_OUTPUT);
  gpioConfig(LED1,GPIO_OUTPUT);
  gpioConfig(LED2,GPIO_OUTPUT);
  gpioConfig(TEC1,GPIO_INPUT);
  gpioWrite(LEDR,OFF);
  gpioWrite(LEDG,OFF);
  gpioWrite(LED1,OFF);
  gpioWrite(LED2,OFF);
  sPuertaAbierta=0;
  sPuertaCerrada=1;
  eCerrarPuertas=0;
  eAbrirPuertas=0;
}
