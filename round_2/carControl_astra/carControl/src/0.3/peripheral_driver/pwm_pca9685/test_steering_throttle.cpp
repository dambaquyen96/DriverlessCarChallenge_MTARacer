
#include "api_control_steering_throttle.h"

int main( int argc, char** argv )
{
    PCA9685 *pca9685 = new PCA9685() ;

    pca9685_init( pca9685 );

    int dir = 0, throttle_val = 0, theta = 0;
    int current_state = 0;

    // 27 is the ESC key

    cout<< endl<< "Hit ESC key to exit"<< flush;

    char key = 0;

    while(pca9685->error >= 0)
    {
        key = getkey();

        switch (key)
        {

        case 't':
            theta++;
            break;

        case 'u':
            theta--;
            break;

        case 'j':
            throttle_val++;
            break;

        case 'g':
            throttle_val--;
            break;

        case 'h':
            dir++;
            if(dir > 2 ) dir = 0;
            break;

        }

        if( key == 27 )
            break;

        setControl( pca9685, dir, throttle_val, theta, current_state );

        sleep(0.1);
    }

    pca9685_release( pca9685 );

    return 0;
}








