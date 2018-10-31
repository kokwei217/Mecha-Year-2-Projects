int in5 = 6; //in5, IR Sensor
int in3 = 2;
int Sin3 = 1;
int out3 = 5; 

unsigned long Tout3;

void setup() {
  // put your setup code here, to run once:
  pinMode(in5 , INPUT);
  pinMode(in3 , INPUT);
  pinMode(out3, OUTPUT);
  
  Serial.begin(19200);
}  

void loop() {
  Serial.print(digitalRead(in3));
  Serial.print(Sin3);
  Serial.print(digitalRead(in5));
  Serial.print("\t");
  Serial.print(millis());
  Serial.print("\t");
  Serial.println(Tout3);

  if (digitalRead(in3) == 0)
  {
    Sin3 = 0;
  }
  
  if (digitalRead(in5) == HIGH)
  {
    if (Sin3 == 1)// sensed metal & solenoid 
    {
      digitalWrite(out3, HIGH);
      Tout3 = millis();
    }
    else 
    {
      Tout3 = millis();
      Serial.println("assembly");
      while (millis() - Tout3 < 1000)
      {
        //nothing
      }
      Sin3 = 1;
    }
  }
  
  if (millis() - Tout3 > 100)
  {
    digitalWrite(out3, LOW);
  }
  
}


