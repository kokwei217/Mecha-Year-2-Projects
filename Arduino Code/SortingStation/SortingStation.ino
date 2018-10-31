// 2, 3, 18, 19, 20, 21  = Interrupt Pin
//Inputs
int metalSensor = 2;
int sortSensor = 3; //IR sensor
int hopperSensor = 8;

//OUTPUTS
int belt_1 = 5;
int belt_2 = 6;
int sortSolenoid = 4;
int rotarySolenoid = 7;

//boolean control and timing interval
bool flag_pass = false;
bool flag_counter = true;
bool flag_rotate = true;
int fp_interval = 2500;
int extendPeriod = 500;
int rotationDelay = 2200;
int queueCounter;

unsigned long currentTime;
unsigned long fp_latchedTime = 0;
unsigned long componentDetect_time = 0;

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
  int hopperState = digitalRead(hopperSensor);
  if (hopperState == LOW && queueCounter > 0 && flag_rotate) {
    digitalWrite(rotarySolenoid, HIGH);
    queueCounter--;
    delay(100);
    digitalWrite(rotarySolenoid, LOW);
  }
}

void metalDetected() {
  flag_pass = true;
  fp_latchedTime = currentTime;
}

void componentDetected_1st () {
  if (!flag_pass && queueCounter < 5) {
    digitalWrite(sortSolenoid, HIGH);
    componentDetect_time = currentTime;
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
  if ((currentTime - componentDetect_time) > extendPeriod) {
    digitalWrite( sortSolenoid, LOW);
    flag_counter = true;
  }

  if ((currentTime - componentDetect_time) > rotationDelay ) {
    flag_rotate = true;
  }
}

