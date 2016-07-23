#ifndef CV_BLOB_H
#define CV_BLOB_H


#include <opencv2\opencv.hpp>




class CvBlob{
public:

	enum {CV_BLOB_BLACK = 1, CV_BLOB_WHITE = 2, CV_BLOB_LABEL = 4};
	CvBlob();
	~CvBlob();

	void SetBlob(IplImage *src, int MaxRect);
	int Labeling(IplImage *dst, int method, int max, int mini);
	void Clear();

private:
	bool bFlag;
	uchar *start;
	uchar *pt, *up, *down, *left, *right;
	int cx, cy;
	int x, y;
	int miniCount, maxCount;
	uchar nLabel;
	IplImage *tmpImg;
	int MaxRect;

protected:
	uchar **pointer;
	CvPoint *coord;
	IplImage *src;
public:

	CvRect *rects;
	int width;
	int height;
};

#endif