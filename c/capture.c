#include <cv.h>
#include <highgui.h>

int
main(int argc, char **argv)
{
  CvCapture* cap = 0;
  IplImage* frame = 0;

  // (1)open the default camera
  cap = cvCaptureFromCAM(0);  /** == cap = cvCreateCameraCapture(0); **/


  cvNamedWindow("capture", CV_WINDOW_AUTOSIZE );
  while(1) {
    // (2)get a new frame from camera and show it
    frame = cvQueryFrame( cap );
    cvShowImage("capture", frame);
    if(cvWaitKey(10) >= 0) break;
  }

  cvReleaseCapture(&cap);
  cvDestroyWindow("capture");

  return 0;
}
