#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/aruco.hpp>
#include <opencv2/core/types.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/gapi.hpp>

#include <iostream>

using namespace std;
using namespace cv;

int main(int argc, char* argv[])
{
  cv::VideoCapture inputVideo; //to open video flux
  inputVideo.open(0);
  int frame_width = inputVideo.get(cv::CAP_PROP_FRAME_WIDTH);
  int frame_height = inputVideo.get(cv::CAP_PROP_FRAME_HEIGHT);
  int fps = inputVideo.get(cv::CAP_PROP_FPS);

  cv::VideoWriter outputVideo("appsrc ! videoconvert ! video/x-raw ! x264enc ! rtph264pay ! udpsink host=127.0.0.1 port=5000",cv::CAP_GSTREAMER,0,fps,cv::Size(frame_width,frame_height), true);

  while (inputVideo.grab())
  {
	    cv::Mat image, imageCopy;

      	inputVideo.retrieve(image);
    	outputVideo.write(image);

    	cv::imshow("out", image);
    	char key = (char) cv::waitKey(10);
    	if (key == 27)
        	break;
  }

  inputVideo.release();
  outputVideo.release();

 return 0;
}
