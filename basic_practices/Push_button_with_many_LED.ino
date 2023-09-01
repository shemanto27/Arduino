int pushbutton_1 = 7;
int pushbutton_2 = 8;
int pushbutton_3 = 9;

int buttonstate_1 = 0;
int buttonstate_2 = 0;
int buttonstate_3 = 0;

int Blue_LED = 10;
int Green_LED = 11;
int Red_LED = 12;



void setup()
{
  pinMode(pushbutton_1, INPUT);
  pinMode(pushbutton_2, INPUT);
  pinMode(pushbutton_3, INPUT);

  pinMode(Red_LED, OUTPUT);
  pinMode(Green_LED, OUTPUT);
  pinMode(Blue_LED, OUTPUT);

}

void loop()
{
buttonstate_1 = digitalRead(pushbutton_1);
buttonstate_2 = digitalRead(pushbutton_2);
buttonstate_3 = digitalRead(pushbutton_3);

if (buttonstate_1 == HIGH)
{
	digitalWrite(Blue_LED,HIGH);
}
  else{
	digitalWrite(Blue_LED,LOW);
  }

if (buttonstate_2 == HIGH)
{
	digitalWrite(Green_LED,HIGH);
}
  else{
	digitalWrite(Green_LED,LOW);
  }

if (buttonstate_3 == HIGH)
{
	digitalWrite(Red_LED,HIGH);
}
  else{
	digitalWrite(Red_LED,LOW);
  }
 
}
