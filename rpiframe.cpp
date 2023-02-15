#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
#include <stdio.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <chrono>
#include <ctime>

using namespace cv;
using namespace std;

#define PORT		5800
#define MAXLINE 8 
#define DRAW_BB 1

VideoCapture cap;

// Define the function to be called when ctrl-c (SIGINT) is sent to process
void signal_callback_handler (int signum) {
	cout << "Caught signal " << signum << endl;
	cap.release(); 
  exit(signum);
}

static void broadcast(int socket, const char *mess) {
	struct sockaddr_in s;

	memset(&s, '\0', sizeof(struct sockaddr_in));
	s.sin_family = AF_INET;
	s.sin_port = (in_port_t)htons(PORT);
	s.sin_addr.s_addr = htonl(INADDR_BROADCAST);

	if(sendto(socket, mess, strlen(mess), 0, (struct sockaddr *)&s, sizeof(struct sockaddr_in)) < 0)
		perror("sendto");
}

static int init_socket() {
	int sockfd;

	// open a socket
	if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
		perror("socket creation failed");
		exit(EXIT_FAILURE);
	}

	// enable broadcast
	int broadcastEnable = 1;
	if(setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, 
		&broadcastEnable, sizeof(broadcastEnable)) < 0) {
		perror("bcast setting failed");
	}

	return sockfd;
}

std::string create_message(int x, int y) {
	std::string msg = "";
	msg = to_string(x) + ":" + to_string(y); 
	cout << "THE MESSAGE: " << msg << endl;
	return msg;
}

std::string gstreamer_pipeline (int capture_width, int capture_height, int display_width, int display_height, int framerate, int flip_method) {
    return "nvarguscamerasrc ! video/x-raw(memory:NVMM), width=(int)" + std::to_string(capture_width) + ", height=(int)" +
           std::to_string(capture_height) + ", framerate=(fraction)" + std::to_string(framerate) +
           "/1 ! nvvidconv flip-method=" + std::to_string(flip_method) + " ! video/x-raw, width=(int)" + std::to_string(display_width) + ", height=(int)" +
           std::to_string(display_height) + ", format=(string)BGRx ! videoconvert ! video/x-raw, format=(string)BGR ! appsink";
}

int main(int, char**)
{
	Mat src, bwframe, hsvframe, hsvframe_mask, contourframe;
	int counter, max_x, max_y;
	int area_threshold = 500;
	vector<vector<Point>> contours;
	vector<Vec4i> hierarchy;

	// values derived from rpistream.cpp and adjustments
	int low_H = 0, low_S = 254, low_V = 169; // min values: (0, 0, 0)
	int high_H = 56, high_S = 255, high_V = 255; // max values: (180, 255, 255)

	// open UDP socket and enable broadcast
	int sockfd = init_socket();
	std::string msg;
	cout << "Initialized socket!" << endl;

	// check socket; say hello! 
	cout << "Say hello..." << endl;
	// msg = "Hello!";
	msg = create_message(347, 25);
	broadcast(sockfd, msg.c_str());

	// Register signal and signal handler
	signal(SIGINT, signal_callback_handler);

  int width = 320; // 320
  int height = 240; // 240 
  int exposure = 1;
  int display_width = 640;
  int display_height = 480;
  int framerate = 30 ;
  int flip_method = 0 ;

  std::string pipeline = gstreamer_pipeline(width,
    height,
    width,
    height,
    framerate,
    flip_method);
  std::cout << "Using pipeline: \n\t" << pipeline << "\n";

  cv::VideoCapture cap(pipeline, cv::CAP_GSTREAMER);
  // initialize video capture
//  int deviceID = CAP_V4L2; // 0 = open default camera

 // cap.set(CAP_PROP_FRAME_WIDTH, width);
//  cap.set(CAP_PROP_FRAME_HEIGHT, height);
	// cap.set(CAP_PROP_AUTO_EXPOSURE, 3); // auto mode
	// cap.set(CAP_PROP_AUTO_EXPOSURE, 1); // manual mode
	// cap.set(CAP_PROP_EXPOSURE, exposure); // desired exposure 

//  cap.open(deviceID);

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
		// record start time
		auto start = std::chrono::system_clock::now();
		// check if we succeeded
    if (!cap.read(src)) {
			cerr << "ERROR! blank frame grabbed\n";
			break;
		}
	
		// capture source image
		cout << "capturing source image" << endl;
		cap >> src;

		// create black and white image
		cvtColor(src, bwframe, COLOR_BGR2GRAY);

		// create hsv image
		cvtColor(src, hsvframe, COLOR_BGR2HSV);

		// create hsv mask
		inRange(hsvframe, Scalar(low_H, low_S, low_V), Scalar(high_H, high_S, high_V), \
			hsvframe_mask);

		// create contourframe
		contourframe = hsvframe_mask.clone();
		findContours(contourframe, contours, hierarchy, RETR_CCOMP, \
			CHAIN_APPROX_SIMPLE);

		/// Approximate contours to polygons + get bounding rects and circles
		vector<vector<Point> > contours_poly( contours.size() );
		vector<Rect> boundRect( contours.size() );
		vector<Point2f>center( contours.size() );
		vector<float>radius( contours.size() );

		max_x, max_y = 0;

		// iterate through all the top-level contours and find bounding box
		cout << "calculating bounding box..." << endl;
		for(int i = 0; i < contours.size(); i++)
		{ 
			approxPolyDP( Mat(contours[i]), contours_poly[i], 3, true );
			boundRect[i] = boundingRect( Mat(contours_poly[i]) );
			minEnclosingCircle( (Mat)contours_poly[i], center[i], radius[i] );
    }

		int max_area = boundRect[0].area();
		cout << "drawing contours and bounding boxes..." << endl;
    for(int i = 0; i < contours.size(); i++)
    {
			Scalar redcolor(0, 0, 255);
			Scalar graycolor(64, 64, 255);
			if (boundRect[i].area() > area_threshold) {
				// if (boundRect[i].area() > (max_x * max_y))

				// drawContours(contourframe, contours, i, color, 4, LINE_8, hierarchy, 0);
				rectangle(contourframe, boundRect[i].tl(), boundRect[i].br(), graycolor, 2, 8, 0);
				rectangle(src, boundRect[i].tl(), boundRect[i].br(), redcolor, 2, 8, 0);

				// circle(contourframe, center[i], (int)radius[i], color, 2, 8, 0);
				// drawMarker(contourframe, center[i], color, MARKER_DIAMOND, 8, 3);

				Point brcenter = Point(boundRect[i].x + (boundRect[i].width)/2,
					boundRect[i].y + (boundRect[i].height)/2);
				drawMarker(contourframe, brcenter, graycolor, MARKER_STAR, 8, 1);
				drawMarker(src, brcenter, redcolor, MARKER_STAR, 8, 1);

				if (boundRect[i].area() > max_area) {
					max_area = boundRect[i].area();
					max_x = (int)brcenter.x;
					max_y = (int)brcenter.y;
				}
				
				cout << "center[" << counter << "][" << i << "]: " << center[i] << ", "; 
				cout << "brcenter[" << counter << "][" << i << "]: " << brcenter;
				cout << "	x: " << brcenter.x << " y: " << brcenter.y << endl;
				cout << "area[" << counter << "][" << i << "]: " << boundRect[i].area() << endl;
				cout << "max_x: " << max_x << ", max_y: " << max_y << endl;
				cout << "max_area: " << max_area << endl;
			}
    }

		// broadcast max_x, max_y
		msg = create_message(max_x, max_y);
		broadcast(sockfd, msg.c_str());
		auto end = std::chrono::system_clock::now();
		std::chrono::duration<double> diff = end - start; 
		cout << diff.count() << endl;
		// write all images to files
		imwrite("./live_src" + to_string(counter) + ".jpg", src);
		imwrite("./live_bwframe" + to_string(counter) + ".jpg", bwframe);
		imwrite("./live_hsvframe" + to_string(counter) + ".jpg", hsvframe);
		imwrite("./live_hsvframe_mask" + to_string(counter) + ".jpg", hsvframe_mask);
		imwrite("./live_contourframe" + to_string(counter) + ".jpg", contourframe);

		// reset vectors and mat frame
		contours_poly.clear();
		boundRect.clear();
		center.clear();
		radius.clear();
	
		// release Mat frames
		src.release();
		bwframe.release();
		hsvframe.release();
		hsvframe_mask.release();
		contourframe.release();

		// show live and wait for a key with timeout long enough to show images
		// imshow("Live", src);
		// if (waitKey(5) > 0) break;
		counter++;
		if (counter % 5 == 0) counter = 0;
  }

  cap.release(); 
  return 0;
}
