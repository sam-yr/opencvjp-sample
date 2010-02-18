#include <cv.h>
#include <highgui.h>

using namespace cv;

int
main (int argc, char **argv)
{
  // (1)open the default camera
  VideoCapture cap(0);
  if(!cap.isOpened())
    return -1;

  Mat frame;
  namedWindow("capture", CV_WINDOW_AUTOSIZE);
  while(1) {
    // (2)get a new frame from camera and show it
    cap >> frame;
    imshow("capture", frame);
    if(waitKey(10) >= 0) break;
  }

  return 0;
}
