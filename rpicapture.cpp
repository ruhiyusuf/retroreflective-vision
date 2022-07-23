/**
  @file videowriter_basic.cpp
  @brief A very basic sample for using VideoWriter and VideoCapture
  @author PkLab.net
  @date Aug 24, 2016
*/

#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>
#include <stdio.h>

using namespace cv;
using namespace std;

int main(int, char**)
{
    Mat src;

    // initialize video capture
    VideoCapture cap;
    int deviceID = CAP_V4L2; // 0 = open default camera
    int apiID = CAP_ANY; // 0 = autodetect default API
    int width = 640;
    int height = 480;  

    cap.set(CAP_PROP_FRAME_WIDTH, width);
    cap.set(CAP_PROP_FRAME_HEIGHT, height);

    cap.open(deviceID, apiID);

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

    //--- INITIALIZE VIDEOWRITER
    VideoWriter writer;
		double fps = 30; // frame rate of created video stream
    int codec = VideoWriter::fourcc('H', '2', '6', '4');
    string filename = "live_video.h264"; // name of the output video file

		cout << "About to open VideoWriter stream!" << endl;
    writer.open(filename, codec, fps, src.size(), isColor);
    // writer.open(filename, codec, fps, src.size(), isColor);
		cout << "Write is open!" << endl;
    // check if we succeeded
    if (!writer.isOpened()) {
        cerr << "Could not open the output video file for write\n";
        return -1;
    }

		cout << "We finished initializing video writer!" << endl;

    //--- GRAB AND WRITE LOOP
    cout << "Writing videofile: " << filename << endl
         << "Press any key to terminate" << endl;
    for (;;)
    {
        // check if we succeeded
        if (!cap.read(src)) {
            cerr << "ERROR! blank frame grabbed\n";
            break;
        }
        // encode the frame into the videofile stream
        writer.write(src);
        // show live and wait for a key with timeout long enough to show images
        imshow("Live", src);
        if (waitKey(5) >= 0)
            break;
    }
    // the videofile will be closed and released automatically in VideoWriter destructor
    return 0;
}
