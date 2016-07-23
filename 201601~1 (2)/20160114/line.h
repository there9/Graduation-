#include <opencv\cv.h>
#include <opencv\highgui.h>

double get_lane_detection(IplImage* org, IplImage* dst);
void line_rgv_to_hsv(IplImage* org);
void line_extract_white(IplImage* src, cv::Mat mat, IplImage* dst);
void cvDetectLine(IplImage* org, IplImage* dst);
/*
int get_angle(IplImage *org, double *left_line, double *right_line);
void draw_standard(IplImage *org);
int get_left_value(IplImage *org, double *left_line);
int get_right_value(IplImage *org, double *right_line);
int get_center_value(IplImage *org, int left, int right);
void draw_line(IplImage *org, int xy[], int rgb[]);
*/