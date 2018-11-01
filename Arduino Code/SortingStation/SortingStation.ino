// 2, 3, 18, 19, 20, 21  = Interrupt Pin
//Inputs
int metalSensor = 2;
int sortSensor = 3; //IR sensor 1st
int hopperSensor = 4;
int in5 = 53; // IR sensor reject
int in3 = 11; //capacitive sensor
int hopperState = LOW;
int Sin3 = 1;

//OUTPUTS
int belt_1 = 5;
int belt_2 = 6;
int sortSolenoid = 8;
int rotarySolenoid = 7;
int rejectSolenoid = 9;

//boolean control and timing interval
bool flag_pass = false;
bool flag_counter = true; //Counter++
bool flag_counterN = false; //Counter--
bool flag_rotate = true;
int fp_interval = 2500; //flag_pass interval
int extendPeriod = 300; //output extension time
int rotationDelay = 2200;
int queueCounter;

unsigned long currentTime;
unsigned long fp_latchedTime = 0;
unsigned long t_out1 = 0;
unsigned long t_out2 = 0;
unsigned long t_out3 = 0;

void setup() {
  Serial.begin(9600);
  pinMode(metalSensor, INPUT_PULLUP);
  pinMode(sortSensor, INPUT_PULLUP);
  pinMode(hopperSensor, INPUT);
  pinMode(in5 , INPUT);
  pinMode(in3 , INPUT);
  pinMode(rejectSolenoid, OUTPUT);
  pinMode(sortSolenoid, OUTPUT);
  pinMode(rotarySolenoid, OUTPUT);
  pinMode(belt_1 , OUTPUT);
  pinMode(belt_2 , OUTPUT);
  attachInterrupt(digitalPinToInterrupt(metalSensor), metalDetected, RISING);
  attachInterrupt(digitalPinToInterrupt(sortSensor), componentDetected_1st, HIGH);
  digitalWrite(belt_1, HIGH);
  digitalWrite(belt_2, HIGH);
  queueCounter  = 0;
}

void loop() {
  Serial.println (queueCounter);
  currentTime = millis();
  timingControl();
  hopperState = digitalRead(hopperSensor);
  if (hopperState == LOW && queueCounter > 0 && flag_rotate) {
    digitalWrite(rotarySolenoid, HIGH);
    flag_counterN = true;
    t_out2 = currentTime;
  }

  //Xing Yik's Part
  Serial.print(digitalRead(in3));
  Serial.print(Sin3);
  Serial.print(digitalRead(in5));
  Serial.print("\t");
  Serial.print(millis());
  Serial.print("\t");
  Serial.println(t_out3);
  
  if (digitalRead(in3) == 0)
  {
    Sin3 = 0;
  }

  if (digitalRead(in5) == HIGH)
  {
    if (Sin3 == 1)// sensed metal & solenoid
    {
      digitalWrite(rejectSolenoid, HIGH);
      t_out3 = millis();
    }
    else
    {
      t_out3 = millis();
      Serial.println("assembly");
      while (millis() - t_out3 < 1000)
      {
        //nothing
      }
      Sin3 = 1;
    }
  }

}

void metalDetected() {
  //  Serial.println("metal");
  flag_pass = true;
  fp_latchedTime = currentTime;
}

void componentDetected_1st () {
  //  Serial.println("push");
  if (!flag_pass && queueCounter < 5) {
    digitalWrite(sortSolenoid, HIGH);
    t_out1 = currentTime;
    flag_rotate = false;
    if (flag_counter) {
      queueCounter++;
      flag_counter = false;
    }
  }
}

void timingControl() {
  //Control passing of metal, unflag after the interval;
  if ( (currentTime - fp_latchedTime) > fp_interval) {
    flag_pass = false;
  }

  //Control extension of solenoid, retract after extendPeriod
  if ((currentTime - t_out1) > extendPeriod) {
    digitalWrite( sortSolenoid, LOW);
    flag_counter = true;
  }

  if ((currentTime - t_out1) > rotationDelay ) {
    flag_rotate = true;
  }

  if ((currentTime - t_out2) > extendPeriod && flag_counterN == true) {
    digitalWrite( rotarySolenoid, LOW);
    queueCounter--;
    flag_counterN = false;
  }

  
  if (currentTime - t_out3 > extendPeriod)
  {
    digitalWrite(rejectSolenoid, LOW);
  }
}

