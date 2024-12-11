### dxl2 다이나믹셀2  

#### 실습과제2  

정지명령(s입력)을 입력했을 때와 프로그램을 종료했을 때(ctrl+c) 바퀴를 손으로 움직일 때 느껴지는 토크 차이  
정지 명령을 입력한 경우는 다이나믹셀이 작동중이기 때문에 계속 정지하기 위해서 제어를 하고 있는 상태이다.  
프로그램을 종료했을 경우에는 다이나믹셀이 제어를 멈추기 때문에 손으로 돌려보면 정지하기 위한 토크가 없는 것을 알 수 있다.  

Ctrl+c를 눌렀을때 실습과제1의 결과와 다른 이유  
키보드를 입력이 발생한 경우에만 해당 동작을 진행시키기 때문에 컨트롤c를 누르면 while문에서 바로 탈출하게 된다.  

다이나믹셀 제어부분
```
        gettimeofday(&start,NULL);
        if (mx.kbhit()) { //키보드입력 체크 
            char c = mx.getch(); //키입력 받기
            switch(c) {
                case 's': vel1 = 0; vel2 = 0; break; //정지
                case 'f': vel1 = 50; vel2 = -50; break; //전진
                case 'b': vel1 = -50; vel2 = 50; break; //후진
                case 'l': vel1 = -50; vel2 = -50; break; //좌회전
                case 'r': vel1 = 50; vel2 = 50; break; //우회전
                default : vel1 = 0; vel2 = 0; break; //정지
            }
            mx.setVelocity(vel1,vel2);
        }
```
