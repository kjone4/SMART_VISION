#include "opencv2/opencv.hpp"
#include <iostream>
using namespace cv;
using namespace std;

int main()
{
    /*
    string src = "nvarguscamerasrc sensor-id=0 ! \
	video/x-raw(memory:NVMM), width=(int)640, height=(int)360, \
    format=(string)NV12, framerate=(fraction)30/1 ! \
	nvvidconv flip-method=0 ! video/x-raw, \
	width=(int)640, height=(int)360, format=(string)BGRx ! \
	videoconvert ! video/x-raw, format=(string)BGR ! appsink"; 
    */
    VideoCapture source("/home/jetson/workspace/linedetect_sim/7_lt_ccw_100rpm_in.mp4"); 
    if (!source.isOpened()){ cout << "Camera error" << endl; return -1; }

    string dst0 = "appsrc ! videoconvert ! video/x-raw, format=BGRx ! \
	nvvidconv ! nvv4l2h264enc insert-sps-pps=true ! \
	h264parse ! rtph264pay pt=96 ! \
	udpsink host=203.234.58.167 port=8000 sync=false";
    VideoWriter writer0(dst0,0, (double)30,Size(640,360),true);
    if(!writer0.isOpened()) {cerr<<"Writer open failed!"<<endl; return -1;}


    string dst1 = "appsrc ! videoconvert ! video/x-raw, format=BGRx ! \
	nvvidconv ! nvv4l2h264enc insert-sps-pps=true ! \
	h264parse ! rtph264pay pt=96 ! \
	udpsink host=203.234.58.167 port=8001 sync=false";
    VideoWriter writer1(dst1,0, (double)30,Size(640,360),false);
    if(!writer1.isOpened()) {cerr<<"Writer open failed!"<<endl; return -1;}


    string dst2 = "appsrc ! videoconvert ! video/x-raw, format=BGRx ! \
	nvvidconv ! nvv4l2h264enc insert-sps-pps=true ! \
	h264parse ! rtph264pay pt=96 ! \
	udpsink host=203.234.58.167 port=8002 sync=false";
    VideoWriter writer2(dst2,0, (double)30,Size(640,90),true);
    if(!writer2.isOpened()) {cerr<<"Writer open failed!"<<endl; return -1;}


    Mat frame, gray, thred, cutthred; // 영상 객체
    TickMeter tm;                     // 시간 측정 객체 생성

    Mat labels, stats, centroids;
    int target;
    Point center(320, 45); // 초기 중심점 설정 (하단 영상 중앙)
    int err;// 위치오차

    while (true) {

        tm.start();

        // 프레임 읽기
        source >> frame;
        if (frame.empty()){ cerr << "frame empty!" << endl; break; }

        // 이진화
        cvtColor(frame, gray, COLOR_BGR2GRAY);
        gray += 100 - mean(gray)[0]; // 영상 밝기 조정
        threshold(gray, thred, 0, 255, THRESH_BINARY | THRESH_OTSU);
        thred(Rect(0, 270, 640, 90)).copyTo(cutthred); // 하단 1/4 크기만 추출

        // 레이블링
        int labeling = connectedComponentsWithStats(cutthred, labels, stats, centroids);

        double closest = DBL_MAX;

        target = 0;
        for (int i = 1; i < labeling; ++i) {
            if (stats.at<int>(i,4) < 50) continue; // 작은 영역 제외

            double dx = center.x - centroids.at<double>(i, 0);
            double dy = center.y - centroids.at<double>(i, 1);
            double distance = sqrt(pow(dx, 2) + pow(dy, 2)); // 거리 계산

            if (distance < closest && distance < 75) {
                closest = distance;
                target = i;
            }
        }

        // 새로운 중심점 업데이트
        if (target > 0) {
            center.x = centroids.at<double>(target, 0);
            center.y = centroids.at<double>(target, 1);
        }
        
        // 시각화
        cvtColor(cutthred, cutthred, COLOR_GRAY2BGR);

        for (int i = 1; i < labeling; ++i) {
            int* p = stats.ptr<int>(i);
            if (stats.at<int>(i,4) < 50) continue;
            rectangle(cutthred, Rect(p[0], p[1], p[2], p[3]), Scalar(255, 0, 0), 2);
            if(i == target){
                rectangle(cutthred, Rect(p[0], p[1], p[2], p[3]), Scalar(0, 0, 255), 2);
                err = (cutthred.cols/2) - centroids.at<double>(target, 0);
            }
        }

        // 출력 스트림에 쓰기
        writer0 << frame;    // 컬러 원본
        writer1 << thred;    // 이진화
        writer2 << cutthred; // 하단 1/4 이진화

        waitKey(20); //delay 20으로 하면 프로그램 30 걸림
        tm.stop();
        cout << "err: " << err;
        cout << " time: " << tm.getTimeMilli() << " ms." << endl;
        tm.reset();
       
    }
    return 0;
}
