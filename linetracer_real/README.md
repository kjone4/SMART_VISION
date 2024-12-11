### 레인 팔로우

#### 목표 설정
```
void get_target2(int labeling, Mat stats, Point& center2, Mat centroids, double& closest, int& target2, int& error2,Mat cutthred){
    closest = DBL_MAX;
    target2 = 0;
    for (int i = 1; i < labeling; ++i) {
            if (stats.at<int>(i,4) < 50) continue; // 작은 영역 제외

            double dx = center2.x - centroids.at<double>(i, 0);
            double dy = center2.y - centroids.at<double>(i, 1);
            double distance = (pow(dx, 2) + pow(dy, 2)); // 거리 계산

            if (distance < closest && distance < 5625) { // 75*75 = 5625
                closest = distance;
                target2 = i;
            }
        }

        // 새로운 중심점 업데이트
        if (target2 > 0) {
            center2.x = centroids.at<double>(target2, 0);
            center2.y = centroids.at<double>(target2, 1);
            error2 = (cutthred.cols/2) - centroids.at<double>(target2, 0);
        }
}
```
기존 라인 트레이서에서 목표물을 추가하여 2개의 선을 찾도록 개선

#### 제어
```
mid = (error + error2) / 2;
```
2개의 에러값을 통해 따라가야할 중심 계산
