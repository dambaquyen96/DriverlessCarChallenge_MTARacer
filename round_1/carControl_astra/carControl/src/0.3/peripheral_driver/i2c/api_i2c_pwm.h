
#include <string.h>
#include <termios.h>
#include <time.h>
#include <iostream>
#include <vector>
#include <fstream>

#include <pca9685.h>

using namespace std;


// At 61 hz a frame is 16.39 milliseconds
// The PCA9685 PWM Driver is 12 bits
// Full Reverse signal to the ESC is 1 ms
// Neutral signal to the ESC is 1.5 ms
// Full Throttle signal to the ESC is 2 ms

#define PWM_FREQ 60

#define THROTTLE_MAX_REVERSE    153 // 0.6 ms/16.39ms * 4096
#define THROTTLE_NEUTRAL        363 // 1.5 ms/16.39ms * 4096
#define THROTTLE_MAX_FORWARD    573 // 2.3 ms/16.39ms * 4096

/*
before 2017-01-12
#define STEERING_MAX_RIGHT      153
#define STEERING_NEUTRAL        363 // 1.5 ms/16.39ms * 4096
#define STEERING_MAX_LEFT       573*/

#define STEERING_MAX_RIGHT      110
#define STEERING_NEUTRAL        300 // 1.5 ms/16.39ms * 4096
#define STEERING_MAX_LEFT       490

// The THROTTLE is plugged into the following PWM channel
#define THROTTLE_CHANNEL  1
// The Steering Servo is plugged into the follow PWM channel
#define STEERING_CHANNEL1 0
#define STEERING_CHANNEL2 2


#define DIR_REVERSE     1
#define DIR_BRAKE       0
#define DIR_FORWARD     2

#define SET_CTRL 1
#define GET_CTRL 2


int
getkey();

void
api_pwm_pca9685_init( PCA9685 *pca9685);


void
api_pwm_pca9685_release( PCA9685 *pca9685 );


int
api_pwm_set_control(PCA9685 *pca9685,
            int dir,
            int &throttle_val,
            int &theta,
            int &current_state);
