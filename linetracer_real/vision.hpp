#ifndef VISION_H
#define VISION_H

#include "opencv2/opencv.hpp"
#include <iostream>
#include <unistd.h>
#include <sys/time.h>
#include <signal.h>

using namespace cv;
using namespace std;

Mat processFrame(Mat frame); // 화면 이진화
void get_target(int labeling, Mat stats, Point& center, Mat centroids, double& closest, int& target, int& error,Mat cutthred);
void get_target2(int labeling, Mat stats, Point& center2, Mat centroids, double& closest, int& target2, int& error2,Mat cutthred);
void draw_target(int labeling, Mat stats, Mat& cutthred, int target, Point center, Mat centroids, int target2, Point center2);
void get_Velocity( int& lvel, int& rvel, int mid, double UNDER200, double OVER200, int RPM);

#endif
