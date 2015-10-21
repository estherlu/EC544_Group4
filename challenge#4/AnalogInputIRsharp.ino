

int sensorPin = A0;    // select the input pin for the potentiometer     
int sensorValue = 0;  // variable to store the value coming from the sensor
int distance;
void setup() {
  // declare the ledPin as an OUTPUT:
    Serial.begin(9600); 
}

void loop() {
  
  sensorValue = analogRead(sensorPin);
  distance=map(sensorValue,0,1023,80,10);
 
  Serial.print(distance);
  Serial.print("Distance in centimeter");
 // Serial.print(sensorValue);
  Serial.println(" ");
  delay(1000);
}
