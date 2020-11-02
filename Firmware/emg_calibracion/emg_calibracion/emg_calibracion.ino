
const int emgPin = A0;  
int  emgVal = 0;        

void setup() {
  Serial.begin(9600);
}

void loop() {

  emgVal = analogRead(emgPin);

  Serial.print("EMG-sensor = ");
  Serial.println(emgVal);
  delay(2);
}
