
#include "api_lane_detection.h"

int
laneDetect(
        cv::Mat imgGrayOrigin,
        cv::Rect roi,
        vector<vector<cv::Point> > &lineSegments
        )
{
    cv::Mat imgCanny;
    vector<cv::Point> aux;
    vector<cv::Vec4i> lines;

    if(imgGrayOrigin.empty())
        return -1;

    cv::Mat imgGray = imgGrayOrigin(roi);

//    cv::Size ksize(3,3);

//    cv::blur( imgGray, imgGray, ksize);

//    cv::imshow("Blur",imgGray);

//    Mat img_bin;
//    threshold( imgGray, img_bin, 0, 255, CV_THRESH_BINARY | CV_THRESH_OTSU);

//    cv::imshow("Bin", img_bin);

    //// Canny
//    cv::Scalar mthres=cv::mean(imgGray);
//    double thr= mthres[0];
    cv::Canny(imgGray, imgCanny, 10, 100, 3);
//    cv::Canny(imgGray, imgCanny, thr*0.7, thr*1.2, 3);

    cv::imshow("Canny", imgCanny);


//    cv::Rect roi3 = cv::Rect(0, 0,
//                            roi.width, roi.height/3);

//    cv::Rect roi2 = cv::Rect(roi3.x, roi3.y + roi.height/3,
//                            roi.width, roi.height/3);

//    cv::Rect roi1 = cv::Rect(roi2.x, roi2.y + roi.height/3,
//                            roi.width, roi.height/3);

//    vector< vector < Point> > contours;
//    vector< Vec4f >  _lines;


//    Mat img1 = Mat::zeros( roi1.size(), CV_8UC1);

//    cv::findContours(img1,contours,cv::RETR_LIST,cv::CHAIN_APPROX_SIMPLE);

//    _lines.resize(contours.size());

//    for( int i = 0; i < contours.size(); i++ )
//    {
//        cout<< endl<< contours[i]<< flush;

//        cv::fitLine(contours[i], _lines[i], CV_DIST_L2, 0, 0.01, 0.01);

//        int lefty = (-_lines[i][2]*_lines[i][1]/_lines[i][0])+_lines[i][3];
//        int righty = ((img1.cols-_lines[i][2])*_lines[i][1]/_lines[i][0])+_lines[i][3];

//        cv::line(img1,Point(img1.cols-1,righty),
//                 Point(0,lefty),Scalar(255),1);

//    }


//    cv::imshow("Canny2", img1);


//    waitKey(-1);

//    int erosion_size = 1;
//    cv::Mat element = cv::getStructuringElement( MORPH_RECT,
//                                         Size( 2*erosion_size + 1, 2*erosion_size+1 ),
//                                         Point( erosion_size, erosion_size ) );
//    cv::dilate( imgCanny, imgCanny, element );
//    cv::erode( imgCanny, imgCanny, element );

//     imshow( "Erosion Demo", imgCanny );
//     waitKey(-1);


    //// Hough
    int houghThreshold = 50;

    cv::HoughLinesP(imgCanny, lines, 2, CV_PI/90, houghThreshold, 5,5);



    //// Filter
    for(size_t i=0; i<lines.size(); i++)
    {
        cv::Point pt1, pt2;
        pt1.x = lines[i][0]+roi.x;
        pt1.y = lines[i][1]+roi.y;
        pt2.x = lines[i][2]+roi.x;
        pt2.y = lines[i][3]+roi.y;

        double X = (double)(pt2.x - pt1.x);
        double Y = (double)(pt2.y - pt1.y);

        float  angle = atan2(Y,X)*180/CV_PI;
        float delta = abs(angle);
        int px=(pt1.x+pt2.x)/2;		//// X-Center of line
        int py=(pt1.y+pt2.y)/2;		//// Y-Center of line

        //// Line in the right side
        if( (px>imgGrayOrigin.cols/2) /*&& Y>0 && angle>20 && angle<80*/)
        //if( (px>imgGrayOrigin.cols/2) && Y>0 && angle>25 && angle<85)
        {
            // Store into vector of pairs of Points for msac
            aux.clear();
            aux.push_back(pt1);
            aux.push_back(pt2);
            lineSegments.push_back(aux);

            //printf("angle = %f\n",angle);
            //cv::line(imgColor,pt1,pt2,cv::Scalar(0,0,255,0),2);
            //cv::imshow("Hough", imgColor);
            //cv::waitKey();
        }
        //// Line in the left side
        if( (px<imgGrayOrigin.cols/2) /*&& Y<0 && angle>-80  && angle<-20*/ )
        {
            // Store into vector of pairs of Points for msac
            aux.clear();
            aux.push_back(pt1);
            aux.push_back(pt2);
            lineSegments.push_back(aux);

            //printf("angle = %f\n",angle);
            //cv::line(imgColor,pt1,pt2,cv::Scalar(0,0,255,0),2);
            //cv::imshow("Hough", imgColor);
            //cv::waitKey();
        }
    }
    aux.clear();
    lines.clear();
    return 0;
}


void
show_line_segments(
        Mat imgOutput,
        std::vector< std::vector< cv::Point > > &lineSegments,
        std::vector<Mat> &vps)
{
    for( int i = 0; i < lineSegments.size(); i++ )
    {
        vector<cv::Point> line = lineSegments.at(i);
        cv::Point pt1 = line.at(0);
        cv::Point pt2 = line.at(1);

        cv::line(imgOutput,pt1,pt2,cv::Scalar(0,0,255,0),2);
    }

    cv::Point vp(0,0);

    if( vps.size() > 0 )
    {
        double vpNorm = cv::norm(vps[0]);
        if(fabs(vpNorm - 1) < 0.001)
        {
            cout<< endl<< "Norm INFINITE"<< flush;
        }
        vp.x = vps[0].at<float>(0,0);
        vp.y = vps[0].at<float>(1,0);

        cout<< endl<< "VSP: "<< vp.x<< ", "
                             << vp.y;
    }
    else
    {
        cout<< endl<< "No vsp"<< endl<< flush;
    }

    cv::circle(imgOutput, vp, 4, 255, 3);

    // Draw Output
    cv::imshow("Result",imgOutput);

    vps.clear();
    lineSegments.clear();
}


void
api_vanishing_point_init(MSAC &msac)
{

    int mode = MODE_NIETO;
    bool verbose = false;
    cv::Size procSize(VIDEO_FRAME_WIDTH, VIDEO_FRAME_HEIGHT/4);

    msac.init(mode, procSize, verbose);

}

void
api_get_vanishing_point(Mat imgGray,
                        Rect roi,
                        MSAC &msac,
                        Point &vp,
                        bool is_show_output)
{
    cv::Mat imgPyr;
    std::vector< std::vector< cv::Point > > lineSegments;
    std::vector<Mat> vps;

    // Pyramid
    cv::pyrDown(imgGray, imgPyr, cv::Size(imgGray.cols/2, imgGray.rows/2));
    cv::pyrUp(imgPyr, imgGray, imgGray.size());


    laneDetect(imgGray, roi, lineSegments);

    int numVps = 1;

    // Multiple vanishing points

    std::vector<int> numInliers;

    std::vector<std::vector<std::vector<cv::Point> > > lineSegmentsClusters;

    // Call msac function for multiple vanishing point estimation

    msac.multipleVPEstimation(lineSegments,
                              lineSegmentsClusters,
                              numInliers,
                              vps,
                              numVps);

    if( vps.size() > 0 )
    {
        vp.x = (int)vps[0].at<float>(0,0);
        vp.y = (int)vps[0].at<float>(1,0);
    }
    else // No vanishing point
    {
        vp.x = 0;
        vp.y = 0;
    }

    if( vp.x < 0 || vp.y < 0 ) { vp.x = 0; vp.y = 0; }

    if( is_show_output )
    {
        Mat imgOutput = imgGray.clone();
        show_line_segments( imgOutput, lineSegments, vps );
    }
    else
    {
        vps.clear();
        lineSegments.clear();
    }

}

