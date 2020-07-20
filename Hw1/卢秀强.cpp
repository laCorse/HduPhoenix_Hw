#include<iostream>
#include<opencv2/opencv.hpp>

#define DEBUG

using namespace cv;
using namespace std;

VideoCapture video("./Hw1/ball.mp4");

void demo1()
{
    Mat frame, hsv, bin;
    char key = 0;
#if !defined(DEBUG)
    double start = static_cast<double>(getTickCount());
#endif
    while(key != 'q')
    {
        video >> frame;
#if defined(DEBUG)
        //验证每帧是否只检测到１个球
        int ball_cnt = 0;
#endif
        if (frame.empty())
        {
            std::cout << "can't read frame!" << std::endl;
            break;
        }

        resize(frame, frame, Size(640, 480));

        cvtColor(frame, hsv, COLOR_BGR2HSV);
        inRange(hsv, Scalar(29,86,6), Scalar(64,255,255), bin);

        vector<vector<Point>> contours;
        findContours(bin, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);



        for (auto contour : contours)
        {
            //区域面积限制
            if (contourArea(contour) < 100)
                continue;
            //获取中心坐标
            RotatedRect rect = minAreaRect(contour);
#if defined(DEBUG)
            cout <<"ball position: " << rect.center << endl;
            //绘制中心
            circle(frame, rect.center, 2, Scalar(0, 0, 255), -1);
            ball_cnt++;
#endif
        }

#if defined(DEBUG)
        //绘制所有轮廓
        drawContours(frame, contours, -1, Scalar(255, 0, 0), 2);
        imshow("bin", bin);
        imshow("frame", frame);
        //报告检测个数情况
        if (ball_cnt == 0) cout << "ball lost!" << endl;
        else if (ball_cnt > 1) cout << "multiple balls detected!" << endl; 
        ball_cnt = 0; 
        cout << "按空格键继续..." << endl;
        key = waitKey(0);
#endif
    }

#if !defined(DEBUG)
    //统计时间开销
    double time = ((double)getTickCount() - start) / getTickFrequency();
    cout << "total processing time:" << time << "s" << endl;
#endif

}



void demo2()
{
    Mat frame, roi, hsv_roi, hsv_frame;
    Mat hist_roi, backpjt;

    /*
    * 手动获取初始追踪框
    */
    video >> frame;
    if (frame.empty())
    {
        std::cout << "can't read frame!" << std::endl;
        return;
    }

    resize(frame, frame, Size(640, 480));
    //手动选取第一帧ROI
    cout << "请手动选取包含球的roi区域!" << endl;
    Rect rect = selectROI(frame);

#if !defined(DEBUG)
    double start = static_cast<double>(getTickCount());
#endif

    roi = frame(rect);
    //获取色调通道图
    cvtColor(roi, hsv_roi, COLOR_BGR2HSV);
    //筛选球的区域
    Mat mask;
    inRange(hsv_roi, Scalar(29,86,6), Scalar(64,255,255), mask);
    //计算球的色调直方图
    int channels[1] = {0};
    int histSize[1] = {180}; 
    float hr[] = {0, 180};
    const float *range[] = {hr};
    calcHist(&hsv_roi, 1, channels, mask, hist_roi, 1, histSize, range, true, false);
    normalize(hist_roi, hist_roi, 0, 255, NORM_MINMAX);

    char key = 0;
    while(key != 'q')
    {   
        video >> frame;
        if (frame.empty())
        {
            std::cout << "can't read frame!" << std::endl;
            break;
        }

        resize(frame, frame, Size(640, 480));     
        cvtColor(frame, hsv_frame, COLOR_BGR2HSV);
        //计算直方图反向投影
        calcBackProject(&hsv_frame, 1, channels, hist_roi, backpjt, range);
        
        //camshift基于meanshift进行改进，使得定位框大小可变
        // meanShift(backpjt, rect, TermCriteria(TermCriteria::COUNT + TermCriteria::EPS, 10, 1));
        RotatedRect trackWindow = CamShift(backpjt, rect, TermCriteria(TermCriteria::COUNT + TermCriteria::EPS, 10, 1));

#if defined(DEBUG)
    Point2f vtx[4];
    trackWindow.points(vtx);
    for (int i = 0; i < 4; i++)
    {
        line(frame, vtx[i], vtx[(i+1)%4], Scalar(0,255,0), 2);
    }
    imshow("back", backpjt);
    imshow("frame", frame);
    cout << "按空格键继续..." << endl;
    key = waitKey(0);
#endif
    }   
#if !defined(DEBUG)
    //统计时间开销
    double time = ((double)getTickCount() - start) / getTickFrequency();
    cout << "total processing time:" << time << "s" << endl;
#endif
}



int main()
{
    if (!video.isOpened())
    {
        std::cout << "can't openc ball.mp4!" << std::endl;
        return -1;
    }

    //使用HSV+查找轮廓
    demo1();
    //使用meanshift/camshift法进行追踪
    // demo2();


    return 0;
}