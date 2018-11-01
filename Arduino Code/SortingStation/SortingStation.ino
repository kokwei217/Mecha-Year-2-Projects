// 2, 3, 18, 19, 20, 21  = Interrupt Pin
//Inputs
int metalSensor = 2;
int sortSensor = 3; //IR sensor
int hopperSensor = 4;
int hopperState = LOW;

//OUTPUTS
int belt_1 = 5;
int belt_2 = 6;
int sortSolenoid = 8;
int rotarySolenoid = 7;

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

void setup() {
  Serial.begin(9600);
  pinMode(metalSensor, INPUT_PULLUP);
  pinMode(sortSensor, INPUT_PULLUP);
  pinMode(hopperSensor, INPUT);
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
}

