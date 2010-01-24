#include <cv.h>
#include <highgui.h>
#include <stdio.h>

IplImage *src_img = 0, *disp_img, *bin_img;
CvMat *xsum, *ysum;

void on_trackbar1(int val);

int
main(int argc, char *argv[])
{
  const char *imagename;
  int level = 125;
  CvPoint xloc, yloc;

  imagename = argc > 1 ? argv[1] : "winter.png";
  src_img = cvLoadImage(imagename, CV_LOAD_IMAGE_COLOR);
  if(src_img == 0)
    return -1;
  disp_img = cvCloneImage(src_img);
  bin_img = cvCreateImage(cvGetSize(src_img), IPL_DEPTH_8U, 1);
  cvCvtColor(src_img, bin_img, CV_RGB2GRAY);

  xsum = cvCreateMat(1, bin_img->width, CV_32FC1);
  ysum = cvCreateMat(bin_img->height, 1, CV_32FC1);

  cvSmooth(bin_img, bin_img, CV_GAUSSIAN, 5, 0, 0, 0);
  cvThreshold(bin_img, bin_img, 125, 255, CV_THRESH_BINARY_INV);

  cvReduce(bin_img, xsum, -1, CV_REDUCE_SUM);
  cvReduce(bin_img, ysum, -1, CV_REDUCE_SUM);

  cvMinMaxLoc(xsum, NULL, NULL, NULL, &xloc, NULL);
  cvMinMaxLoc(ysum, NULL, NULL, NULL, &yloc, NULL);
  printf("(%d, %d)\n", xloc.x, yloc.y);

  cvCircle(disp_img, cvPoint(xloc.x, yloc.y), 10, CV_RGB(255, 0, 0), 2, 8, 0);

  cvNamedWindow("Image", CV_WINDOW_AUTOSIZE);
  cvCreateTrackbar("Trackbar1", "Image", &level, 255, on_trackbar1);
  cvShowImage("Image", disp_img);
  cvNamedWindow("BinImage", CV_WINDOW_AUTOSIZE);
  cvShowImage("BinImage", bin_img);
  cvWaitKey(0);

  cvDestroyWindow("Threshold");
  cvReleaseImage(&src_img);
  cvReleaseImage(&bin_img);

  return 0;
}

void
on_trackbar1(int val)
{
  CvPoint xloc, yloc;

  cvCvtColor(src_img, bin_img, CV_RGB2GRAY);
  cvThreshold(bin_img, bin_img, val, 255, CV_THRESH_BINARY_INV);

  cvReduce(bin_img, xsum, -1, CV_REDUCE_SUM);
  cvReduce(bin_img, ysum, -1, CV_REDUCE_SUM);

  cvMinMaxLoc(xsum, NULL, NULL, NULL, &xloc, NULL);
  cvMinMaxLoc(ysum, NULL, NULL, NULL, &yloc, NULL);
  printf("(%d, %d)\n", xloc.x, yloc.y);

  cvCopy(src_img, disp_img, NULL);
  cvCircle(disp_img, cvPoint(xloc.x, yloc.y), 10, CV_RGB(255, 0, 0), 2, 8, 0);

  cvShowImage("Image", disp_img);
  cvShowImage("BinImage", bin_img);
}
