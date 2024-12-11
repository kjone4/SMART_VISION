### camera2(11.05)
```
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
```
카메라에서 영상을 받아 오는 코드
```
  writer1 << frame;
  writer2 << gray;
  writer3 << thred;
```
VideoWriter로 영상입력
```
TickMeter tm;
tm.start();
tm.stop();
tm.getTimeMilli()
tm.reset();
```
시간측정

![image](https://github.com/user-attachments/assets/3637ac65-e3da-4353-9c3a-1ada74089a20)

