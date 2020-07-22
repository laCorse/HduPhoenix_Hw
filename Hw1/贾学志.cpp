#include "opencv2/opencv.hpp"
#include <iostream>
using namespace cv;
using namespace std;

int main()
{
	//VideoCapture capture("C://Users//ThinkPad//Desktop//ball.mp4");
	VideoCapture capture("ball.mp4");
	if (!capture.isOpened())
	{
		printf("can not open ...\n");
		return -6;
	}
	while (1)
	{
		Mat frame, blurimg, hsv, inrange;
		capture >> frame;
		namedWindow("output", WINDOW_NORMAL);
		if (frame.empty())
		{
			break;
		}

		cvtColor(frame, hsv, COLOR_BGR2HSV);
		inRange(hsv, Scalar(29, 86, 6), Scalar(64, 255, 255), inrange);
		blur(inrange, blurimg, Size(5, 5));


		vector<vector<Point> > contours;
		vector<Vec4i> hierarchy;
		Mat imgcontours;
		Point2f center;
		float radius;

		findContours(blurimg, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE);

		/*
			Find the contour with the largest area enclosed //找出所包围的面积最大的轮廓
		*/
		double maxarea = 0;
		int maxareaidx = 0;
		for (int index = contours.size() - 1; index >= 0; index--)
		{
			double tmparea = fabs(contourArea(contours[index]));
			if (tmparea > maxarea)
			{
				maxarea = tmparea;
				maxareaidx = index;
			}
		}

		/*
			Draw the smallest enclosing circle on the selected contour//在选中的轮廓上绘制最小包围圆
		*/
		minEnclosingCircle(contours[maxareaidx], center, radius);
		circle(frame, center, radius, Scalar(255, 255, 0), 5);

		/*
			Output image, coordinates//输出图像，坐标
		*/
		imshow("output", frame);
		cout << center << endl;
		waitKey(10);
	}
	waitKey(0);
	return 0;
}
