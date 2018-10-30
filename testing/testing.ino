int belt_1 = 5;
int belt_2 = 6;
int sensor = A15;
int sensorState = 0;

int sortSolenoid = 4;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(belt_1, OUTPUT);
  pinMode(sensor, INPUT_PULLUP);
  pinMode(sortSolenoid, OUTPUT);
  digitalWrite(belt_1, HIGH);
}

void loop() {
  // put your main code here, to run repeatedly:
  sensorState = analogRead(sensor);
  Serial.println(sensorState);
  if (sensorState > 960){
    Serial.println("PUSH");
    digitalWrite(sortSolenoid, HIGH);
    delay(200);
    digitalWrite(sortSolenoid,LOW);
  }
  
  

}

void success (){
  Serial.println("IR");
}

