//get image from astra camera and record video , it is capable to control car by keyboard
#include "api_kinect_cv.h"
#include "api_lane_detection.h"
#include "api_i2c_pwm.h"
#include "api_uart.h"
#include "utils.h"
#include <iostream>
#include <iomanip>
#include <fstream>
#include <stdio.h>
#include <OpenNI.h>
#include <termios.h>

using namespace openni;

#define SAMPLE_READ_WAIT_TIMEOUT 2000 //2000ms
#define VIDEO_FRAME_WIDTH 640
#define VIDEO_FRAME_HEIGHT 480

VideoWriter depth_videoWriter;
VideoWriter bgr_videoWriter;
bool is_save_file = true;

int cport_nr;
char buf_send[BUFF_SIZE];

void setThrottle(int speed)
{
	if (speed>=0)
    sprintf(buf_send, "f%d\n", speed);
	else
	{ 
		speed=-speed;
		sprintf(buf_send, "b%d\n", speed);
	}
}



void analyzeFrame(const VideoFrameRef& frame)
{
    DepthPixel* depth_img_data;
    RGB888Pixel* color_img_data;

    int w = frame.getWidth();
    int h = frame.getHeight();

    Mat depth_img(h, w, CV_16U);
    Mat depth_img_8u;
    Mat color_img(h, w, CV_8UC3);
	
    switch (frame.getVideoMode().getPixelFormat())
    {
    case PIXEL_FORMAT_DEPTH_1_MM:
    case PIXEL_FORMAT_DEPTH_100_UM:

        depth_img_data = (DepthPixel*)frame.getData();

        memcpy(depth_img.data, depth_img_data, h*w*sizeof(DepthPixel));

        normalize(depth_img, depth_img_8u, 255, 0, NORM_MINMAX);

        depth_img_8u.convertTo(depth_img_8u, CV_8U);
		
		if ((is_save_file)&&(!depth_img_8u.empty()))
           depth_videoWriter.write(depth_img_8u);

        //imshow("depth", depth_img_8u);
        break;
    case PIXEL_FORMAT_RGB888:
        color_img_data = (RGB888Pixel*)frame.getData();

        memcpy(color_img.data, color_img_data, h*w*sizeof(RGB888Pixel));

        cvtColor(color_img, color_img, COLOR_RGB2BGR);

		if ((is_save_file)&&(!color_img.empty()))
			bgr_videoWriter.write(color_img);
		
        //imshow("bgr", color_img);
        break;
    default:
        printf("Unknown format\n");
    }
}





///////// utilitie functions  ///////////////////////////

int main()
{

	Status rc = OpenNI::initialize();
    if (rc != STATUS_OK)
    {
        printf("Initialize failed\n%s\n", OpenNI::getExtendedError());
        return 1;
    }

    Device device;
    rc = device.open(ANY_DEVICE);
    if (rc != STATUS_OK)
    {
        printf("Couldn't open device\n%s\n", OpenNI::getExtendedError());
        return 2;
    }

    VideoStream depth, color;

    if (device.getSensorInfo(SENSOR_DEPTH) != NULL)
    {
        rc = depth.create(device, SENSOR_DEPTH);
        if (rc == STATUS_OK)
        {
            VideoMode depth_mode = depth.getVideoMode();
            depth_mode.setFps(30);
            depth_mode.setResolution(640, 480);
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
            color_mode.setResolution(640, 480);
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

	int codec = CV_FOURCC('D','I','V', 'X');
    Size output_size(VIDEO_FRAME_WIDTH, VIDEO_FRAME_HEIGHT);

    string depth_filename = "depth.avi";
	string bgr_filename = "bgr.avi";
    
    Mat depthMap;
	Mat bgrImage;

    Mat crop_depthMap;
    Mat crop_grayImage;

    char key = 0;

    bool start = false;
    bool stop = false;

    if(is_save_file)
    {
        depth_videoWriter.open(depth_filename, codec, 24, output_size, false);
		bgr_videoWriter.open(bgr_filename, codec, 24, output_size, true);
    }

	cout<< " Press s to start and f to stop ok"<< endl<< flush;



	 ////////  Init image caputre   /////////////////////////////////////////////
   
    int dir = 0, throttle_val = 0, theta = 0;
    int current_state = 0;

    ////////  Init PCA9685 driver   ///////////////////////////////////////////

    PCA9685 *pca9685 = new PCA9685();

    api_pwm_pca9685_init( pca9685 );

    if (pca9685->error >= 0)
        api_pwm_set_control( pca9685, dir, throttle_val, theta, current_state );

    /////////  Init UART here   ///////////////////////////////////////////////

	cport_nr = api_uart_open();

    if( cport_nr == -1 )
    {
        cerr<< "Error: Canot Open ComPort";
        return -1;
    }

    ///////////////////////////////////////////////////////////////////////////

    //============ End of Init phase  ==========================================

    //============ Car Control start here. You must modify this section ===

	int count_s;
    while ( true )
    {
        key = getkey();
        if( key == 's')
            start = true;      
        if( key == 'f') break;         
		
		if( start )
        {       
			if ( key == 'k')	//phai 
			{
				//cout<<"phai"<<endl;
				theta=theta+5;
                //cout<<"theta1000:"<<theta<<'\n';

				if (theta>70) 
				{
				    theta=70;
				    //cout<<"call phai"<<endl;
				}
                //cout<<"theta1100:"<<theta<<'\n';

			}
			if ( key == 'h') //trai
			{
				//cout<<"trai"<<endl;
				theta=theta-5;
                //cout<<"theta1200:"<<theta<<'\n';
                
				if (theta<-70) 
				{
				theta=-70;
				//cout<<"call trai"<<endl;
				}
                //cout<<"theta1300:"<<theta<<'\n';

			}
			if ( key == 'l') //trai
			{
				theta = 45;
                //cout<<"theta1400:"<<theta<<'\n';
			}
			
			if ( key == 'r') //trai
			{
			    throttle_val=0;
				count_s=0;
			}
			if ( key == 'p') //trai
			{
			    throttle_val=50;
				count_s=0;
			}
            //cout<<"theta1500:"<<theta<<'\n';
			if ( key == 'u') //tang toc 
			{
				//cout<<"tang"<<endl;
				throttle_val=throttle_val+5;
				if (throttle_val>50) throttle_val=50;
				count_s=0;
			}
            //cout<<"theta1600:"<<theta<<'\n';
			if ( key == 'j') //giam toc
			{	
				//cout<<"giam"<<endl;
				throttle_val=throttle_val-5;
				if (throttle_val<0) throttle_val=0;
				count_s=0;
			}
            //cout<<"theta1700:"<<theta<<'\n';


		    int readyStream = -1;
		    rc = OpenNI::waitForAnyStream(streams, 2, &readyStream, SAMPLE_READ_WAIT_TIMEOUT);
            //cout<<"theta1800:"<<theta<<'\n';
		    if (rc != STATUS_OK)
		    {
		        printf("Wait failed! (timeout is %d ms)\n%s\n", SAMPLE_READ_WAIT_TIMEOUT, OpenNI::getExtendedError());
                //cout<<"theta1900:"<<theta<<'\n';
		        break;
		    }
            //cout<<"theta2000:"<<theta<<'\n';

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
            //cout<<"theta2100:"<<theta<<'\n';

		    analyzeFrame(frame);
			
			++count_s;
			if (count_s==1)
			{
				setThrottle(throttle_val);
				api_uart_write(cport_nr, buf_send);
			}  	
			if (count_s==100) count_s=2;
        	//// Check PCA9685 driver ////////////////////////////////////////////
        	if (pca9685->error < 0)
        	{
            	//cout<< endl<< "Error: PWM driver"<< endl<< flush;
            	break;
        	}
            //cout<<"theta2200:"<<theta<<'\n';
          
        	api_pwm_set_control( pca9685, dir, throttle_val, theta, current_state );			
			cout<<"theta:"<<theta<<",throttle:"<<throttle_val<<endl;
		    char key = waitKey(1);
		    if( key == 27 ) break;
		}

    }

	if(is_save_file )
    {
        depth_videoWriter.release();
		bgr_videoWriter.release();
    }

    depth.stop();
    color.stop();
    depth.destroy();
    color.destroy();
    device.close();
    OpenNI::shutdown();    
	api_pwm_pca9685_release( pca9685 );
    return 0;
}
