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
void get_target(int labeling, Mat stats, Point& center, Mat centroids, double& closest, int& target);
void draw_target(int labeling, Mat stats, Mat& cutthred, int target, Point center, int& error, Mat centroids);

#endif
