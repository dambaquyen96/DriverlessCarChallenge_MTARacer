#include <Encoder.h>
#include <PID_v1.h>

#define PIN_INPUT 0   
#define PIN_OUTPUT 10  

Encoder myEnc(2, 3);

double Setpoint, Input, Output; 
double Kp=3, Kd=1, Ki=5;   
PID myPID(&Input, &Output, &Setpoint, Kp, Ki, Kd, DIRECT);

long t0,t1,v;

char incomingByte;
String ss="";
boolean sending=true,first=true,back=false,front=false,wrong=false;
int pwm_out;

void setPwmFrequency(int pin, int divisor) {
  byte mode;
  if(pin == 5 || pin == 6 || pin == 9 || pin == 10) {
    switch(divisor) {
      case 1: mode = 0x01; break;
      case 8: mode = 0x02; break;
      case 64: mode = 0x03; break;
      case 256: mode = 0x04; break;
      case 1024: mode = 0x05; break;
      default: return;
    }
    if(pin == 5 || pin == 6) {
      TCCR0B = TCCR0B & 0b11111000 | mode;
    } else {
      TCCR1B = TCCR1B & 0b11111000 | mode;
    }
  } else if(pin == 3 || pin == 11) {
    switch(divisor) {
      case 1: mode = 0x01; break;
      case 8: mode = 0x02; break;
      case 32: mode = 0x03; break;
      case 64: mode = 0x04; break;
      case 128: mode = 0x05; break;
      case 256: mode = 0x06; break;
      case 1024: mode = 0x7; break;
      default: return;
    }
    TCCR2B = TCCR2B & 0b11111000 | mode;
  }
}

void setup() {
  Serial.begin(115200);


  
  //***throttle*** 488hz
  //max_reverve: 1ms / (1/488) * 256 = 125
  //neutral:     1.5ms / (1/488) * 256 = 187
  //max_forward: 2ms / (1/488) * 256 = 250
  setPwmFrequency(10, 64);    //set frequency pwm throttle
  
  //analogWriteResolution(12);

  v=0;
  Input = v; 
  t0=millis();

  myPID.SetMode(AUTOMATIC);

  
}

long currPos = 0,position  = -999;
long newPos;

void loop() 
{ 
  if (Serial.available() > 0)     //dang tin hieu truyen: VD : f12 : chay front , van toc 12 vong/s  hoac VD : b20 : chay back , van toc 20 vong/s
  {
      incomingByte = Serial.read(); 
      if (first)
      {
        if (incomingByte=='f')
        {
          front=true;
          back=false;
          first=false;
          sending=true;
        }
        else if (incomingByte=='b')
        {
          back=true;
          front=false;
          first=false;
          sending=true;
        }
        else wrong=true;    //truyen sai cau truc lenh
      }
      else
      {
        if (!wrong)         //neu truyen dung
        {
          if (incomingByte!=10)         
          {
            ss.concat(incomingByte);
            sending=true;              
          }
          else
          {
            Setpoint = ss.toInt(); 
            if (Setpoint<=25) 
            {
              //Kp=3; Kd=1; Ki=5;
              Kp=3; Kd=1; Ki=6;
            }               
            else 
            {
              //Kp=2; Kd=3; Ki=10;
              //Kp=2; Kd=3; Ki=9;
              Kp=3; Kd=1; Ki=6;
            }
            ss="";                                
            sending=false;                       
            first=true;
          }
        }
      }  
  }
  if (!sending)                             
  {
    t1=millis();
    if (t1-t0>=100)
    {
      currPos=newPos;
           
      if (position<0) position=-position;
      v=velocity(position);
      Input = (double)v;   
      myPID.Compute();
//      Serial.println("***************");
//      Serial.print("Velocity:  ");
//      Serial.println(v);
//      Serial.print("PWM_PID:  ");

      if (Setpoint<=25)
      {
        if (back) 
        {
          pwm_out=186+Output/24;
          //Serial.println(pwm_out);   
          analogWrite(PIN_OUTPUT,pwm_out);
          
        }
        else if (front) 
        {
          pwm_out=185-Output/24;
          //Serial.println(pwm_out);   
          analogWrite(PIN_OUTPUT,pwm_out);
          
        }
      }
      else
      {
        if (back) 
        {
          pwm_out=186+Output/18;
//          Serial.println(pwm_out);   
          analogWrite(PIN_OUTPUT,pwm_out);
          
        }
        else if (front) 
        {
          pwm_out=185-Output/18;
//          Serial.println(pwm_out);   
          analogWrite(PIN_OUTPUT,pwm_out);
          
        }
      }


//        Serial.print(pwm_out);
//        
//        Serial.print(" ");      
//      Serial.println(v);
      
//      analogWrite(PIN_OUTPUT, map(Output , 0 , 255 , 0 , 4095)); 


      t0=millis();
    } 
    newPos = myEnc.read();
    if ((newPos-currPos) != position) 
    {
      position = newPos-currPos;
    }
  }
}

long velocity(long pos)
{
  long velo;
  velo=pos*10/400;
  return velo;
}



/*
#include <Encoder.h>
#include <PID_v1.h>

#define PIN_INPUT 0   
#define PIN_OUTPUT_THROTTLE 10  
#define PIN_OUTPUT_THETA1 5   //banh truoc
#define PIN_OUTPUT_THETA2 6   //banh sau

Encoder myEnc(2, 3);

double Setpoint, Input, Output; 
double Kp=3, Kd=1, Ki=5;   
PID myPID(&Input, &Output, &Setpoint, Kp, Ki, Kd, DIRECT);

long t0,t1,v;

char incomingByte;
String throttle="",theta="";
boolean sending=true,
        first=true,
        back=false,
        front=false,
        wrong=false,
        next=false,
        left=false,
        right=false;
int pwm_out,
    servo_angle,
    servo_PWM1,
    servo_PWM2;

void setPwmFrequency(int pin, int divisor) {
  byte mode;
  if(pin == 5 || pin == 6 || pin == 9 || pin == 10) {
    switch(divisor) {
      case 1: mode = 0x01; break;
      case 8: mode = 0x02; break;
      case 64: mode = 0x03; break;
      case 256: mode = 0x04; break;
      case 1024: mode = 0x05; break;
      default: return;
    }
    if(pin == 5 || pin == 6) {
      TCCR0B = TCCR0B & 0b11111000 | mode;
    } else {
      TCCR1B = TCCR1B & 0b11111000 | mode;
    }
  } else if(pin == 3 || pin == 11) {
    switch(divisor) {
      case 1: mode = 0x01; break;
      case 8: mode = 0x02; break;
      case 32: mode = 0x03; break;
      case 64: mode = 0x04; break;
      case 128: mode = 0x05; break;
      case 256: mode = 0x06; break;
      case 1024: mode = 0x7; break;
      default: return;
    }
    TCCR2B = TCCR2B & 0b11111000 | mode;
  }
}

void setup() {
  Serial.begin(115200);

  //***throttle*** 488hz
  //max_reverve: 1ms / (1/488*1000) * 256 = 125
  //neutral:     1.5ms / (1/488*1000) * 256 = 187
  //max_forward: 2ms / (1/488*1000) * 256 = 250
  setPwmFrequency(10, 64);    //set frequency pwm throttle
  //***theta***    244hz
  //max_reverve: 1ms / (1/244*1000) * 256 = 63
  //neutral:     1.5ms / (1/244*1000) * 256 = 93.7
  //max_forward: 2ms / (1/244*1000) * 256 = 124
  setPwmFrequency(5,256);     //set frequency pwm theta 1
  setPwmFrequency(6,256);     //set frequency pwm theta 2
  
  //analogWriteResolution(12);

  v=0;
  Input = v; 
  t0=millis();

  myPID.SetMode(AUTOMATIC);

  
}

long currPos = 0,position  = -999;
long newPos;

void loop() 
{ 
  //dang tin hieu truyen: VD:f12l10
  //VD : f12 : chay front , van toc 12 vong/s , hoac VD : b20 : chay back , van toc 20 vong/s
  //VD : l10 : servo quay left xap xi 10 do , hoac VD : r10 : servo quay right xap xi 10 
  if (Serial.available() > 0)     
  {
      incomingByte = Serial.read(); 
      if (first)    //bat dau truyen toc do
      {
        if (incomingByte=='f')      //chay toi
        {
          front=true;
          back=false;
          first=false;
          sending=true;
          left=false;
          right=false;
        }
        else if (incomingByte=='b') //chay lui
        {
          back=true;
          front=false;
          first=false;
          sending=true;
          left=false;
          right=false;
        }
        else wrong=true;    //truyen sai cau truc lenh
      }
      else
      {
        if (!wrong)         //neu truyen dung thi moi vao day de lay gia tri
        {
          if (incomingByte!=10)         //neu chua gap ki hieu enter
          {
            if ((incomingByte=='l')||(incomingByte=='r')) next=true;  //bat dau truyen gia tri cua servo
            if (!next) throttle.concat(incomingByte);                  //luu gia tri toc do
            else 
            {
              if (incomingByte=='l') left=true;   //xac nhan quay trai
              if (incomingByte=='r') right=true;  //xac nhan quay phai
              if ((incomingByte!='l')&&(incomingByte!='r')) theta.concat(incomingByte); //lay gia tri goc quay
            }
            sending=true;              //xac nhan dang truyen du lieu
          }
          else
          {
            Setpoint = throttle.toInt();  //muc tieu toc do can dat
            servo_angle = theta.toInt();     //muc tieu goc can quay
            if (Setpoint<=25)   
            {
              //Kp=3; Kd=1; Ki=5;
              Kp=3; Kd=1; Ki=6;
            }               
            else 
            {
              //Kp=2; Kd=3; Ki=10;
              //Kp=2; Kd=3; Ki=9;
              Kp=3; Kd=1; Ki=6;
            }

            //khoi tao lai cac gia tri dieu kien
            throttle="";             
            theta="";                   
            sending=false;                       
            first=true;
            next=false;
          }
        }
      }  
  }
  if (!sending)                             
  {
    t1=millis();
    if (t1-t0>=100)
    {
      currPos=newPos;
           
      if (position<0) position=-position;
      v=velocity(position);
      Input = (double)v;   
      myPID.Compute();
      Serial.println("***************");
      Serial.print("Velocity:  ");
      Serial.println(v);
      Serial.print("PWM_PID:  ");

      //dieu khien dong co
      setPwmFrequency(10, 64);    //set frequency pwm throttle
      if (Setpoint<=25)
      {
        if (back) 
        {
          pwm_out=186+Output/24;
          Serial.println(pwm_out);   
          analogWrite(PIN_OUTPUT_THROTTLE,pwm_out);
          
        }
        else if (front) 
        {
          pwm_out=185-Output/24;
          Serial.println(pwm_out);   
          analogWrite(PIN_OUTPUT_THROTTLE,pwm_out);
          
        }
      }
      else
      {
        if (back) 
        {
          pwm_out=186+Output/18;
          Serial.println(pwm_out);   
          analogWrite(PIN_OUTPUT_THROTTLE,pwm_out);
          
        }
        else if (front) 
        {
          pwm_out=185-Output/18;
          Serial.println(pwm_out);   
          analogWrite(PIN_OUTPUT_THROTTLE,pwm_out);
          
        }
      }


      //dieu khien servo
      if (left)   //neu la quay trai
      {
        servo_PWM1=94+servo_angle;
        Serial.print("servo_PWM1: ");  
        Serial.println(servo_PWM1);
        setPwmFrequency(5,256);
        analogWrite(PIN_OUTPUT_THETA1,servo_PWM1);      //xuat xung servo 1
        
        servo_PWM2=94-servo_angle;
        Serial.print("servo_PWM2: ");  
        Serial.println(servo_PWM2);
        setPwmFrequency(6,256);
        analogWrite(PIN_OUTPUT_THETA2,servo_PWM2);      //xuat xung servo 2
      }

      if (right)  //neu la quay phai
      {
        servo_PWM1=94-servo_angle;
        Serial.print("servo_PWM1: ");  
        Serial.println(servo_PWM1);
        setPwmFrequency(5,256);
        analogWrite(PIN_OUTPUT_THETA1,servo_PWM1);      //xuat xung servo 1
        
        servo_PWM2=94+servo_angle;
        Serial.print("servo_PWM2: ");  
        Serial.println(servo_PWM2);
        setPwmFrequency(6,256);
        analogWrite(PIN_OUTPUT_THETA2,servo_PWM2);      //xuat xung servo 2
      }

//        Serial.print(pwm_out);
//        
//        Serial.print(" ");      
//      Serial.println(v);
      
//      analogWrite(PIN_OUTPUT, map(Output , 0 , 60 , 64 , 124)); 

      t0=millis();
    } 
    newPos = myEnc.read();
    if ((newPos-currPos) != position) 
    {
      position = newPos-currPos;
    }
  }
}

long velocity(long pos)
{
  long velo;
  velo=pos*10/400;
  return velo;
}
*/
