#include "opencv2/opencv.hpp"
#include <opencv2/core/mat.hpp>
#include <iostream>
#include <sys/socket.h> 
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <unistd.h> 
#include <string.h>

using namespace cv;

int capDev = 0;

    cv::VideoCapture cap(capDev); // open the default camera
    
int main(int argc, char** argv)
{   
    //OpenCV Code
    //----------------------------------------------------------

    Mat img, imgGray;
    img = Mat::zeros(480 , 640, CV_8UC1);   
     //make it continuous
    if (!img.isContinuous()) {
        img = img.clone();
    }

    int imgSize = img.total() * img.elemSize();
    int bytes = 0;
    int key;
    

    //make img continuos
    if ( ! img.isContinuous() ) { 
          img = img.clone();
          imgGray = img.clone();
    }
        
    std::cout << "Image Size:" << imgSize << std::endl;

    while(1) {
                
            /* get a frame from camera */
                cap >> img;
            
                //do video processing here 
                cvtColor(img, imgGray, cv::COLOR_BGR2GRAY);

    }

}
