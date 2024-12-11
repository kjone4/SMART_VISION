manualdrive 1103  

카메라 입력 영상 저장
w = 속도 50씩 증가  
s = 속도 50씩 뒤로 증가  
d = 우회전  
a = 좌회전  
q = 오른쪽 바퀴 속도 증가(좌회전)  
e = 왼쪽 바퀴 속도 증가(우회전)  
r = 정지

가속 제어
```
        if(goal1>vel1) vel1 += 5;
        else if(goal1<vel1) vel1 -= 5;
        else vel1 = goal1;
        if(goal2>vel2) vel2 += 5;
        else if(goal2<vel2) vel2 -= 5;
        else vel2 = goal2;
```
가속시 5씩 점진적으로 증감하도록 제어
