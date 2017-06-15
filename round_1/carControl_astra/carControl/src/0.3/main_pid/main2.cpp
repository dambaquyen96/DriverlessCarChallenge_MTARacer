/*
#include "api_kinect_cv.h"
#include "api_lane_detection.h"
#include "api_i2c_pwm.h"
#include "api_uart.h"

///////// utilitie functions  ///////////////////////////

// get distance from vanishing point to Center Point

float get_center_dist( Rect center, Point vp )
{
    float min_x = center.x;
    float max_x = center.x + center.width;


    float px = (float)vp.x;

    if( vp.x == 0 && vp.y == 0 )
        return 0;

    float dist = 0;

    if ( px >= max_x ) // the vp is in the right of center point
    {
        dist = px - max_x;
    }

    if( px <= min_x ) // the vp is in the left of center point
    {
        dist = px - min_x;
    }

    return dist;
}


///////// Main  PID  here  //////////////////////////////////////

int main( int argc, char* argv[] )
{

    VideoCapture capture;

    capture.open( CAP_OPENNI2 );
    if( !capture.isOpened() )
        capture.open( CAP_OPENNI );

    if( !capture.isOpened() )
    {
        cout << "Can not open a capture object." << endl;
        return -1;
    }

    capture.set( CAP_OPENNI_IMAGE_GENERATOR_OUTPUT_MODE, CAP_OPENNI_VGA_30HZ );

    int dir = 0, throttle_val = 0, theta = 0;
    int current_state = 0;

    char key = 0;

    //=========== Init  =======================================================

    ////////  Init PCA9685 driver   ///////////////////////////////////////////

    PCA9685 *pca9685 = new PCA9685() ;

    api_pwm_pca9685_init( pca9685 );

    if (pca9685->error >= 0)
        api_pwm_set_control( pca9685, dir, throttle_val, theta, current_state );

    /////////  Init UART here   ///////////////////////////////////////////////

    int fd = api_uart_open();

    char serial_buffer_recv[150];


    /////////  Init Lane and Center point detection  ////////////////////////

    Mat depthMap;
    Mat grayImage;
    Mat colorDepthMap;
    Mat dispMap;

    float max_disparity = api_kinect_cv_get_max_disparity( capture );


    MSAC msac;
    cv::Rect roi1 = cv::Rect(0, VIDEO_FRAME_HEIGHT*3/4,
                            VIDEO_FRAME_WIDTH, VIDEO_FRAME_HEIGHT/4);

    api_vanishing_point_init( msac );

    ////////  Init direction and ESC speed  ///////////////////////////

    dir = DIR_REVERSE;
    int set_throttle_val = 15;
    throttle_val = 15;
    theta = 0;

    float delta = 0.01;
    if(argc == 2)
    delta = atof( argv[1]);

    if(argc == 3 )
        set_throttle_val = atoi(argv[2]);

    int center_width  = 30;
    int center_height = 30;
    int center_x = VIDEO_FRAME_WIDTH  / 2 - center_width/2;
    int center_y = VIDEO_FRAME_HEIGHT / 2 - center_height/2;

    Rect center ( center_x, center_y, center_width, center_height);


    int frame_width = VIDEO_FRAME_WIDTH;
    int frame_height = VIDEO_FRAME_HEIGHT;

    vector< Rect > rects;
    api_kinect_cv_center_rect_gen( rects, frame_width, frame_height);

    int slice_nb = 3;
    int lower_slice_idx = 0;
    int upper_slice_idx = lower_slice_idx + slice_nb;
    int lower_bound = DIST_MIN + lower_slice_idx * SLICE_DEPTH;
    int upper_bound = lower_bound + slice_nb*SLICE_DEPTH;

    Rect center_rect = rects[lower_slice_idx];
    center_rect = center_rect + Size(0, SLICE_DEPTH);

    Rect intersect;


    ///////////////////////////////////////////////////////////////////////////

    //============ End of Init phase  ==========================================

    //============  PID Car Control start here. You must modify this section ===

    bool start = false;

    double st = 0, et = 0, fps = 0;
    double freq = getTickFrequency();


    bool is_show_cam = true;

    while ( true )
    {
        bool has_collision = false;
        Point center_point(0,0);
        vector< Rect > output_boxes;

        st = getTickCount();

        key = getkey();
        if( key == 's')
            start = !start;

        if( key == 'f') break;

        if( start )
        {
            //// Check PCA9685 driver ////////////////////////////////////////////
            if (pca9685->error < 0)
            {
                cout<< endl<< "Error: PWM driver"<< endl<< flush;
                break;
            }

            // get sensor values from uart

            //api_uart_read( fd, serial_buffer_recv, 150 );

            //printf("Read from serial port: %s\n", serial_buffer_recv);


            ////////  Get input image from camera   //////////////////////////////


            api_kinect_cv_get_images( capture, depthMap, grayImage, dispMap );

            ////////  Detect obtacles  ////////////////////////////////////////

#if 0
            Rect roi2(0, 132, 640, 238);
            api_kinect_cv_get_obtacle_rect( depthMap, output_boxes, roi2,
                                            lower_bound, upper_bound );

            for( int i = 0; i< output_boxes.size(); i++ )
            {
                intersect = output_boxes[i] & center_rect;
                if( intersect.area() != 0 )
                {
                    cout<< endl<< "Has Collision detect"<< flush;
                    has_collision = true;
                }
            }
#endif

            ////////// Detect Center Point ////////////////////////////////////

#if 1
            api_get_vanishing_point( grayImage, roi1, msac, center_point, is_show_cam);
//            api_get_lane_center(grayImage, center_point, is_show_cam);

//            api_get_lane_center(grayImage, center_point, is_show_cam);

//            if( center_point.x != 0 && center_point.y != 0 )
//                cout<< endl<< "Center Lane: "<< center_point<< flush;
//            else
//                cout<< endl<< "No Center Lane"<< flush;


#endif
            ///////  Your PID code here  //////////////////////////////////////////

            float dist = get_center_dist( center, center_point );

            theta += exp(delta*dist);

            cout << endl<< " CenterROI: "<< center << flush;
            cout << endl<< " Center Lane: "<< center_point << flush;
            cout << endl<< " Theta: "<< theta << flush;

            /////////  Set Control to PWM driver here /////////////////////////////

            if( has_collision)
                throttle_val = 0;
            else
                throttle_val = set_throttle_val;

            api_pwm_set_control( pca9685, dir, throttle_val, theta, current_state );

            et = getTickCount();

            fps = 1.0 / ((et-st)/freq);
            cout<< endl<< "FPS: "<< fps<< flush;


            //////// using for debug stuff  ///////////////////////////////////
            if(is_show_cam)
            {
//                Mat binImg = Mat::zeros(depthMap.size(), CV_8UC1);

//                for( int i = 0; i< output_boxes.size(); i++ )
//                    rectangle( binImg, output_boxes[i], Scalar( 255) );

//                imshow( "BoundingRect", binImg );

//                api_kinect_cv_disparity2color(dispMap, colorDepthMap, max_disparity);

//                if(!depthMap.empty())
//                    imshow( "depth", depthMap );

                if(!grayImage.empty())
                    imshow( "gray", grayImage );

//                if(!colorDepthMap.empty())
//                    imshow( "coloredDepth", colorDepthMap );

                waitKey(10);

            }

            if( key == 27 )
                break;

        }

        else
        {
            theta = 0;
            throttle_val = 0;
            api_pwm_set_control( pca9685, dir, throttle_val, theta, current_state );

            sleep(1);
        }
    }

    //////////  Release //////////////////////////////////////////////////////

    api_pwm_pca9685_release( pca9685 );


    return 0;
}*/
















