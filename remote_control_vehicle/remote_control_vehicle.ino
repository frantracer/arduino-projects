#include <SoftwareSerial.h>
#include <BitbloqSoftwareSerial.h>
#include <BitbloqUS.h>
#include <Servo.h>

#define LIGHT_SENSOR_LEFT_PIN A2
#define LIGHT_SENSOR_RIGHT_PIN A3
#define INFRARED_SENSOR_PIN_1 2
#define INFRARED_SENSOR_PIN_2 3
#define DISTANCE_SENSOR_PIN 5
#define LEFT_WHEEL_PIN 8
#define RIGHT_WHEEL_PIN 9
#define HEAD_SERVO_PIN 11
#define BUZZER_PIN 12

#define MOVE 0
#define STOP 1
#define FIND_DIRECTION 2
#define ROTATE 3
int state = MOVE;

#define DISTANCE_THRESHOLD 10.0
#define SPEED 100 // [0, 100]
#define ROTATION_TIME 500
#define BACKWARD_TIME 500
#define HEAD_ROTATION_TIME 1000

Servo left_servo;
Servo right_servo;
Servo front_servo;

US distance_sensor(DISTANCE_SENSOR_PIN, DISTANCE_SENSOR_PIN);
bqSoftwareSerial bt(0, 1, 19200);

int head_angle = 90;
int direction_time = -1;
char new_direction = 0;
  
void setup() {  
  pinMode(INFRARED_SENSOR_PIN_1, INPUT);
  pinMode(INFRARED_SENSOR_PIN_2, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  
  front_servo.attach(HEAD_SERVO_PIN);
  left_servo.attach(LEFT_WHEEL_PIN);
  right_servo.attach(RIGHT_WHEEL_PIN);

  front_servo.write(head_angle);
}

void loop() {
  if(bt.available() > 0) {
    char data = bt.read();

    switch(data) {
      case 'U':
        moveForward();
        break;
      case 'D':
        moveBackward();
        break;
      case 'R':
        rotateRight();
        break;
      case 'L':
        rotateLeft();
        break;
      case 'S':
        stopServos();
        break;
      case 'I':
        beep();
        break;
    }
  }
}

void moveForward() {
  left_servo.write(map(SPEED, 0, 100, 90, 0));
  right_servo.write(map(SPEED, 0, 100, 90, 180));
}

void moveBackward() {
  left_servo.write(map(SPEED, 0, 100, 90, 180));
  right_servo.write(map(SPEED, 0, 100, 90, 0));
}

void stopServos() {
  left_servo.write(90);
  right_servo.write(90);
}

void rotateLeft() {
  left_servo.write(180);
  right_servo.write(180);
}

void rotateRight() {
  left_servo.write(0);
  right_servo.write(0);
}

void beep() {
  tone(BUZZER_PIN, 261, 1000);
  delay(1000);
}
