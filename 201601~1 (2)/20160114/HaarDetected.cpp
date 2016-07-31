#include <opencv2/opencv.hpp>
#include <iostream>
#include "CarInfo.h"
const int KEY_SPACE = 32;
const int KEY_ESC = 27;

CvHaarClassifierCascade *cascade;
CvMemStorage            *storage;


bool detect(IplImage *img);
bool DetectHaarCar(IplImage *img)
{
	
	cascade = (CvHaarClassifierCascade*)cvLoad("test.xml", 0, 0, 0);
	storage = cvCreateMemStorage(0);

	assert(cascade && storage&&img);
	IplImage * frame = cvCreateImage(cvSize(img->width, img->height), img->depth, img->nChannels);
	cvResize(img, frame);
	int key = 0;

	
	if (!frame)
	return false;

	bool isCar = detect(frame);

	//cvDestroyAllWindows();
	cvReleaseImage(&frame);
	cvReleaseHaarClassifierCascade(&cascade);
	cvReleaseMemStorage(&storage);

	return isCar;
}

bool detect(IplImage *img)
{
	IplImage *copyImage = (IplImage*)cvClone(img);
	CvSize img_size = cvSize(img->width, img->height);
	CvSeq *object = cvHaarDetectObjects(
		img,
		cascade,
		storage,
		1.5, //1.1,//1.5, //-------------------SCALE FACTOR
		2, //1        //------------------MIN NEIGHBOURS
		CV_HAAR_DO_CANNY_PRUNING, //CV_HAAR_DO_CANNY_PRUNING
		cvSize(80, 80),//cvSize(300, 300),//, // ------MINSIZE
		 img_size//cvSize(70,70)//cvSize(640,480)  //---------MAXSIZE
		);
	CvRect *r;
	//std::cout << "Total: " << object->total << " cars detected." << std::endl;

	for (int i = 0; i < (object ? object->total : 0); i++)
	{
		
		r = (CvRect*)cvGetSeqElem(object, i);
		cvRectangle(img,
		cvPoint(r->x, r->y),
		cvPoint(r->x + r->width, r->y + r->height),
		CV_RGB(255, 0, 0), 2, 8, 0);
		returnCar.RoiImage = *r;
		returnCar.RoiImage.x = 0;
		returnCar.RoiImage.width = img->width;
		cvSetImageROI(copyImage,*r);
		returnCar.detection_X = r->x + (r->width/2); 
		
	}
	cvShowImage("a", img);
	cvReleaseImage(&copyImage);
	if(object->total>0)
	{
		cvClearSeq(object);
		return true;
	}
	else
	{
		cvClearSeq(object);
		return false;
	}

}