#include <cv.h>
#include <highgui.h>
#include <stdio.h>

int
main (int argc, char **argv)
{
  int x, y;
  int width=1025, height=978;
  IplImage *img=0, *hsv_img=0;
  double c, f;
  
  // (1)allocate and initialize an image
  img = cvCreateImage(cvSize(width, height), IPL_DEPTH_8U, 3);
  if(img==0) return -1;
  cvZero(img);
  hsv_img = cvCloneImage(img);
  cvCvtColor(img, hsv_img, CV_BGR2HSV);
  f = cvGetTickFrequency()*1000;

  // (2)create hue-value gradation image
  c = cvGetTickCount();
  for(y=0; y<height; y++) {
    for(x=0; x<width; x++) {
      int a = img->widthStep*y+(x*3);
      hsv_img->imageData[a+0] = (x*180/width);
      hsv_img->imageData[a+1] = 255;
      hsv_img->imageData[a+2] = ((height-y)*255/height);
    }
  }
  printf("%f\n", (cvGetTickCount()-c)/f);

  cvCvtColor(hsv_img, img, CV_HSV2BGR);

  // (3)show the iamge, and quit when any key pressed
  cvNamedWindow ("Gradation", CV_WINDOW_AUTOSIZE);
  cvShowImage ("Gradation", img);
  cvWaitKey (0);

  cvDestroyWindow("Gradation");
  cvReleaseImage(&img);
  
  return 0;
}
