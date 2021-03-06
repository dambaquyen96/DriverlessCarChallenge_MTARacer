
#include "api_lane_detection.h"
enum ConvolutionType {
    /* Return the full convolution, including border */
    CONVOLUTION_FULL,
    
    /* Return only the part that corresponds to the original image */
    CONVOLUTION_SAME,
    
    /* Return only the submatrix containing elements that were not influenced by the border */
    CONVOLUTION_VALID
};


struct res_contour
{
 //   vector<Point> contour;
    int u;
    double l;
};	

bool Compare(const res_contour x,const res_contour y)
{
    return x.l <y.l;
}
double sqr(double x) {
    return x * x;
}

bool inRange(double val, double l, double r) {
    return (l <= val && val <= r);
}
double angle( cv::Point pt1, cv::Point pt2, cv::Point pt0 ) {
    double dx1 = pt1.x - pt0.x;
    double dy1 = pt1.y - pt0.y;
    double dx2 = pt2.x - pt0.x;
    double dy2 = pt2.y - pt0.y;
    return (dx1*dx2 + dy1*dy2)/sqrt((dx1*dx1 + dy1*dy1)*(dx2*dx2 + dy2*dy2) + 1e-10);
}
void find_Contours(Mat img,Mat &dst,string tmp_name,vector<vector<Point> > &contours)
{
    Mat bin_img=img.clone();
    int erosion_size = 7;
    cv::Mat element = cv::getStructuringElement(MORPH_RECT,
                                                Size(2 * erosion_size + 1, 2 * erosion_size + 1),
                                                Point(erosion_size, erosion_size));
    dilate(img, bin_img, element);
    erode(bin_img, bin_img, element);
    vector<Vec4i> hierarchy;
    //cv::findContours(img, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));

    
    ////CvMemStorage *storage = cvCreateMemStorage(0);
    ////CvSeq *seq_contour = cvCreateSeq(CV_SEQ_ELTYPE_POINT, sizeof(CvSeq), sizeof(CvPoint), storage);
    cv::findContours(bin_img, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));
    
    /// Draw contours
    int xmax, ymax, xmin, ymin;
    int xleft, xright, ytop, ybottom;
    Mat drawing = Mat::zeros(img.size(), CV_8UC1);
    int width = drawing.cols;
    int height = drawing.rows;
    int x=20,y=20;
    int maxh=0,imax=-1;
    double maxl=0;
    
    for (int i = 0; i< contours.size(); i++)
    {
        int count = contours[i].size();
        if (count>30)
        {
            for (int t = 0; t < count; t++)
            {
                if (t == 0){
                    xleft = xright = contours[i][t].x;
                    ytop = ybottom = contours[i][t].y;
                }
                if (contours[i][t].x > xright){
                    xright = contours[i][t].x;
                }
                if (contours[i][t].x < xleft)
                {
                    xleft = contours[i][t].x;
                }
                if (contours[i][t].y > ytop)
                {
                    ytop = contours[i][t].y;
                }
                if (contours[i][t].y < ybottom)
                {
                    ybottom = contours[i][t].y;
                }
                xmin = xleft; xmax = xright;
                ymin = ybottom; ymax = ytop;
            }
            int w = xmax - xmin;
            double l = sqrt((ymax - ymin)*(ymax-ymin)+(xmax-xmin)*(xmax-xmin)),h=ymax-ymin;
	
            int s = w*h;
            if ( xmin!=0&&xmax!=width-1)
                //if ()
            {
                if (h > maxh&&h>height/4)
                {
                    maxh = h; imax = i;
                }
	//	if(l>height*3/4)
	//drawContours(drawing, contours, i, color, CV_FILLED, 8, hierarchy, 3, Point());
            }
        }
    }
    Scalar color = Scalar(255,255,255);
    if(imax!=-1)drawContours(drawing, contours, imax, color, CV_FILLED, 8, hierarchy, 3, Point());
    
    
    /// Show in a window
    // namedWindow("Contours", CV_WINDOW_AUTOSIZE);
    //int erosion_size = 7;
   // cv::Mat element = cv::getStructuringElement(MORPH_RECT,                                                Size(2 * erosion_size + 1, 2 * erosion_size + 1),                                                Point(erosion_size, erosion_size));
    dilate(drawing, dst, element);
    erode(dst, dst, element);
    
    //  dst=drawing;
    // imshow(tmp_name, dst);
    //contours.clear();
    //hierarchy.clear();
    //cv::findContours(dst, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));
    
    
}
void find_Contours2(Mat img,Mat &dst,string tmp_name,vector<vector<Point> > &contours,vector<vector<Point> > &res_contours)
{
    Mat bin_img=img.clone();
    int erosion_size = 7;
    cv::Mat element = cv::getStructuringElement(MORPH_RECT,
                                                Size(2 * erosion_size + 1, 2 * erosion_size + 1),
                                                Point(erosion_size, erosion_size));
   // dilate(img, bin_img, element);
   // erode(bin_img, bin_img, element);
    vector<Vec4i> hierarchy;
    //cv::findContours(img, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));

    
    ////CvMemStorage *storage = cvCreateMemStorage(0);
    ////CvSeq *seq_contour = cvCreateSeq(CV_SEQ_ELTYPE_POINT, sizeof(CvSeq), sizeof(CvPoint), storage);
    cv::findContours(bin_img, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));
    
    /// Draw contours
    int xmax, ymax, xmin, ymin;
    int xleft, xright, ytop, ybottom;
    Mat drawing = Mat::zeros(img.size(), CV_8UC1);
    int width = drawing.cols;
    int height = drawing.rows;
    res_contour res_Contours[100];
    int x=20,y=20;
    int maxh=0,imax=-1;
    double maxl=0;
    int j=0;
    for (int i = 0; i< contours.size(); i++)
    {
        int count = contours[i].size();
        if (count>30)
        {
            for (int t = 0; t < count; t++)
            {
                if (t == 0){
                    xleft = xright = contours[i][t].x;
                    ytop = ybottom = contours[i][t].y;
                }
                if (contours[i][t].x > xright){
                    xright = contours[i][t].x;
                }
                if (contours[i][t].x < xleft)
                {
                    xleft = contours[i][t].x;
                }
                if (contours[i][t].y > ytop)
                {
                    ytop = contours[i][t].y;
                }
                if (contours[i][t].y < ybottom)
                {
                    ybottom = contours[i][t].y;
                }
                xmin = xleft; xmax = xright;
                ymin = ybottom; ymax = ytop;
            }
            int w = xmax - xmin;
            double l = sqrt((ymax - ymin)*(ymax-ymin)+(xmax-xmin)*(xmax-xmin)),h=ymax-ymin;
            int s = w*h;
            if ( xmin!=0&&xmax!=width-1)
                //if ()
            {
                if (l>height/3&&s>50&&h>height/4)
                {
                   // res_Contours[j].contour=contours[i];
                    res_Contours[j].l=l;
                    res_Contours[j].u=i;
                    j++;
                    if (j>100) {
                        return;
                    }
                   // maxl = l; imax = i;
                }
            }
        }
    }
    sort(res_Contours, res_Contours+j, Compare);
    Scalar color = Scalar(255,255,255);
    if(j>3)
    for(int t=0;t<3;t++)
   // if(imax!=-1)
    {
        drawContours(drawing, contours, res_Contours[t].u, color, CV_FILLED, 8, hierarchy, 3, Point());
     //   res_contours[t]=contours[res_Contours[t].u];
    }
    else
        
        for(int t=0;t<j;t++)
            // if(imax!=-1)
        {            drawContours(drawing, contours, res_Contours[t].u, color, CV_FILLED, 8, hierarchy, 3, Point());
         //   res_contours[t]=contours[res_Contours[t].u];
        }
    
    /// Show in a window
    // namedWindow("Contours", CV_WINDOW_AUTOSIZE);
    //int erosion_size = 7;
   // cv::Mat element = cv::getStructuringElement(MORPH_RECT,                                                Size(2 * erosion_size + 1, 2 * erosion_size + 1),                                                Point(erosion_size, erosion_size));
    dilate(drawing, dst, element);
    erode(dst, dst, element);
    
    //  dst=drawing;
    // imshow(tmp_name, dst);
    //contours.clear();
    //hierarchy.clear();
    //cv::findContours(dst, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));
    
    
}

void detectLane(Mat &imgGray,Rect roi,string &tmp_name,vector<Point> &vec_match_loc,int &isLane,int &ymin,int &ymax){
    vec_match_loc.clear();
    Mat img = imgGray(roi).clone();
    GaussianBlur(img, img, Size(11, 11), 0);
    //    Size ksize(9,9);
    //    blur(img, img, ksize );
    Mat img_bin;
    //    threshold( img, img_bin, 0, 255, CV_THRESH_BINARY | CV_THRESH_OTSU);
    adaptiveThreshold(img, img_bin, 255, 0, 0, 71, -11);
    if(roi.x==0) tmp_name="left";
    else tmp_name="right";
    //imshow(tmp_name, img_bin);
    string tmp_nameC=tmp_name+"_contours";
    Mat dst;
    vector<vector<Point> > contours;
    Point L(0,0),R(0,0);
  //  Processing(img_bin, img_bin,tmp_nameC,contours);
    //imshow(tmp_name, img_bin);
}

void get_lines_byme(Mat &imgGray,Rect roi,string &tmp_name,vector<Point> &vec_match_loc,int &isLane,int &ymin,int &ymax)
{
    // Blur and threshold
    vec_match_loc.clear();
    Mat img = imgGray(roi).clone();
    GaussianBlur(img, img, Size(11, 11), 0);
    //    Size ksize(9,9);
    //    blur(img, img, ksize );
    Mat img_bin;
    //    threshold( img, img_bin, 0, 255, CV_THRESH_BINARY | CV_THRESH_OTSU);
    adaptiveThreshold(img, img_bin, 255, 0, 0, 71, -11);
    if(roi.x==0) tmp_name="left";
    else tmp_name="right";
    //imshow(tmp_name, img_bin);
    string tmp_nameC=tmp_name+"_contours";
    Mat dst;
    vector<vector<Point> > contours;
vector<vector<Point> > res_contours;
    Point L(0,0),R(0,0);
    find_Contours(img_bin, img_bin,tmp_nameC,contours);
//find_Contours2(img_bin, img_bin,tmp_nameC,contours,res_contours);
    //Processing(img_bin, img_bin,tmp_nameC,contours);
    //imshow(tmp_nameC, img_bin);
    //
    //  vector<Vec4i> hierarchy;
    // contours.clear();
    //hierarchy.clear();
    // Mat img_bin1=img_bin.clone();
    //cv::findContours(img_bin1, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));
    //   imshow(tmp_name, img_bin);
    if(roi.x==0){
        // if(contours.size()>0){
        //left
        //  for(int i=0;i<contours[0].size();i++)
        //       L.x+=contours[0][i].x+roi.x;
        //   L.x=L.x/contours[0].size();
        //   vec_match_loc.push_back(L);
        int xmin=img_bin.cols,xmax=0;
        ymin=-1,ymax=-1;
        
        int t=0;
        for (int i=img_bin.rows-2; i>=0; i--) {
            for (int j=img_bin.cols-1; j>=0; j--) {
                if (img_bin.at<uchar>(i, j) == 255){
                    //&&img_bin.at<uchar>(i, j-1)==255) {
                    L.x=L.x+(j+roi.x);
                    L.y=L.y+(i+roi.y);
                    if(xmin>j){
                        xmin=j;
                        ymin=i;
                    }
                    if(xmax<j){
                        xmax=j;
                        ymax=i;
                    }
                    //isLane=true;
                    //     Point p(j+roi.x,i+roi.y);
                    //      vec_match_loc.push_back(p);
                    t++;
                    break;
                }
            }
            //             if(isLane) break;
        }
        if(t>0){
            L.x=L.x/t;
            L.y=L.y/t;
            if(ymax<ymin&&(xmax-xmin)>=0) isLane=1;                 //re phai
            else isLane=2;                          //re trai
        }
        // vec_match_loc.push_back(L);
        Point minP,maxP;
        minP.x=xmin;maxP.x=xmax;minP.y=ymin;maxP.y=ymax;
        vec_match_loc.push_back(L);
        vec_match_loc.push_back(minP);
        vec_match_loc.push_back(maxP);
    }
    else{
        //if(contours.size()>0){
        //     for(int i=0;i<contours[0].size();i++)
        //        R.x+=contours[0][i].x+roi.x;
        //    R.x=R.x/contours[0].size();
        //     vec_match_loc.push_back(R);
        int t=0;
        int xmin=img_bin.cols,xmax=0;
        ymin=-1,ymax=-1;
        
        for (int i=img_bin.rows-2; i>=0; i--) {
            for (int j=0; j<=img_bin.cols-1; j++) {
                if (img_bin.at<uchar>(i, j) ==255){
                    // 0&&img_bin.at<uchar>(i, j+1)==255) {
                    //     Point p(j+roi.x,i+roi.y);
                    //       vec_match_loc.push_back(p);
                    //      t=1;
                    //     break;
                    R.x+=(j+roi.x);
                    R.y+=(i+roi.y);
                    t++;
                    if(xmin>j){
                        xmin=j;
                        ymin=i;
                    }
                    if(xmax<j){
                        xmax=j;
                        ymax=i;
                    }
                    //isLane=true;
                    break;
                }
            }
            //if(isLane) break;
        }
        if(t>0){
            R.x=R.x/t;
            R.y=R.y/t;
            if(ymax>ymin&&(xmax-xmin)>=0) isLane=1;                 //re trai
            else isLane=2;                          //re phai
            
        }
        Point minP,maxP;
        minP.x=xmin;maxP.x=xmax;minP.y=ymin;maxP.y=ymax;
        vec_match_loc.push_back(R);
        vec_match_loc.push_back(minP);
        vec_match_loc.push_back(maxP);
        //}
        
    }
}
void Processing(Mat img,Mat &dst,string tmp_name,vector<vector<Point> > &contours)
{
    //dst=img.clone();
    vector<Vec4i> hierarchy;
    cv::findContours(img, contours, hierarchy, CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));
    std::vector<std::vector<cv::Point> > squares;
    
    vector<Point> approx, approxRectangle;
    Rect bounding_rect(0, 0, 0, 0);
    double max_area = 0;
    int index = 0;
    for (int i = 0; i < contours.size(); i++)// xet tung contour
    {
        approxPolyDP(Mat(contours[i]), approx, arcLength(Mat(contours[i]), false)*0.03, true);
        
        if( approx.size() == 4 && fabs(contourArea(cv::Mat(approx))) > 100) //&& cv::isContourConvex(cv::Mat(approx))) {
        {
            double maxCosine = 0;
            for( int j = 2; j < 5; j++ )
            {
                double cosine = fabs(angle(approx[j%4], approx[j-2], approx[j-1]));
                maxCosine = MAX(maxCosine, cosine);
            }
           // if( maxCosine < 0.3 ) {
                squares.push_back(approx);
           // }
        }
    }
    for ( int i = 0; i< squares.size(); i++ ) {
        // draw contour
        cv::drawContours(dst, squares, i, cv::Scalar(255,0,0), CV_FILLED, 8, std::vector<cv::Vec4i>(), 0, cv::Point());
        // draw bounding rect
        //cv::Rect rect = boundingRect(cv::Mat(squares[i]));
        //cv::rectangle(dst, rect.tl(), rect.br(), cv::Scalar(0,255,0), 2, 8, 0);
        // draw rotated rect
        cv::RotatedRect minRect = minAreaRect(cv::Mat(squares[i]));
        cv::Point2f rect_points[4];
        minRect.points( rect_points );
        for ( int j = 0; j < 4; j++ ) {
            cv::line( dst, rect_points[j], rect_points[(j+1)%4], cv::Scalar(0,0,255), 1, 8 ); // blue
        }
    }
    
}
void detect_centerPoint(Mat img,Rect roi,string tmp_name){
    Mat grayImage;
    cvtColor(img, grayImage, CV_BGR2GRAY);
    
    GaussianBlur(grayImage, grayImage, Size(11, 11), 0);
    Mat img_bin = grayImage(roi).clone();
    //    Size ksize(9,9);
    //    blur(img, img, ksize );
    Mat img_thresh;
      //  threshold( img_bin, img_thresh, 200, 255, CV_THRESH_BINARY);
    adaptiveThreshold(img_bin, img_thresh, 255, 0, 0, 91, -15);
    int erosion_size=5;
    cv::Mat element = cv::getStructuringElement(MORPH_RECT,
                                                Size(2 * erosion_size + 1, 2 * erosion_size + 1),
                                                                       Point(erosion_size, erosion_size));
    dilate(img_thresh, img_thresh, element);
    erode(img_thresh, img_thresh, element);
    //cv::Canny(img_thresh, img_thresh, 0, 50, 5);
    //imshow(tmp_name, img_thresh);
    string tmp_nameC=tmp_name+"_contours";
    Mat dst=img(roi).clone();
    vector<vector<Point> > contours;
    Point L(0,0),R(0,0);
    Processing(img_thresh, dst,tmp_nameC,contours);
     //imshow(tmp_nameC, dst);

}

void detect_obs(Mat &imgGray,Mat &dst,Rect roi)
{
    Mat img = imgGray(roi).clone();
    GaussianBlur(img, img, Size(11, 11), 0);
    //    Size ksize(9,9);
    //    blur(img, img, ksize );
    Mat img_bin;
        threshold( img, img_bin, 100, 255,CV_THRESH_BINARY);
    
    Mat mThres_Gray;
    double CannyAccThresh = threshold(img,mThres_Gray,0,255,CV_THRESH_BINARY|CV_THRESH_OTSU);
    double CannyThresh = 0.1 * CannyAccThresh;
    Mat cannyImg;
    Canny(img,cannyImg,CannyThresh,CannyAccThresh);
    
    dst=cannyImg.clone();
    //cv::imshow("canny", dst);
}

void centerPoint(cv::Mat &imgGray, Point &center_point, bool is_show_output, Point &carPoint, vector<Point> &vec_left_points, vector<Point> &vec_right_points,double &distance)
{
    
    vec_left_points.clear();
    vec_right_points.clear();
    
    //  generate roi
    int frame_width  = imgGray.cols;
    int frame_height = imgGray.rows;

static Point prvpoint(frame_width / 2, frame_height);
	int nguong = 50;
     Mat dst;
    cv::Rect roi= cv::Rect(0, frame_height*3/8,
                                 frame_width, frame_height/2);
   // detect_obs(imgGray, dst,roi);
    

    cv::Rect roi_left = cv::Rect(0, frame_height*5/8,
                                 frame_width/2-14, frame_height/4);
    
    cv::Rect roi_right = cv::Rect(roi_left.width, roi_left.y,
                                  frame_width - roi_left.width, roi_left.height);
    
    
    ////////////////////////////////////////////////////////////////////
    
    
    string t_name="";
    int isRight=0,isLeft=0;
    int yminL,yminR,ymaxL,ymaxR;
    get_lines_byme(imgGray, roi_left, t_name, vec_left_points,isLeft,yminL,ymaxL);
   // detectLane(imgGray, roi_left, t_name, vec_left_points,isLeft,yminL,ymaxL);
  //  detectLane(imgGray, roi_right, t_name, vec_right_points,isRight,yminR,ymaxR);

    get_lines_byme(imgGray, roi_right, t_name, vec_right_points,isRight,yminR,ymaxR);
    int deltaY=10;
//cout<<isRight<<" "<<isLeft;
    if(isRight>0&&isLeft>0)
    {
	
        
            if (isLeft==1&&isRight==2&&abs(ymaxL-yminR)<deltaY) {
                cout<<"1"<<endl;
                   center_point.x=vec_left_points[0].x+distance/2+10;
                
                //center_point.x=vec_left_points[0].x+frame_width/2;
                center_point.y=vec_left_points[0].y;
            }
            else if (isLeft==2&&isRight==1&&abs(ymaxL-yminR)<deltaY)
            {
                cout<<"2"<<endl;
                
                center_point.x=vec_right_points[0].x-distance/2-10;
                
              //  center_point.x=vec_right_points[0].x-frame_width/2;
                center_point.y=vec_right_points[0].y;
            }
            else
            {
	double newdistance=vec_right_points[0].x-vec_left_points[0].x;

       // if(newdistance<frame_width*7/24)
       // {
        //    center_point.x=1;
        //    center_point.y=1;
	//	cout<<"fail";
        //}
        //else{
				cout << "3" << endl;
                if(abs(distance-newdistance)<frame_width/4) {
					
                    distance=newdistance;
                    center_point.x=(vec_left_points[0].x+vec_right_points[0].x)/2;
                    center_point.y=(vec_left_points[0].y+vec_right_points[0].y)/2;
                }
          //     }
                
            }
        
    }
   else if(isRight>0&&isLeft==0)
    {
        if(isRight==1){
			cout << "5" << endl;
			if (abs(prvpoint.x - vec_right_points[0].x) < nguong){
				center_point.x = vec_right_points[0].x - distance / 2 - 10;

				//center_point.x=vec_right_points[0].x-frame_width/2;
				center_point.y = vec_right_points[0].y;
			}
        }
        else{
			cout << "6" << endl;
			if (abs(prvpoint.x - vec_right_points[0].x) < nguong){
				center_point.x = vec_left_points[0].x + distance / 2 + 10;

				//center_point.x=vec_right_points[0].x+frame_width/4;
				center_point.y = vec_right_points[0].y;
			}
        }
        
    }
    else
    {
        if(isLeft==1){
            cout<<"7"<<endl;
			if (abs(prvpoint.x - vec_right_points[0].x) < nguong){
				center_point.x = vec_left_points[0].x + distance / 2 + 10;

				//center_point.x=vec_left_points[0].x+frame_width/2;
				center_point.y = vec_left_points[0].y;
			}
        }
        else if(isLeft==2){
            cout<<"8"<<endl;
			if (abs(prvpoint.x - vec_right_points[0].x) < nguong){
				center_point.x = vec_right_points[0].x - distance / 2 + 10;

				//center_point.x=vec_left_points[0].x-frame_width/4;
				center_point.y = vec_left_points[0].y;
			}
        }
      
    }
	cout <<"Tam trong:"<< center_point.x << " " << center_point.y << endl;
    //    if(vec_left_points.size()>0&&vec_right_points.size()>0)
    //    {
    //        center_point.x=(vec_left_points[0].x+vec_right_points[0].x)/2;
    //        center_point.y=(vec_left_points[0].y+vec_right_points[0].y)/2;
    //        // carPoint.x=(vec_left_points[1].x+vec_right_points[1].x)/2;
    //        // carPoint.y=(vec_left_points[1].y+vec_right_points[1].y)/2;
    //    }
    
}
