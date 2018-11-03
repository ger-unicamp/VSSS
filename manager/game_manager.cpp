#include "../vision/vision.h"
#include "../vision/ImageProcessor.hpp"
#include "program_settings.hpp"

//Global variables---------------------------------------------------------------------
VideoCapture capture;
VSSSBuffer<Mat> view_fb;
VSSSBuffer<Mat> proc_fb;
VSSSBuffer<GameState> game_buffer;

int main(int argc, char *argv[])
{
	ProgramSettings settings("vision/calibration/data.json", argc, argv);

	cout << settings.camera_parameters << endl;
	if (system(settings.camera_parameters.c_str()))
	{
		cerr << "ERROR: Could not set camera parameters" << endl;
		exit(EXIT_FAILURE);
	}

	cout << "Opening camera..." << endl;
	capture = VideoCapture(settings.camera_number); // open the first camera

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

	cout << endl
		 << "Press 'ESC' to quit, 'space' to toggle frame processing" << endl;
	cout << endl
		 << "Start grabbing..." << endl;

	int key = 0;
	FrameCapture fc(capture, proc_fb, &key);
	GameViewer gv(view_fb, &key);

	ImageProcessor ip(settings.flip, settings.team_color, settings);
	ip.start(view_fb, proc_fb, game_buffer, &key);

	while (13)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(100));

		if (key == 27 /*ESC*/)
			break;
	}

	return 0;
}
