#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace cv;
using namespace std;

int main()
{
	Mat src, hsv_frame, hsv_mask;
	Mat src_resized, hsv_frame_resized, hsv_mask_resized;

  // initialize video capture
  VideoCapture cap;
  int deviceID = CAP_AVFOUNDATION;        // 0 = open default camera
  int apiID = cv::CAP_ANY; // 0 = autodetect default API
  int width = 640; 
  int height = 480;
  int down_width = 640/2; 
  int down_height = 480/2;

  cap.set(CAP_PROP_FRAME_WIDTH, width);
  cap.set(CAP_PROP_FRAME_HEIGHT, height);

  cap.open(deviceID, apiID);
  // check if succeeded in opening camera
  if (!cap.isOpened()) {
    cerr << "ERROR! Unable to open camera\n";
    return -1;
  }
  cout << "Hello I think the camera turned ON!\n"; 
  cout << "Start grabbing, press any key to terminate\n";

	namedWindow("Original Image", WINDOW_NORMAL);
	namedWindow("HSV Frame", WINDOW_NORMAL);
	namedWindow("HSV Mask", WINDOW_NORMAL);

	namedWindow("HSV Values", WINDOW_NORMAL);
	createTrackbar("Low H",  "HSV Values", 0, 179, nullptr);
	createTrackbar("Low S",  "HSV Values", 0, 255, nullptr);
	createTrackbar("Low V",  "HSV Values", 0, 255, nullptr);
	createTrackbar("High H", "HSV Values", 0, 179, nullptr);
	createTrackbar("High S", "HSV Values", 0, 255, nullptr);
	createTrackbar("High V", "HSV Values", 0, 255, nullptr);

	int low_H, low_S, low_V, high_H, high_S, high_V = 0;

  for (;;)
  {
    cap.read(src);

    if (src.empty()) {
      cerr << "ERROR! Blank frame grabbed.\n";
      break;
    }

    // writing hsv image
    cv::cvtColor(src, hsv_frame, cv::COLOR_RGB2HSV);

		low_H = getTrackbarPos("Low H", "HSV Values");
		low_S = getTrackbarPos("Low S", "HSV Values");
		low_V = getTrackbarPos("Low V", "HSV Values");

		high_H = getTrackbarPos("High H", "HSV Values");
		high_S = getTrackbarPos("High S", "HSV Values");
		high_V = getTrackbarPos("High V", "HSV Values");

		// create mask image
    inRange(hsv_frame, Scalar(low_H, low_S, low_V), Scalar(high_H, high_S,
			high_V), hsv_mask);

		// show live and wait for a key with timeout long enough to show images
		resize(src, src_resized, Size(down_width, down_height), INTER_LINEAR);	
		resize(hsv_frame, hsv_frame_resized, Size(down_width, down_height), INTER_LINEAR);	
		resize(hsv_mask, hsv_mask_resized, Size(down_width, down_height), INTER_LINEAR);	
		//imshow("src", src);
		//imshow("hsvframe", hsv_frame);
		//imshow("mask", hsv_mask);
		imshow("Original Image", src_resized);
		imshow("HSV Frame", hsv_frame_resized);
		imshow("HSV Mask", hsv_mask_resized);

    if (waitKey(5) >= 0) break;
  }
  return 0;
}
