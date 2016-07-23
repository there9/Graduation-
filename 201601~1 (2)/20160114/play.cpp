#include <opencv\cv.h>
#include <opencv\highgui.h>
#include "line.h"
#include "CarInfo.h"
using namespace cv;
void show_cam();
void set_frame_Size();
void set_frame_Zero();	
bool FindCar_NumberModule(IplImage *img);
IplImage *frame_original;//�̹����� �����ϴ� ����
IplImage *frame_copy_for_line;//hsv
IplImage *frame_detect_line;//����
Mat mat_src;
Mat mat_dst;
//CvCapture* capture = cvCaptureFromFile("carnumber1.mp4");//������ ���
CvCapture* capture = cvCaptureFromFile("video3.h264");//������ ���
	
void main ()
{
	//CvCapture* capture = cvCreateFileCapture("testAVI.avi");
	//CvCapture* capture = cvCaptureFromAVI("testAVI.avi");
	
	cvNamedWindow("cam1",1);//����
	//cvNamedWindow("cam2",1);//hsv
	//cvNamedWindow("cam3",1);//����
//-------------------------------------------------------------------------------------------	
	namedWindow("Control", 640); //create a window called "Control"
	int iLowH = 0;
	int iHighH = 255;

	int iLowS = 0; 
	int iHighS = 80;

	int iLowV = 160;
	int iHighV = 255;

	int iLowC =30;
	int iHighC =220;
	 //Create trackbars in "Control" window
	cvCreateTrackbar("LowH", "Control", &iLowH, 179); //Hue (0 - 179)
	cvCreateTrackbar("HighH", "Control", &iHighH, 179);

	cvCreateTrackbar("LowS", "Control", &iLowS, 255); //Saturation (0 - 255)
	cvCreateTrackbar("HighS", "Control", &iHighS, 255);

	cvCreateTrackbar("LowV", "Control", &iLowV, 255); //Value (0 - 255)
	cvCreateTrackbar("HighV", "Control", &iHighV, 255);

	cvCreateTrackbar("iLowC", "Control", &iLowC, 255); //Value (0 - 255)
	cvCreateTrackbar("iHighC", "Control", &iHighC, 255);
	while (frame_original == NULL)
	{
		cvGrabFrame(capture);
		frame_original = cvRetrieveFrame(capture);
	}
//-------------------------------------------------------------------------------------------
	set_frame_Size();
	//----temp------
	//int temp=0;
	//----temp------
	IplImage * s = cvCreateImage(cvSize(500, 500), IPL_DEPTH_8U, 3);
	while(capture) {//������ ��� ����

	
		//returnCar.detection_X = 0;
		
		frame_original = cvQueryFrame(capture);//�����󿡼� �̹��� ����
		cvResize(frame_original, s);
		FindCar_NumberModule(s);
		frame_copy_for_line=cvCloneImage(frame_original);//�����̹����� �����Ͽ� ����
		//cvNot(frame_original,frame_original);//�������
		//hsv��ȯ
		line_rgv_to_hsv(frame_copy_for_line);
		//white ����
		mat_src = cvarrToMat(frame_copy_for_line);//��u��?ȯ?(IplImage -> Mat)
		inRange(mat_src, Scalar(iLowH, iLowS, iLowV), Scalar(iHighH, iHighS, iHighV),  mat_dst);//white �ߩ���a�ب�����?mat_dst��??��u��a
		frame_detect_line = &IplImage(mat_dst);
		//line_extract_white(frame_copy_for_line,mat,frame_detect_line);
		//ħ��-��â ����
		cvErode(frame_detect_line,frame_detect_line);
		cvDilate(frame_detect_line,frame_detect_line); 
		//ĳ�Ͽ���
		cvCanny(frame_detect_line, frame_detect_line, iLowC, iHighC,3); //ĳ�Ͽ���   
		//line �Լ� 
		double result = get_lane_detection(frame_original, frame_detect_line);
		//������ȯ �� ���� ����
		//cvDetectLine(frame_original, frame_detect_line);
		
		show_cam();//ȭ�� �ٿ��
		
		set_frame_Zero();//IplImage �ʱ�ȭ
		//esc���� �� ����
		if(cvWaitKey(33) == 27)
			break;
	}

	//���� �޸� ���� ����
	cvReleaseCapture(&capture);
	cvDestroyWindow("cam");
	cvReleaseImage(&frame_original);	
	cvReleaseImage(&frame_copy_for_line);
	cvReleaseImage(&frame_detect_line);
}
void show_cam(){
	cvShowImage("cam1",frame_original);//����� �̹��� �����쿡 �ٿ��
	//cvShowImage("cam2",frame_copy_for_line);//����� �̹��� �����쿡 �ٿ��
	//cvShowImage("cam3",frame_detect_line);//����� �̹��� �����쿡 �ٿ��
		
}
void set_frame_Size(){
	//frame_original = cvQueryFrame(capture);//�����󿡼� �̹��� ����
	//frame_copy_for_line = cvCreateImage(cvSize(frame_original->width,frame_original->height), 8, 1);
	//frame_detect_line =  cvCreateImage(cvSize(frame_original->width,frame_original->height), 8, 1);;//������ ���ΰ����� ���� ����
	frame_original = cvCreateImage(cvSize( 640, 480), IPL_DEPTH_8U, 1);
	frame_copy_for_line = cvCreateImage(cvSize( 640, 480), IPL_DEPTH_8U, 1);
	frame_detect_line = cvCreateImage(cvSize( 640, 480), IPL_DEPTH_8U, 1);
}
void set_frame_Zero(){
	cvSetZero(frame_original);//�̹����� �����ϴ� ����
	cvSetZero(frame_copy_for_line);//������ ���ΰ����� ���� ����
	cvSetZero(frame_detect_line);
}