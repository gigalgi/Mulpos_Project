#include "Wire.h"
#include <MPU6050_light.h>

MPU6050 mpu(Wire);
unsigned long timer = 0;
const int waitTime = 10;//ms
const int ledPin = 13;
void setup() 
{
  pinMode(ledPin, OUTPUT);
  Serial.begin(38400);
  Wire.begin();
  
  byte status = mpu.begin();
  while(status!=0){ } // en spera hasta conecarse con el modulo mpu6050
  digitalWrite(ledPin, HIGH);
  delay(1000);
  mpu.calcOffsets(); // calculando offset de gyro y acelerometro
  digitalWrite(ledPin, LOW);
}

void loop() 
{
  mpu.update();// se refresca el valor del gyro y acelerometro
  
  if((millis()-timer)>waitTime)// se envia el dato cada x tiempo segun var waitTime
  { 
    String Xang = "x" + String(constrain(round(mpu.getAngleX()),-90,90)) + "|";//se obtiene el angulo en X, se limita su valor
    String Yang = "y" + String(constrain(round(mpu.getAngleY()),-90,90)) + "|"; //se obtiene el angulo en Y, se limita su valor
    Serial.println(Xang);
    Serial.println(Yang);
    timer = millis();  
  }
}
