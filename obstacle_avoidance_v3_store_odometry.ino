#include "bump_sensors.h"
#include "motors_test_control.h"
#include "encoders.h"
#include "EEPROM.h"
#include "line_sensors.h"

#define STATE_INITIAL 0
#define GO_FORWARDS 1
#define OBJECT_DETECTED 2
#define AVOID_OBSTACLE 3
#define LINE_DETECTED 4
#define  BUTTON_A_PIN 14
#define BUTTON_B_PIN 30

Bump_Sensors_c bump_sensors_obj;
motors_test motors_c;
Line_Sensors_c line_sensors_obj;
int state;
unsigned long robot_start_time, record_start_time;
odom odom_c;
int ee_x_address, ee_y_address;
int count_coord;
float button_b_flag;

void drive_forwards(float L_PWM = 20, float R_PWM = 20) {
  motors_c.set_motors_power (L_PWM, R_PWM);
}

void avoid_obstacle(float target_reading = 20000, float Bias_PWM = 20, float Turn_PWM = 20){
  float W;

  float avg_reading = (bump_sensors_obj.avg_readings[0] + bump_sensors_obj.avg_readings[1]) / 2;
  float diff = (target_reading - avg_reading) / 35000;
  float L_PWM = Bias_PWM + Turn_PWM * diff;
  float R_PWM = Bias_PWM -Turn_PWM * diff;


  
  motors_c.set_motors_power(L_PWM, R_PWM);
  
}

void update_state(){
  if (state == STATE_INITIAL){
    state = GO_FORWARDS;
  }
  else if (state == GO_FORWARDS && (bump_sensors_obj.avg_readings[0] < 8000 || bump_sensors_obj.avg_readings[1] < 8000)){
    state = OBJECT_DETECTED;
  }
  else if (state == OBJECT_DETECTED){
    record_start_time = millis();
    state = AVOID_OBSTACLE;
  }
  else if (state == AVOID_OBSTACLE &&  line_sensors_obj.readLineSensor(2) > 4000)
    state  = LINE_DETECTED;
}

void do_action(){
  if (state == GO_FORWARDS){
    drive_forwards();
  }
  
  else if (state == OBJECT_DETECTED){
    motors_c.set_motors_power(0, 0);
    delay(200);
  }

  else if (state == AVOID_OBSTACLE){

    avoid_obstacle(65000, 20, 30);
  }

  else if (state == LINE_DETECTED){
    motors_c.set_motors_power(0, 0);
  }

  

  
}


void setup() {
  state = STATE_INITIAL;
  bump_sensors_obj.initialise();
  motors_c.initialize();
  line_sensors_obj.initialise();
  pinMode(BUTTON_A_PIN, INPUT_PULLUP);
  pinMode(BUTTON_B_PIN, INPUT_PULLUP);
  button_b_flag = 0;
  
  Serial.begin(9600);
  delay(1500);
  Serial.println("***RESET***");

  robot_start_time = millis();
  ee_x_address = sizeof(int);
  ee_y_address = 2500;
  count_coord = 0;

  setupEncoder0();
  setupEncoder1();

}

void loop() {
   if (digitalRead(BUTTON_B_PIN) == LOW && button_b_flag == 0){
    button_b_flag = 1;
   }
    
   if ((millis() - robot_start_time) % 100 == 0 && button_b_flag == 1){
    Serial.print("UPDATING LOCATION ");
    odom_c.update_location();

    
    EEPROM.put(ee_x_address, float(odom_c.X_prev*2/1.8));
    EEPROM.put(ee_y_address, float(odom_c.Y_prev*2/1.8));
    ee_x_address = ee_x_address + sizeof(float);
    ee_y_address = ee_y_address + sizeof(float);
    count_coord++;
    EEPROM.put(0, count_coord);
   
    bump_sensors_obj.get_avg_readings(1);
  
    update_state();
  
    do_action();
   }
   


}
