#include <Controllino.h>

#define IP1_SLEEP_TIME 2500
#define IP4_SLEEP_TIME 1800
#define EXTEND_TIME 300
#define ROTATION_DELAY 1100
#define FLAG_REJECT_DELAY 1800

//Inputs
const int metalSensor = CONTROLLINO_IN0;
const int sortSensor = CONTROLLINO_A1; //first ir sensor
const int hopperSensor = CONTROLLINO_A2;
const int ip3 = CONTROLLINO_IN1; // assembly detection
const int ip4 = CONTROLLINO_A4; // ir sensor 2nd
const int ip5 = CONTROLLINO_A5; //reject detection
const int ip6 = CONTROLLINO_A6;
const int ip7 = CONTROLLINO_A7;

//OUTPUTS
const int belt_1 = CONTROLLINO_D2;
const int belt_2 = CONTROLLINO_D1;
const int sortSolenoid = CONTROLLINO_D4;
const int rotarySolenoid = CONTROLLINO_D5;
const int rejectSolenoid = CONTROLLINO_D6;

//boolean control and timing interval
volatile bool isMetal = false;
volatile bool isAssembly = false;
bool isReject = false;
bool flag_counterIn = true; //Counter++
bool flag_counterOut = false; //Counter--
bool flag_rotate = true;
bool flag_check = false;

unsigned long currentTime;
unsigned long metal_latchedTime = 0;
unsigned long t_out1, t_out2, t_out3;
unsigned long t_ip3, t_ip4 ;
int queueCounter = 0;

char c;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(metalSensor, INPUT_PULLUP);
  pinMode(sortSensor, INPUT);
  pinMode(hopperSensor, INPUT);
  pinMode(ip3 , INPUT_PULLUP);
  pinMode(ip4, INPUT);
  pinMode(ip5, INPUT);
  pinMode(ip6, INPUT);
  pinMode(ip7, INPUT);
  pinMode(rejectSolenoid, OUTPUT);
  pinMode(sortSolenoid, OUTPUT);
  pinMode(rotarySolenoid, OUTPUT);
  pinMode(belt_1 , OUTPUT);
  pinMode(belt_2 , OUTPUT);
  attachInterrupt(digitalPinToInterrupt(metalSensor), metalDetected, RISING);
  attachInterrupt(digitalPinToInterrupt(ip3), assemblyDetected, RISING);
  t_out1 = t_out2 = t_out3 = t_ip3 = t_ip4 = 0;
}

void loop() {
  //  Serial.println (queueCounter);
  //  Serial.println(isReject);
  currentTime = millis();
  timingControl();

  if (Serial.available() > 0) {
    c = Serial.read();
    if (c == 'a') {
      digitalWrite(belt_1 , HIGH);
      digitalWrite(belt_2, HIGH);
    }
    else {
      digitalWrite(belt_1 , LOW);
      digitalWrite(belt_2 , LOW);
    }
  }

  if (digitalRead(ip6) == HIGH) {
    digitalWrite(belt_1 , HIGH);
    digitalWrite(belt_2, HIGH);
  }

  if (digitalRead(ip7) == LOW) {
    digitalWrite(belt_1 , LOW);
    digitalWrite(belt_2 , LOW);
  }

  if (digitalRead(sortSensor) == HIGH && !isMetal && queueCounter < 5)
    pushRing();

  if (digitalRead(hopperSensor) == LOW && queueCounter > 0 && flag_rotate) {
    digitalWrite(rotarySolenoid, HIGH);
    flag_counterOut = true;
    t_out2 = currentTime;
  }

  //detection of non-assembled products then prepare to flag it as a "reject"
  if (digitalRead(ip4) == LOW && !isAssembly) {
    //    Serial.println("REJECT");
    flag_check = true; // isReject trigger method , need this so it only triggers once
    t_ip4 = currentTime;
  }

  if (digitalRead(ip5) == HIGH && isReject) {
    digitalWrite(rejectSolenoid, HIGH);
    t_out3 = currentTime;
    isReject = false;
  }
}

void metalDetected() {
  isMetal = true;
  metal_latchedTime = currentTime;
}

void pushRing() {
  digitalWrite(sortSolenoid, HIGH);
  t_out1 = currentTime;
  flag_rotate = false;
  if (flag_counterIn) {
    queueCounter++;
    flag_counterIn = false;
  }
}

void assemblyDetected() {
  Serial.println("assembly");
  isAssembly = true;
  t_ip3 = currentTime;
}

void timingControl() {
  //Control passing of metal, unflag after the interval;
  if ((currentTime - metal_latchedTime) > IP1_SLEEP_TIME)
    isMetal = false;

  //Control extension of solenoid, retract after extendPeriod
  if ((currentTime - t_out1) > EXTEND_TIME) {
    digitalWrite( sortSolenoid, LOW);
    flag_counterIn = true;
  }

  if ((currentTime - t_out1) > ROTATION_DELAY )
    flag_rotate = true;

  if ((currentTime - t_out2) > EXTEND_TIME && flag_counterOut == true) {
    digitalWrite( rotarySolenoid, LOW);
    queueCounter--;
    flag_counterOut = false;
  }

  if (currentTime - t_out3 > EXTEND_TIME)
    digitalWrite(rejectSolenoid, LOW);

  //flag reject afteer 1.8sec of sensing "reject"
  if ((currentTime - t_ip4) > FLAG_REJECT_DELAY && flag_check) {
    flag_check = false;
    isReject = true;
  }
  if ((currentTime - t_ip3) > IP4_SLEEP_TIME)
    isAssembly = false;
}

