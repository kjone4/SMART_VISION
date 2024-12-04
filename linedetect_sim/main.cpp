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
    VideoCapture source("/home/jetson/workspace/linedetect_sim/5_lt_cw_100rpm_out.mp4"); 
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

    TickMeter tm;                  // 시간 측정 객체 생성
    signal(SIGINT, ctrlc_handler); //시그널핸들러
    
    // 영상 객체
    Mat frame, cutthred;
    Mat labels, stats, centroids;
    int target; // 따라갈 객체
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
        get_target(labeling, stats, center, centroids, closest, target);
        
        // 시각화
        cvtColor(cutthred, cutthred, COLOR_GRAY2BGR);

        draw_target(labeling, stats, cutthred, target, center, error, centroids);

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

        tm.stop();
        double injury_time = 30 - tm.getTimeMilli(); // 모자란 시간
        if(injury_time < 0){injury_time = -injury_time;}
        else usleep(injury_time * 1000);
        cout << "err: " << error;
        cout << " lvel: " << lvel;
        cout << " rvel: " << rvel;
        cout << " time: " << tm.getTimeMilli() + injury_time << " ms." << endl;
        tm.reset();  
    }
    mx.close(); // 장치닫기
    return 0;
}
