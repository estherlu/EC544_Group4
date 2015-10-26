#define THRESHOLD 250
#define IR_PIN A0

void setup() 
{
  Serial.begin(9600); 
}

void loop() 
{ 
  float sensorValue = 0.0048828125 * analogRead(IR_PIN);

  float distance = 60*pow(sensorValue, -1.10);     // worked out from graph 65 = theretical distance / (1/Volts)S - luckylarry.co.uk
                      // print the distance
  delay(100);                                     // arbitary wait time.


 /* if (sensorValue < THRESHOLD )
  {
    Serial.write("clear");; //High Reflectance.  
  }
  else 
  {
    Serial.write("stop"); // Low Reflectance.
  }*/

  
  Serial.print("value: ");
  Serial.println(sensorValue);
  Serial.println(distance);   

  delay(1000);
}
