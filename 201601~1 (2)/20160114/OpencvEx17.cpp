#include "stdafx.h"
#include <math.h>
#include "BlobLabeling.h"
using namespace std;

bool DetectHaarCar(IplImage *img);
int find_board(IplImage *image_copy, IplImage *inlabled);
void FindQuad(CvSeq *seq, CvPoint quad[4]);
// 번호판의 가로 세로 비율에 부합되는지 검출하는 함수
bool findRect(CvPoint quad[4],int length[4], int former_big_area, int former_small_area);
bool findRect2(CvPoint quad[4],int length[4]);
bool range_filter(CvPoint quad[4],CvPoint quad_predict[4]);


double angle(CvPoint Cen,CvPoint First,CvPoint Second);
// 라벨링된 객체 이미지들을 왼쪽부터 순서대로 정렬한다.
void limit_frame(CvPoint qaud[4], IplImage *img);


int main()
{
   
  //CvCapture* capture = cvCaptureFromCAM(0);
   CvCapture* capture = cvCaptureFromFile("kkk.mp4");
   //CvCapture 구조체에서 Frame을 받아올 IplImage 구조체 포인터
   IplImage* image_capture=0;
   IplImage* image_gray=0;      
   IplImage* image_processing=0;      
   IplImage* image_wrap=0;   //래핑
   IplImage* inlabeled=0;
   IplImage* image_copy_from_processing=0;   
     
   int LowC = 141;
   int HighC = 235;
   int threshold = 130; 
   //윈도우정의
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
		
			 cvCvtColor(image_capture, image_gray, CV_BGR2GRAY);//회색
			 cvCanny(image_gray, image_processing, LowC, HighC); //캐니에지   
			 cvCopy(image_processing,image_copy_from_processing);
			 cvFindContours(image_copy_from_processing, storage, &contours, sizeof(CvContour),CV_RETR_TREE);
			 for(CvSeq *seq = contours; seq != NULL; seq = seq->h_next)
			 {
				 FindQuad(seq, quad);//점 4개 구하기   
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

					 cvWarpPerspective(image_gray, image_wrap, warfMat);//가져오기
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
						 for(int i=0;i<4;i++){//tmp에 이전값 저장
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

//범위 벗어난 점들 수정하는 함수
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

   int x_len,y_len;//ratio1=가로비율 , ratio2 = 세로비율 ratio 가로*세로 비율, area 는 넓이
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
      //각점과 점사이의 거리를 구한다.
      length[i] = cvRound(sqrt((double)(x_len * x_len + y_len * y_len)));
      if(length[i]==0)return false;//점과점사이 거리가 0이면 제거
   }
   
   
   if(length[0] > length[2])
      width_ratio = abs(length[0] / length[2]);//가로비율
   else
      width_ratio = abs(length[2] / length[0]);
   if(width_ratio>1.1)return false;
   if(length[1] > length[3])
      height_ratio = abs(length[1] / length[3]);//세로비율
   else
      height_ratio = abs(length[3] / length[1]);
   if(height_ratio>1.2)return false;
   if((double)length[0]/length[1]<1.15)return false;//(가로/세로)사각형 비율이 기준보다 낮으면 제거
   
   degree[0]=angle(quad[0], quad[3], quad[1]);
   degree[1]=angle(quad[2], quad[1], quad[3]);
   degree_result=degree[0]>degree[1]?degree[0]/degree[1]:degree[1]/degree[0];
   if(degree_result>1.2)return false;//이상한 모양 사각형 제거
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
      //각점과 점사이의 거리를 구한다.
      length[i] = cvRound(sqrt((double)(x_len * x_len + y_len * y_len)));
      if(length[i]==0)return false;//점과점사이 거리가 0이면 제거
   }
   
   if(length[0] > length[2])
      width_ratio = abs(length[0] / length[2]);//가로비율
   else
      width_ratio = abs(length[2] / length[0]);
   if(width_ratio>1.3)return false;
   if(length[1] > length[3])
      height_ratio = abs(length[1] / length[3]);//세로비율
   else
      height_ratio = abs(length[3] / length[1]);
   if(height_ratio>1.3)return false;
   if((double)length[0]/length[1]<1.2)return false;//(가로/세로)사각형 비율이 기준보다 낮으면 제거
   
   degree[0]=angle(quad[0], quad[3], quad[1]);
   degree[1]=angle(quad[2], quad[1], quad[3]);
   degree_result=degree[0]>degree[1]?degree[0]/degree[1]:degree[1]/degree[0];
   if(degree_result>1.1)return false;//이상한 모양 사각형 제거

   return true;
}
// 번호판의 가로 세로 비율에 부합되는지 검출하는 함수
bool findRect2(CvPoint quad[4],int length[4]){

   int x_len,y_len, area;//ratio1=가로비율 , ratio2 = 세로비율 ratio 가로*세로 비율, area 는 넓이
   int width_ratio,height_ratio;
   double degree[2];
   double degree_result;
   area = quad[0].x*quad[1].y+quad[1].x*quad[2].y+quad[2].x*quad[0].y      
         -quad[0].x*quad[2].y-quad[2].x*quad[1].y-quad[1].x*quad[0].y;
   if(area<600)return false;//크기가 작으면 제거
   for(int i = 0; i < 4; ++i){
      if(i < 3){
         x_len = quad[i].x - quad[i + 1].x;
         y_len = quad[i].y - quad[i + 1].y;
      }
      else{
         x_len = quad[i].x - quad[0].x;
         y_len = quad[i].y - quad[0].y;
      }
      //각점과 점사이의 거리를 구한다.
      length[i] = cvRound(sqrt((double)(x_len * x_len + y_len * y_len)));
      if(length[i]==0)return false;//점과점사이 거리가 0이면 제거
   }
   
   if(length[0] > length[2])
      width_ratio = abs(length[0] / length[2]);//가로비율
   else
      width_ratio = abs(length[2] / length[0]);
   if(width_ratio>1.3)return false;
   if(length[1] > length[3])
      height_ratio = abs(length[1] / length[3]);//세로비율
   else
      height_ratio = abs(length[3] / length[1]);
   if(height_ratio>1.3)return false;
   if((double)length[0]/length[1]<1.2)return false;//(가로/세로)사각형 비율이 기준보다 낮으면 제거
   
   degree[0]=angle(quad[0], quad[3], quad[1]);
   degree[1]=angle(quad[2], quad[1], quad[3]);
   degree_result=degree[0]>degree[1]?degree[0]/degree[1]:degree[1]/degree[0];
   if(degree_result>1.1)return false;//이상한 모양 사각형 제거
   
   return true;
}

void FindQuad(CvSeq *seq, CvPoint quad[4]){
   int i;
   CvPoint *pt = NULL;//CvPoint는 x,y를 저장하는 구조체
   double length = 0.0;
   double width = 0.0;
   double height = 0.0;
   double maxLen = 0.0;
   CvPoint *st;
   CvPoint **ptArr = new CvPoint*[seq->total];//좌표를 모두 저장할 변수선언

   for(i = 0; i <seq->total; ++i)
      ptArr[i] = (CvPoint*)cvGetSeqElem(seq, i);//시퀸스 요소를 반환하는 함수 cvGetSeqElem()

   st = ptArr[0];   //하나는 무조건 있으니까?

   //나머지들은 CvPoint 포인트변수 pt에 서 처리 
   for(i = 1; i < seq->total; ++i){
      pt = ptArr[i];
      width = st->x - pt->x;//첫번째꺼와 나머지꺼의 x를 뺀 차이가 가로길이
      height = st->y - pt->y;//세로길이

      length = sqrt(width * width + height * height);//width와 height를 이용 - length는 대각선 길이

      if(maxLen < length){//최고길이를 찾는 코드
         maxLen = length;
         quad[0] = *pt;//가장 거리가 먼 CvPoint 하나를 저장
      }
   }
   //위에 maxLen은 최대 len의 point를 quad에 할당할 때 사용?   
   //첫번째점 제외, 첫번째 점에서 가장 거리가 먼 점 제외한 뒤 그 중에서 가장 거리가 먼 point를 찾는 코드
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
   
   //첫번째점 제외, 첫번째 점에서 가장 거리가 먼 점 제외한 뒤 
   //거기서 또 하나를 제외한 점을 제외한 뒤 그 중에서 가장 거리가 먼 point를 찾는 코드
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
   //quad는 0~3으로 가면서 서로 거리가 가장 먼 점을 구한다?


   bool restart = true;
   CvPoint tmp;
   while(restart){//y를 작은것->큰것 순으로 정렬
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

   norm1 = sqrt(lega1*lega1+legb1*legb1);//두 벡터의 크기 
   norm2 = sqrt(lega2*lega2+legb2*legb2);//두 벡터의 크기 
   norm = norm1 * norm2; 
   prod = (lega1*lega2)+(legb1*legb2);//두 벡터의 내적 
   angle = acos(prod/norm); 

   curl = (lega1*legb2)-(legb1*lega2);//두 벡터의 외적 

   if (curl<=0) return angle/3.141592654*180; 

   else return (360-angle/3.141592654*180); 
} 