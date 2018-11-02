#include "vision.h"

FrameCapture::FrameCapture() {

}

FrameCapture::FrameCapture(VideoCapture &capture,
							VSSSBuffer<Mat> &processing_buffer,
							int *waitkey_buf)
{
	this->start(capture, processing_buffer, waitkey_buf);
}

FrameCapture::~FrameCapture() {
	if ((this->frame_capture_th).joinable()) {
		(this->frame_capture_th).join();
	}
}

void FrameCapture::start(VideoCapture &capture,
				VSSSBuffer<Mat> &processing_buffer,
				int *waitkey_buf)
{
	this->frame_capture_th = std::thread(frame_capture, &capture, &processing_buffer, waitkey_buf);
}

void frame_capture(VideoCapture *capture,
				VSSSBuffer<Mat> *processing_buffer,
				int *waitkey_buf)
{

	Mat temp_frame;

	size_t nFrames = 0;
	int64 t0 = cv::getTickCount();

	while (*waitkey_buf != 27 /* ESC */) {

		(*capture) >> temp_frame; // read the next frame from camera

        if (temp_frame.empty())
        {
            cerr << "ERROR: Can't grab camera frame." << endl;
            break;
        }

		processing_buffer->update(temp_frame);

		nFrames++;
		if (nFrames % 100 == 0)
		{
		    const int N = 100;
		    int64 t1 = cv::getTickCount();
		    cout << "Frames captured: " << cv::format("%5lld", (long long int)nFrames)
		         << "    Average FPS: " << cv::format("%9.1f", (double)getTickFrequency() * N / (t1 - t0))
		         << "    Average time per frame: " << cv::format("%9.2f ms", (double)(t1 - t0) * 1000.0f / (N * getTickFrequency()))
		         << std::endl;
		    t0 = t1;
		}

	}
}
