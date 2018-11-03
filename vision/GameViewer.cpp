#include "vision.h"

GameViewer::GameViewer()
{
}

GameViewer::GameViewer(VSSSBuffer<Mat> &view_buffer, int *waitkey_buf)
{
	this->start(view_buffer, waitkey_buf);
}

GameViewer::~GameViewer()
{
	if ((this->frame_show_th).joinable())
	{
		(this->frame_show_th).join();
	}
}

void GameViewer::start(VSSSBuffer<Mat> &view_buffer, int *waitkey_buf)
{
	this->frame_show_th = std::thread(frame_show, &view_buffer, waitkey_buf);
}

void frame_show(VSSSBuffer<Mat> *view_buffer, int *waitkey_buf)
{

	while (*waitkey_buf != 27 /* ESC */)
	{
		Mat local_frame;

		view_buffer->get(local_frame);

		imshow("Game View", local_frame);

		*waitkey_buf = waitKey(1);
	}
}
