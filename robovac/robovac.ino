#include <Servo.h>
#include <BitbloqUS.h>

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
#define SPEED 50 // [0, 100]
#define ROTATION_TIME 500
#define BACKWARD_TIME 500
#define HEAD_ROTATION_TIME 1000

Servo left_servo;
Servo right_servo;
Servo front_servo;

US distance_sensor(DISTANCE_SENSOR_PIN, DISTANCE_SENSOR_PIN);

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
  switch(state) {
    case MOVE:
      if(distance_sensor.read() > DISTANCE_THRESHOLD && onFloor()) {
        moveForward();
      } else {
        state = STOP;
      }
      break;
    case STOP:
      stopServos();
      //beep();
      state = FIND_DIRECTION;
      break;
    case FIND_DIRECTION:
      new_direction = 0;
    
      front_servo.write(135);
      delay(HEAD_ROTATION_TIME);
      if(distance_sensor.read() > DISTANCE_THRESHOLD && onFloor()) {
        new_direction = 'l';
      }

      front_servo.write(45);
      delay(HEAD_ROTATION_TIME);
      if(distance_sensor.read() > DISTANCE_THRESHOLD && onFloor()) {
        new_direction = 'r';
      }

      front_servo.write(90);
      if(new_direction == 0) {
        new_direction = 'b';
      }
      
      state = ROTATE;
      break;
    case ROTATE:
      int current_time = millis();
      if(direction_time == -1) {
        direction_time = current_time;
      }

      switch(new_direction) {
        case 'b':
          if(current_time - direction_time <= BACKWARD_TIME) {
            moveBackward();
          } else if(current_time - direction_time <= BACKWARD_TIME + ROTATION_TIME) {
            rotateRight();
          } else {
            new_direction = 0;
          }
          break;
        case 'l':
          if(current_time - direction_time <= ROTATION_TIME) {
            rotateLeft();
          } else {
            new_direction = 0;
          }
          break;
        case 'r':
          if(current_time - direction_time <= ROTATION_TIME) {
            rotateRight();
          } else {
            new_direction = 0;
          }
          break;
        default:
          state = MOVE;
          direction_time = -1;
      }
      break;
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

bool onFloor() {
  if(digitalRead(INFRARED_SENSOR_PIN_1) == 1 && digitalRead(INFRARED_SENSOR_PIN_2) == 1) {
    return true;
  } else {
    return false;
  }
}
