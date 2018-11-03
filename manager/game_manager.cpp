#include "../vision/vision.h"
#include "program_settings.hpp"

//Global variables---------------------------------------------------------------------
VideoCapture capture;
VSSSBuffer<Mat> view_fb;
VSSSBuffer<Mat> proc_fb;

int main(int, char **)
{
    Mat frame(480, 854, CV_8UC3);

    ProgramSettings settings("vision/calibration/data.json");
    system(settings.camera_parameters.c_str());

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

    cout << endl
         << "Press 'ESC' to quit, 'space' to toggle frame processing" << endl;
    cout << endl
         << "Start grabbing..." << endl;

    int key = 0;
    FrameCapture fc(capture, proc_fb, &key);
    GameViewer gv(view_fb, &key);

    while (13)
    {
        Mat temp_frame;
        proc_fb.get(temp_frame);
        cv::resize(temp_frame, frame, frame.size());
        view_fb.update(frame);

        if (key == 27 /*ESC*/)
            break;
    }

    return 0;
}
