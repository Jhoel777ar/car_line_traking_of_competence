// Definiciones de pines
#define S1 A0  // Sensor 1 (izquierda)
#define S2 A1  // Sensor 2
#define S3 A2  // Sensor 3
#define S4 A3  // Sensor 4 (derecha)
#define IN1 9  // Motor izquierdo, entrada 1
#define IN2 10 // Motor izquierdo, entrada 2
#define ENA 5  // Motor izquierdo, PWM velocidad
#define IN3 6  // Motor derecho, entrada 1
#define IN4 7  // Motor derecho, entrada 2
#define ENB 11 // Motor derecho, PWM velocidad

float Kp_base = 25.0; 
float Ki = 0.1;     
float Kd_base = 15.0; 
float Kp = Kp_base;   
float Kd = Kd_base;  
float error = 0, previousError = 0, integral = 0, derivative = 0;
float setpoint = 1.5; 
int baseSpeed = 100;  
int maxSpeed = 200;  
int minSpeed = 0;    
const float integralLimit = 50.0; 

int sensorThreshold = 500; 
int sensorValues[4];       
bool digitalValues[4];     

bool intersectionDetected = false;
unsigned long intersectionTime = 0;
const int intersectionDelay = 500; 

#define MAX_INTERSECTIONS 20
char pathHistory[MAX_INTERSECTIONS];
int pathIndex = 0;
int leftCount = 0, rightCount = 0, straightCount = 0;

int sensorMin[4] = {1023, 1023, 1023, 1023};
int sensorMax[4] = {0, 0, 0, 0};

void setup() {
  pinMode(S1, INPUT);
  pinMode(S2, INPUT);
  pinMode(S3, INPUT);
  pinMode(S4, INPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(ENA, OUTPUT);
  pinMode(ENB, OUTPUT);

  Serial.begin(9600);

  Serial.println("Calibrando sensores... Mueve el carro sobre la línea y fondo blanco.");
  unsigned long startTime = millis();
  while (millis() - startTime < 10000) {
    sensorValues[0] = analogRead(S1);
    sensorValues[1] = analogRead(S2);
    sensorValues[2] = analogRead(S3);
    sensorValues[3] = analogRead(S4);
    
    for (int i = 0; i < 4; i++) {
      if (sensorValues[i] < sensorMin[i]) sensorMin[i] = sensorValues[i];
      if (sensorValues[i] > sensorMax[i]) sensorMax[i] = sensorValues[i];
    }
  }
  sensorThreshold = (sensorMin[0] + sensorMax[0]) / 2;
  Serial.println("Calibración completa.");
}

void loop() {
  readSensors();
  if (isIntersection()) {
    handleIntersection();
  } else if (isLineLost()) {
    stopMotors();
  } else {
    followLine();
  }
}

void readSensors() {
  sensorValues[0] = analogRead(S1);
  sensorValues[1] = analogRead(S2);
  sensorValues[2] = analogRead(S3);
  sensorValues[3] = analogRead(S4);
  for (int i = 0; i < 4; i++) {
    digitalValues[i] = sensorValues[i] > sensorThreshold ? 0 : 1; 
  }
}

bool isIntersection() {
  return (digitalValues[0] && digitalValues[1] && digitalValues[2] && digitalValues[3]) ||
         (digitalValues[1] && digitalValues[2] && digitalValues[3]);
}

bool isLineLost() {
  return (!digitalValues[0] && !digitalValues[1] && !digitalValues[2] && !digitalValues[3]);
}

void handleIntersection() {
  if (!intersectionDetected) {
    intersectionDetected = true;
    intersectionTime = millis();
    stopMotors();
  }
  if (millis() - intersectionTime > intersectionDelay) {
    char decision = chooseDirection();
    if (decision == 'L') {
      turnLeft();
      delay(500); 
    } else if (decision == 'R') {
      turnRight();
      delay(500);
    } else {
      goStraight();
      delay(200); 
    }
    if (pathIndex < MAX_INTERSECTIONS) {
      pathHistory[pathIndex++] = decision;
      if (decision == 'L') leftCount++;
      else if (decision == 'R') rightCount++;
      else straightCount++;
    }
    intersectionDetected = false;
  }
}

char chooseDirection() {
  int minCount = min(leftCount, min(rightCount, straightCount));
  if (minCount == leftCount && digitalValues[0]) return 'L'; 
  if (minCount == rightCount && digitalValues[3]) return 'R';
  return 'S';
}

void followLine() {
  float position = 0;
  int sensorCount = 0;
  for (int i = 0; i < 4; i++) {
    if (digitalValues[i]) {
      position += i;
      sensorCount++;
    }
  }
  if (sensorCount > 0) {
    position /= sensorCount; 
  } else {
    position = setpoint;
  }

  error = position - setpoint;
  
  if (abs(error) > 1.0) {
    Kp = Kp_base * 1.5; 
    Kd = Kd_base * 0.8; 
  } else {
    Kp = Kp_base;       
    Kd = Kd_base * 1.2;
  }

  integral += error;
  integral = constrain(integral, -integralLimit, integralLimit); 
  
  derivative = error - previousError;
  float correction = Kp * error + Ki * integral + Kd * derivative;
  previousError = error;

  int leftSpeed = baseSpeed + correction;
  int rightSpeed = baseSpeed - correction;

  leftSpeed = constrain(leftSpeed, minSpeed, maxSpeed);
  rightSpeed = constrain(rightSpeed, minSpeed, maxSpeed);

  setMotorSpeed(leftSpeed, rightSpeed);
}

void setMotorSpeed(int leftSpeed, int rightSpeed) {
  if (leftSpeed >= 0) {
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
  } else {
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, HIGH);
    leftSpeed = -leftSpeed;
  }

  if (rightSpeed >= 0) {
    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, LOW);
  } else {
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, HIGH);
    rightSpeed = -rightSpeed;
  }

  analogWrite(ENA, leftSpeed);
  analogWrite(ENB, rightSpeed);
}

void stopMotors() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
  analogWrite(ENA, 0);
  analogWrite(ENB, 0);
}

void turnRight() {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  analogWrite(ENA, baseSpeed);
  analogWrite(ENB, baseSpeed);
}

void turnLeft() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  analogWrite(ENA, baseSpeed);
  analogWrite(ENB, baseSpeed);
}

void goStraight() {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  analogWrite(ENA, baseSpeed);
  analogWrite(ENB, baseSpeed);
}