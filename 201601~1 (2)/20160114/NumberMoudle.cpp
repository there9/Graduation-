#include "stdafx.h"
#include <math.h>
#include "BlobLabeling.h"
#include "CarInfo.h"
using namespace std;

StringOverCount stringOverCount;
int CarNumberResult(char *number, int count);
int numberTopSame(double *arr);
void Templete_Binary_Conver(IplImage * temp_Number[], IplImage *Number[]);


IplImage * Roi_Image_Cut(IplImage *Roi_Image, CvRect cvRect);
// �ܰ��� ���� �Լ��� ����� �ܰ��� ����ǥ���� �̿��� 
// �ֿܰ� �� 4���� �����Ѵ�.
int find_board(IplImage *image_copy, IplImage *inlabled);

int find_board(IplImage *image_copy, IplImage *inlabeled){
	//��ȣ���� ���� ������ ? ���
	//cvNot(image_copy, image_copy);
	char numberChar[11] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '?' };
	double templete_similar_arr[10] = { 0 };

	IplImage* tempB = 0;
	double min, max;

	char total_CarNumber[10] = { '0' };

	

	CBlobLabeling inblob;
	inblob.SetParam(image_copy, 40);
	inblob.DoLabeling();
	cvCvtColor(image_copy, inlabeled, CV_GRAY2BGR);
	IplImage* roiImage = (IplImage*)cvClone(inlabeled);

	inblob.BlobBigSizeConstraint(60, 80);
	inblob.BlobSmallSizeConstraint(9, 18);
	inblob.Blob_X_SmallSort();
	if (inblob.m_nBlobs>4 && inblob.m_nBlobs<10)
	{
		
		IplImage* temp_Number[10], *pResizeImg[10], *Number[10];
		temp_Number[9] = cvLoadImage("nine.png", 0);
		temp_Number[8] = cvLoadImage("eight.png", 0);
		temp_Number[7] = cvLoadImage("seven.png", 0);
		temp_Number[6] = cvLoadImage("six.png", 0);
		temp_Number[5] = cvLoadImage("five.png", 0);
		temp_Number[4] = cvLoadImage("four.png", 0);
		temp_Number[3] = cvLoadImage("three.png", 0);
		temp_Number[2] = cvLoadImage("two.png", 0);
		temp_Number[1] = cvLoadImage("one.png", 0);
		temp_Number[0] = cvLoadImage("zero.png", 0);

		Templete_Binary_Conver(temp_Number, Number);


		for (int j = 0; j<inblob.m_nBlobs; j++)
		{
			// inblob get
			CvPoint inpt1 = cvPoint(inblob.m_recBlobs[j].x, inblob.m_recBlobs[j].y);
			CvPoint inpt2 = cvPoint(inpt1.x + inblob.m_recBlobs[j].width, inpt1.y + inblob.m_recBlobs[j].height);
			cvDrawRect(inlabeled, inpt1, inpt2, cvScalar(0, 0, 255), 2);
			//���ɿ����� �簢�� ������� �ڸ��� �Լ�

			CvRect tempRect = cvRect(inblob.m_recBlobs[j].x - 2, inblob.m_recBlobs[j].y - 2, inblob.m_recBlobs[j].width + 4, inblob.m_recBlobs[j].height + 4);
			IplImage *capture_resize = Roi_Image_Cut(roiImage, tempRect);
			//���ø� ���� get
			for (int i = 0; i<10; i++)
			{
				pResizeImg[i] = cvCreateImage(cvSize(inblob.m_recBlobs[j].width, inblob.m_recBlobs[j].height), 8, 3);
				cvResize(Number[i], pResizeImg[i]);
				tempB = cvCreateImage(cvSize(capture_resize->width - pResizeImg[i]->width + 1, capture_resize->height - pResizeImg[i]->height + 1), IPL_DEPTH_32F, 1); // �������� ���� �̹���(C)
				cvMatchTemplate(capture_resize, pResizeImg[i], tempB, CV_TM_CCOEFF_NORMED); // �������� ���Ͽ� C �� �׸���.
				cvMinMaxLoc(tempB, &min, &max, NULL, NULL, NULL); // �������� �ִ밪�� ���� ��ġ ã��
				templete_similar_arr[i] = max;
			}
			total_CarNumber[j] = numberChar[numberTopSame(templete_similar_arr)];
			for (int k = 0; k<10; k++)
			{
				cvReleaseImage(&pResizeImg[k]);
			}

		}
		for (int i = 0; i<10; i++)
		{
			cvReleaseImage(&Number[i]);
			cvReleaseImage(&temp_Number[i]);
		}
	}
	cvReleaseImage(&tempB);
	
	return CarNumberResult(total_CarNumber, inblob.m_nBlobs);
}
//���ø� ���絵�� ���� ���� ���
int CarNumberResult(char *number, int count)
{
	string findCarnumber;
	for (int k = 0; k<count; k++)
	{
		if (number[k] != '?'&&count>5)
		{	
			findCarnumber += number[k];
		}
	}
	if (findCarnumber.size() > 5 && findCarnumber.size() < 8)
	{
		if (findCarnumber.size() == 7)
		{
			findCarnumber.erase(2, 1);
		}
		stringOverCount.CheckingCarNumber(findCarnumber);
		if (stringOverCount.NumberCountReturn())
		{
			returnCar.carNumber = stringOverCount.MaxCallCarString();
			stringOverCount.init();
		}
		//cout << returnCar.carNumber << endl;
	}
	return findCarnumber.size();
}
//����ȣ�� ����ϴ� �Լ�
int numberTopSame(double *arr)
{
	double maxNumber = -2;
	int numberindex = 0;
	for (int k = 0; k<10; k++)
	{
		if ((arr[k])>maxNumber)
		{
			maxNumber = arr[k];
			if (maxNumber<0.4)
			{
				numberindex = 10;
			}
			else
			{
				numberindex = k;
			}
		}
	}
	return numberindex;
}
//���ø� ���ڸ� ����ȭ ��Ű�� �Լ�
void Templete_Binary_Conver(IplImage * temp_Number[], IplImage *Number[])
{
	for (int i = 0; i<10; i++)
	{
		Number[i] = cvCreateImage(cvSize(temp_Number[i]->width, temp_Number[i]->height), 8, 3);
		cvThreshold(temp_Number[i], temp_Number[i], 100, 255, CV_THRESH_OTSU);
		//cvAdaptiveThreshold(temp_Number[i], temp_Number[i], 255,CV_ADAPTIVE_THRESH_MEAN_C);
		cvNot(temp_Number[i], temp_Number[i]);
		cvCvtColor(temp_Number[i], Number[i], CV_GRAY2BGR);
	}
	return;
}
//�󺧸��� �κ��� ���ɿ������� �ξ� �ڸ��� �Լ�
IplImage * Roi_Image_Cut(IplImage *Roi_Image, CvRect cvRect)
{
	IplImage*  capture_resize = 0;
	cvSetImageROI(Roi_Image, cvRect);
	capture_resize = cvCreateImage(cvSize(cvRect.width, cvRect.height), 8, 3);
	cvResize(Roi_Image, capture_resize);

	return capture_resize;
}