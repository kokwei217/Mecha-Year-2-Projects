// 2, 3, 18, 19, 20, 21  = Interrupt Pin
//Inputs
int metalSensor = 2;
int sortSensor = 3;
int hopperSensor = 4;
int in5 = 18;
int in4 = 53;
int in3 = 11;
int hopperState = LOW;

//OUTPUTS
int belt_1 = 5;
int belt_2 = 6;
int sortSolenoid = 8;
int rotarySolenoid = 7;
int rejectSolenoid = 9;

//boolean control and timing interval
bool flag_metal = false;
bool flag_counter = true; //Counter++
bool flag_counterN = false; //Counter--
bool flag_rotate = true;
bool flag_check = false;
bool flag_reject = false;
bool flag_pass = true;
int fm_interval = 2500; //flag_metal interval
int extendPeriod = 300; //output extension time
int rotationDelay = 1100;
int queueCounter;

unsigned long currentTime;
unsigned long fp_latchedTime = 0;
unsigned long passTime = 0; //time when assembled product pass through ip 5
unsigned long t_out1 = 0;
unsigned long t_out2 = 0;
unsigned long t_out3 = 0;
unsigned long t_ip4 = 0;
unsigned long t_ip3 = 0;


void setup() {
  Serial.begin(9600);
  pinMode(metalSensor, INPUT_PULLUP);
  pinMode(sortSensor, INPUT_PULLUP);
  pinMode(hopperSensor, INPUT);
  pinMode(in5 , INPUT_PULLUP);
  pinMode(in4, INPUT);
  pinMode(in3 , INPUT);
  pinMode(rejectSolenoid, OUTPUT);
  pinMode(sortSolenoid, OUTPUT);
  pinMode(rotarySolenoid, OUTPUT);
  pinMode(belt_1 , OUTPUT);
  pinMode(belt_2 , OUTPUT);
  attachInterrupt(digitalPinToInterrupt(metalSensor), metalDetected, RISING);
  attachInterrupt(digitalPinToInterrupt(sortSensor), componentDetected_1st, RISING);
  attachInterrupt(digitalPinToInterrupt(in5), rejectionCheck, RISING);
  digitalWrite(belt_1, HIGH);
  digitalWrite(belt_2, HIGH);
  queueCounter  = 0;
}

void loop() {
  Serial.print (queueCounter);
  currentTime = millis();
  timingControl();
  hopperState = digitalRead(hopperSensor);
  if (hopperState == LOW && queueCounter > 0 && flag_rotate) {
    digitalWrite(rotarySolenoid, HIGH);
    flag_counterN = true;
    t_out2 = currentTime;
  }

  //Xing Yik's Part
  Serial.print(flag_pass);
  Serial.println(flag_reject);

  //detect assembled
  if (digitalRead(in3) == LOW)  {
    flag_pass = true;
    //    flag_reject = false;
    t_ip3 = currentTime;
  }

  //detect Others
  if (digitalRead(in4) == LOW && !flag_pass) {
    Serial.println("REJECT");
    flag_check = true;
    t_ip4 = currentTime;
  }
}


void metalDetected() {
  flag_metal = true;
  fp_latchedTime = currentTime;
}

void componentDetected_1st () {
  if (!flag_metal && queueCounter < 5) {
    digitalWrite(sortSolenoid, HIGH);
    t_out1 = currentTime;
    flag_rotate = false;
    if (flag_counter) {
      queueCounter++;
      flag_counter = false;
    }
  }
}

void rejectionCheck () {
  if (flag_reject ) {
    digitalWrite(rejectSolenoid, HIGH);
    t_out3 = currentTime;
    flag_reject = false;
  }
}

void timingControl() {
  //Control passing of metal, unflag after the interval;
  if ( (currentTime - fp_latchedTime) > fm_interval) {
    flag_metal = false;
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

  if (currentTime - t_out3 > extendPeriod) {
    digitalWrite(rejectSolenoid, LOW);
  }

  //flag reject afteer 1.8sec
  if ((currentTime - t_ip4) > 1800 && flag_check) {
    flag_check = false;
    flag_reject = true;
  }

  if ((currentTime - t_ip3) > 1500) {
    flag_pass = false;
  }
}

