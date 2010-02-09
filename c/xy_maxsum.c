#include <cv.h>
#include <highgui.h>

void on_trackbar1(int val);

IplImage *src_img = 0, *disp_img, *bin_img;
CvMat *xsum, *ysum;

/** main function **/
int
main(int argc, char *argv[])
{
  const char *imagename;
  int level = 125;
  CvPoint xloc, yloc;

  // (1)load a specified file as a 3-channel color image 
  //    and convert to agrayscale image
  imagename = argc > 1 ? argv[1] : "../image/winter.png";
  src_img = cvLoadImage(imagename, CV_LOAD_IMAGE_COLOR);
  if(src_img == 0)
    return -1;
  disp_img = cvCloneImage(src_img);
  bin_img = cvCreateImage(cvGetSize(src_img), IPL_DEPTH_8U, 1);
  cvCvtColor(src_img, bin_img, CV_RGB2GRAY);

  // (2)smooth and threshold the grayscale image
  cvSmooth(bin_img, bin_img, CV_GAUSSIAN, 3, 0, 0, 0);
  cvThreshold(bin_img, bin_img, level, 255, CV_THRESH_BINARY_INV);

  // (3)reduce a binary image to a vector as summation of all of the image rows/columns
  xsum = cvCreateMat(1, bin_img->width, CV_32FC1);
  ysum = cvCreateMat(bin_img->height, 1, CV_32FC1);
  cvReduce(bin_img, xsum, -1, CV_REDUCE_SUM);
  cvReduce(bin_img, ysum, -1, CV_REDUCE_SUM);

  // (4)find max location of each vector
  cvMinMaxLoc(xsum, NULL, NULL, NULL, &xloc, NULL);
  cvMinMaxLoc(ysum, NULL, NULL, NULL, &yloc, NULL);

  // (5)draw lines and a circle to indicate found location
  cvLine(disp_img, cvPoint(0, yloc.y), cvPoint(disp_img->width-1, yloc.y), CV_RGB(255,100,0), 2, 8, 0);
  cvLine(disp_img, cvPoint(xloc.x, 0), cvPoint(xloc.x, disp_img->height-1), CV_RGB(255,100,0), 2, 8, 0);
  cvCircle(disp_img, cvPoint(xloc.x, yloc.y), 10, CV_RGB(255, 0, 0), 2, 8, 0);

  // (6)show the binary and the result image respectively
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

/** callback function for trackbar events **/
void
on_trackbar1(int val)
{
  CvPoint xloc, yloc;

  // (7)every trackbar event, run processes (2)-(6) as above
  cvCvtColor(src_img, bin_img, CV_RGB2GRAY);
  cvSmooth(bin_img, bin_img, CV_GAUSSIAN, 3, 0, 0, 0);
  cvThreshold(bin_img, bin_img, val, 255, CV_THRESH_BINARY_INV);

  cvReduce(bin_img, xsum, -1, CV_REDUCE_SUM);
  cvReduce(bin_img, ysum, -1, CV_REDUCE_SUM);
  cvMinMaxLoc(xsum, NULL, NULL, NULL, &xloc, NULL);
  cvMinMaxLoc(ysum, NULL, NULL, NULL, &yloc, NULL);

  cvCopy(src_img, disp_img, NULL);
  cvLine(disp_img, cvPoint(0, yloc.y), cvPoint(disp_img->width-1, yloc.y), CV_RGB(255,100,0), 2, 8, 0);
  cvLine(disp_img, cvPoint(xloc.x, 0), cvPoint(xloc.x, disp_img->height-1), CV_RGB(255,100,0), 2, 8, 0);
  cvCircle(disp_img, cvPoint(xloc.x, yloc.y), 10, CV_RGB(255, 0, 0), 2, 8, 0);

  cvShowImage("Image", disp_img);
  cvShowImage("BinImage", bin_img);
}
