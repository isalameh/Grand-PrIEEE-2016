/* VERSION:
 *     DATE: April 16, 2015
 *     DESCRIPTION: - 5 speeds
 *                  - Threshold using adaptive offset
 */

#include <Servo.h>

#include "Linescanner.h"

/* COMMENTS:
 *   Servo Details:
 *       For our servo, we can turn from minimum 20 to maximum 104 degrees.
 *             20 being the maximum right turn
 *             62 being the center
 *             104 being the maximum left turn
 */
 
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/* PIN DEFINITIONS */

//Linescanner
#define DATA A0                       //Pin A on Linescanner 
#define SYNC 4                        //Pin B on Linescanner, GRAY WIRE
#define CLOCK 5                       //Pin C on Linescanner, WHITE WIRE

//Servo
#define SERVO_PIN 9           //Pin 11 is out now

//Motor
#define MOTOR_PIN 6           //Pin 6 works for the motor driver

//Encoder
//#define ENCODER1 2
//#define ENCODER2 3

//RF Switch
#define KILL 8

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/* VARIABLES */

//Servo Variables
Servo myServo;
float prevPos;
float SERVO_CENTER = 62;
float CENTER_OF_ARRAY = 63;
float MAX_SERVO_LEFT = 104;
float MAX_SERVO_RIGHT = 20;

//Motor Variable
int MAX_SPEED = 50;
int MED_FAST_SPEED = 45;
int MED_MED_SPEED = 40;
int MED_SLOW_SPEED = 35;
int SLOW_SPEED = 30;

int R_SHARP_ANGLE = 102;    // line center ranged from 10 pixels to the right
int R_MED_SHARP_ANGLE = 92;
int R_MED_MED_SHARP_ANGLE = 80;
int R_MED_SMOOTH_ANGLE = 70;
int L_SHARP_ANGLE = 22;    // line center ranged from 10 pixels to the left
int L_MED_SHARP_ANGLE = 32;
int L_MED_MED_SHARP_ANGLE = 44;
int L_MED_SMOOTH_ANGLE = 54;

//Pixel Array Variables
int pixels[128];
int ALL_ONES = 20;
int ADAPT_THRESHOLD;
int THRES_OFFSET = 8;

int LEFT_PIXELS = -42;                //Correlate to pixels 0 - 21
int RIGHT_PIXELS = 42;                //Correlate to pixels 105 - 128

//Extra variables
int THRES_COUNTER = 101;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

//Call Linescanner class to read in Camera data
Linescanner linescanner(DATA, SYNC, CLOCK);

void setup()
{  
  //Linescanner setup
  pinMode(CLOCK, OUTPUT);
  pinMode(SYNC, OUTPUT);
  pinMode(KILL, INPUT);
  pinMode(MOTOR_PIN, OUTPUT);
  
  //Servo setup
  myServo.attach(SERVO_PIN);
  myServo.write(SERVO_CENTER);

  //Motor setup
  
  //Begin the bit integration
    //Set the data rate in bits per second, 9600 is the speed for Uno
  Serial.begin(9600);
}

void loop()
{

  //Find average of pixels for threshold
  //if(THRES_COUNTER > 100)
  //{
  //  THRES_COUNTER = 0;
    ADAPT_THRESHOLD = findThreshold(pixels);
  //}

  int counter = 0;
  
  //Read in RF Signal to turn motor on or off
  int var = digitalRead(KILL);

  //If signal is 0, turn motor on (Run the car)
  if(var == 0)
  {
    //THRES_COUNTER += 1;
    linescanner.scan(7390);             //Signaling camera to start scanning
  
    linescanner.read(pixels);           //Reading pixels from camera
  
    //CAN MOVE THIS CODE AFTER FINDING NEW THRESHOLD TO PRINT ARRAY
    
   /* for(int i = 0; i < 128; i++)        //Printing out result of camera on serial monitor
    {
      if(pixels[i] > ADAPT_THRESHOLD)        //Threshold is from 0 - 1024 value of analog read
                                        //Determines range to detect white line
      {
        counter = counter + 1;
        Serial.print(1);
      }
    
      else
      {
        Serial.print(0);
      }
    
      //Serial.print(pixels[i]);
      //Serial.print(" ");
    }
  
    Serial.println("\n");
    */
     
  
    float middle = findMiddle(pixels); //Find the center of the line
    int angle = servoTurn(middle);      //Determine angle for servo to turn
    myServo.write(angle);
    
    //Motor Speeds
    if(angle > R_SHARP_ANGLE || L_SHARP_ANGLE > angle) 
    {
      //Set the duty cycle of the motor to low speed
      int motorPower  = motorPWM(SLOW_SPEED);     //Currently 20% motor
      analogWrite(MOTOR_PIN, motorPower);
    }
    
    else if(angle > R_MED_SHARP_ANGLE || L_MED_SHARP_ANGLE > angle)
    {
      int motorPower = motorPWM(MED_SLOW_SPEED);
      analogWrite(MOTOR_PIN, motorPower);
    }
    
    else if(angle > R_MED_MED_SHARP_ANGLE || L_MED_MED_SHARP_ANGLE > angle)
    {
      int motorPower = motorPWM(MED_MED_SPEED);
      analogWrite(MOTOR_PIN, motorPower);
    }
    
    else if(angle > R_MED_SMOOTH_ANGLE || L_MED_SMOOTH_ANGLE > angle) 
    {
      //Set the duty cycle of the motor to medium speed
      int motorPower  = motorPWM(MED_FAST_SPEED);     //Currently 30% motor
      analogWrite(MOTOR_PIN, motorPower);
    }
    
    else 
    {
      //Set the duty cycle of the motor
      int motorPower  = motorPWM(MAX_SPEED);     //Currently 40% motor
      analogWrite(MOTOR_PIN, motorPower);
    }
  }  //End of running the motor

  //If signal is 1, turn motor off (Stop the car)
  if(var == 1)
  {
    analogWrite(MOTOR_PIN, 0);
    counter = 0;
    THRES_COUNTER = 0;
  }
}

/* Finding the middle of the line
 *   Began: MARCH 21, 2015
 *   Edited: MARCH 28, 2015
 */
float findMiddle(int* pixelArray)
{
  float middle = 1;
  int count = 0;
  float finish = 0;
  int FLAG = 0;
  
  for(int i = 0; i < 128; i++)
  {
    if(pixelArray[i] > ADAPT_THRESHOLD)         //Detect the line, count the width of the line
    {
      finish = i;
      count = count + 1;
      
      /*if(count > ALL_ONES)
      {
        FLAG = 1;
        break; 
      }*/
      
      if(pixelArray[i+1] == 128 || pixelArray[i+1] == 0)
      {
        break;
      }
    }
  }

  middle = finish - (count/2);        //Determine the center of the line

  /*if(FLAG == 1)
  {
    middle = prevPos;
  }*/
  
  return middle;
}


/* Turning the Servo a certain angle
 *   Began: MARCH 25, 2015
 *   Edited: MARCH 28, 2015
 */
float servoTurn(float lineCenter)
{  
  //  -absolutePos = Turn left
  //   absolutePos = Turn right
  float absolutePos = lineCenter - CENTER_OF_ARRAY;
  
  /* DEFAULT CASE
   * Cases where this portion of code should run:
   *   We do not see the line
   *   We go off the course
   */
  if(lineCenter == 0)                 //If we do not detect the line
  {
    if(prevPos < LEFT_PIXELS)         //Previous position of line was to the left
    {
      return MAX_SERVO_LEFT;          //Wheels turn left
    }
    
    else if(prevPos > RIGHT_PIXELS)   //Previous position of line was to the right
    {
      return MAX_SERVO_RIGHT;         //Wheels turn right
    }
    
    else                              //Default: Make car go straight
    {
      return SERVO_CENTER;
    }
  }
  
  //LOCK WHEELS RIGHT                                      
  else if(absolutePos > RIGHT_PIXELS) //lineCenter > camera pixel 105
  {
    prevPos = absolutePos;            //Save this position for later reference
    return MAX_SERVO_RIGHT;
  }
  
  //LOCK WHEELS LEFT
  else if(absolutePos < LEFT_PIXELS)  //lineCenter < camera pixel 21
  {
    prevPos = absolutePos;            //Save this position for later reference
    return MAX_SERVO_LEFT;
  }
  
  //TURN WHEELS A CERTAIN ANGLE
  else                                //lineCenter between camera pixel 21 - 105
  {
    prevPos = absolutePos;
    return (SERVO_CENTER - absolutePos);
  }
  
  //DEFAULT IF NOT SATISFY ANY IF-ELSE STATEMENTS
  //Make car go straight
  return SERVO_CENTER;
}


/* Find the best threshold to determine the location of the line
 *   Began: APRIL 12, 2015
 *   Edited: APRIL 16, 2015
 */
int findThreshold(int* pixelArray)
{
  int average;
  int maximum = 0;

  for(int i = 0; i < 128; i++)
  {
    average = average + pixelArray[i];

    if(pixelArray[i] > maximum)
    {
      maximum = pixelArray[i];
    }
  }
  
  average = average/128;

  //Adaptive threshold
  int offset = adaptThresOffset(maximum, average);
  offset = offset + average;
  
  //Static threshold (THRES_OFFSET varies, look at top of file)
  //int offset = average + THRES_OFFSET;
  
  return offset;
}


/* Algorithm for adaptive thresholding
 *   Began: APRIL 16, 2015
 *   Edited: APRIL 16, 2015
 */
int adaptThresOffset(int maximum, int average)
{
  return ((maximum - average)/2);
}


/* Calculate the duty cycle for the motor
 *   Began: APRIL 11, 2015
 *   Edited: APRIL 11, 2015
 */
int motorPWM(int value)
{
  int motorDuty = 2.55 * value;       //2.55 * value inputed
  return motorDuty;
}
