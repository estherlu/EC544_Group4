int switchState=0;
boolean leader=false;
int id;
int red=7;
int blue=6;
int green=5;
void setup() {
  Serial.begin(9600);
  pinMode(green, OUTPUT);
  pinMode(blue, OUTPUT);
  pinMode(red, OUTPUT);

  pinMode(8, INPUT);
  digitalWrite(green,HIGH);
}

void loop() {

   switchState=digitalRead(8);
  //(write code)if the ID is lowest Leader == 1

  if(leader==true)
  {
    digitalWrite(blue,HIGH);
    digitalWrite(green,LOW);
    digitalWrite(red,LOW);
  }
  
  
  else
  { 
   if(switchState==1)
  { digitalWrite(red, HIGH);
    digitalWrite(green,LOW);
  }
  Serial.println(switchState);
   delay(300);
}
}
