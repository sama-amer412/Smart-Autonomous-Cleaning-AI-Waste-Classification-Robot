#include <Servo.h>

// Pin Definitions
#define TRIG 8
#define ECHO 2
#define ENA 3
#define ENB 11
#define IN1 4
#define IN2 5
#define IN3 6
#define IN4 7
#define SERVO_PIN 9
#define MOP_MOTOR_PIN A4
#define PUMP_RELAY A3
#define IR1_Obstacle A0
#define IR2_Obstacle A1
#define IR_EDGE A2
#define buzzer A5

Servo myServo;

// Ultrasonic Interrupt Variables
volatile unsigned long startTime = 0;
volatile unsigned long endTime = 0;
volatile bool newData = false;
float distance = 0;

// State Machine and Timing Variables
unsigned long lastCycleResetTime = 0;
unsigned long stageStartTime = 0;
bool isCleaningCycle = false;
int cleaningStage = 0; // 0 = Idle Navigation, 1 = Stage 1, 2 = Stage 2, 3 = Stage 3

char mode = 'M';   // 'M' = Manual, 'A' = Auto

void setup() {
  Serial.begin(115200);
  
  pinMode(TRIG, OUTPUT);
  pinMode(ECHO, INPUT);
  pinMode(ENA, OUTPUT);
  pinMode(ENB, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(MOP_MOTOR_PIN, OUTPUT);
  pinMode(PUMP_RELAY, OUTPUT);
  pinMode(buzzer, OUTPUT);
  pinMode(IR1_Obstacle, INPUT);
  pinMode(IR2_Obstacle, INPUT);
  pinMode(IR_EDGE, INPUT);

  myServo.attach(SERVO_PIN);
  myServo.write(90); // Center servo
  
  attachInterrupt(digitalPinToInterrupt(ECHO), echoISR, CHANGE);

  // Active-Low Relays Initial State: HIGH = OFF
  digitalWrite(PUMP_RELAY, HIGH);
  digitalWrite(MOP_MOTOR_PIN, HIGH);
  digitalWrite(buzzer, LOW);
  
  lastCycleResetTime = millis();
}

void loop() {
  distance = getDistance();

  // Check incoming Serial commands from ESP
  if (Serial.available()) {
    char cmd = Serial.read();

    if (cmd == 'M') {
      mode = 'M';
      stopMotors();
      resetCleaningSystem();
    }
    else if (cmd == 'A') {
      mode = 'A';
      resetCleaningSystem();
      lastCycleResetTime = millis();
    }

    if (mode == 'M') {
      manualControl(cmd);
    }
  }

  // Auto Mode execution
  if (mode == 'A') {
    autoMode();
  }

  delay(20);
}

void resetCleaningSystem() {
  isCleaningCycle = false;
  cleaningStage = 0;
  digitalWrite(PUMP_RELAY, HIGH);
  digitalWrite(MOP_MOTOR_PIN, HIGH);
  digitalWrite(buzzer, LOW);
}

void manualControl(char cmd) {
  if (cmd == 'F') forward();
  if (cmd == 'B') backward();
  if (cmd == 'L') turnLeft();
  if (cmd == 'R') turnRight();
  if (cmd == 'S') stopMotors();

  
  if (cmd == 'P') digitalWrite(PUMP_RELAY, LOW);
  if (cmd == 'MO') digitalWrite(PUMP_RELAY, LOW);

  if (cmd == 'C') {
    digitalWrite(PUMP_RELAY, HIGH);
    digitalWrite(MOP_MOTOR_PIN, HIGH);
  }
}

void autoMode() {
  unsigned long currentMillis = millis();

  // If 50 seconds have elapsed, start the cleaning sequence
  if (!isCleaningCycle && (currentMillis - lastCycleResetTime >= 50000)) {
    isCleaningCycle = true;
    cleaningStage = 1;
    stageStartTime = currentMillis;
    
    // Stage 1 Start: Relays must be OFF (HIGH)
    digitalWrite(PUMP_RELAY, HIGH);
    digitalWrite(MOP_MOTOR_PIN, HIGH);
  }

  if (isCleaningCycle) {
    executeCleaningCycle(currentMillis);
  } else {
    normalMovement(); // Regular navigation outside of the cleaning cycle
  }
}

void executeCleaningCycle(unsigned long currentMillis) {
  unsigned long elapsedStageTime = currentMillis - stageStartTime;

  // --- STAGE 1: First 20 Seconds (Cleaning with FAN only) ---  
  if (cleaningStage == 1) {
    digitalWrite(PUMP_RELAY, HIGH); 
    digitalWrite(MOP_MOTOR_PIN, HIGH);
    
    if (elapsedStageTime >= 20000) {
      cleaningStage = 2;
      stageStartTime = currentMillis; // Reset clock for Stage 2
      stopMotors();                   // Stop motors for Stage 2
      digitalWrite(PUMP_RELAY, LOW);  // Turn Pump ON (LOW)
    } else {
      normalMovement();               // Move normally during Stage 1
    }
  }
  
  // --- STAGE 2: Next 10 Seconds (spray water with Pump) ---
  else if (cleaningStage == 2) {
    stopMotors();                    // Keep motors OFF completely
    digitalWrite(PUMP_RELAY, LOW);   // Pump remains ON
    digitalWrite(MOP_MOTOR_PIN, HIGH); // Mop remains OFF
    
    if (elapsedStageTime >= 10000) {
      cleaningStage = 3;
      stageStartTime = currentMillis;   // Reset clock for Stage 3
      digitalWrite(PUMP_RELAY, HIGH);  // Turn Pump OFF (HIGH)
      digitalWrite(MOP_MOTOR_PIN, LOW); // Turn Mop ON (LOW)
    }
  }
  
  // --- STAGE 3: Final 20 Seconds (Mopping stage) ---
  else if (cleaningStage == 3) {
    digitalWrite(PUMP_RELAY, HIGH);   // Pump OFF
    digitalWrite(MOP_MOTOR_PIN, LOW);  // Mop ON
    
    if (elapsedStageTime >= 20000) {
      // End of Cycle Cleanup
      digitalWrite(MOP_MOTOR_PIN, HIGH); // Turn Mop OFF
      
      // Notification sound
      digitalWrite(buzzer, HIGH);
      delay(1000); 
      digitalWrite(buzzer, LOW);

      // Reset variables back to idle autonomous navigation
      isCleaningCycle = false;
      cleaningStage = 0;
      lastCycleResetTime = millis(); 
    } else {
      normalMovement(); // Resume normal movement logic
    }
  }
}

void normalMovement() {
  if (digitalRead(IR_EDGE) == HIGH) {
    stopMotors();
    backward();
    delay(500); 
    turnRight();
    delay(400);
    stopMotors();
    return;
  }

  if (distance < 15 || digitalRead(IR1_Obstacle) == LOW || digitalRead(IR2_Obstacle) == LOW) {
    stopMotors();
    
    myServo.write(20); 
    delay(400);
    float rightDistance = getDistance();
    
    myServo.write(160); 
    delay(400);
    float leftDistance = getDistance();
    
    myServo.write(90); 
    delay(150);

    // Decision Logic based on your exact instructions
    if (rightDistance > leftDistance) {
      turnRight();
    } else {
      turnLeft();
    }
    delay(500); 
    stopMotors();
  } 
  else {
    forward();
  }
}

void forward() {
  digitalWrite(IN1, HIGH); digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH); digitalWrite(IN4, LOW);
  analogWrite(ENA, 120);
  analogWrite(ENB, 120);
}

void backward() {
  digitalWrite(IN1, LOW);  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW);  digitalWrite(IN4, HIGH);
  analogWrite(ENA, 120);
  analogWrite(ENB, 120);
}

void turnLeft() {
  digitalWrite(IN1, LOW);  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH); digitalWrite(IN4, LOW);
  analogWrite(ENA, 150);
  analogWrite(ENB, 150);
}

void turnRight() {
  digitalWrite(IN1, HIGH); digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);  digitalWrite(IN4, HIGH);
  analogWrite(ENA, 150);
  analogWrite(ENB, 150);
}

void stopMotors() {
  digitalWrite(IN1, LOW); digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW); digitalWrite(IN4, LOW);
  analogWrite(ENA, 0);
  analogWrite(ENB, 0);
}

float getDistance() {
  digitalWrite(TRIG, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG, LOW);
  
  unsigned long pingTimeout = millis();
  while(!newData) {
    if(millis() - pingTimeout > 30) break; 
  }

  if (newData) {
    unsigned long duration = endTime - startTime;
    distance = duration * 0.0343 / 2.0;
    newData = false;
  }
  return distance;
}

void echoISR() {
  if (digitalRead(ECHO)) {
    startTime = micros();
  } else {
    endTime = micros();
    newData = true;
  }
}