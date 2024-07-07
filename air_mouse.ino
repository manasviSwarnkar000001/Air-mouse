#define TRIGGER_PIN 4
#define LEFT_PIN 5
#define RIGHT_PIN 6
#define TOGGLE_PIN 7
#define THRESHOLD_TIME 15
#define RESET_TIME 400
#include <math.h>

const int x_out = A1; /* connect x_out of adxl335 to A1 of UNO board */
const int y_out = A2; /* connect y_out of adxl335 to A2 of UNO board */
const int z_out = A3; /* connect z_out of adxl335 to A3 of UNO board */
const int ledpin_normal = 10;
const int ledpin_drag = 11;

int led_state = 0;
bool buttonWasPressed = false;

// Replace timing system with number of loops encountered to determine the 

void setup() 
{
  Serial.begin(9600);
  pinMode(TRIGGER_PIN, INPUT_PULLUP);
  pinMode(LEFT_PIN, INPUT_PULLUP);
  pinMode(RIGHT_PIN, INPUT_PULLUP);
  pinMode(TOGGLE_PIN, INPUT_PULLUP);
  pinMode(ledpin_normal, OUTPUT);
  pinMode(ledpin_drag, OUTPUT);
  digitalWrite(ledpin_normal, LOW);
  digitalWrite(ledpin_drag, LOW);
}

// Normal mode = 10 (= ledpin_normal), Drag mode = 11 (= ledpin_drag)
void changemode(int led_num) 
{
  if (led_num == ledpin_normal) 
  {
    digitalWrite(ledpin_normal, HIGH);
    digitalWrite(ledpin_drag, LOW);
    led_state = ledpin_normal;
  } 
  
  if (led_num == ledpin_drag)
  {
    digitalWrite(ledpin_normal, LOW);
    digitalWrite(ledpin_drag, HIGH);
    led_state = ledpin_drag;
  }
}

int start_time, end_time;
void loop() 
{
  int press_time;
  int buttonState = digitalRead(TRIGGER_PIN);
  int leftState = digitalRead(LEFT_PIN);
  int rightState = digitalRead(RIGHT_PIN);
  int toggleState = digitalRead(TOGGLE_PIN);
  int x_adc_value, y_adc_value, z_adc_value;
  double x_g_value, y_g_value, z_g_value;
  double roll, pitch;
  x_adc_value = analogRead(x_out); /* Digital value of voltage on x_out pin */
  y_adc_value = analogRead(y_out); /* Digital value of voltage on y_out pin */
  z_adc_value = analogRead(z_out); /* Digital value of voltage on z_out pin */

  /*
  Serial.print("x = ");
  Serial.print(x_adc_value);
  Serial.print("\t\t");
  Serial.print("y = ");
  Serial.print(y_adc_value);
  Serial.print("\t\t");
  Serial.print("z = ");
  Serial.print(z_adc_value);
  Serial.print("\t\t");
  */
  //delay(100);

  x_g_value = ((((double)(x_adc_value * 5) / 1024) - 1.65) / 0.330); /* Acceleration in x-direction in g units */
  y_g_value = ((((double)(y_adc_value * 5) / 1024) - 1.65) / 0.330); /* Acceleration in y-direction in g units */
  z_g_value = ((((double)(z_adc_value * 5) / 1024) - 1.80) / 0.330); /* Acceleration in z-direction in g units */

  roll = (((atan2(y_g_value, z_g_value) * 180) / 3.14) + 180);                             /* Formula for roll */
  pitch = (((atan2(-x_g_value, sqrt(sq(y_g_value) + sq(z_g_value))) * 180) / 3.14) + 180); /* Formula for pitch */

  // Parameters: (roll, pitch)
  // Mean values of parameters: Rest/LEVEL (seems to always change) - (202, 155), Extreme UP (,104), Extreme DOWN (,265), Extreme LEFT (104,), RIGHT (277,)
  // OLD Observed (favorable): Roll varies roughly from 45 to 300
  // OLD Pitch from say 90 to 250

  int x = map(roll, 74.00, 280.0, -80.0, 80.0);
  int y = map(pitch, 100.0, 265.0, -80.0, 80.0);

  /*
  Serial.print("Roll = ");
  Serial.print(roll);
  Serial.print("\t");
  Serial.print("Pitch = ");
  Serial.print(pitch);
  Serial.print("\n");
*/

  if (toggleState == LOW && !buttonWasPressed)
  {
    start_time = millis();
    buttonWasPressed = true;
  }
  if (toggleState == HIGH && buttonWasPressed)
  {
    buttonWasPressed = false;
    end_time = millis();
    press_time = end_time - start_time;
    start_time = 0;
    // Serial.print("Press Time = ");
    // Serial.println(press_time);
    
    if (press_time >= THRESHOLD_TIME && press_time < RESET_TIME) 
    {
      if (led_state == ledpin_drag || led_state == 0)
      {
        changemode(ledpin_normal); // initial normal one glows
      }
      else if (led_state == ledpin_normal)
        changemode(ledpin_drag); //toggling
    }
    
    if(press_time >= RESET_TIME)
    {
      digitalWrite(ledpin_normal, LOW);
      digitalWrite(ledpin_drag, LOW);
      // Serial.print(led_state);
      // Serial.print("__Resetting__");
      led_state = 0;
    }  
  }



  if (led_state == ledpin_normal || led_state == ledpin_drag) 
  {
      //Serial.println("Button is pressed");
      //Serial.print(roll);
      //Serial.print(",");
      //Serial.print(pitch);
      //Serial.print(",");
      Serial.print(led_state);
      Serial.print(",");
      Serial.print(x);
      Serial.print(",");
      Serial.print(y);
      //Serial.println("OK");

      Serial.print(",");
      if (leftState == LOW)
        Serial.print(1);
      else
        Serial.print(0);

      Serial.print(",");
      if (rightState == LOW)
        Serial.println(1);
      else
        Serial.println(0);
  }
  else 
  {
      if (leftState == LOW && rightState == LOW)
        Serial.println("10, 0, 0, 1, 1");
      if (leftState == HIGH && rightState == LOW)
        Serial.println("10, 0, 0, 0, 1");
      if (leftState == LOW && rightState == HIGH)
        Serial.println("10, 0, 0, 1, 0");
      //Serial.println("Button is not pressed");
  } 

  // Serial.print("Prev led normal state = ");
  // Serial.println(led_state);

  delay(100);
}