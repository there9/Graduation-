#include <opencv\cv.h>
#include <opencv\highgui.h>
#include "line.h"
#include "CarInfo.h"
using namespace cv;
void show_cam();
void set_frame_Size();
void set_frame_Zero();	
bool FindCar_NumberModule(IplImage *img);
IplImage *frame_original;//이미지를 저장하는 변수
IplImage *frame_copy_for_line;//hsv
IplImage *frame_detect_line;//변형
Mat mat_src;
Mat mat_dst;
//CvCapture* capture = cvCaptureFromFile("carnumber1.mp4");//동영상 재생
CvCapture* capture = cvCaptureFromFile("video3.h264");//동영상 재생
	
void main ()
{
	//CvCapture* capture = cvCreateFileCapture("testAVI.avi");
	//CvCapture* capture = cvCaptureFromAVI("testAVI.avi");
	
	cvNamedWindow("cam1",1);//원본
	//cvNamedWindow("cam2",1);//hsv
	//cvNamedWindow("cam3",1);//변형
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
	while(capture) {//동영상 재생 구간

	
		//returnCar.detection_X = 0;
		
		frame_original = cvQueryFrame(capture);//동영상에서 이미지 추출
		cvResize(frame_original, s);
		FindCar_NumberModule(s);
		frame_copy_for_line=cvCloneImage(frame_original);//원본이미지를 복사하여 저장
		//cvNot(frame_original,frame_original);//영상반전
		//hsv변환
		line_rgv_to_hsv(frame_copy_for_line);
		//white 추출
		mat_src = cvarrToMat(frame_copy_for_line);//형u변?환?(IplImage -> Mat)
		inRange(mat_src, Scalar(iLowH, iLowS, iLowV), Scalar(iHighH, iHighS, iHighV),  mat_dst);//white 추ß출a해Ø서品?mat_dst에??저u장a
		frame_detect_line = &IplImage(mat_dst);
		//line_extract_white(frame_copy_for_line,mat,frame_detect_line);
		//침색-팽창 연산
		cvErode(frame_detect_line,frame_detect_line);
		cvDilate(frame_detect_line,frame_detect_line); 
		//캐니엣지
		cvCanny(frame_detect_line, frame_detect_line, iLowC, iHighC,3); //캐니에지   
		//line 함수 
		double result = get_lane_detection(frame_original, frame_detect_line);
		//허프변환 및 라인 검출
		//cvDetectLine(frame_original, frame_detect_line);
		
		show_cam();//화면 뛰우기
		
		set_frame_Zero();//IplImage 초기화
		//esc누를 시 종료
		if(cvWaitKey(33) == 27)
			break;
	}

	//변수 메모리 해제 구간
	cvReleaseCapture(&capture);
	cvDestroyWindow("cam");
	cvReleaseImage(&frame_original);	
	cvReleaseImage(&frame_copy_for_line);
	cvReleaseImage(&frame_detect_line);
}
void show_cam(){
	cvShowImage("cam1",frame_original);//추출된 이미지 윈도우에 뛰우기
	//cvShowImage("cam2",frame_copy_for_line);//추출된 이미지 윈도우에 뛰우기
	//cvShowImage("cam3",frame_detect_line);//추출된 이미지 윈도우에 뛰우기
		
}
void set_frame_Size(){
	//frame_original = cvQueryFrame(capture);//동영상에서 이미지 추출
	//frame_copy_for_line = cvCreateImage(cvSize(frame_original->width,frame_original->height), 8, 1);
	//frame_detect_line =  cvCreateImage(cvSize(frame_original->width,frame_original->height), 8, 1);;//원본을 라인검출을 위해 복사
	frame_original = cvCreateImage(cvSize( 640, 480), IPL_DEPTH_8U, 1);
	frame_copy_for_line = cvCreateImage(cvSize( 640, 480), IPL_DEPTH_8U, 1);
	frame_detect_line = cvCreateImage(cvSize( 640, 480), IPL_DEPTH_8U, 1);
}
void set_frame_Zero(){
	cvSetZero(frame_original);//이미지를 저장하는 변수
	cvSetZero(frame_copy_for_line);//원본을 라인검출을 위해 복사
	cvSetZero(frame_detect_line);
}