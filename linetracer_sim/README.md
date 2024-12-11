# LINE TRACER 

화면에 보이는 라인을 따라서 다이나믹셀 제어

### 코드 설명       
       
#### 속도 처리
```
void get_Velocity( int& lvel, int& rvel, int error, double UNDER200, double OVER200){
    
    if( error >= 200  || error <= -200 ){
        lvel= 100 - OVER200*error;
        if(lvel < 0){
            lvel = 10;
        }
        rvel= -(100 + OVER200*error);
        if(rvel > 0){
            rvel = -10;
        }
    }
    else{
        lvel= 100 - UNDER200*error;
        if(lvel < 0){
            lvel = 10;
        }
        rvel= -(100 + UNDER200*error);
        if(rvel > 0){
            rvel = -10;
        }
    }
}
```
1. 에러 값의 범위에 따라서 게인값을 조절(200 이상 = 0.7, 이하 = 0.2)
2. error 값이 커서 부호가 바뀌는 경우 처저속도 10으로 고정
