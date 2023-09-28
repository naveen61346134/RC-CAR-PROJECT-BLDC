#include <Servo.h>

#define REL_PHA1 6  // relay to swtich bldc phase 1
#define REL_PHA2 7  // relay to switch bldc phase 2

#define CH3 A0 // MOTOR PWM SIGNAL INPUT
#define CH2 A1 // SERVO TURN SIGNAL INPUT
#define CH4 A2  // Reverse Mode INPUT

#define SERVO A3  // SERVO OUTPUT
#define FR_MOTOR 9  // Front right motor OUTPUT
#define FL_MOTOR 10 // Front left motor OUTPUT
#define B_MOTOR 11  // Back motor OUTPUT

Servo SER;  // creating servo motor objecr
Servo FRM;  // creating front right motor object
Servo FLM;  // creating front left motor object
Servo BM;   // creating back motor object

void setup() {
  Serial.begin(9600);
  pinMode(CH2, INPUT);   // seting pin modes of all the pins using
  pinMode(CH3, INPUT);
  pinMode(CH4, INPUT);
  pinMode(SERVO, OUTPUT);
  pinMode(B_MOTOR, OUTPUT);
  pinMode(REL_PHA1, OUTPUT);
  pinMode(REL_PHA2, OUTPUT);
  pinMode(FR_MOTOR, OUTPUT);
  pinMode(FL_MOTOR, OUTPUT);

  SER.attach(SERVO, 0, 180);          // SERVO initialization with min and max output values 
  esc_init_calibrate();
}

const int bldc_throttle_low = 1342;        // motor throttle calibrate (low value)
const int bldc_throttle_high = 1941;       // motor throttle calibrate (high value)
const int bldc_pwm_calibrate_low = 1000;   // motor pwm calibrate (low value)
const int bldc_pwm_calibrate_high = 2000;  // motor pwm calibrate (high value)

char buff[200];

void loop() {
  //int pwm_val = pulseIn(CH3, HIGH, 25000);
  //int pwm_mapp = map(pwm_val, bldc_throttle_low, bldc_throttle_high, bldc_pwm_calibrate_low, bldc_pwm_calibrate_high);
  
  int rev_mode = 0; // default mode (forward)
  int state = pulseIn(CH4, HIGH, 25000);   // read reverse button state
  if(state > 1011){
    rev_mode = 1;
  }
  else{
    rev_mode = 0;
  }
  while(rev_mode == 0){ // FORWARD (rev_mode=0)
    int pwm_val = pulseIn(CH3, HIGH, 25000);
    int pwm_mapp = map(pwm_val, bldc_throttle_low, bldc_throttle_high, bldc_pwm_calibrate_low, bldc_pwm_calibrate_high);

    int turn_val = pulseIn(CH2, HIGH, 25000);  // read servo throttle value
    if(turn_val < 1280 && turn_val > 1240){             // servo exact straight
      int sfm = 90;
      sprintf(buff, "STRAIGHT: %d = %d", sfm, pwm_mapp);
      Serial.println(buff);
      goForward(pwm_mapp);
    }
    else if(turn_val > 1268){         // servo right. wheel(left)
      int sfr = map(turn_val, 1286, 1595, 90, 180);
      sprintf(buff, "RIGHT TURN: %d = %d", sfr, pwm_mapp);
      Serial.println(buff);
      goLeft(pwm_mapp, sfr);
    }
    else if(turn_val < 1240){         // servo left. wheel(right)
      int sfl = map(turn_val, 1230, 998, 90, 0);
      sprintf(buff, "LEFT TURN: %d = %d", sfl, pwm_mapp);
      Serial.println(buff);
      goRight(pwm_mapp, sfl);
    }
    else{
      Serial.println("ELSE");
      goForward(pwm_mapp);
    }

    int state = pulseIn(CH4, HIGH, 25000);
    if(state > 2000){
      rev_mode = 1;
    }
    else{
      rev_mode = 0;
    }
  }


  int turn_value = pulseIn(CH2, HIGH, 25000); // read servo throttle value again cause of (out of scope compilation issue)

  int pwm_value = pulseIn(CH3, HIGH, 25000);
  int pwm_mapped = map(pwm_value, bldc_throttle_low, bldc_throttle_high, bldc_pwm_calibrate_low, bldc_pwm_calibrate_high);

  if(rev_mode == 1 && turn_value < 1285 && turn_value > 1235){       // servo exact straight
    sprintf(buff, "GO BACK: 90 = %d", pwm_value);
    Serial.println(buff);
    goBack(pwm_mapped);
  }
  else if(rev_mode == 1 && turn_value > 1268){
    int sfll = map(turn_value, 1280, 1594, 90, 180);
    sprintf(buff, "GO BACK DIR(>): %d = %d", sfll, pwm_mapped);
    Serial.println(buff);
    goBackDir(pwm_mapped, sfll);
  }
  else if(rev_mode == 1 && turn_value < 1240){
    int sfrr = map(turn_value, 1006, 1388, 0, 90);
    sprintf(buff, "GO BACK DIR(>): %d = %d", sfrr, pwm_mapped);
    Serial.println(buff);
    goBackDir(pwm_mapped, sfrr);
  }
}

void esc_init_calibrate(){

  Serial.println("ESC initialization and calibration process");
  delay(1500);
  Serial.println("STARTING......");
  delay(500);
  FRM.attach(FR_MOTOR, 1000, 2000);
  FLM.attach(FL_MOTOR, 1000, 2000);
  BM.attach(B_MOTOR, 1000, 2000);

  FRM.write(200);
  FLM.write(200);
  BM.write(200);
  delay(5000);
  FRM.write(0);
  FLM.write(0);
  BM.write(0);
  delay(2000);
  FRM.write(10);
  FLM.write(10);
  BM.write(10);

  Serial.println("CALIBRATION DONE!!");
  
}

void goBackDir(int pwm_val, int servo_val){
  //Serial.println("GO BACK DIR");
  digitalWrite(REL_PHA1, HIGH);
  digitalWrite(REL_PHA2, HIGH);

  BM.writeMicroseconds(pwm_val);
  SER.write(servo_val);

  digitalWrite(REL_PHA1, LOW);
  digitalWrite(REL_PHA2, LOW);
}

void goBack(int pwm_val){
  digitalWrite(REL_PHA1, HIGH);   // turn on relay 1 to switch phase1 to phase 3 (REVERSE Stage 1)
  digitalWrite(REL_PHA2, HIGH);   // turn on relay 2 to switch phase3 to phase 1 (Reverse stage 2)

  BM.writeMicroseconds(pwm_val);
  FRM.writeMicroseconds(pwm_val);
  FLM.writeMicroseconds(pwm_val);

  digitalWrite(REL_PHA1, LOW);    // turn off relay 1 to switch phase3 to phase 1 (Forward Stage 1)
  digitalWrite(REL_PHA2, LOW);    // turn off relay 2 to switch phase1 to phase 3 (Forward stage 2)
}

void goRight(int pwm_val, int servo_val){
  int avg_pwm = pwm_val/1.5;
  Serial.println(avg_pwm);
  BM.writeMicroseconds(avg_pwm);    // decrease back wheel speed
  FLM.writeMicroseconds(pwm_val);   // increase left wheel speed
  FRM.writeMicroseconds(avg_pwm);   // decrease right wheel speed
  SER.write(servo_val); // turn left
}

void goLeft(int pwm_val, int servo_val){
  int avg_pwm = pwm_val/2;
  Serial.println(pwm_val);
  BM.writeMicroseconds(avg_pwm);    //decrease back wheel speed
  FLM.writeMicroseconds(avg_pwm);   //decrease left wheel speed
  FRM.writeMicroseconds(pwm_val);   // increase right wheel speed
  SER.write(servo_val); // turn right
}

void goForward(int pwm_val){
  BM.writeMicroseconds(pwm_val);
  FRM.writeMicroseconds(pwm_val);
  FLM.writeMicroseconds(pwm_val);
}
