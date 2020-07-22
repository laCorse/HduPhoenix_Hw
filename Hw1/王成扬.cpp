#include<opencv.hpp>
using namespace cv;

#define LEFT 100000
#define RIGHT -10000
#define UP 10000
#define DOWN -10000

Mat frame, hsvFrame, dstFrame;

Vec3i up = { 64,255,255 };

Vec3i low = { 29,86,6 };

int myLeft, myRight, myUp, myDown;

void help()
{
	std::cout << "���ո����֡���ţ����������λ��" << std::endl;
}

void check()
{
	int rows = dstFrame.rows, cols = dstFrame.cols;

	int count = 0;

	myLeft = LEFT;
	myRight = RIGHT;
	myUp = UP;
	myDown = DOWN;
	
	for (int i = 0; i < rows; i++)
	{
		for (int j = 0; j < cols; j++)
		{
			if (dstFrame.at<uchar>(i, j))
			{
				count++;

				myLeft = (j < myLeft) ? j : myLeft;

				myRight = (j > myRight) ? j : myRight;

				myUp = (i < myUp) ? i : myUp;

				myDown = (i > myDown) ? i : myDown;
			}
		}
	}

	if (count)std::cout << "��ǰ֡�������ĵ���������λ��Ϊ " << "(" << (myLeft + myRight) / 2 << "," << (myUp + myDown) / 2 << ")" << std::endl;
	else std::cout << "��ǰ֡û������" << std::endl;
}

int main()
{
	help();

	namedWindow("ԭͼ", WINDOW_NORMAL);

	namedWindow("��ֵ��ͼ��", WINDOW_NORMAL);

	VideoCapture capture("c://users//86152//desktop//HduPhoenix_Hw//Hw1//ball.mp4");

	while (1)
	{
		capture >> frame;

		if (frame.empty())
		{
			std::cout << "��ǰ�Ѿ�Ϊ���һ֡"<<std::endl;
			break;
		}

		cvtColor(frame, hsvFrame, COLOR_BGR2HSV);

		inRange(hsvFrame, low, up, dstFrame);

		check();

		imshow("ԭͼ", frame);

		imshow("��ֵ��ͼ��", dstFrame);

		waitKey(0);
	}

	waitKey(0);

	return 0;

}