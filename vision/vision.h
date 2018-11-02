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
template <class T> class VSSSBuffer {
	bool empty;
	T frame_buffer;
	condition_variable not_empty;
	mutex frame_mtx;

public:

	VSSSBuffer();
	void update(const T &t);
	void get(T &t);
};

class FrameCapture {
	std::thread frame_capture_th;

public:

	FrameCapture();
	FrameCapture(VideoCapture &capture,
		 		VSSSBuffer<Mat> &processing_buffer,
	  	 		int *waitkey_buf);
	~FrameCapture();
	void start(VideoCapture &capture,
		 		VSSSBuffer<Mat> &processing_buffer,
	  	 		int *waitkey_buf);

};

class GameViewer {
	std::thread frame_show_th;

public:
	GameViewer();
	GameViewer(VSSSBuffer<Mat> &view_buffer, int *waitkey_buf);
	~GameViewer();
	void start(VSSSBuffer<Mat> &view_buffer, int *waitkey_buf);

};


//Function declarations----------------------------------------------------------------
void frame_capture(VideoCapture &capture, VSSSBuffer<Mat> &processing_buffer, int *waitkey_buf);
void frame_show();

#endif
