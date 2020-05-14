/**
 * @file vision.h
 * @author
 * @brief Header file to FrameCapture and GameViewer
 * @version 0.1
 * @date 2020-05-10
 * 
 * @copyright Copyright (c) 2020
 * 
 */
#ifndef vision_h
#define vision_h

#include <opencv2/opencv.hpp>
#include "opencv2/core/cvdef.h"
#include "opencv2/videoio/videoio_c.h"

#include <iostream>
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
