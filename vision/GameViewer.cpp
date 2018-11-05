#include "vision.h"

GameViewer::GameViewer()
{
}

GameViewer::GameViewer(VSSSBuffer<vector<Mat>> &view_buffer, int *waitkey_buf)
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

void GameViewer::start(VSSSBuffer<vector<Mat>> &view_buffer, int *waitkey_buf)
{
	this->frame_show_th = std::thread(frame_show, &view_buffer, waitkey_buf);
}

void frame_show(VSSSBuffer<vector<Mat>> *view_buffer, int *waitkey_buf)
{

	while (*waitkey_buf != ESC_CHAR)
	{
		vector<Mat> local_frames;

		view_buffer->get(local_frames);

		for (uint i = 0; i < local_frames.size(); i++)
			imshow(std::string("Game View") + to_string(i), local_frames[i]);

		*waitkey_buf = waitKey(1);
	}
}
