// D0, D1, IN0, IN1, SDA, SCL  = Interrupt Pin
//Inputs
const int metalSensor = 2;
const int sortSensor = 3;
const int hopperSensor = 4;
const int in5 = 18;
const int in4 = 53;
const int in3 = 21;
int hopperState = LOW;

//OUTPUTS
const int belt_1 = 5;
const int belt_2 = 6;
const int sortSolenoid = 8;
const int rotarySolenoid = 7;
const int rejectSolenoid = 9;

//boolean control and timing interval
bool isMetal = false;
bool isAssembly = false;
bool isReject = false;
bool flag_counterIn = true; //Counter++
bool flag_counterOut = false; //Counter--
bool flag_rotate = true;
bool flag_check = false;
const int fm_interval = 2500; // metal detected, sort sensor sleep interval
const int extendPeriod = 300; //output extension time
const int rotationDelay = 1100;
int queueCounter;

unsigned long currentTime;
unsigned long metal_latchedTime = 0;
unsigned long t_out1, t_out2, t_out3;
unsigned long t_ip4, t_ip3;


void setup() {
  Serial.begin(9600);
  pinMode(metalSensor, INPUT_PULLUP);
  pinMode(sortSensor, INPUT_PULLUP);
  pinMode(hopperSensor, INPUT);
  pinMode(in5 , INPUT_PULLUP);
  pinMode(in4, INPUT);
  pinMode(in3 , INPUT_PULLUP);
  pinMode(rejectSolenoid, OUTPUT);
  pinMode(sortSolenoid, OUTPUT);
  pinMode(rotarySolenoid, OUTPUT);
  pinMode(belt_1 , OUTPUT);
  pinMode(belt_2 , OUTPUT);
  attachInterrupt(digitalPinToInterrupt(metalSensor), metalDetected, RISING);
  attachInterrupt(digitalPinToInterrupt(sortSensor), componentDetected_1st, RISING);
  attachInterrupt(digitalPinToInterrupt(in5), rejectionCheck, RISING);
  attachInterrupt(digitalPinToInterrupt(in3), assemblyCheck, RISING);
  digitalWrite(belt_1, HIGH);
  digitalWrite(belt_2, HIGH);
  queueCounter  = 0;
  t_out1 = t_out2 = t_out3 = t_ip4 = t_ip3 0;
}

void loop() {
  Serial.print (queueCounter);
  currentTime = millis();
  timingControl();
  hopperState = digitalRead(hopperSensor);
  if (hopperState == LOW && queueCounter > 0 && flag_rotate) {
    digitalWrite(rotarySolenoid, HIGH);
    flag_counterOut = true;
    t_out2 = currentTime;
  }

  //Xing Yik's Part
  Serial.print(isAssembly);
  Serial.println(isReject);

  //detect Others
  if (digitalRead(in4) == LOW && !isAssembly) {
    Serial.println("REJECT");
    flag_check = true;
    t_ip4 = currentTime;
  }
}


void metalDetected() {
  isMetal = true;
  metal_latchedTime = currentTime;
}

void componentDetected_1st () {
  if (!isMetal && queueCounter < 5) {
    digitalWrite(sortSolenoid, HIGH);
    t_out1 = currentTime;
    flag_rotate = false;
    if (flag_counterIn) {
      queueCounter++;
      flag_counterIn = false;
    }
  }
}

void rejectionCheck () {
  if (isReject ) {
    digitalWrite(rejectSolenoid, HIGH);
    t_out3 = currentTime;
    isReject = false;
  }
}

void assemblyCheck() {
  isAssembly = true;
  t_ip3 = currentTime;
}

void timingControl() {
  //Control passing of metal, unflag after the interval;
  if ( (currentTime - metal_latchedTime) > fm_interval) {
    isMetal = false;
  }

  //Control extension of solenoid, retract after extendPeriod
  if ((currentTime - t_out1) > extendPeriod) {
    digitalWrite( sortSolenoid, LOW);
    flag_counterIn = true;
  }

  if ((currentTime - t_out1) > rotationDelay ) {
    flag_rotate = true;
  }

  if ((currentTime - t_out2) > extendPeriod && flag_counterOut == true) {
    digitalWrite( rotarySolenoid, LOW);
    queueCounter--;
    flag_counterOut = false;
  }

  if (currentTime - t_out3 > extendPeriod) {
    digitalWrite(rejectSolenoid, LOW);
  }

  //flag reject afteer 1.8sec of sensing "reject"
  if ((currentTime - t_ip4) > 1800 && flag_check) {
    flag_check = false;
    isReject = true;
  }

  if ((currentTime - t_ip3) > 1700) {
    isAssembly = false;
  }
}

