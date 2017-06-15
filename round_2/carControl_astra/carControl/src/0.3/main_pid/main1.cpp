
#include "api_kinect_cv.h"
#include "api_lane_detection.h"
#include "api_i2c_pwm.h"
#include "api_uart.h"

float prepoint=320;
int c=0;
int dist=0;
///////// utilitie functions  ///////////////////////////

// get distance from vanishing point to Center Point

float get_center_dist( Rect center, Point vp )
{
//    float min_x = center.x;
//    float max_x = center.x + center.width;


    float px = (float)vp.x;

    float dist = 0;

    if (abs(px-prepoint)>150) px=prepoint;
    else prepoint=px;

//    if ( px >= max_x ) // the vp is in the right of center point
//    {
//        dist = px - max_x;
//    }

//    if( px <= min_x ) // the vp is in the left of center point
//    {
//        dist = px - min_x;
//    }

    if ( px >= 320 ) // the vp is in the right of center point
    {
        dist = px - 320;
    }

    if( px <= 320 ) // the vp is in the left of center point
    {
        dist = px - 320;
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

    bool is_show_cam = true;

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


    /////////  Init Lane and vanishing point detection  ////////////////////////

    Mat depthMap;
    Mat grayImage;
    Mat colorDepthMap;
    Mat disparityMap;

    MSAC msac;
    cv::Rect roi = cv::Rect(0, VIDEO_FRAME_HEIGHT*3/4,
                            VIDEO_FRAME_WIDTH, VIDEO_FRAME_HEIGHT/4);

    // Do not forget this !!!!!!!!!!!!!
    api_vanishing_point_init( msac );

    float max_disparity = api_kinect_cv_get_max_disparity( capture );


    ////////  Init direction and ESC speed  ///////////////////////////

    dir = DIR_REVERSE;
    throttle_val = 16;
    theta = 0;

    float delta = 0.004;

    int center_width  = 30;
    int center_height = 30;
    int center_x = VIDEO_FRAME_WIDTH  / 2 - center_width/2;
    int center_y = VIDEO_FRAME_HEIGHT / 2 - center_height/2;

    Rect center ( center_x, center_y, center_width, center_height);

    string gray_filename = "gray.avi";
    VideoWriter gray_videoWriter;



    ///////////////////////////////////////////////////////////////////////////

    //============ End of Init phase  ==========================================

    //============  PID Car Control start here. You must modify this section ===

    bool start = false;
    bool stop = false;

    while ( true )
    {

        ////////  Get input image from camera   //////////////////////////////

        api_kinect_cv_get_images( capture, depthMap, grayImage ,disparityMap);
//            api_kinect_cv_get_images( capture, depthMap, grayImage );

        if(is_show_cam)
        {
            //api_kinect_cv_depth_map_gray2color( grayImage, colorDepthMap, max_disparity );
            api_kinect_cv_disparity2color(grayImage, colorDepthMap, max_disparity);
//                if(!depthMap.empty())
//                    imshow( "depth", depthMap );

            if(!grayImage.empty())
            {
                imshow( "gray", grayImage );
                gray_videoWriter.write(grayImage);
            }

//                if(!colorDepthMap.empty())
//                    imshow( "coloredDepth", colorDepthMap );

//                if(!disparityMap.empty())
//                    imshow( "disparityMap", disparityMap );
            waitKey(10);

        }

        key = getkey();
        if( key == 's')
            start = true;
        if( key == 'f')
            stop = true;
        if( stop ) break;

        if( start )
        {
            //// Check PCA9685 driver ////////////////////////////////////////////
            if (pca9685->error < 0)
            {
                cout<< endl<< "Error: PWM driver"<< endl<< flush;
                break;
            }

            // get sensor values from uart

//            api_uart_read(
//                        fd, serial_buffer_recv, 150 );

//            printf("Read from serial port: %s\n", serial_buffer_recv);




            if( key == 27 )
                break;

            ////////// Detect Vanishing Point ////////////////////////////////////

            Point vp;

            api_get_vanishing_point( grayImage, roi, msac, vp);


            ///////  Your PID code here  //////////////////////////////////////////

            if (((vp.x==0)&&(vp.y==0))||(vp.x<0)||(vp.y<0)||(vp.x>640)||(vp.y>480))				//neu mat tin hieu vanishingpoint
            {
                ++c;
                throttle_val=11;
            }
            else
            {
                if (dir == DIR_FORWARD)
                {
                    dir = DIR_REVERSE;		//kiem tra xem neu dang di lui thi doi chieu
                    throttle_val=15;
                }
                dist = get_center_dist( center, vp );
                theta = dist/11;
                c=0;
            }

            if (c>15) 			//neu mat tin hieu lien tuc trong 15 lan thi di lui lai voi theta khac
            {
                dir = DIR_FORWARD;
                theta = -dist/6;
            }


            if (theta>30) theta=30;
            if (theta<-30) theta=-30;

            cout << endl<< " center: "<< center << flush;
            cout << endl<< " vp: "<< vp << flush;
            cout << endl<< " Theta: "<< theta<< flush;

            /////////  Set Control to PWM driver here /////////////////////////////

            api_pwm_set_control( pca9685, dir, throttle_val, theta, current_state );

        }
    }

    //////////  Release //////////////////////////////////////////////////////

    api_pwm_pca9685_release( pca9685 );


  /////////test depth distance/////////////////


//    while(true)
//    {
//        Mat depthMap;
//        Mat bgrImage;

//        for (int i = 0; i < 640; i++)
//        {
//            for (int j = 0; j < 480; j++)
//            {

//            }
//        }
//        capture.grab();

//        capture.retrieve(depthMap,CAP_OPENNI_DEPTH_MAP);
//        capture.retrieve(bgrImage,CAP_OPENNI_BGR_IMAGE);


//        cout<<"rows:"<<depthMap.rows<<"cols:"<<depthMap.cols<<endl;
//        int depth=depthMap.at<unsigned short>(320,240);

//        if (depth < 900)
//        {
//            cout<<"have object!!!"<<endl;
//            break;
//        }
//        //cout<<"depth: "<<depth<<" mm"<<endl;
//        cout<<"no object!!!"<<endl;

//        imshow("RGBImage",bgrImage);

//        imshow("Depth",depthMap);

//        waitKey(10);

//        key = getkey();
//        if( key == 's') break;
//    }

    return 0;
}
