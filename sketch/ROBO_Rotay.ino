/*  ROBO I/O Board Test Sketch - Encoder only version
*   Hardware:
*   - Arduino MEGA
*   - ROBO I/O Board PCB
*   - Rotary encoders extension board
*  ROBO moves servos through:
*   - extension board rotaries (one selects which servo to move, the other servo position)
*  by Barito (updated april 2026)
*/

#include <Servo.h>

// ===================== CONFIG =====================

#define SERVO_COUNT 16
#define DC_MOTORS 2
#define STEPPERS_NUM 4
#define SENSORS_NUM 12
#define ENCODERS_NUM 2
#define BUTTONS_NUM 4
#define TRIM_NUM 2

int8_t servoSpeed[SERVO_COUNT];

// ===================== SERVOS =====================

Servo servos[SERVO_COUNT];

const uint8_t servoPins[SERVO_COUNT] = {
  9, 10, 11, 12,
  5, 6, 7, 8,
  14, 2, 3, 4,
  18, 17, 16, 15
};

int16_t servoPosition[SERVO_COUNT];
int16_t lastPosition[SERVO_COUNT];

// solo servo usati
const uint8_t activeServoMap[] = {3, 7, 11, 15};
#define ACTIVE_SERVO_COUNT (sizeof(activeServoMap)/sizeof(activeServoMap[0]))

int16_t selectedServo = 0;

// ===================== DC MOTORS =====================

struct DCMotor {
  uint8_t IN1Pin;
  uint8_t IN2Pin;
  uint8_t PWMPin;
};

DCMotor dcMotors[DC_MOTORS] = {
  {26, 24, 22},
  {28, 30, 32}
};

// ===================== STEPPERS =====================

struct StepperMotor {
  uint8_t stepPin;
  uint8_t dirPin;
  uint8_t enablePin;
};

StepperMotor steppers[STEPPERS_NUM] = {
  {25, 27, 23},
  {33, 35, 23},
  {29, 31, 23},
  {37, 39, 23}
};

// ===================== SENSORS =====================

struct Sensor {
  uint8_t pin;
  uint8_t mode;
};

Sensor sensors[SENSORS_NUM] = {
  {A2, INPUT},
  {A3, INPUT},
  {A4, INPUT},
  {A5, INPUT},
  {A6, INPUT},
  {A7, INPUT},
  {41, INPUT},
  {43, INPUT},
  {45, INPUT},
  {47, INPUT},
  {49, INPUT},
  {51, INPUT}
};

// ===================== ENCODERS =====================

struct Encoder {
  uint8_t pinA;
  uint8_t pinB;
};

Encoder encoders[ENCODERS_NUM] = {
  {52, 48},
  {34, 46}
};

int8_t lastA[ENCODERS_NUM];

// ===================== BUTTONS =====================

uint8_t pushButton[BUTTONS_NUM] = {36, 50, 38, 40};

// ===================== TRIMMERS =====================

uint8_t trimmer[TRIM_NUM] = {A9, A8};

// ===================== EXT =====================

uint8_t EXTpin = 44;

// ===================== BUTTON READ =====================

bool Button_Read(uint8_t idx) {
  const unsigned long debounceDelay = 50;

  static uint8_t lastReading[BUTTONS_NUM] = {HIGH};
  static uint8_t stableState[BUTTONS_NUM] = {HIGH};
  static unsigned long lastDebounceTime[BUTTONS_NUM] = {0};

  uint8_t reading = digitalRead(pushButton[idx]);

  if (reading != lastReading[idx]) {
    lastDebounceTime[idx] = millis();
  }

  if ((millis() - lastDebounceTime[idx]) > debounceDelay) {
    if (reading != stableState[idx]) {
      stableState[idx] = reading;

      if (stableState[idx] == LOW) {
        lastReading[idx] = reading;
        return true;
      }
    }
  }

  lastReading[idx] = reading;
  return false;
}

// ===================== SETUP =====================

void setup() {

  // SERVOS
  for (int i = 0; i < SERVO_COUNT; i++) {
    servos[i].attach(servoPins[i]);
    servoPosition[i] = 90;
    lastPosition[i] = 90;
    servos[i].write(90);
    delay(100);
  }

  // DC MOTORS
  for (int i = 0; i < DC_MOTORS; i++) {
    pinMode(dcMotors[i].IN1Pin, OUTPUT);
    pinMode(dcMotors[i].IN2Pin, OUTPUT);
    pinMode(dcMotors[i].PWMPin, OUTPUT);
  }

  // STEPPERS
  for (int i = 0; i < STEPPERS_NUM; i++) {
    pinMode(steppers[i].stepPin, OUTPUT);
    pinMode(steppers[i].dirPin, OUTPUT);
    pinMode(steppers[i].enablePin, OUTPUT);
    digitalWrite(steppers[i].enablePin, HIGH);
  }

  // SENSORS
  for (int i = 0; i < SENSORS_NUM; i++) {
    pinMode(sensors[i].pin, sensors[i].mode);
  }

  // ENCODERS
  for (int i = 0; i < ENCODERS_NUM; i++) {
    pinMode(encoders[i].pinA, INPUT_PULLUP);
    pinMode(encoders[i].pinB, INPUT_PULLUP);
    lastA[i] = digitalRead(encoders[i].pinA);
  }

  // BUTTONS
  for (int i = 0; i < BUTTONS_NUM; i++) {
    pinMode(pushButton[i], INPUT_PULLUP);
  }

  Serial.begin(115200);
  Serial.println("Encoder mode - Serial debug only");
}

// ===================== LOOP =====================

void loop() {

  // ===================== ENCODER CONTROL =====================
  updateEncoder(0, selectedServo);
  selectedServo = constrain(selectedServo, 0, ACTIVE_SERVO_COUNT - 1);

  int realServo = activeServoMap[selectedServo];

  updateEncoder(1, servoPosition[realServo]);
  servoPosition[realServo] = constrain(servoPosition[realServo], 0, 180);

  // BUTTONS
  if (Button_Read(2)) { // LEFT BUTTON on extension board
    servoPosition[realServo] = 90;
  }
  if (Button_Read(3)) { // RIGHT BUTTON on extension board
    for (int i = 0; i < SERVO_COUNT; i++) {
      servoPosition[i] = 90;
    }
  }

  // ===================== TIME BASED MOTION =====================
  unsigned long now = millis();

  for (int i = 0; i < SERVO_COUNT; i++) {

    if (servoPosition[i] == lastPosition[i]) continue;

    int speed = servoSpeed[i];
    if (speed <= 0) speed = 1;

    // intervallo dinamico (più speed = più veloce)
    unsigned long interval = 20 / speed;
    if (interval < 1) interval = 1;

    static unsigned long lastUpdate[SERVO_COUNT] = {0};

    if (now - lastUpdate[i] < interval) continue;

    lastUpdate[i] = now;

    // movimento graduale verso target
    if (servoPosition[i] > lastPosition[i]) {
      lastPosition[i]++;
    } else if (servoPosition[i] < lastPosition[i]) {
      lastPosition[i]--;
    }

    servos[i].write(lastPosition[i]);
  }

  // ===================== DEBUG =====================
  Debug(realServo);
  delay(100); // piccolo delay per evitare spam seriale
}

// ===================== ENCODER =====================

void updateEncoder(int idx, int &value) {
  int8_t currentA = digitalRead(encoders[idx].pinA);

  if (lastA[idx] == HIGH && currentA == LOW) {
    if (digitalRead(encoders[idx].pinB) == HIGH) {
      value++;
    } else {
      value--;
    }
  }

  lastA[idx] = currentA;
}

// ===================== DEBUG =====================

void Debug(int servo) {
  Serial.print("Servo pin: ");
  Serial.print(servoPins[servo]);
  Serial.print("  Pos: ");
  Serial.println(servoPosition[servo]);
}