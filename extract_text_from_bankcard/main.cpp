#include <iostream>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;


int main(int argc, char * argv[])
{
	string filename = "../images/bankcard.jpg";
	Mat srcImage = imread(filename);

	// ͼƬ�ü�
	Mat szImage = srcImage(Rect(20, 110, 320, 50));
	imwrite("szImage.jpg", szImage);

	// ͼ��ҶȻ�
	Mat grayImage;
	cvtColor(szImage, grayImage, CV_BGR2GRAY);

	// �˲�
	Mat blurImage;
	GaussianBlur(grayImage, blurImage, Size(3, 3), 0, 0);

	// ��̬ѧ�ݶ�
	Mat gradImage;
	Mat morphKernel = getStructuringElement(MORPH_ELLIPSE, Size(3, 3));
	morphologyEx(blurImage, gradImage, MORPH_GRADIENT, morphKernel);

	// ��ֵ��
	Mat binImage;
	threshold(gradImage, binImage, 0.0, 255.0, THRESH_BINARY | THRESH_OTSU);

	// ˮƽ����
	Mat dilateImage;
	morphKernel = getStructuringElement(MORPH_RECT, Size(12, 1));
	morphologyEx(binImage, dilateImage, MORPH_CLOSE, morphKernel);

	// ����
	Mat mask = Mat::zeros(binImage.size(), CV_8UC1);
	vector<vector<Point>> contours;
	vector<Vec4i> hierarchy;
	findContours(dilateImage, contours, hierarchy, CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));
	
	// ��������
	for (int idx = 0; idx >= 0; idx = hierarchy[idx][0])
	{
		
		Rect rect = boundingRect(contours[idx]);
		if ((rect.area() > 1000) && (rect.width > rect.height))
		{
			rectangle(szImage, rect, Scalar(0, 255, 0), 2);
		}
	}

#ifdef _DEBUG
	imwrite("dst.jpg", szImage);
#endif
	waitKey(0);
	return 0;
}