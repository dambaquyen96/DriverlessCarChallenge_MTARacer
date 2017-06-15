/********************************************************
 * PID Basic Example
 * Reading analog input 0 to control analog PWM output 3
 ********************************************************/

#include <PID_v1.h>

#define PIN_INPUT 0   //chan doc encoder thuc te tra ve , don vi la toc do (m/s,cm/us ...)
#define PIN_OUTPUT 3  //chan tra ra xung can cap cho dong co sau khi tinh toan PID dua tren setpoint

//Define Variables we'll be connecting to
//Input : la gia tri encoder do duoc tu chan PIN_INPUT , don vi la toc do
//Setpoint : la gia tri muc tieu can dat cua he thong, cung don vi voi Input
//Output : la xung PWM tra ra de nap cho dong co tu cac thong tin dau vao sau khi tinh toan PID 
double Setpoint, Input, Output; 

//Specify the links and initial tuning parameters
double Kp=2, Ki=5, Kd=1;    //can phai thay doi cac gia tri Kp,Ki,Kd cho phu hop , ta xem Output co bi dao dong nhieu khong va toc do co dung khong de dieu chinh

PID myPID(&Input, &Output, &Setpoint, Kp, Ki, Kd, DIRECT);

void setup()
{
  //initialize the variables we're linked to
  Input = analogRead(PIN_INPUT); //doc encoder
  Setpoint = 100;                //gia tri setpoint can dat

  //turn the PID on
  myPID.SetMode(AUTOMATIC);
}

void loop()
{
  Input = analogRead(PIN_INPUT);  
  myPID.Compute();
  analogWrite(PIN_OUTPUT, Output);  //gia tri output tra ra
}


