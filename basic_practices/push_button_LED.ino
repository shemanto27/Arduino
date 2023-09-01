int pushbutton = 12;
int buttonstate = 0;
int LED = 13;

void setup()
{
  pinMode(pushbutton, INPUT);
  pinMode(LED, OUTPUT);
}

void loop()
{
buttonstate = digitalRead(pushbutton);

if (buttonstate == HIGH)
{
	digitalWrite(LED,HIGH);
}
  else{
	digitalWrite(LED,LOW);
  }
 
}
