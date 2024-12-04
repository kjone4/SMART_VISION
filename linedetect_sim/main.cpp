#include "vision.hpp"
#include "dxl.hpp"

bool ctrl_c_pressed = false;
void ctrlc_handler(int){ ctrl_c_pressed = true; }

bool mode = false;
double k = 0.15;

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
    ///home/jetson/workspace/linedetect_sim/5_lt_cw_100rpm_out.mp4
    ///home/jetson/workspace/linedetect_sim/7_lt_ccw_100rpm_in.mp4


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

    TickMeter tm;                     // 시간 측정 객체 생성
    signal(SIGINT, ctrlc_handler);
    
    // 영상 객체
    Mat frame, cutthred;
    Mat labels, stats, centroids;
    int target; // 따라갈 선
    Point center(320, 45); // 초기 중심점 설정 (하단 영상 중앙)
    int error;// 위치오차

    // dxl 객체
    Dxl mx;
    int lvel = 0, rvel = 0;
    if(!mx.open()) { cout << "dynamixel open error"<<endl; return -1; }

    while (true) {

        tm.start();

        // 프레임 읽기
        source >> frame;
        if (frame.empty()){ cerr << "frame empty!" << endl; break; }

        // 이진화
        cutthred = processFrame(frame);

        // 레이블링
        int labeling = connectedComponentsWithStats(cutthred, labels, stats, centroids);

        double closest = DBL_MAX;
        target = 0;
        for (int i = 1; i < labeling; ++i) {
            if (stats.at<int>(i,4) < 50) continue; // 작은 영역 제외

            double dx = center.x - centroids.at<double>(i, 0);
            double dy = center.y - centroids.at<double>(i, 1);
            double distance = (pow(dx, 2) + pow(dy, 2)); // 거리 계산

            if (distance < closest && distance < 5625) { // 75*75 = 5625
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
                circle(cutthred, center, 5, Scalar(0,0,255), -1);
                error = (cutthred.cols/2) - centroids.at<double>(target, 0);
            }
            else if( target == 0 ){
                circle(cutthred, center, 5, Scalar(0,0,255), -1);
                //cout << "line mia" << endl;
            }
        }

        // 출력 스트림에 쓰기
        writer0 << frame;    // 컬러 원본
        //writer1 << thred;    // 이진화
        writer2 << cutthred; // 하단 1/4 이진화

        if(mx.kbhit())// 없으면제어멈춤
        { 
            char ch= mx.getch();
            if(ch== 'q') break;
            else if(ch== 's') {
                mode= true;
            }
        }

        lvel= 100 -k*error;
        if(lvel < 0){
            lvel = 10;
        }
        rvel= -(100 + k*error);
        if(mode) {
            mx.setVelocity(lvel, rvel);
        }
        if (ctrl_c_pressed) break; //Ctrl+c입력시 탈출

        waitKey(20);
        tm.stop();
        cout << "err: " << error;
        cout << " lvel: " << lvel;
        cout << " rvel: " << rvel;
        cout << " time: " << tm.getTimeMilli() << " ms." << endl;
        tm.reset();  
    }
    mx.close(); // 장치닫기
    return 0;
}
