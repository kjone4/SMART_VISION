#include "opencv2/opencv.hpp"
#include <iostream>
#include<time.h>
using namespace cv;
using namespace std;
int main()
{
    string src = "nvarguscamerasrc sensor-id=0 ! \
	video/x-raw(memory:NVMM), width=(int)640, height=(int)360, \
    format=(string)NV12, framerate=(fraction)30/1 ! \
	nvvidconv flip-method=0 ! video/x-raw, \
	width=(int)640, height=(int)360, format=(string)BGRx ! \
	videoconvert ! video/x-raw, format=(string)BGR ! appsink"; 

    VideoCapture source(src, CAP_GSTREAMER); 
    if (!source.isOpened()){ cout << "Camera error" << endl; return -1; }

    string dst1 = "appsrc ! videoconvert ! video/x-raw, format=BGRx ! \
	nvvidconv ! nvv4l2h264enc insert-sps-pps=true ! \
	h264parse ! rtph264pay pt=96 ! \
	udpsink host=203.234.58.167 port=8541 sync=false";
	
    VideoWriter writer1(dst1,0, (double)30,Size(640,360),true);
    if(!writer1.isOpened()) {cerr<<"Writer open failed!"<<endl; return -1;}

    string dst2 = "appsrc ! videoconvert ! video/x-raw, format=BGRx ! \
	nvvidconv ! nvv4l2h264enc insert-sps-pps=true ! \
	h264parse ! rtph264pay pt=96 ! \
	udpsink host=203.234.58.167 port=3624 sync=false";
	
    VideoWriter writer2(dst2,0, (double)30,Size(640,360),false);
    if(!writer2.isOpened()) {cerr<<"Writer open failed!"<<endl; return -1;}

    string dst3 = "appsrc ! videoconvert ! video/x-raw, format=BGRx ! \
	nvvidconv ! nvv4l2h264enc insert-sps-pps=true ! \
	h264parse ! rtph264pay pt=96 ! \
	udpsink host=203.234.58.167 port=3841 sync=false";

    VideoWriter writer3(dst3,0, (double)30,Size(640,360),false);
    if(!writer3.isOpened()) {cerr<<"Writer open failed!"<<endl; return -1;}

    Mat frame, gray, thred;
    TickMeter tm;	// 객체 생성
    while (true) {
        tm.start();	// 측정 시작
      	source >> frame;
        	if (frame.empty()){ cerr << "frame empty!" << endl; break; }

	    cvtColor(frame, gray, COLOR_BGR2GRAY);
        threshold(gray, thred, 128, 255, THRESH_BINARY);

	    writer1 << frame;
	    writer2 << gray;
        writer3 << thred;
	    waitKey(30);
        tm.stop();	// 측정 종료
        cout << "Image inverse took " << tm.getTimeMilli() << "ms." << endl;
        tm.reset();
    }
    return 0;
}
