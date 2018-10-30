#include "vision.h"//header

//Global variables---------------------------------------------------------------------
VideoCapture capture;
FrameBuffer view_fb;
FrameBuffer proc_fb;


//main---------------------------------------------------------------------------------
int main(int, char**)
{
    Mat frame(480, 854, CV_8UC3);

    cout << "Opening camera..." << endl;
    capture = VideoCapture(1); // open the first camera

    if (!capture.isOpened())
    {
        cerr << "ERROR: Can't initialize camera capture" << endl;
        return 1;
    }

	capture.set(CAP_PROP_FOURCC, CV_FOURCC('M', 'J', 'P', 'G'));
    capture.set(CAP_PROP_FRAME_WIDTH, 1280);
   	capture.set(CAP_PROP_FRAME_HEIGHT, 720);
	capture.set(CAP_PROP_FPS, 60);


    cout << "Frame width: " << capture.get(CAP_PROP_FRAME_WIDTH) << endl;
    cout << "     height: " << capture.get(CAP_PROP_FRAME_HEIGHT) << endl;
    cout << "Capturing FPS: " << capture.get(CAP_PROP_FPS) << endl;

    cout << endl << "Press 'ESC' to quit, 'space' to toggle frame processing" << endl;
    cout << endl << "Start grabbing..." << endl;

    size_t nFrames = 0;
    bool enableProcessing = false;
    int64 processingTime = 0;
	std::thread thr_frame_reader(frame_reader);
	int key;
	std::thread thr_frame_show(frame_show, &key);
	
    for (;;)
    {
    	Mat temp_frame;
    	proc_fb.get(temp_frame);
		cv::resize(temp_frame, frame, frame.size());
		
        if (key == 27/*ESC*/)
            break;
    }
    
    std::cout << "Number of captured frames: " << nFrames << endl;
    std::terminate();
    return nFrames > 0 ? 0 : 1;
}

//Function implementations-------------------------------------------------------------

FrameBuffer::FrameBuffer() : frame_buffer(480, 854, CV_8UC3) {
	this->empty = true;
}

void FrameBuffer::get(Mat &m) {
	unique_lock<mutex> lck(this->frame_mtx);		
	while(empty) {
		(this->not_empty).wait(lck);
	}
	m = this->frame_buffer;
	empty = true;			
	lck.unlock();
}

void FrameBuffer::update(const Mat &m) {
	unique_lock<mutex> lck(this->frame_mtx);
			
	this->frame_buffer = m;
	empty = false;
	(this->not_empty).notify_one();
			
	lck.unlock();
}

void frame_reader(){

	Mat temp_frame;

	size_t nFrames = 0;
	int64 t0 = cv::getTickCount();
    int64 processingTime = 0;

	for(;;){
		capture >> temp_frame; // read the next frame from camera
		
        if (temp_frame.empty())
        {
            cerr << "ERROR: Can't grab camera frame." << endl;
            break;
        }
        
		view_fb.update(temp_frame);
		proc_fb.update(temp_frame);

		nFrames++;
		if (nFrames % 100 == 0)
		{
		    const int N = 100;
		    int64 t1 = cv::getTickCount();
		    cout << "Frames captured: " << cv::format("%5lld", (long long int)nFrames)
		         << "    Average FPS: " << cv::format("%9.1f", (double)getTickFrequency() * N / (t1 - t0))
		         << "    Average time per frame: " << cv::format("%9.2f ms", (double)(t1 - t0) * 1000.0f / (N * getTickFrequency()))
		         << "    Average processing time: " << cv::format("%9.2f ms", (double)(processingTime) * 1000.0f / (N * getTickFrequency()))
		         << std::endl;
		    t0 = t1;
		    processingTime = 0;
		}

	}
}

void frame_show(int *waitkey_buf) {

	while(1) {
		Mat local_frame;
		
		view_fb.get(local_frame);
		
		imshow("Frame", local_frame);
		
		*waitkey_buf = waitKey(1);
	}
}
