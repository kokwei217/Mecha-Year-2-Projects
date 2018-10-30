// 2, 3, 18, 19, 20, 21  = Interrupt Pin
//Define states
int metalSensor = 2;
int sortSensor = 3;
int metalSensorState;
int sortSensorState;

void setup() {
  Serial.begin(9600);
  pinMode(metalSensor, INPUT);
  pinMode(sortSensor, INPUT);
}

void loop() {
  metalSensorState = digitalRead(metalSensor);
  Serial.println(metalSensorState);
}

