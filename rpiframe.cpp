/*
  @file videowriter_basic.cpp
  @brief A very basic sample for using VideoWriter and VideoCapture
  @author PkLab.net
  @date Aug 24, 2016
*/

#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>
#include <iostream>
#include <stdio.h>
#include <signal.h>

using namespace cv;
using namespace std;

VideoCapture cap;

// Define the function to be called when ctrl-c (SIGINT) is sent to process
void signal_callback_handler (int signum) {
   cout << "Caught signal " << signum << endl;
//  	cv::imwrite("./live_frame.bmp", src);
   cap.release(); 
   exit(signum);
}

int main(int, char**)
{
    Mat src;

		// Register signal and signal handler
		signal(SIGINT, signal_callback_handler);

    // initialize video capture
    // int deviceID = 0; // 0 = open default camera
    int deviceID = CAP_V4L2; // 0 = open default camera
    // int width = 640;
    // int height = 480;  

    // cap.set(CAP_PROP_FRAME_WIDTH, width);
    // cap.set(CAP_PROP_FRAME_HEIGHT, height);

    // cap.open(deviceID, apiID);
    cap.open(deviceID);

		// check if we succeeded
    if (!cap.isOpened()) {
        cerr << "ERROR! Unable to open camera\n";
        return -1;
    }

		cout << "We succeeded in opening the camera!" << endl;

    // get one frame from camera to know frame size and type
    cap >> src;
    // check if we succeeded
    if (src.empty()) {
        cerr << "ERROR! blank frame grabbed\n";
        return -1;
    }

    bool isColor = (src.type() == CV_8UC3);

		cout << "We grabbed a frame from the camera!" << endl;

		cout << "isColor: " << isColor << endl;
		cout << "src.size(): " << src.size() << endl;

    //--- GRAB AND WRITE LOOP
    cout << "Writing videofile: " << endl
         << "Press any key to terminate" << endl;
    while(true) {
        // check if we succeeded
        if (!cap.read(src)) {
            cerr << "ERROR! blank frame grabbed\n";
            break;
        }
	cout << "writing to file" << endl;
	cap >> src;
	// encode the frame into the videofile stream
        cv::imwrite("./live_frame.jpg", src);

        // show live and wait for a key with timeout long enough to show images
        // imshow("Live", src);
	if (waitKey(5) > 0) break;
    }

    // the videofile will be closed and released automatically in VideoWriter destructor
		cout << "last write" << endl;
		cv::imwrite("./live_frame.bmp", src);
    cap.release(); 
    return 0;
}
