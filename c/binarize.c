#include <cv.h>
#include <highgui.h>

int
main(int argc, char **argv)
{
  IplImage *src_img=0, *gray_img;
  IplImage *bin_img, *bininv_img, *trunc_img, *tozero_img, *tozeroinv_img;
  IplImage *adaptive_img;
  char *imagename;

  // (1)load a specified file and convert it into grayscale image.
  //    allocate destination images.
  imagename = argc > 1 ? argv[1] : "../image/trains.png";
  src_img = cvLoadImage(imagename, CV_LOAD_IMAGE_COLOR);
  if(src_img == 0)
    return -1;
  gray_img = cvCreateImage(cvGetSize(src_img), IPL_DEPTH_8U, 1);
  cvCvtColor(src_img, gray_img, CV_BGR2GRAY);  
  bin_img = cvCreateImage(cvGetSize(src_img), IPL_DEPTH_8U, 1);
  bininv_img = cvCreateImage(cvGetSize(src_img), IPL_DEPTH_8U, 1);
  trunc_img = cvCreateImage(cvGetSize(src_img), IPL_DEPTH_8U, 1);
  tozero_img = cvCreateImage(cvGetSize(src_img), IPL_DEPTH_8U, 1);
  tozeroinv_img = cvCreateImage(cvGetSize(src_img), IPL_DEPTH_8U, 1);
  adaptive_img = cvCreateImage(cvGetSize(src_img), IPL_DEPTH_8U, 1);

  // (2)apply a fixed-level threshold to each pixel
  cvThreshold(gray_img, bin_img, 0, 255, CV_THRESH_BINARY|CV_THRESH_OTSU);
  cvThreshold(gray_img, bininv_img, 0, 255, CV_THRESH_BINARY_INV|CV_THRESH_OTSU);
  cvThreshold(gray_img, trunc_img, 0, 255, CV_THRESH_TRUNC|CV_THRESH_OTSU);
  cvThreshold(gray_img, tozero_img, 0, 255, CV_THRESH_TOZERO|CV_THRESH_OTSU);
  cvThreshold(gray_img, tozeroinv_img, 0, 255, CV_THRESH_TOZERO_INV|CV_THRESH_OTSU);

  // (3)apply an adaptive threshold to a grayscale image
  cvAdaptiveThreshold(gray_img, adaptive_img, 255, CV_ADAPTIVE_THRESH_GAUSSIAN_C, CV_THRESH_BINARY, 7, 8);
  
  // (4)show source and destination images 
  cvNamedWindow("Source", CV_WINDOW_AUTOSIZE);
  cvNamedWindow("Binary", CV_WINDOW_AUTOSIZE);
  cvNamedWindow("Binary Inv", CV_WINDOW_AUTOSIZE);
  cvNamedWindow("Trunc", CV_WINDOW_AUTOSIZE);
  cvNamedWindow("ToZero", CV_WINDOW_AUTOSIZE);
  cvNamedWindow("ToZero Inv", CV_WINDOW_AUTOSIZE);
  cvNamedWindow("Adaptive", CV_WINDOW_AUTOSIZE);
  cvShowImage("Source", src_img);
  cvShowImage("Binary", bin_img);
  cvShowImage("Binary Inv", bininv_img);
  cvShowImage("Trunc", trunc_img);
  cvShowImage("ToZero", tozero_img);
  cvShowImage("ToZero Inv", tozeroinv_img);
  cvShowImage("Adaptive", adaptive_img);
  cvWaitKey (0);

  cvDestroyWindow("Source");
  cvDestroyWindow("Binary");
  cvDestroyWindow("Binary Inv");
  cvDestroyWindow("Trunc");
  cvDestroyWindow("ToZero");
  cvDestroyWindow("ToZero Inv");
  cvDestroyWindow("Adaptive");
  cvReleaseImage(&src_img);
  cvReleaseImage(&gray_img);
  cvReleaseImage(&bin_img);
  cvReleaseImage(&bininv_img);
  cvReleaseImage(&trunc_img);
  cvReleaseImage(&tozero_img);
  cvReleaseImage(&tozeroinv_img);
  cvReleaseImage(&adaptive_img);

  return 0;
}
