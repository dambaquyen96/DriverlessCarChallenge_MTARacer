//Automatic car control.
//Line detection: Wavelet transform
//Target point: vanishing point
//Control: pca9685
#include "api_kinect_cv.h"
#include "api_lane_detection.h"
#include "api_i2c_pwm.h"
#include "api_uart.h"
#include "utils.h"
#include <iostream>

using namespace openni;

#define SAMPLE_READ_WAIT_TIMEOUT 2000 //2000ms
#define VIDEO_FRAME_WIDTH 640
#define VIDEO_FRAME_HEIGHT 480

bool is_save_file = true;
VideoWriter depth_videoWriter;	
VideoWriter color_videoWriter;
Status rc;
Device device;

char analyzeFrame(const VideoFrameRef& frame, Mat& depth_img, Mat& color_img) {
    //static int cnt = 0;
    DepthPixel* depth_img_data;
    RGB888Pixel* color_img_data;

    int w = frame.getWidth();
    int h = frame.getHeight();

    depth_img = Mat(h, w, CV_16U);
    color_img = Mat(h, w, CV_8UC3);
    Mat depth_img_8u;
	
    switch (frame.getVideoMode().getPixelFormat())
    {
    case PIXEL_FORMAT_DEPTH_1_MM: return 'm';
    case PIXEL_FORMAT_DEPTH_100_UM:

        depth_img_data = (DepthPixel*)frame.getData();

        memcpy(depth_img.data, depth_img_data, h*w*sizeof(DepthPixel));

        normalize(depth_img, depth_img_8u, 255, 0, NORM_MINMAX);

        depth_img_8u.convertTo(depth_img_8u, CV_8U);
		
		if ((is_save_file)&&(!depth_img_8u.empty()))
           depth_videoWriter.write(depth_img_8u);

        return 'd';
    case PIXEL_FORMAT_RGB888:
        color_img_data = (RGB888Pixel*)frame.getData();

        memcpy(color_img.data, color_img_data, h*w*sizeof(RGB888Pixel));

        cvtColor(color_img, color_img, COLOR_RGB2BGR);

		if ((is_save_file)&&(!color_img.empty()))
			color_videoWriter.write(color_img);
		
        return 'c';
    default:
        printf("Unknown format\n");
        return 'u';
    }
}


int cport_nr;
char buf_send[BUFF_SIZE];

void setThrottle(int speed) {
	if (speed>=0)
    sprintf(buf_send, "f%d\n", speed);
	else { 
		speed=-speed;
		sprintf(buf_send, "b%d\n", speed);
	}
}

///////// utilitie functions  ///////////////////////////

main( int argc, char* argv[] )
{
	 ////////  Init image caputre   /////////////////////////////////////////////
    VideoStream depth, color;
    /*int initStatus = initOpenNI(depth, color);
    if (!initStatus) {
        printf("init failed!\n");
        return initStatus;
    }*/
    rc = OpenNI::initialize();
    if (rc != STATUS_OK)
    {
        printf("Initialize failed\n%s\n", OpenNI::getExtendedError());
        return 0;
    }
    rc = device.open(ANY_DEVICE);
    if (rc != STATUS_OK)
    {
        printf("Couldn't open device\n%s\n", OpenNI::getExtendedError());
        return 0;
    }
    if (device.getSensorInfo(SENSOR_DEPTH) != NULL)
    {
        rc = depth.create(device, SENSOR_DEPTH);
        if (rc == STATUS_OK)
        {
            VideoMode depth_mode = depth.getVideoMode();
            depth_mode.setFps(30);
            depth_mode.setResolution(VIDEO_FRAME_WIDTH, VIDEO_FRAME_HEIGHT);
            depth_mode.setPixelFormat(PIXEL_FORMAT_DEPTH_100_UM);
            depth.setVideoMode(depth_mode);

            rc = depth.start();
            if (rc != STATUS_OK)
            {
                printf("Couldn't start the color stream\n%s\n", OpenNI::getExtendedError());
            }
        }
        else
        {
            printf("Couldn't create depth stream\n%s\n", OpenNI::getExtendedError());
        }
    }

    if (device.getSensorInfo(SENSOR_COLOR) != NULL)
    {
        rc = color.create(device, SENSOR_COLOR);
        if (rc == STATUS_OK)
        {
            VideoMode color_mode = color.getVideoMode();
            color_mode.setFps(30);
            color_mode.setResolution(VIDEO_FRAME_WIDTH, VIDEO_FRAME_HEIGHT);
            color_mode.setPixelFormat(PIXEL_FORMAT_RGB888);
            color.setVideoMode(color_mode);

            rc = color.start();
            if (rc != STATUS_OK)
            {
                printf("Couldn't start the color stream\n%s\n", OpenNI::getExtendedError());
            }
        }
        else
        {
            printf("Couldn't create color stream\n%s\n", OpenNI::getExtendedError());
        }
    }
    
    VideoFrameRef frame;
    VideoStream* streams[] = {&depth, &color};
    
	string color_filename = "color.avi";
	string depth_filename = "depth.avi";
	Mat depthImg, colorImg, grayImage;
	int codec = CV_FOURCC('D','I','V', 'X');
	int video_frame_width = VIDEO_FRAME_WIDTH;
    int video_frame_height = VIDEO_FRAME_HEIGHT;
	Size output_size(video_frame_width, video_frame_height);

	if(is_save_file) {
        color_videoWriter.open(color_filename, codec, 24, output_size, true);
        //depth_videoWriter.open(depth_filename, codec, 24, output_size, false);
	}

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

	cport_nr = api_uart_open();

    if( cport_nr == -1 ) {
        cerr<< "Error: Canot Open ComPort";
        return -1;
    }

    MSAC msac;
    cv::Rect roi1 = cv::Rect(0, VIDEO_FRAME_HEIGHT*3/4,
                            VIDEO_FRAME_WIDTH, VIDEO_FRAME_HEIGHT/4);

    api_vanishing_point_init( msac );

    ////////  Init direction and ESC speed  ///////////////////////////

    dir = DIR_REVERSE;
    int set_throttle_val = 10;
    throttle_val = 10;
    theta = 0;
    
    //FILE* thetaLogs = fopen("log.txt", "w");

    float delta = 0.01;
    if(argc == 2) delta = atof( argv[1]);
    if(argc == 3 ) set_throttle_val = atoi(argv[2]);

    int center_width  = 30;
    int center_height = 30;
    int center_x = VIDEO_FRAME_WIDTH  / 2 - center_width/2;
    int center_y = VIDEO_FRAME_HEIGHT / 2 - center_height/2;
    Rect center ( center_x, center_y, center_width, center_height);


    int frame_width = VIDEO_FRAME_WIDTH;
    int frame_height = VIDEO_FRAME_HEIGHT;

    //============ End of Init phase  ==========================================

    //============  PID Car Control start here. You must modify this section ===

    bool start = false;

    double st = 0, et = 0, fps = 0;
    double freq = getTickFrequency();


    bool is_show_cam = false;
	int count_s,count_ss;
    while ( true )
    {
        Point center_point(0,0);

        st = getTickCount();
        key = getkey();
        if( key == 's') start = !start;
        if( key == 'f') break;

        if( start )
        {
        
            
			++count_s;
			count_ss=0;
			if (count_s==1)
			{
				throttle_val = set_throttle_val;
				setThrottle(throttle_val);
				api_uart_write(cport_nr, buf_send);
			}  	
			if (count_s==100) count_s=2;
            //// Check PCA9685 driver ////////////////////////////////////////////
            if (pca9685->error < 0)
            {
                cout<< endl<< "Error: PWM driver"<< endl<< flush;
                break;
            }


            ////////  Get input image from camera   //////////////////////////////

            int readyStream = -1;
		    rc = OpenNI::waitForAnyStream(streams, 2, &readyStream, SAMPLE_READ_WAIT_TIMEOUT);
		    if (rc != STATUS_OK)
		    {
		        printf("Wait failed! (timeout is %d ms)\n%s\n", SAMPLE_READ_WAIT_TIMEOUT, OpenNI::getExtendedError());
		        break;
		    }

		    switch (readyStream)
		    {
		    case 0:
		        // Depth
		        depth.readFrame(&frame);
		        break;
		    case 1:
		        // Color
		        color.readFrame(&frame);
		        break;
		    default:
		        printf("Unxpected stream\n");
		    }
		    char recordStatus = analyzeFrame(frame, depthImg, colorImg);
		   
            ////////// Detect Center Point ////////////////////////////////////

#if 1
if (recordStatus == 'c') {
            cvtColor(colorImg, grayImage, CV_BGR2GRAY);
            if(is_save_file)
            {
                if(!colorImg.empty())
                    color_videoWriter.write(colorImg);
				//if (!depthImg.empty() )
                    //depth_videoWriter.write(depthImg);
			}
            api_get_vanishing_point( grayImage, roi1, msac, center_point, is_show_cam,"Canny");
}
#endif
            ///////  Your PID code here  //////////////////////////////////////////

            float dist = get_center_dist( center, center_point );
            theta = dist/3;
            cout << endl<< " Center Lane: "<< center_point << flush;
            cout << endl<< " Theta: "<< theta << flush;

            /*fprintf(thetaLogs, "1000 theta  %lf\n", (double)theta);
            fprintf(thetaLogs, "1000 center   %d %d\n", (int)center.x, (int)center.y);
            fprintf(thetaLogs, "1000 cpoint   %d %d\n", (int)center_point.x, (int)center_point.y);
            fprintf(thetaLogs, "1000 dist   %lf\n", (double)dist);*/

            /*cout << endl<< " CenterROI: "<< center << flush;

            /////////  Set Control to PWM driver here /////////////////////////////

            /*if( has_collision)
                throttle_val = 0;
            else
                throttle_val = set_throttle_val;*/

			
            api_pwm_set_control( pca9685, dir, throttle_val, theta, current_state );
            //fprintf(thetaLogs, "1100 %lf\n", (double)theta);
            et = getTickCount();

            fps = 1.0 / ((et-st)/freq);
            cout<< endl<< "FPS: "<< fps<< flush;
			

            //////// using for debug stuff  ///////////////////////////////////
            if(is_show_cam) {
                if(!grayImage.empty())
                    imshow( "gray", grayImage );
                waitKey(10);
            }
            if( key == 27 ) break;
        }
        else {
            //fprintf(thetaLogs, "1200 %lf\n", (double)theta);
			theta = 0;
            throttle_val = 0;
			count_s=0;
			++count_ss;
			if (count_ss==1) {
				setThrottle(throttle_val);
				api_uart_write(cport_nr, buf_send);
			}  	
			if (count_ss==100) count_ss=2;            

            api_pwm_set_control( pca9685, dir, throttle_val, theta, current_state );
            //fprintf(thetaLogs, "1300 %lf\n", (double)theta);
            sleep(1);
        }
    }
    //////////  Release //////////////////////////////////////////////////////
	if(is_save_file)
    {
        color_videoWriter.release();
        //depth_videoWriter.release();

	}
	//fclose(thetaLogs);	

    api_pwm_pca9685_release( pca9685 );
    return 0;
}

