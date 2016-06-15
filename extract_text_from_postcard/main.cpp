#include <iostream>
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

int main(int argc, char * argv[])
{
	string filename = "../images/postcard_1.jpg";
	Mat srcImage = imread(filename);

	// step1, ���ý��������������²���  
	Mat pyrImage;
	pyrDown(srcImage, pyrImage);

	// setp2, ͼ��ҶȻ�
	Mat grayImage;
	cvtColor(pyrImage, grayImage, CV_BGR2GRAY);

	// step3, �˲�
	Mat blurImage;
	GaussianBlur(grayImage, blurImage, Size(3, 3), 0, 0);

	// step4, ��̬ѧ�ݶ�
	Mat gradImage;
	Mat morphKernel = getStructuringElement(MORPH_ELLIPSE, Size(3, 3));
	morphologyEx(blurImage, gradImage, MORPH_GRADIENT, morphKernel);

#ifdef _DEBUG
	imshow("grad", gradImage);
#endif

	// step5, ��ֵ��
	Mat binImage;
	threshold(gradImage, binImage, 0.0, 255.0, THRESH_BINARY | THRESH_OTSU);

	// step6, ˮƽ����
	Mat dilateImage;
	morphKernel = getStructuringElement(MORPH_RECT, Size(9, 1));
	morphologyEx(binImage, dilateImage, MORPH_CLOSE, morphKernel);
#ifdef _DEBUG
	imshow("connected", dilateImage);
#endif

	// step7, ����
	Mat mask = Mat::zeros(binImage.size(), CV_8UC1);
	vector<vector<Point>> contours;
	vector<Vec4i> hierarchy;
	findContours(dilateImage, contours, hierarchy, CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));

	// step8, ���˷��ַ���������
	for (int idx = 0; idx >= 0; idx = hierarchy[idx][0])
	{
		Rect rect = boundingRect(contours[idx]);
		Mat maskROI(mask, rect); 
		maskROI = Scalar(0, 0, 0);

		drawContours(mask, contours, idx, Scalar(255, 255, 255), FILLED);
		// �����������ı���
		double r = (double)countNonZero(maskROI) / (rect.width * rect.height);

		if (r > .45 /* assume at least 45% of the area is filled if it contains text */
			&&
			(rect.height > 8 && rect.width > 8) /* constraints on region size */
			/* these two conditions alone are not very robust. better to use something
			like the number of significant peaks in a horizontal projection as a third condition */
			)
		{
			rectangle(pyrImage, rect, Scalar(0, 255, 0), 2);
		}
	}
	imshow("result", pyrImage);
	waitKey(0);
	return 0;
}