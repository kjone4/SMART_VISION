#ifndef VISION_H
#define VISION_H

#include "opencv2/opencv.hpp"
#include <iostream>
#include <unistd.h>
#include <sys/time.h>
#include <signal.h>

using namespace cv;
using namespace std;

Mat processFrame(Mat frame);

#endif
