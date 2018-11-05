#ifndef vision_h
#define vision_h

#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp> // cv::Canny()
#include <iostream>
#include "opencv2/imgcodecs.hpp"
#include <thread>
#include <mutex>
#include <chrono>
#include <condition_variable>
#include "../strategy/game_state.hpp"
#include "../manager/constants.h"

using namespace cv;
using namespace std;

//Classes------------------------------------------------------------------------------
template <class T>
class VSSSBuffer
{
	bool empty;
	T frame_buffer;
	condition_variable not_empty;
	mutex frame_mtx;

  public:
	VSSSBuffer();
	void update(T const &t);
	void get(T &t);
};

class FrameCapture
{
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

class GameViewer
{
	std::thread frame_show_th;

  public:
	GameViewer();
	GameViewer(VSSSBuffer<vector<Mat>> &view_buffer, int *waitkey_buf);
	~GameViewer();
	void start(VSSSBuffer<vector<Mat>> &view_buffer, int *waitkey_buf);
};

class Color
{
  public:
	int rmin;
	int rmax;
	int gmin;
	int gmax;
	int bmin;
	int bmax;

	Color(int rmin, int rmax, int gmin, int gmax, int bmin, int bmax)
	{
		this->rmin = rmin;
		this->rmax = rmax;
		this->gmin = gmin;
		this->gmax = gmax;
		this->bmin = bmin;
		this->bmax = bmax;
	}
	Color()
	{
		this->rmin = 0;
		this->rmax = 0;
		this->gmin = 0;
		this->gmax = 0;
		this->bmin = 0;
		this->bmax = 0;
	}
};

//Function declarations----------------------------------------------------------------
void frame_capture(VideoCapture *capture, VSSSBuffer<Mat> *processing_buffer, int *waitkey_buf);
void frame_show(VSSSBuffer<vector<Mat>> *view_buffer, int *waitkey_buf);

#endif
