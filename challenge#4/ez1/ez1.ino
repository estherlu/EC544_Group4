// Maxbotix EZ1 test program
// Mike Grusin, SFE, 2011
// This program is free, use it however you wish!

// HARDWARE:
// Make the following connections between the EZ1 and the Arduino
// +5V - +5V
// GND - GND
// AN - Analog In 0

// INSTRUCTIONS:
// Upload this sketch to your Arduino, then activate the Serial Monitor
// (set the Serial Monitor to 9600 baud)

void setup()
{
  // initialize serial communications
  Serial.begin(9600); 
}

void loop()
{
  int sensor, inches, x;
  
  // read the analog output of the EZ1 from analog input 0
  sensor = analogRead(0);
  
  // convert the sensor reading to inches
  inches = sensor / 2; //512 = 254 inches, 
  
  // print out the decimal result
  Serial.print(inches,DEC);
  
  // print out a graphic representation of the result
  Serial.print(" ");
  for (x=0;x<(inches/5);x++)
  {
    Serial.print(".");
  }
  Serial.println("|");

  // pause before taking the next reading
  delay(100);   
}            
