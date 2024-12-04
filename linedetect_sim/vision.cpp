#include "vision.hpp"

// 하단 1/4 영역의 이진화를 수행하는 함수
Mat processFrame(Mat frame) {
    Mat gray, thred, cutthred;
    cvtColor(frame, gray, COLOR_BGR2GRAY); // 그레이스케일 변환
    gray += 100 - mean(gray)[0]; // 영상 밝기 조정
    threshold(gray, thred, 0, 255, THRESH_BINARY | THRESH_OTSU);// 이진화
    thred(Rect(0, 270, 640, 90)).copyTo(cutthred); // 하단 1/4 크기만 추출
    return cutthred;
}
