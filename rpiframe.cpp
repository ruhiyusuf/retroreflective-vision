#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
#include <stdio.h>
#include <signal.h>

using namespace cv;
using namespace std;

VideoCapture cap;

// Define the function to be called when ctrl-c (SIGINT) is sent to process
void signal_callback_handler (int signum) {
	cout << "Caught signal " << signum << endl;
	cap.release(); 
  exit(signum);
}

int main(int, char**)
{
	Mat src, bwframe, hsvframe, hsvframe_threshold, contourframe;
	int counter;
	vector<vector<Point>> contours;
	vector<Vec4i> hierarchy;

	// values derived from rpistream.cpp and adjustments
	int low_H = 0, low_S = 0, low_V = 238; // min values: (0, 0, 0)
	int high_H = 152, high_S = 255, high_V = 255; // max values: (180, 255, 255)

	// Register signal and signal handler
	signal(SIGINT, signal_callback_handler);

  // initialize video capture
  int deviceID = CAP_V4L2; // 0 = open default camera
  int width = 1920;
  int height = 1080;  
	int exposure = 1;

  cap.set(CAP_PROP_FRAME_WIDTH, width);
  cap.set(CAP_PROP_FRAME_HEIGHT, height);
	// cap.set(CAP_PROP_AUTO_EXPOSURE, 3); // auto mode
	// cap.set(CAP_PROP_AUTO_EXPOSURE, 1); // manual mode
	// cap.set(CAP_PROP_EXPOSURE, exposure); // desired exposure 

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

	cout << "We grabbed a frame from the camera!" << endl;

	cout << "src.size(): " << src.size() << endl;

  //--- GRAB AND WRITE LOOP
  cout << "Capturing and writing frame: " << endl
       << "Press any key to terminate" << endl;

	counter = 0;
  while(true) {
		// check if we succeeded
    if (!cap.read(src)) {
			cerr << "ERROR! blank frame grabbed\n";
			break;
		}
	
		cout << "writing to file" << endl;
		cap >> src;

		// writing src image
		imwrite("./live_src" + to_string(counter) + ".jpg", src);

		// writing black and white image
		cvtColor(src, bwframe, COLOR_BGR2GRAY);
		imwrite("./live_bwframe" + to_string(counter) + ".jpg", bwframe);

		// writing hsv image
		cvtColor(src, hsvframe, COLOR_BGR2HSV);
		imwrite("./live_hsvframe" + to_string(counter) + ".jpg", hsvframe);

		inRange(hsvframe, Scalar(low_H, low_S, low_V), Scalar(high_H, high_S, high_V), \
			hsvframe_threshold);
		imwrite("./live_hsvframe_threshold" + to_string(counter) + ".jpg", \
			hsvframe_threshold);

		threshold(hsvframe_threshold, contourframe, 100, 255, THRESH_BINARY);
		findContours(contourframe, contours, hierarchy, RETR_CCOMP, \
			CHAIN_APPROX_SIMPLE);

		/// Approximate contours to polygons + get bounding rects and circles
		vector<vector<Point> > contours_poly( contours.size() );
		vector<Rect> boundRect( contours.size() );
		vector<Point2f>center( contours.size() );
		vector<float>radius( contours.size() );

		// iterate through all the top-level contours and find bounding box
		cout << "calculating bounding box..." << endl;
		for(int i = 0; i < contours.size(); i++)
		{ 
			approxPolyDP( Mat(contours[i]), contours_poly[i], 3, true );
			boundRect[i] = boundingRect( Mat(contours_poly[i]) );
			minEnclosingCircle( (Mat)contours_poly[i], center[i], radius[i] );
    }

		cout << "drawing contours and bounding boxes..." << endl;
    for(int i = 0; i < contours.size(); i++)
    {
			Scalar color(128, 0, 128);
			// Scalar color( rand()&255, rand()&255, rand()&255 );
			drawContours(contourframe, contours, i, color, 4, LINE_8, hierarchy, 0);
			rectangle(contourframe, boundRect[i].tl(), boundRect[i].br(), color, 2, 8, 0);
			// circle(drawing, center[i], (int)radius[i], color, 2, 8, 0);
    }
		
		imwrite("./live_contourframe" + to_string(counter) + ".jpg", \
			contourframe);

		// show live and wait for a key with timeout long enough to show images
		// imshow("Live", src);
		// if (waitKey(5) > 0) break;
		counter++;
		if (counter % 5 == 0) counter = 0;
  }

  cap.release(); 
  return 0;
}
