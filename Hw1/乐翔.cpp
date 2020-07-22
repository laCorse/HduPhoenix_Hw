#include <iostream>  
#include <opencv.hpp>

using namespace cv;
using namespace std;

int hmin = 29, hmax = 64, smin = 86, smax = 255, vmin = 6, vmax = 255;
int g_nStructElementSize = 3;
int g_nGaussianBlurValue = 6;

int main()
{
	VideoCapture cap;
	cap.open("..\\HduPhoenix_Hw\\Hw1\\ball.mp4");

	while (true)
	{
		Mat frame;
		Mat hsv;
		cap >> frame;
		cvtColor(frame, hsv, COLOR_BGR2HSV);

		Mat mask;
		inRange(hsv, Scalar(hmin, smin, vmin), Scalar(hmax, smax, vmax), mask); //二值化
		//imshow("mask", mask);

		Mat erosion;
		Mat element = getStructuringElement(MORPH_RECT, Size(2 * g_nStructElementSize + 1, 2 * g_nStructElementSize + 1), Point(g_nStructElementSize, g_nStructElementSize));
		erode(mask, erosion, element);
		//imshow("腐蚀", erosion);

		Mat gaussian;
		GaussianBlur(erosion, gaussian, Size(g_nGaussianBlurValue * 2 + 1, g_nGaussianBlurValue * 2 + 1), 0, 0);
		//imshow("高斯滤波", gaussian);

		vector<vector<Point> > contours;
		vector<Vec4i> hierarchy;
		Point2f center;
		float radius;
		String x, y;

		findContours(gaussian, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE);
		double maxarea = 0;
		int maxareaidx = 0;


		for (int index = contours.size() - 1; index >= 0; index--) //find the maxarea return contour index
		{
			double tmparea = fabs(contourArea(contours[index]));
			if (tmparea > maxarea)
			{
				maxarea = tmparea;
				maxareaidx = index;
			}
		}
		minEnclosingCircle(contours[maxareaidx], center, radius); //using index searching the min circle,get center & radius

		circle(frame, static_cast<Point>(center), (int)radius, Scalar(0, 0, 255), 3); //using contour index to drawing circle
		x = to_string(int(center.x));
		y = to_string(int(center.y));
		x = "("+x +","+ y+")";
		putText(frame, x, static_cast<Point>(center), FONT_HERSHEY_SIMPLEX, 0.7, (255, 255, 0), 2);

		imshow("轮廓", frame);

		waitKey(1);
	}
	return 0;
}
