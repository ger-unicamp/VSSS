#ifndef vision_h
#define vision_h

#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>  // cv::Canny()
#include <iostream>
#include "opencv2/imgcodecs.hpp"
#include <highgui.h>
#include <thread>
#include <mutex>
#include <chrono>
#include <condition_variable>

using namespace cv;
using namespace std;

//Classes------------------------------------------------------------------------------
class FrameBuffer {
	bool empty;
	Mat frame_buffer;
	condition_variable not_empty;
	mutex frame_mtx;
	
	public:
	
		FrameBuffer();
		
		void update(const Mat &m);
		
		void get(Mat &m);
};


//Function declarations----------------------------------------------------------------
void frame_reader();
void frame_show();

#endif

