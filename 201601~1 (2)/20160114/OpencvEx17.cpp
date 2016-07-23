#include "stdafx.h"
#include <math.h>
#include "BlobLabeling.h"
using namespace std;

bool DetectHaarCar(IplImage *img);
int find_board(IplImage *image_copy, IplImage *inlabled);
void FindQuad(CvSeq *seq, CvPoint quad[4]);
// ��ȣ���� ���� ���� ������ ���յǴ��� �����ϴ� �Լ�
bool findRect(CvPoint quad[4],int length[4], int former_big_area, int former_small_area);
bool findRect2(CvPoint quad[4],int length[4]);
bool range_filter(CvPoint quad[4],CvPoint quad_predict[4]);


double angle(CvPoint Cen,CvPoint First,CvPoint Second);
// �󺧸��� ��ü �̹������� ���ʺ��� ������� �����Ѵ�.
void limit_frame(CvPoint qaud[4], IplImage *img);


int main()
{
   
  //CvCapture* capture = cvCaptureFromCAM(0);
   CvCapture* capture = cvCaptureFromFile("kkk.mp4");
   //CvCapture ����ü���� Frame�� �޾ƿ� IplImage ����ü ������
   IplImage* image_capture=0;
   IplImage* image_gray=0;      
   IplImage* image_processing=0;      
   IplImage* image_wrap=0;   //����
   IplImage* inlabeled=0;
   IplImage* image_copy_from_processing=0;   
     
   int LowC = 141;
   int HighC = 235;
   int threshold = 130; 
   //����������
   cvNamedWindow( "cam", 0 );
    cvResizeWindow( "cam", 640, 480 );
   
   while(image_capture==NULL)
   {
      cvGrabFrame(capture);
      image_capture = cvRetrieveFrame(capture);
   }
   image_gray = cvCreateImage(cvSize(image_capture->width,image_capture->height), 8, 1);
   image_processing = cvCreateImage(cvSize(image_capture->width,image_capture->height), 8, 1);
   image_wrap = cvCreateImage(cvSize(200, 40), 8, 1);
   image_copy_from_processing = cvCreateImage(cvSize(image_capture->width,image_capture->height), 8, 1);
   inlabeled = cvCreateImage(cvSize(image_wrap->width, image_wrap->height), 8, 3); 
                  
   //printf("%d, %d",image_capture->height,image_capture->width);//480*640
   
   CvMemStorage *storage = cvCreateMemStorage(0);
   CvSeq *contours = NULL;
   CvPoint quad[4] = {0};
   CvPoint quad_predict[4] = {0};
   CvPoint quad_tmp[4] = {0};

   CvPoint2D32f srcPoint[4] = {0};
   CvPoint2D32f dstPoint[4] = {(0, 0), (199, 0), (0, 39), (199, 39)};
   dstPoint[0].x = 0;
   dstPoint[1].x = 199;
   dstPoint[2].x = 0;
   dstPoint[3].x = 199;
   dstPoint[0].y = 0;
   dstPoint[1].y = 0;
   dstPoint[2].y = 39;
   dstPoint[3].y = 39;

   CvMat *warfMat = cvCreateMat(3, 3, CV_32FC1);
   
   int length[4]={0};
   int count=0;
   bool is_left;
   char result;

   while (true)
   {
         cvGrabFrame( capture );
		 image_capture = cvRetrieveFrame(capture);
         
		 if(DetectHaarCar(image_capture)==true)
		 {
		
			 cvCvtColor(image_capture, image_gray, CV_BGR2GRAY);//ȸ��
			 cvCanny(image_gray, image_processing, LowC, HighC); //ĳ�Ͽ���   
			 cvCopy(image_processing,image_copy_from_processing);
			 cvFindContours(image_copy_from_processing, storage, &contours, sizeof(CvContour),CV_RETR_TREE);
			 for(CvSeq *seq = contours; seq != NULL; seq = seq->h_next)
			 {
				 FindQuad(seq, quad);//�� 4�� ���ϱ�   
				 if(findRect2(quad,length))
				 {
					 int count_inner=0;
					 srcPoint[0].x = (float)quad[0].x + 1;
					 srcPoint[0].y = (float)quad[0].y + 1;
					 srcPoint[1].x = (float)quad[1].x - 1;
					 srcPoint[1].y = (float)quad[1].y + 1;
					 srcPoint[2].x = (float)quad[3].x + 1;
					 srcPoint[2].y = (float)quad[3].y - 1;
					 srcPoint[3].x = (float)quad[2].x - 1;
					 srcPoint[3].y = (float)quad[2].y - 1;
					 cvGetPerspectiveTransform(srcPoint, dstPoint, warfMat);

					 cvWarpPerspective(image_gray, image_wrap, warfMat);//��������
					 cvAdaptiveThreshold(image_wrap, image_wrap, 255, CV_ADAPTIVE_THRESH_MEAN_C, CV_THRESH_BINARY, 5,5);

					 count_inner=find_board(image_wrap,inlabeled);

					 if(count_inner>4 && count_inner<10)
					 {
						 int x_len=0;
						 int y_len=0;
						 cvLine(image_capture, quad[0], quad[1], CV_RGB(255, 0, 255));
						 cvLine(image_capture, quad[1], quad[2], CV_RGB(255, 0, 255));
						 cvLine(image_capture, quad[2], quad[3], CV_RGB(255, 0, 255));
						 cvLine(image_capture, quad[3], quad[0], CV_RGB(255, 0, 255));
						 cvCircle(image_capture, quad[0], 5, CV_RGB(255, 0, 0), 2);
						 cvCircle(image_capture, quad[1], 5, CV_RGB(0, 0, 255), 2);
						 cvCircle(image_capture, quad[2], 5, CV_RGB(0, 0, 0), 2);
						 cvCircle(image_capture, quad[3], 5, CV_RGB(255, 255, 255), 2);
						 if(quad[3].y>quad[2].y)is_left=true;
						 else is_left=false;
						 quad_predict[0].x=quad[0].x-20;
						 quad_predict[0].y=quad[0].y-20;
						 quad_predict[1].x=quad[1].x+20;
						 quad_predict[1].y=quad[1].y-20;
						 quad_predict[2].x=quad[2].x+20;
						 quad_predict[2].y=quad[2].y+20;
						 quad_predict[3].x=quad[3].x-20;
						 quad_predict[3].y=quad[3].y+20;
						 for(int i=0;i<4;i++){//tmp�� ������ ����
							 limit_frame(quad_predict, image_capture);
							 quad_tmp[i].x=quad[i].x;
							 quad_tmp[i].y=quad[i].y;
							 //printf("quad_tmp[i].x : %d\n",quad_tmp[i].x);
						 }
					 }                                    
					 cvClearSeq(contours);   
				 }
			 }
			 
		 }
		 cvShowImage("cam", image_capture);

		 if (cv::waitKey(30) >= 0) //wait for 'esc' key press for 30ms. If 'esc' key is pressed, break loop
		 {
			 cout << "esc key is pressed by user"<<  endl;
			 break; 
		 }

   }
   
   cvReleaseCapture(&capture);
   cvReleaseImage(&image_wrap);
   cvReleaseImage(&inlabeled);
   cvReleaseImage(&image_capture);
   cvReleaseImage(&image_gray);
   cvReleaseImage(&image_processing);
   cvReleaseImage(&image_copy_from_processing);
   
   cvDestroyAllWindows();

   return 0;
}

//���� ��� ���� �����ϴ� �Լ�
void limit_frame(CvPoint quad_predict[4], IplImage *img)
{
   for(int i=0; i<4; i++)
   {
      if(quad_predict[i].x < 0)
      {
         quad_predict[i].x =0;
      }
      if(quad_predict[i].y < 0)   
      {
         quad_predict[i].y =0;
      }
      if(quad_predict[i].x > img->width)   
      {
         quad_predict[i].x =img->width;
      }
      if(quad_predict[i].y > img->height)   
      {
         quad_predict[i].y =img->height;
      }
   }
}


bool range_filter(CvPoint quad[4],CvPoint quad_predict[4]){
   if(quad[0].x<quad_predict[0].x)return false;
   if(quad[0].y<quad_predict[0].y)return false;
   if(quad[1].x>quad_predict[1].x)return false;
   if(quad[1].y<quad_predict[1].y)return false;
   if(quad[2].x>quad_predict[2].x)return false;
   if(quad[2].y>quad_predict[2].y)return false;
   if(quad[3].x<quad_predict[3].x)return false;
   if(quad[3].y>quad_predict[3].y)return false;
   
   return true;

}
bool findRect(CvPoint quad[4],int length[4], int former_big_area, int former_small_area){

   int x_len,y_len;//ratio1=���κ��� , ratio2 = ���κ��� ratio ����*���� ����, area �� ����
   int width_ratio,height_ratio;
   double degree[2];
   double degree_result;
   int area = quad[0].x*quad[1].y+quad[1].x*quad[2].y+quad[2].x*quad[0].y      
         -quad[0].x*quad[2].y-quad[2].x*quad[1].y-quad[1].x*quad[0].y;
   if(area<former_small_area)return false;
   if(area>former_big_area)return false;
   
   /*for(int i = 0; i < 4; ++i){
      if(i < 3){
         x_len = quad[i].x - quad[i + 1].x;
         y_len = quad[i].y - quad[i + 1].y;
      }
      else{
         x_len = quad[i].x - quad[0].x;
         y_len = quad[i].y - quad[0].y;
      }
      //������ �������� �Ÿ��� ���Ѵ�.
      length[i] = cvRound(sqrt((double)(x_len * x_len + y_len * y_len)));
      if(length[i]==0)return false;//���������� �Ÿ��� 0�̸� ����
   }
   
   
   if(length[0] > length[2])
      width_ratio = abs(length[0] / length[2]);//���κ���
   else
      width_ratio = abs(length[2] / length[0]);
   if(width_ratio>1.1)return false;
   if(length[1] > length[3])
      height_ratio = abs(length[1] / length[3]);//���κ���
   else
      height_ratio = abs(length[3] / length[1]);
   if(height_ratio>1.2)return false;
   if((double)length[0]/length[1]<1.15)return false;//(����/����)�簢�� ������ ���غ��� ������ ����
   
   degree[0]=angle(quad[0], quad[3], quad[1]);
   degree[1]=angle(quad[2], quad[1], quad[3]);
   degree_result=degree[0]>degree[1]?degree[0]/degree[1]:degree[1]/degree[0];
   if(degree_result>1.2)return false;//�̻��� ��� �簢�� ����
   */
   for(int i = 0; i < 4; ++i){
      if(i < 3){
         x_len = quad[i].x - quad[i + 1].x;
         y_len = quad[i].y - quad[i + 1].y;
      }
      else{
         x_len = quad[i].x - quad[0].x;
         y_len = quad[i].y - quad[0].y;
      }
      //������ �������� �Ÿ��� ���Ѵ�.
      length[i] = cvRound(sqrt((double)(x_len * x_len + y_len * y_len)));
      if(length[i]==0)return false;//���������� �Ÿ��� 0�̸� ����
   }
   
   if(length[0] > length[2])
      width_ratio = abs(length[0] / length[2]);//���κ���
   else
      width_ratio = abs(length[2] / length[0]);
   if(width_ratio>1.3)return false;
   if(length[1] > length[3])
      height_ratio = abs(length[1] / length[3]);//���κ���
   else
      height_ratio = abs(length[3] / length[1]);
   if(height_ratio>1.3)return false;
   if((double)length[0]/length[1]<1.2)return false;//(����/����)�簢�� ������ ���غ��� ������ ����
   
   degree[0]=angle(quad[0], quad[3], quad[1]);
   degree[1]=angle(quad[2], quad[1], quad[3]);
   degree_result=degree[0]>degree[1]?degree[0]/degree[1]:degree[1]/degree[0];
   if(degree_result>1.1)return false;//�̻��� ��� �簢�� ����

   return true;
}
// ��ȣ���� ���� ���� ������ ���յǴ��� �����ϴ� �Լ�
bool findRect2(CvPoint quad[4],int length[4]){

   int x_len,y_len, area;//ratio1=���κ��� , ratio2 = ���κ��� ratio ����*���� ����, area �� ����
   int width_ratio,height_ratio;
   double degree[2];
   double degree_result;
   area = quad[0].x*quad[1].y+quad[1].x*quad[2].y+quad[2].x*quad[0].y      
         -quad[0].x*quad[2].y-quad[2].x*quad[1].y-quad[1].x*quad[0].y;
   if(area<600)return false;//ũ�Ⱑ ������ ����
   for(int i = 0; i < 4; ++i){
      if(i < 3){
         x_len = quad[i].x - quad[i + 1].x;
         y_len = quad[i].y - quad[i + 1].y;
      }
      else{
         x_len = quad[i].x - quad[0].x;
         y_len = quad[i].y - quad[0].y;
      }
      //������ �������� �Ÿ��� ���Ѵ�.
      length[i] = cvRound(sqrt((double)(x_len * x_len + y_len * y_len)));
      if(length[i]==0)return false;//���������� �Ÿ��� 0�̸� ����
   }
   
   if(length[0] > length[2])
      width_ratio = abs(length[0] / length[2]);//���κ���
   else
      width_ratio = abs(length[2] / length[0]);
   if(width_ratio>1.3)return false;
   if(length[1] > length[3])
      height_ratio = abs(length[1] / length[3]);//���κ���
   else
      height_ratio = abs(length[3] / length[1]);
   if(height_ratio>1.3)return false;
   if((double)length[0]/length[1]<1.2)return false;//(����/����)�簢�� ������ ���غ��� ������ ����
   
   degree[0]=angle(quad[0], quad[3], quad[1]);
   degree[1]=angle(quad[2], quad[1], quad[3]);
   degree_result=degree[0]>degree[1]?degree[0]/degree[1]:degree[1]/degree[0];
   if(degree_result>1.1)return false;//�̻��� ��� �簢�� ����
   
   return true;
}

void FindQuad(CvSeq *seq, CvPoint quad[4]){
   int i;
   CvPoint *pt = NULL;//CvPoint�� x,y�� �����ϴ� ����ü
   double length = 0.0;
   double width = 0.0;
   double height = 0.0;
   double maxLen = 0.0;
   CvPoint *st;
   CvPoint **ptArr = new CvPoint*[seq->total];//��ǥ�� ��� ������ ��������

   for(i = 0; i <seq->total; ++i)
      ptArr[i] = (CvPoint*)cvGetSeqElem(seq, i);//������ ��Ҹ� ��ȯ�ϴ� �Լ� cvGetSeqElem()

   st = ptArr[0];   //�ϳ��� ������ �����ϱ�?

   //���������� CvPoint ����Ʈ���� pt�� �� ó�� 
   for(i = 1; i < seq->total; ++i){
      pt = ptArr[i];
      width = st->x - pt->x;//ù��°���� ���������� x�� �� ���̰� ���α���
      height = st->y - pt->y;//���α���

      length = sqrt(width * width + height * height);//width�� height�� �̿� - length�� �밢�� ����

      if(maxLen < length){//�ְ���̸� ã�� �ڵ�
         maxLen = length;
         quad[0] = *pt;//���� �Ÿ��� �� CvPoint �ϳ��� ����
      }
   }
   //���� maxLen�� �ִ� len�� point�� quad�� �Ҵ��� �� ���?   
   //ù��°�� ����, ù��° ������ ���� �Ÿ��� �� �� ������ �� �� �߿��� ���� �Ÿ��� �� point�� ã�� �ڵ�
   maxLen = 0;
   for(i = 0; i < seq->total; ++i){
      pt = ptArr[i];
      width = quad[0].x - pt->x;
      height = quad[0].y - pt->y;

      length = sqrt(width * width + height * height);

      if(maxLen < length){
         maxLen = length;
         quad[1] = *pt;
      }
   }
   
   //ù��°�� ����, ù��° ������ ���� �Ÿ��� �� �� ������ �� 
   //�ű⼭ �� �ϳ��� ������ ���� ������ �� �� �߿��� ���� �Ÿ��� �� point�� ã�� �ڵ�
   maxLen = 0;
   for(i = 0; i < seq->total; ++i){
      pt = ptArr[i];
      width = quad[0].x - pt->x;
      height = quad[0].y - pt->y;
      double width2 = quad[1].x - pt->x;
      double height2 = quad[1].y - pt->y;

      length = sqrt(width * width + height * height) +
         sqrt(width2 *width2 + height2 * height2);
      if(maxLen < length){
         maxLen = length;
         quad[2] = *pt;
      }
   }



   maxLen = 0;
   for(i = 0; i < seq->total; ++i){
      pt = ptArr[i];
      int x1 = quad[0].x, y1 = quad[0].y;
      int x2 = quad[1].x, y2 = quad[1].y;
      int x3 = quad[2].x, y3 = quad[2].y;
      int x4 = pt->x,     y4 = pt->y;

      length = abs(x1 * (y4 - y3) + x4 * (y3 - y1) + x3 * (y1 - y4)) +
            abs(x1 * (y4 - y2) + x4 * (y2 - y1) + x2 * (y1 - y4));

      if(maxLen < length){
         maxLen = length;
         quad[3] = *pt;
      }
   }   
   //quad�� 0~3���� ���鼭 ���� �Ÿ��� ���� �� ���� ���Ѵ�?


   bool restart = true;
   CvPoint tmp;
   while(restart){//y�� ������->ū�� ������ ����
      restart = false;
      for(i = 0; i < 3; ++i){
         if(quad[i].y > quad[i + 1].y){
            tmp = quad[i];
            quad[i] = quad[i + 1];
            quad[i + 1] = tmp;
            restart = true;
         }
      }
   }
   if(quad[0].x > quad[1].x){
      tmp = quad[0];
      quad[0] = quad[1];
      quad[1] = tmp;
   }
   if(quad[2].x < quad[3].x){
      tmp = quad[2];
      quad[2] = quad[3];
      quad[3] = tmp;
   }
   delete[] ptArr;
}

double angle(CvPoint Cen,CvPoint First,CvPoint Second) 
{ 
   double lega1,lega2,legb1,legb2; 
   double norm,norm1,norm2,angle,prod,curl; 
   int x1,y1,x2,y2,x3,y3; 

   x2 = Cen.x; y2 = Cen.y;
   x1 = First.x; y1 = First.y; 
   x3 = Second.x; y3 = Second.y; 

   lega1 = x1-x2; 
   legb1 = y1-y2; 
   lega2 = x3-x2; 
   legb2 = y3-y2; 

   norm1 = sqrt(lega1*lega1+legb1*legb1);//�� ������ ũ�� 
   norm2 = sqrt(lega2*lega2+legb2*legb2);//�� ������ ũ�� 
   norm = norm1 * norm2; 
   prod = (lega1*lega2)+(legb1*legb2);//�� ������ ���� 
   angle = acos(prod/norm); 

   curl = (lega1*legb2)-(legb1*lega2);//�� ������ ���� 

   if (curl<=0) return angle/3.141592654*180; 

   else return (360-angle/3.141592654*180); 
} 