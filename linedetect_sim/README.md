# LINE DETECT 

화면에 보이는 라인을 따라서 추적하는 프로그램

### 코드 설명       

#### 화면 처리
```
// 이진화
       Mat processFrame(Mat frame) {
    Mat gray, thred, cutthred;
    cvtColor(frame, gray, COLOR_BGR2GRAY); // 그레이스케일 변환
    gray += 100 - mean(gray)[0]; // 영상 밝기 조정
    threshold(gray, thred, 0, 255, THRESH_BINARY | THRESH_OTSU);// 이진화
    thred(Rect(0, 270, 640, 90)).copyTo(cutthred); // 하단 1/4 크기만 추출
    return cutthred;
}
```

#### 목표설정, error값
```
// 목표 설정
void get_target(int labeling, Mat stats, Point& center, Mat centroids, double& closest, int& target, int& error, Mat cutthred){
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
            error = (cutthred.cols/2) - centroids.at<double>(target, 0);
        }
}
```

#### 목표 화면에 표시
```
//박스 그리기
void draw_target(int labeling, Mat stats, Mat& cutthred, int target, Point center, Mat centroids){
    for (int i = 1; i < labeling; ++i) {
            int* p = stats.ptr<int>(i);
            if (stats.at<int>(i,4) < 50) continue;
            rectangle(cutthred, Rect(p[0], p[1], p[2], p[3]), Scalar(255, 0, 0), 2);
            if(i == target){
                rectangle(cutthred, Rect(p[0], p[1], p[2], p[3]), Scalar(0, 0, 255), 2);
                circle(cutthred, center, 5, Scalar(0,0,255), -1);
            }
            else if( target == 0 ){
                circle(cutthred, center, 5, Scalar(0,0,255), -1);
                //cout << "line mia" << endl;
            }
        }
}
```
