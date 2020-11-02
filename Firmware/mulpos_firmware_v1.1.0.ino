/*
 *Codigo de control para el proyecto MULPOS y STARPI, desarrollado y mantenido por Moveo Robotics
 *Para mas informacion y documentacion visita:  https://www.moveorobotics.com o envia un email a support@moveorobotics.com
 *Este poryecto se encuntra protegido bajo licencia MIT
*/

//--------Bibliotecas necesarias-----------//
#include <Servo.h>
//----------------------------------------//

//-------Configuracion de los servos-------//
/*
            Ejes de referencia
                   z
                   |
                   |
                   |_______ y
                  /
                 /
                x
*/

//---------------Muneca y Antebrazo--------//
//Pronacion y Supinacion
Servo servox;
//Flexion y Extencion
Servo servoy;
//Abduccion y Aduccion esta acciones se encuntra desabilitada por el momomento sera integrada en futuras versiones
Servo servoz;
//----------------Dedos-------------------//
//Menique
Servo servof1;
//Anular
Servo servof2;
//Medio
Servo servof3;
//Indice
Servo servof4;
//Pulgar
Servo servof5;
//Opositor de Pulgar
Servo servoPOf5;
//---------Posicion CamaraSTARPI----------//
//YAW
Servo servoCamz;
//PITCH
Servo servoCamy;
//----------------------------------------//

//----------Variables de control----------//
//variables de control para modo app o modo estimacion de pose y/o proyecto STARPI         
int modoAnt = 0;
int valAccion = 0;
int vaPosX = 0;
int vaPosY = 0;
int vjPosX = 0;
int vjPosY = 0;
int vaCamPosZ = 0;
int vaCamPosY = 0;
const int modoCtrl = 13;
//varables de control para modo EMG
String instruccion;  
String comando; 
const int emgPin = A0;  
int emgVal = 0; 
int emgValAnt = 0; 
//asigne aqui el valor maximo obtenido al ejecutar el codigo calibrar emg => "su valor maximo" * 0.7
const int emgLimSup = 1023 * 0.7;
bool flanSub = false; 
bool setTimer = false;  
bool startTimer = false; 
bool seleccionado = false;
int puntero = 0;
int modo = 0;
//aqui puede ajutar el timpo de espera para leer los pulsos musculares
int tiempoEspera = 3000;//tiempo de espera para modo
int tiempoEsperaModo = 2000;//tiempo espera modo en modo
//----------------------------------------//
int tiempoEsperaServos = 180;
int servoPos = 180;
unsigned long antes = 0;  
unsigned long ahora = 0;
unsigned long antesS = 0;  
unsigned long ahoraS = 0;
int varTiempo = 0;
int varTiempoS = 0;
//----------------------------------------//

//-------Configuracion de inicio----------//
void setup()
{ 
  //Configuracion puerto serial
  Serial.begin(38400);
  Serial.setTimeout(5);
  
  //Asignacion de pines para cada servo
  servof1.attach(2);
  servof2.attach(3);
  servof3.attach(4);
  servof4.attach(5);
  servof5.attach(6);
  servoPOf5.attach(7);
  
  servox.attach(8);
  servoy.attach(9);
  servoz.attach(10);
   
  servoCamz.attach(11);
  servoCamy.attach(12);
  
  //Pin para selecionar modo
  pinMode(modoCtrl, INPUT);
  //Verificmaos el modo al iniciar
  modoAnt = digitalRead(modoCtrl);
  
  //Valores de inicio para cada servo posicion inicial
  servox.write(90);
  servoy.write(90);
  servoz.write(90);

  servoCamz.write(90);
  servoCamy.write(90);

  servof1.write(180);
  servof2.write(180);
  servof3.write(180);
  servof4.write(180);
  servof5.write(180);
  delay(1500);
  servoPOf5.write(180);
}
//----------------------------------------//

//----------Bucle principal---------------//
void loop()
{
   modoControl();
}
//----------------------------------------//

//--------Funciones de control------------//

//Define el modo de control
void modoControl()
{
  if(digitalRead(modoCtrl) != modoAnt)
  {
    posServosPredMuneca();
    posServosPredMano();
    posServosPredCam();
    puntero = 0;
    modo = 0;
    servoPos = 180;
    comando = ' ';
    modoAnt = digitalRead(modoCtrl);
  }
  
  if(digitalRead(modoCtrl))
  {
    leerComando();
    epCamAppControlador();
  }
  else
  {
    emgControlador();
  }
  
}

//Lee los comanods que llegan por el puerto serial
void leerComando()
{
   while(Serial.available())
   {
      if(Serial.available()> 0)
      {
        comando = Serial.readStringUntil('|');
      }
      instruccion = comando;
      instruccion.remove(0,1);
      valAccion = instruccion.toInt();
   }
}

//Ejecutar los comandos recibidos para control por app o estmicacio de pose con la camara y/o STRAPI
void epCamAppControlador()
{
  switch(comando[0])
  {
     case 'x':
          //Control de muneca con acelerometro y giroscopio Pronacion y Supinacion
          vaPosX = map(valAccion,-95,95,0,180);
          if (vaPosX >= 75 && vaPosX <= 105)
          {
            servox.write(90); 
          }
          else 
          {
             servox.write(vaPosX);
          }
          break;
    
     case 'y':
          //Control de muneca con acelerometro y giroscopio Flexion y Extencion
          vaPosY = map(valAccion,-90,90,0,180);
          if (vaPosY >= 72 && vaPosY <= 107)
          {
            servoy.write(90); 
          }
          else 
          {
            servoy.write(vaPosY);
          }
          break;

     case 'a':  
          //Control de muneca con joystick pad Pronacion y Supinacion
          vjPosX = constrain(map(valAccion,0,180,180,0),0,180);
          servox.write(vjPosX);
          break;   
      
     case 'b':
          //Control de muneca con joystick pad Flexion y Extencion
          vjPosY = constrain(valAccion,0,180);
          servoy.write(vjPosY);
          break; 

     case 's':
          //Muneca en posicion incial
          servox.write(90);
          servoy.write(90);
          break;

     case '^':
          //Control de camara YAW con acelerometro y giroscopio del telefono con app de vision
          vaCamPosZ = map(valAccion,-100,100,0,180);
          if (vaCamPosZ >= 75 && vaCamPosZ <= 105)
          {
            servoCamz.write(90); 
          }
          else 
          {
            servoCamz.write(vaCamPosZ);
          }
          break;
    
     case '>':
          //Control de camara PITCH con acelerometro y giroscopio del telefono con app de vision
          vaCamPosY = map(valAccion,-90,90,0,180);
          if (vaCamPosY >= 72 && vaCamPosY <= 107)
          {
            servoCamy.write(90); 
          }
          else 
          {
            servoCamy.write(vaCamPosY);
          }
          break;
          
     case 'c': 
           //Cerrar Menique
           servof1.write(0);
           break;
              
     case '1':
           //Abrir Menique 
           servof1.write(180);
           break;
     
     case 'd':
           //Cerrar Anular
           servof2.write(0);
           break;
     
     case '2':
           //Abrir Anular
           servof2.write(180);
           break;
     
     case 'e':
           //Cerrar Medio
           servof3.write(0);
           break;
       
     case '3':
           //Abrir Medio
           servof3.write(180);
           break;
     
     case 'f':
           //Cerrar Indice
           servof4.write(0);
           break;
     
     case '4':
           //Abrir Indice
           servof4.write(180);
           break;
     
     case 'g':
           //Oponer Pulgar y Cerrar
           servoPOf5.write(90);
           delay(1500);
           servof5.write(0);
           break;
           
     case '5':
           //Retonrar Pulagr y Abrir
           servof5.write(180);
           delay(1500);
           servoPOf5.write(180);
           break;
          
     case 'h':
           //Sujetar
           servof1.write(0);
           servof2.write(0);
           servof3.write(0);
           servof4.write(0);
           servoPOf5.write(90);
           delay(1500);
           servof5.write(0);
           break; 
     case '6':
           //Soltar
           servof1.write(180);
           servof2.write(180);
           servof3.write(180);
           servof4.write(180);
           servof5.write(180);
           delay(1500);
           servoPOf5.write(180); 
           break;  
      
     case 'j':
           //Saludar
           servoy.write(10);
           servox.write(150);
           delay(700);
           servox.write(30);
           delay(700);
           servox.write(150);
           delay(700);
           servox.write(30);
           delay(700);
           servox.write(150);
           delay(700);
           comando = 'm' ;   
           break;
         
     case 'k':
           //Despedirse
           servoy.write(90);
           servox.write(180);
           servof1.write(5);
           servof2.write(180);
           servof3.write(5);
           servof4.write(180);
           servof5.write(5);
           delay(5000);
           comando = 'm' ;
            break;
           
     case 'm':
           //Posicion Inicial
           servox.write(90);
           servoy.write(90);
           servof1.write(180);
           servof2.write(180);
           servof3.write(180);
           servof4.write(180);
           servof5.write(180);
           delay(1500);
           servoPOf5.write(180);
           comando = ' ';
           break;
           
     case 'o':
           //Giro a la Derecha
           servox.write(180);
           break;
           
     case 'p':
           //Giro a la Izquierda
           servox.write(0);
           break;
           
     case 'q':
           //Felxion hacia Arriba
           servoy.write(10);
           break;
           
     case 'n':
           //Flexion hacia Abajo
           servoy.write(180);
           break;
           
     case 'l':
           //Posicion inicial Muneca
           servox.write(90);
           servoy.write(90);
           break;
           
     case 'r':
           //Senalar con el Indice "ahi"
           servox.write(90);
           servoy.write(90);
           servof1.write(0);
           servof2.write(0);
           servof3.write(0);
           servof4.write(180);
           servoPOf5.write(90);
           delay(1500);
           servof5.write(0);
           delay(5000);
           comando = 'm' ;
           break;
           
     case 't':
           //Gesto de Pulgar Abajo "mal"
           servoy.write(90);
           servox.write(0);
           servof1.write(0);
           servof2.write(0);
           servof3.write(0);
           servof4.write(0);
           servoPOf5.write(180);
           servof5.write(180);
           delay(5000);
           comando = 'm' ;
           break;
           
     case 'u':
           //Sujetar con cierre medio
           servox.write(90);
           servoy.write(90);
           servof1.write(90);
           servof2.write(90);
           servof3.write(90);
           servof4.write(90);
           servoPOf5.write(90);
           delay(1500);
           servof5.write(90);
           break; 
           
     case 'v':
           //Sujecion con dedos modo pinza
           servox.write(90);
           servoy.write(90);
           servof1.write(0);
           servof2.write(0);
           servof3.write(0);
           servof4.write(180);
           servof5.write(180);
           delay(3000);
           servoPOf5.write(90);
           delay(1500);
           servof5.write(0);
           servof4.write(0);
           break;
           
     case 'z':
           //Senal con el dedo Medio "fuck you"
           servox.write(90);
           servoy.write(90);
           servof1.write(0);
           servof2.write(0);
           servof3.write(180);
           servof4.write(0);
           servoPOf5.write(90);
           delay(1500);
           servof5.write(0);
           delay(5000);
           comando='m' ;
           break;
//----------------------------------------//          
//-------Comandos Estimacion de Pose-------//                     

     case '!':
           //Sujetar, cerrar todos los dedos
           servoPOf5.write(90);
           delay(1500);
           servof1.write(0);
           servof2.write(0);
           servof3.write(0);
           servof4.write(0);
           servof5.write(0);
           break;

     case '@':
           //Cerrar todos menos el Indice 
           servoPOf5.write(90);
           delay(1500);
           servof1.write(0);
           servof2.write(0);
           servof3.write(0);
           servof4.write(180);
           servof5.write(0);
           break;
 
     case '#':
           //Cerrar Menique, Anular y Pulgar
           servoPOf5.write(90);
           delay(1500);
           servof1.write(0);
           servof2.write(0);
           servof3.write(180);
           servof4.write(180);
           servof5.write(0);
           break;
    
     case '$':
           //Cerrar Menique y Pulagr
           servoPOf5.write(90);
           delay(1500);
           servof1.write(180);
           servof2.write(180);
           servof3.write(180);
           servof4.write(0);
           servof5.write(0);
           break;
    
     case '%':
           //Abrir todos los Dedos
           servof1.write(180);
           servof2.write(180);
           servof3.write(180);
           servof4.write(180);
           servof5.write(180);
           delay(1500);
           servoPOf5.write(180);
           break;
           
//----------------------------------------//
//----------------------------------------//
  }
}

//Controla el brazo al activar el modo de control por electromiografo y ejecuta los comanodos para este modo en particular
void emgControlador()
{
  selecModo();
}

//Define el modo de control deseado se puede controlar el agarre de la mano y la muneca
void selecModo()
{
  emgVal = leerEMG();
  if(emgVal >= emgLimSup && emgVal >= emgValAnt)
  {
    switch(verifModo())
    {
      case 1:
           //Modo de agarre
           agarre(true);
           break;
           
      case 2:
           //Modo de giro de muneca Pronacion y Supinacion
           giroMuneca(true);
           break;
           
      case 3:
           //Modo para volver a la posicion de inicio
           servoPos = 180;
           posServosPredMano();
           posServosPredMuneca();
           break;
           
      default:
           //Modo a la espera de una nueva seleccion
           puntero = 0;
           break;
    }
    emgValAnt = emgVal; 
  }
}

//Verifica el modo que se quiere seleccionar mediante pulsos creados por el musculo
int verifModo()
{
  varTiempo = 0; 
  antes = millis();
  while(varTiempo <= tiempoEspera)
  {
    ahora = millis();
    varTiempo=ahora-antes;
    emgVal = leerEMG();
    if(emgVal >= emgLimSup && emgVal >= emgValAnt)
    {
      flanSub = true;
    }
    else if(emgVal < emgLimSup && flanSub)
    {
      flanSub = false;
      puntero++;

    }    
    emgValAnt = emgVal; 
  }
  int auxPuntero = puntero;
  puntero = 0;
  return auxPuntero;
}

//Verifica y selecciona los otros modos de operacion dentro del modo principal seleccionado
int verifModoEnModo()
{
  emgVal = leerEMG();
  if(emgVal >= emgLimSup && !setTimer && !startTimer)
  {
    antes = millis();
    ahora = millis();
    setTimer = true;
    startTimer = true;
  }
  
  varTiempo=ahora-antes;
  if(varTiempo <= tiempoEsperaModo && startTimer)
  {
    emgVal = leerEMG();
    if(emgVal >= emgLimSup && emgVal >= emgValAnt)
    {
      flanSub = true;
    }
    else if(emgVal < emgLimSup && flanSub)
    {
      flanSub = false;
      puntero++;
    }    
    emgValAnt = emgVal;
    ahora = millis();
  }
  else
  {
    setTimer = false;
    emgVal = leerEMG();
    if(emgVal < emgLimSup)
    {
      startTimer = false;
      if(puntero != modo && puntero > 0)
      {
        seleccionado = false;
      }
      else if(puntero == modo && puntero > 0)
      {
        modo = 0;
        puntero = 0;
        servoPos = 180;
        posServosPredMano();
      }
      if(puntero > 0 && !seleccionado)
      {
        modo = puntero;
        puntero = 0;
        servoPos = 180;
        posServosPredMano();
        seleccionado = true;
      }
    }
  }
  return modo;
}

//Controla el modo Agarre y sus submodos
void agarre(bool modBucle)
{
  int modoAnt;
  while(modBucle)
  {  
    if(digitalRead(modoCtrl)) modBucle = false;
    switch(verifModoEnModo())
    {
      case 0:
        //agarre normal con todos los dedos
        if(emgVal >= emgLimSup)
        {
          servoPOf5.write(90);
          servof1.write(moverServos());
          servof2.write(moverServos());
          servof3.write(moverServos());
          servof4.write(moverServos());
          servof5.write(moverServos());
        }
        //mantiene el opositor del pulagr abierto si la mano no se a cerrado
        if(servoPos == 180) servoPOf5.write(180);
        modoAnt = modo;
        break;
        
      case 1:
        //agarre tipo pinza con indice y pulgar
        servof1.write(0);
        servof2.write(0);
        servof3.write(0);
        servoPOf5.write(90);
        if(emgVal >= emgLimSup)
        {
          servof4.write(moverServos());
          servof5.write(moverServos());
        }
        modoAnt = modo;
        break;
        
      case 2:
        //abre los dedos para soltar el objeto
        posServosPredMano();
        servoPos = 180;
        modo = modoAnt;
        break;
        
      default:
        //sale del modo agarre y vulve el menu de selecion 
        puntero = 0;
        modo = 0;
        servoPos = 180;
        modBucle = false;
        break;
    }
  }
}

//Controla el giro de la muneca y sus submodos
void giroMuneca(bool modBucle)
{
  while(modBucle)
  {  
    if(digitalRead(modoCtrl)) modBucle = false;
    switch(verifModoEnModo())
    {
      case 0:
        //manitiene la posicion de la muneca en el medio
        servox.write(90);
        servoPos = 180;
        break;
        
      case 1:
        //gira la muneca a la derecha sentido horario
        if(emgVal >= emgLimSup)
        {
          servox.write(map(moverServos(), 180, 0, 90, 0)); 
        }
        break;
        
      case 2:
        //gira la muneca a la izquierda sentido antihorario
        if(emgVal >= emgLimSup)
        {
          servox.write(map(moverServos(), 180, 0, 90, 180));
        }
        break;
        
      default:
        //sale del modo giro de muneca y vulve el menu de selecion
        puntero = 0;
        modo = 0;
        servoPos = 180;
        modBucle = false;
        break;
    }
  }
}

//Lee la senal del electromiografo pin A0
int leerEMG()
{
  return map(analogRead(emgPin),0,1023,1023,0);
}

//Controla el movimiento de los servos acrde al modo
int moverServos()
{
  ahoraS = millis();
  varTiempoS = ahoraS - antesS;
  if(varTiempoS >= tiempoEsperaServos)                    
  {
    servoPos -= 10; 
    servoPos = constrain(servoPos, 0, 180);
    antesS = ahoraS;
  }
  return servoPos;
}

//Coloca los dedos en pococion de inicio
void posServosPredMano()
{
  servof1.write(180);
  servof2.write(180);
  servof3.write(180);
  servof4.write(180);
  servof5.write(180);
  servoPOf5.write(180);
}

//Coloca la muneca en Posicion de inicio
void posServosPredMuneca()
{
  servox.write(90);
  servoy.write(90);
  servoz.write(90);
}

//Coloca la camara para vision remota en posicion de incio
void posServosPredCam()
{
  servoCamz.write(90);
  servoCamy.write(90);
}
//------Fin Funciones de Control----------//
//------------------Fin-------------------//
