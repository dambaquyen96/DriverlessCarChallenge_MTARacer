
//#include "api_i2c_pwm.h"

//int main( int argc, char** argv )
//{
//    PCA9685 *pca9685 = new PCA9685() ;

//    api_pwm_pca9685_init( pca9685 );

//    int dir = 0, throttle_val = 0, theta = 0;
//    int current_state = 0;

//    // 27 is the ESC key

//    cout<< endl<< "Hit ESC key to exit"<< flush;

//    char key = 0;

//    while(pca9685->error >= 0)
//    {
//        key = getkey();

//        switch (key)
//        {

//        case 't':
//            theta++;
//            break;

//        case 'u':
//            theta--;
//            break;

//        case 'j':
//            throttle_val++;
//            break;

//        case 'g':
//            throttle_val--;
//            break;

//        case 'h':
//            dir++;
//            if(dir > 2 ) dir = 0;
//            break;

//        }

//        if( key == 27 )
//            break;

//        api_pwm_set_control( pca9685, dir, throttle_val, theta, current_state );

//        cout<< endl<< "Dir: "<< dir<< "; theta: "
//            << theta<< "; throttle: "<< throttle_val<< flush;

//        sleep(0.1);
//    }

//    api_pwm_pca9685_release( pca9685 );

//    return 0;
//}








#include "api_control_steering_throttle.h"
#include "api_i2c_pwm.h"
#include "api_uart.h"
#include <iostream>

using namespace std;
char buf_send[BUFF_SIZE];

void setControl(int speed,int angle)
{

	if (speed>=0) sprintf(buf_send, "f%d\n", speed);
	else
	{ 
		speed=-speed;
		sprintf(buf_send, "b%d\n", speed);
	}

/*
	if ((speed>=0) && (angle>=0)) sprintf(buf_send, "f%dr%d\n", speed , angle);
	else if ((speed<0) && (angle>=0))
	{
		speed=-speed;
		sprintf(buf_send, "b%dr%d\n", speed , angle);
	}
	else if ((speed>=0) && (angle<0)) 
	{
		angle=-angle;
		sprintf(buf_send, "f%dl%d\n", speed , angle);
	}
	else if ((speed<0) && (angle<0))
	{
		speed=-speed;
		angle=-angle;
		sprintf(buf_send, "b%dl%d\n", speed , angle);
	}
*/
}

int main( int argc, char** argv )
{
    PCA9685 *pca9685 = new PCA9685() ;

    api_pwm_pca9685_init( pca9685 );

    int dir = 0, throttle_val = 0, theta = 0;
    int current_state = 0;

    // 27 is the ESC key

    //cout<< endl<< "Hit ESC key to exit"<< flush;

    char key = 0;

    int cport_nr = api_uart_open();
    if( cport_nr == -1 )
    {
        cerr<< "Error: Canot Open ComPort";
        return -1;
    }

    while(pca9685->error >= 0)
    {
        key = getkey();
        static unsigned int uintbuffer = 0;
        static unsigned int uintTemp = 280;
        switch (key)
        {

        case 't':
            theta++;
			
            break;

        case 'u':
            theta--;
			
            break;

        case 'j':
			if (throttle_val<30) 
			{
				throttle_val++;
        		setControl(throttle_val,theta);
				api_uart_write(cport_nr, buf_send);
        		
			}
            break;

        case 'g':
            if (throttle_val>-30)
			{
				throttle_val--;
        		setControl(throttle_val,theta);
				api_uart_write(cport_nr, buf_send);
        		
			}
            break;        

        case 'h':
            dir++;
            if(dir > 2 ) dir = 0;   
            break;
        /*case 'c':
            char ch;
            uintbuffer = 0;
            for(ch = getkey();; ch = getkey()) {
                if (ch >= '0' && ch <= '9')
                    uintbuffer = uintbuffer * 10 + ch - '0';
                if (ch == '\n') {
                    uintTemp = uintbuffer;
                    fprintf(stderr, "uTemp = %d\n", uintTemp);
                    break;
                }
            }
            break;*/
        case 'w':
            uintTemp += 10;
            fprintf(stderr, "uTemp = %d\n", uintTemp);
            break;
        case 's':
            uintTemp -= 10;
            fprintf(stderr, "uTemp = %d\n", uintTemp);
            break;
        case 'v':
            uintTemp++;
            fprintf(stderr, "uTemp = %d\n", uintTemp);
            break;
        case 'b':
            uintTemp--;
            fprintf(stderr, "uTemp = %d\n", uintTemp);
            break;
        }
        

        if( key == 27 )
            break;
	
        api_pwm_set_control( pca9685, dir, throttle_val, theta, current_state );
        
        // pca9685->setPWM(STEERING_CHANNEL1,0,pwm1);
        pca9685->setPWM(STEERING_CHANNEL2, 0, uintTemp);

        // cout<< endl<< "Theta: "<< theta<< "; Throttle: "<< throttle_val<< flush;
		//	cout<< endl<< "Data: "<< buf_send<< flush;

        sleep(1);
    }

    api_pwm_pca9685_release( pca9685 );

    return 0;
}






