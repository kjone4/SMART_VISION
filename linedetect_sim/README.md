# LINE DETECT 

화면에 보이는 라인을 따라서 추적하는 프로그램

### 코드 설명
```
// 이진화
        cvtColor(frame, gray, COLOR_BGR2GRAY);
        gray += 100 - mean(gray)[0]; // 영상 밝기 조정
        threshold(gray, thred, 0, 255, THRESH_BINARY | THRESH_OTSU);
        thred(Rect(0, 270, 640, 90)).copyTo(cutthred); // 하단 1/4 크기만 추출
```
화면 이진화
