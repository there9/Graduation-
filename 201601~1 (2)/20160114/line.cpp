#include "line.h"
#include <opencv/cv.h>
#include <opencv/highgui.h>


#define height_ratio 2/3
#define scanning_high_ratio 1/2
#define scanning_low_ratio 6/10

#define PI 3.14159265358979323846
#define THETA_LEFT 0.777343

#define THETA_RIGHT PI-THETA_LEFT
#define RHO_LEFT 270
#define RHO_RIGHT -180

double get_lane_detection(IplImage* org, IplImage* dst);
void set_low_lane_curve(IplImage* org, IplImage* dst, CvPoint *low_right, CvPoint  *high_right);
void set_high_lane_curve(IplImage* org, IplImage* dst, CvPoint *low, CvPoint  *high, bool is_left);
void draw_standard(IplImage *org);
int get_current_curve(IplImage *org,CvPoint *low, CvPoint *high);
double get_current_degree(IplImage *org,CvPoint left, CvPoint right,int index_left, int index_right);
double angle(int x1,int y1,int x2,int y2);

void cvDetectLine(IplImage* org, IplImage* dst);
int get_angle(IplImage *org, double *left_line, double *right_line);
int get_left_value(IplImage *org, double *left_line);
int get_right_value(IplImage *org, double *right_line);
int get_center_value(IplImage *org, int left, int right);
void draw_line(IplImage *org, int xy[], int rgb[]);
void line_rgv_to_hsv(IplImage* src){
	cv::Mat img1(src);
	cvtColor(img1,img1,cv::COLOR_RGB2HSV);
}
void line_extract_white(IplImage* src, cv::Mat mat_dst, IplImage* dst){
	IplImage *temp=cvCloneImage(src);
	cv::Mat mat_src(temp);
	inRange(mat_src, cv::Scalar(0, 0, 160), cv::Scalar(255, 30, 255), mat_dst);
	dst = new IplImage(mat_dst);
}
double get_lane_detection(IplImage* org, IplImage* dst){
	CvPoint low_left[4], high_left[4];
	CvPoint low_right[4], high_right[4]; 
	for(int i=0;i<4;i++){
		int y = org->height*scanning_low_ratio;
		low_left[i].y=y;
		low_left[i].x=-1;
		low_right[i].y=y;
		low_right[i].x=-1;
	}
	for(int j=0;j<4;j++){
		int y = org->height*scanning_high_ratio;
		high_left[j].y=y;
		high_right[j].y=y;
	}
	draw_standard(org);
	set_low_lane_curve(org, dst, low_left, low_right);
	set_high_lane_curve(org, dst, low_left, high_left, true);
	set_high_lane_curve(org, dst, low_right, high_right, false);
	for(int i=0;i<4;i++){
		if(low_left[i].x>0){
		cvCircle(org, cvPoint(low_left[i].x, low_left[i].y), 6, CV_RGB(255, 255, 255),2);
	        cvCircle(org, cvPoint(high_left[i].x, high_left[i].y), 6, CV_RGB(255, 255, 255),2);
	        cvLine(org, cvPoint(low_left[i].x, low_left[i].y), cvPoint(high_left[i].x, high_left[i].y), CV_RGB(255, 255, 255),4);
		}	
	
	}
	



	int index_left = get_current_curve(org, low_left, high_left);
	int index_right = get_current_curve(org,low_right, high_right);
	if(index_left>0){
		cvCircle(org, cvPoint(low_left[index_left].x, low_left[index_left].y), 6, CV_RGB(120, 255, 255),2);
        	cvCircle(org, cvPoint(high_left[index_left].x, high_left[index_left].y), 6, CV_RGB(120, 255, 255),2);
        	cvLine(org, cvPoint(low_left[index_left].x, low_left[index_left].y), cvPoint(high_left[index_left].x, high_left[index_left].y), CV_RGB(120, 255, 255),4);
	}
	if(index_right>0){
		cvCircle(org, cvPoint(low_right[index_right].x, low_right[index_right].y), 6, CV_RGB(120, 255, 255),2);
        	cvCircle(org, cvPoint(high_right[index_right].x, high_right[index_right].y), 6, CV_RGB(120, 255, 255),2);
        	cvLine(org, cvPoint(low_right[index_right].x, low_right[index_right].y), cvPoint(high_right[index_right].x, high_right[index_right].y), CV_RGB(120, 255, 255),4);
	}
	//---------------------------------------------------------------------------------------------------------------

	if(index_right<0&&index_left<0){	
		//return -8000.0;
	}
	else if(index_right<0){
		double temp = angle(low_left[index_left].x, high_left[index_left].y, high_left[index_left].x,low_left[index_left].y);//low_x,low_y,high_x,high_y	
		double theta = THETA_LEFT;
		int rho = RHO_LEFT;
		CvPoint pt1,pt2;
		double a = cos(theta), b = sin(theta);
		double x0 = a*rho, y0 = b*rho;
		pt1.x = cvRound(x0 + 1000*(-b));
		pt1.y = cvRound(y0 + 1000*(a));
		pt2.x = cvRound(x0 - 1000*(-b));
		pt2.y = cvRound(y0 - 1000*(a));
		cvLine(org, pt1, pt2, CV_RGB(0,0,0), 2, 4 );
		temp = temp-THETA_LEFT;
		//return temp*180/PI;

	}
	
	else if(index_left<0){
		double temp = angle(low_right[index_right].x, high_right[index_right].y, high_right[index_right].x,low_right[index_right].y);
		temp = temp-PI/2;
		temp = (THETA_RIGHT-PI/2)-temp;
		
		//return temp*180/PI;

	}
	else{
		
		index_right=0;
		index_left=0;
		double temp_x = (high_left[index_left].x+high_right[index_right].x)/2;
		double temp_y = org->height*scanning_high_ratio;
		
		cvLine(org, cvPoint(high_left[index_left].x, org->height*scanning_high_ratio), cvPoint(high_right[index_right].x, org->height*scanning_high_ratio), CV_RGB(0, 0, 0),4);
		cvLine(org, cvPoint(temp_x, temp_y), cvPoint(org->width/2, org->height), CV_RGB(0, 0, 0),4);
		
		//double temp1;
		//temp1 = angle(low_left[index_left].x,high_left[index_left].y,high_left[index_left].x,low_left[index_left].y);
		//printf("radi : %f degree : %f\n",temp1,temp1*180/PI);
		//return -8000;	
		double temp = PI/2-angle(org->width/2, temp_y, temp_x, org->height);
		printf("%f\n",temp);
		return temp*180/PI;
		
	

	}
	//cvDetectLine(org,dst);
	return -8000;
}
void set_low_lane_curve(IplImage* org, IplImage* dst, CvPoint *low_left, CvPoint  *low_right){
	int center = dst->width/2;
	int i = dst->height*scanning_low_ratio;	
	int count_left = 0;
	int count_right = 0;
	for(int j=center;j>0;j--)
	{
		int index = j+i*dst->widthStep;
		unsigned char n_index = dst->imageData[index];
		if(n_index==255 && count_left<4){
			low_left[count_left].x=j;
			low_left[count_left].y=i;
			count_left++;
		}
	}
	for(int k=center;k<org->width;k++)
	{
		int index = k+i*dst->widthStep;
		unsigned char n_index = dst->imageData[index];
		if(n_index==255 && count_right<4){
			low_right[count_right].x=k;
			low_right[count_right].y=i;
			count_right++;
		}
	}	
}
void set_high_lane_curve(IplImage* org, IplImage* dst, CvPoint *low, CvPoint  *high, bool is_left){
	int x_add;
	if(is_left)
		x_add=1;
	else
		x_add=-1;
	int height = dst->height*scanning_high_ratio;
	if(is_left){
		for(int index=0;index<4;index++){
			if(low[index].x>-1){
				int x=low[index].x;
				int y=low[index].y;
				while(y>height){
					if(dst->imageData[(x+1)+y*dst->widthStep]!=0){
						x=x+1;
					}
					else if(dst->imageData[(x+1)+(y-1)*dst->widthStep]!=0){
						x=x+1;
						y=y-1;
					}
					else if(dst->imageData[x+(y-1)*dst->widthStep]!=0){
						y=y-1;
					}
					else if(dst->imageData[(x-1)+(y-1)*dst->widthStep]!=0){
						x=x-1;
						y=y-1;
					}
					else{
						break;
					}
				}
				high[index].x=x;
				high[index].y=y;
			}
		}
	}
	else{
	for(int index=0;index<4;index++){
			if(low[index].x>-1){
				int x=low[index].x;
				int y=low[index].y;
				while(y>height){
					if(dst->imageData[(x-1)+y*dst->widthStep]!=0){
						x=x-1;
					}
					else if(dst->imageData[(x-1)+(y-1)*dst->widthStep]!=0){
						x=x-1;
						y=y-1;
					}
					else if(dst->imageData[x+(y-1)*dst->widthStep]!=0){
						y=y-1;
					}
					else if(dst->imageData[(x+1)+(y-1)*dst->widthStep]!=0){
						x=x+1;
						y=y-1;
					}
					else{
						break;
					}
				}
				high[index].x=x;
				high[index].y=y;
			}
		}
	}
}
int get_current_curve(IplImage *org,CvPoint *low, CvPoint *high){
	int index = -1;
	int max=10;
	for(int i=3;i>=0;i--){
		if(low[i].x>0){
			int x = abs(low[i].x-high[i].x);
			int y = abs(low[i].y-high[i].y);
			if(y>10){
				x = x*x;
				y = y*y;
				int temp = x+y;
			
				if(max<=temp){
					max = temp;
					index = i;
				}
			}
		}
	}
	
	return index;
}
double angle(int x1,int y1,int x2,int y2)
 {
    double dx,dy,da;

    dx=x2-x1;
    dy=y2-y1;

	if (!dx) dx=1e-6;

    da=atan(dy/dx);

    if (dx<0) da+=PI;

	return da;
 }

void draw_standard(IplImage *org){
	int width = org->width;
	int height = org->height;
    int center_x = width/2;
	int bottom_y = height*height_ratio;
	cvLine(org, cvPoint(center_x, 0), cvPoint(center_x, height), CV_RGB(0, 255, 0));
	cvLine(org, cvPoint(0, bottom_y), cvPoint(width, bottom_y), CV_RGB(0, 255, 0));
	bottom_y = height*scanning_low_ratio;
	cvLine(org, cvPoint(0, bottom_y), cvPoint(width, bottom_y), CV_RGB(0, 255, 0));
	bottom_y = height*scanning_high_ratio;
	cvLine(org, cvPoint(0, bottom_y), cvPoint(width, bottom_y), CV_RGB(0, 255, 0));
	double theta = THETA_LEFT;
	int rho = RHO_LEFT;
	CvPoint pt1,pt2;
	double a = cos(theta), b = sin(theta);
    double x0 = a*rho, y0 = b*rho;
    pt1.x = cvRound(x0 + 1000*(-b));
    pt1.y = cvRound(y0 + 1000*(a));
    pt2.x = cvRound(x0 - 1000*(-b));
    pt2.y = cvRound(y0 - 1000*(a));
    cvLine(org, pt1, pt2, CV_RGB(0,0,0), 2, 4 );
	theta = THETA_RIGHT;
	rho = RHO_RIGHT;
	a= cos(theta), b = sin(theta);
	double x1 = a*rho, y1 = b*rho;
	pt1.x = cvRound(x1 + 1000*(-b));
    pt1.y = cvRound(y1 + 1000*(a));
    pt2.x = cvRound(x1 - 1000*(-b));
    pt2.y = cvRound(y1 - 1000*(a));
    cvLine(org, pt1, pt2, CV_RGB(0,0,0), 2, 4 );
	

}


void cvDetectLine(IplImage* org, IplImage* dst)
{
		CvMemStorage* storage = cvCreateMemStorage(0);
        CvSeq* lines = 0;
		double pi=3.1415926;
        int i;
        lines = cvHoughLines2(dst, storage, CV_HOUGH_STANDARD, 1, CV_PI/180, 50, 0, 0 );
        int number_of_lines = MIN(lines->total,100);
		int count_left=0;
		int count_right=0;
		float rho_left=0.0;
        float theta_left=0.0;
		float rho_right=0.0;
        float theta_right=0.0; 
		//
		for( i = 0; i < MIN(lines->total,100); i++ )
        {
                float* line = (float*)cvGetSeqElem(lines,i);
                float rho = line[0];
				float theta = line[1];
				if(theta>pi/3&&theta<pi*2/3)
					continue;
				if(theta<pi/9 ||theta>pi*8/9)
					continue;
				CvPoint pt1,pt2;
				double a = cos(theta), b = sin(theta);
                double x0 = a*rho, y0 = b*rho;
                pt1.x = cvRound(x0 + 1000*(-b));
                pt1.y = cvRound(y0 + 1000*(a));
                pt2.x = cvRound(x0 - 1000*(-b));
                pt2.y = cvRound(y0 - 1000*(a));

				if(theta<pi/2){//left
					rho_left = rho_left + line[0];
					theta_left = theta_left + line[1];
					count_left++;
				}
				else{
					rho_right =rho_right+line[0];
					theta_right =theta_right+line[1];
					count_right++;
				}
        }
		if(count_right==0||count_left==0){
			return;
		}
		double left_line[2]={0.0};
		double right_line[2]={0.0};
		left_line[0]=rho_left/count_left;
		left_line[1]=theta_left/count_left;
		right_line[0]=rho_right/count_right;
		right_line[1]=theta_right/count_right;
		printf("왼쪽각도: %f,왼쪽 rho : %f, 오른쪽 : %f, 오른쪽 rho : %f\n",left_line[1],left_line[0],right_line[1],right_line[0]);
		int temp = get_angle(org, left_line,right_line);
		
		
}    
int get_angle(IplImage *org, double *left_line, double *right_line){
	int left=0;
	int right=0;
	int result=0;
	left = get_left_value(org, left_line);
	right = get_right_value(org, right_line);
	result = get_center_value(org, left,right);
	return result;
}
int get_left_value(IplImage *org, double *left_line){
	CvPoint pt[2];
	int rho = left_line[0];
	int center_x = org->width/2;
	int bottom_y = org->height*height_ratio;
	int height_gap=40;
	double theta = left_line[1];
	double cosine = cos(theta), sine = sin(theta);
    double x0 = cosine*rho, y0 = sine*rho;
    double a_left = bottom_y-y0;
	double b_left = a_left*tan(theta);
	int result_left_x = cvRound(x0-b_left);
	pt[0].x = cvRound(x0 + 1000*(-sine));
    pt[0].y = cvRound(y0 + 1000*(cosine));
    pt[1].x = cvRound(x0 - 1000*(-sine));
    pt[1].y = cvRound(y0 - 1000*(cosine));
	cvLine(org, pt[0], pt[1], CV_RGB(0,0,255), 2, 4 );
	return result_left_x;
		
}

int get_right_value(IplImage *org, double *right_line){
	CvPoint pt[2];
	int rho = right_line[0];
	int center_x = org->width/2;
	int bottom_y = org->height*height_ratio;
	int height_gap=40;
	double theta = right_line[1];	
	double cosine = cos(theta);		double sine = sin(theta);
    double x0 = cosine*rho;			double y0 = sine*rho;
	double a_right = abs(y0-bottom_y);
	double b_right = a_right*tan(theta);
	int result_right_x = cvRound(x0-b_right);
	double a = cos(theta), b = sin(theta);
    double x = a*rho, y = b*rho;
    pt[0].x = cvRound(x0 + 1000*(-sine));
	pt[0].y = cvRound(y0 + 1000*(cosine));
	pt[1].x = cvRound(x0 - 1000*(-sine));
	pt[1].y = cvRound(y0 - 1000*(cosine));
	cvLine(org, pt[0], pt[1], CV_RGB(0,0,255), 2, 4 );
	cvLine(org, pt[0], pt[1], CV_RGB(0,0,255), 2, 4 );
	
	return result_right_x;	
}
int get_center_value(IplImage *org, int left, int right){
	int center_x = org->width/2;
	int bottom_y = org->height*height_ratio;
	int height_gap = 40;
	int result_gap_center = cvRound((left+right)/2);
	int gap_size = center_x-result_gap_center;
	int xy[4]={0};
	xy[0] = result_gap_center; xy[1] = bottom_y-height_gap; xy[2] = result_gap_center; xy[3] = bottom_y+height_gap;
	int rgb[3]={0};
	rgb[0] = 255; rgb[1] = 255; rgb[2] = 255;
	draw_line(org, xy, rgb);
	CvFont font;
	cvInitFont(&font, CV_FONT_HERSHEY_SIMPLEX, 1.0, 1.0, 0, 1, CV_AA);
	char szText[12];
	if(gap_size>0)
		sprintf(szText, "left %d", abs(gap_size)); 
	else
		sprintf(szText, "right %d", abs(gap_size)); 
	cvPutText(org, szText, cvPoint(10, 130), &font, cvScalar(255, 255, 255, 0));
	return gap_size;
}
void draw_line(IplImage *org, int *xy, int *rgb){
	cvLine(org, cvPoint(xy[0], xy[1]), cvPoint(xy[2], xy[3]), CV_RGB(rgb[0], rgb[1], rgb[2]));
}

