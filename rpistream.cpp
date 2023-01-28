#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace cv;
using namespace std;

const String window_name_src = "Original Capture";
const String window_name_trackbar = "HSV Values";
const String window_name_hsvframe = "HSV Capture";
const String window_name_hsvmask = "HSV Mask";

const int max_value_H = 360/2;
const int max_value = 255;
int low_H = 0, low_S = 0, low_V = 0;
int high_H = max_value_H, high_S = max_value, high_V = max_value;

static void on_low_H_thresh_trackbar(int, void *)
{
    low_H = min(high_H-1, low_H);
    setTrackbarPos("Low H", window_name_trackbar, low_H);
}

static void on_high_H_thresh_trackbar(int, void *)
{
    high_H = max(high_H, low_H+1);
    setTrackbarPos("High H", window_name_trackbar, high_H);
}

static void on_low_S_thresh_trackbar(int, void *)
{
    low_S = min(high_S-1, low_S);
    setTrackbarPos("Low S", window_name_trackbar, low_S);
}

static void on_high_S_thresh_trackbar(int, void *)
{
    high_S = max(high_S, low_S+1);
    setTrackbarPos("High S", window_name_trackbar, high_S);
}

static void on_low_V_thresh_trackbar(int, void *)
{
    low_V = min(high_V-1, low_V);
    setTrackbarPos("Low V", window_name_trackbar, low_V);
}

static void on_high_V_thresh_trackbar(int, void *)
{
    high_V = max(high_V, low_V+1);
    setTrackbarPos("High V", window_name_trackbar, high_V);
}

std::string gstreamer_pipeline (int capture_width, int capture_height, int display_width, int display_height, int framerate, int flip_method) {
    return "nvarguscamerasrc ! video/x-raw(memory:NVMM), width=(int)" + std::to_string(capture_width) + ", height=(int)" +
           std::to_string(capture_height) + ", framerate=(fraction)" + std::to_string(framerate) +
           "/1 ! nvvidconv flip-method=" + std::to_string(flip_method) + " ! video/x-raw, width=(int)" + std::to_string(display_width) + ", height=(int)" +
           std::to_string(display_height) + ", format=(string)BGRx ! videoconvert ! video/x-raw, format=(string)BGR ! appsink";
}

int main()
{
  Mat src, hsv_frame, hsv_mask;
  Mat src_resized, hsv_frame_resized, hsv_mask_resized;

  // initialize video capture
/*
 * VideoCapture cap;
  int deviceID = CAP_V4L2;        // 0 = open default camera
  int apiID = cv::CAP_ANY; // 0 = autodetect default API
*/
  int width = 640; 
  int height = 480;
  int down_width = 640/2; 
  int down_height = 480/2;

//  cap.set(CAP_PROP_FRAME_WIDTH, width);
//  cap.set(CAP_PROP_FRAME_HEIGHT, height);

 /* int capture_width = 1280 ;
  int capture_height = 720 ;
  int display_width = 1280 ;
  int display_height = 720 ;
*/
  int display_width = 640;
  int display_height = 480;
  int framerate = 30 ;
  int flip_method = 0 ;

  std::string pipeline = gstreamer_pipeline(width,
    height,
    display_width,
    display_height,
    framerate,
    flip_method);
  std::cout << "Using pipeline: \n\t" << pipeline << "\n";
 
  cv::VideoCapture cap(pipeline, cv::CAP_GSTREAMER);
  // check if succeeded in opening camera
  if (!cap.isOpened()) {
    cerr << "ERROR! Unable to open camera\n";
    return -1;
  }
  cout << "Hello I think the camera turned ON!\n"; 
  cout << "Start grabbing, press any key to terminate\n";

	namedWindow(window_name_src, WINDOW_NORMAL);
	namedWindow(window_name_hsvframe, WINDOW_NORMAL);
	namedWindow(window_name_hsvmask, WINDOW_NORMAL);

	namedWindow(window_name_trackbar, WINDOW_NORMAL);
	createTrackbar("Low H",  window_name_trackbar, &low_H,  max_value_H, on_low_H_thresh_trackbar);
	createTrackbar("High H", window_name_trackbar, &high_H, max_value_H, on_high_H_thresh_trackbar);
	createTrackbar("Low S",  window_name_trackbar, &low_S,  max_value, on_low_S_thresh_trackbar);
	createTrackbar("High S", window_name_trackbar, &high_S, max_value, on_high_S_thresh_trackbar);
	createTrackbar("Low V",  window_name_trackbar, &low_V,  max_value, on_low_V_thresh_trackbar);
	createTrackbar("High V", window_name_trackbar, &high_V, max_value, on_high_V_thresh_trackbar);

  for (;;)
  {
    cap.read(src);

    if (src.empty()) {
      cerr << "ERROR! Blank frame grabbed.\n";
      break;
    }

    // writing hsv image
    cv::cvtColor(src, hsv_frame, cv::COLOR_RGB2HSV);

		/*
		low_H = getTrackbarPos("Low H", "HSV Values");
		low_S = getTrackbarPos("Low S", "HSV Values");
		low_V = getTrackbarPos("Low V", "HSV Values");
		high_H = getTrackbarPos("High H", "HSV Values");
		high_S = getTrackbarPos("High S", "HSV Values");
		high_V = getTrackbarPos("High V", "HSV Values");
		*/

		// create mask image
    inRange(hsv_frame, Scalar(low_H, low_S, low_V), Scalar(high_H, high_S,
			high_V), hsv_mask);

		// show live and wait for a key with timeout long enough to show images
		resize(src, src_resized, Size(down_width, down_height), INTER_LINEAR);	
		resize(hsv_frame, hsv_frame_resized, Size(down_width, down_height), INTER_LINEAR);	
		resize(hsv_mask, hsv_mask_resized, Size(down_width, down_height), INTER_LINEAR);	
		imshow("Original Image", src_resized);
		imshow("HSV Frame", hsv_frame_resized);
		imshow("HSV Mask", hsv_mask_resized);

    // if (waitKey(5) >= 0) break;
    char key = (char) waitKey(30);
    if (key == 'q' || key == 27)
    {
			cout << "low_h: " << low_H << endl;
			cout << "low_s: " << low_S << endl;
			cout << "low_v: " << low_V << endl;
			cout << "high_h: " << high_H << endl;
			cout << "high_s: " << high_S << endl;
			cout << "high_v: " << high_V << endl;
    	break;
    }

  }
  return 0;
}
