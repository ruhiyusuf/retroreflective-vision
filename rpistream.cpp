#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace cv;
using namespace std;

int main()
{
  Mat frame; 

  // initialize video capture
  VideoCapture cap;
  int deviceID = CAP_V4L2;        // 0 = open default camera
  int apiID = cv::CAP_ANY; // 0 = autodetect default API
  int width = 640;
  int height = 480;

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

  for (;;)
  {
    cap.read(frame);

    if (frame.empty()) {
      cerr << "ERROR! Blank frame grabbed.\n";
      break;
    }
   
    imshow("Live", frame);
    if (waitKey(5) >= 0) break;
  }
  return 0;
}
