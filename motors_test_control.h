#ifndef MOTORS_TEST_CONTROL_H
#define MOTORS_TEST_CONTROL_H

#define L_PWM_PIN 10
#define R_PWM_PIN 9
#define L_DIR_PIN 16
#define R_DIR_PIN 15
#define FORWARD LOW
#define REVERSE HIGH

class motors_test{
  public:
    motors_test(){
      
    }
    void initialize(){
      pinMode(L_PWM_PIN, OUTPUT);
      pinMode(R_PWM_PIN, OUTPUT);
      pinMode(L_DIR_PIN, OUTPUT);
      pinMode(R_DIR_PIN, OUTPUT);
    
      digitalWrite(L_DIR_PIN, FORWARD);
      digitalWrite(R_DIR_PIN, FORWARD);
    
      analogWrite(L_PWM_PIN, 0);
      analogWrite(R_PWM_PIN, 0);
    }
    void set_motors_power(float l_pwm, float r_pwm){
      if (l_pwm < 0){
        l_pwm *= -1;
        digitalWrite(L_DIR_PIN, REVERSE);
      }
      else{
        digitalWrite(L_DIR_PIN, FORWARD);
      }
      
      if (r_pwm < 0){
        r_pwm *= -1;
        digitalWrite(R_DIR_PIN, REVERSE);
      }
      else{
        digitalWrite(R_DIR_PIN, FORWARD);
      }
      if (l_pwm > 50 or r_pwm >50){
        analogWrite(L_PWM_PIN, 50);
        analogWrite(R_PWM_PIN, 50);  
      }
      else{
      analogWrite(L_PWM_PIN, l_pwm);
      analogWrite(R_PWM_PIN, r_pwm);
      }
}
};

#endif
