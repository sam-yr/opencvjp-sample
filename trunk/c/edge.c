#include <cv.h>
#include <highgui.h>

int
main (int argc, char **argv)
{
  IplImage *src_img, *sobel_img, *laplaian_img, *canny_img;
  IplImage *tmp_img;
  char *imagename;

  // (1)load a specified file as a grayscale image
  //    and allocate destination images
  imagename = argc > 1 ? argv[1] : "../image/bike_sign.png";
  src_img = cvLoadImage(imagename, CV_LOAD_IMAGE_GRAYSCALE);
  if(src_img == 0)
    return -1;
  tmp_img = cvCreateImage(cvGetSize(src_img), IPL_DEPTH_16S, 1);
  sobel_img = cvCreateImage(cvGetSize(src_img), IPL_DEPTH_8U, 1);
  laplaian_img = cvCreateImage(cvGetSize(src_img), IPL_DEPTH_8U, 1);
  canny_img = cvCreateImage(cvGetSize(src_img), IPL_DEPTH_8U, 1);
  
  // (2)calculate the first image derivatives using an Sobel operator
  cvSobel(src_img, tmp_img, 1, 1, 3);
  cvConvertScaleAbs(tmp_img, sobel_img, 1, 0);

  // (3)calculate the Laplacian of an image
  cvLaplace(src_img, tmp_img, 3);
  cvConvertScaleAbs(tmp_img, laplaian_img, 1, 0);
  
  // (4)implement the Canny algorithm for edge detection
  cvCanny(src_img, canny_img, 50.0, 200.0, 3);

  // (5)show original gray and their edge images respectively, and quit when any key pressed
  cvNamedWindow("Original(GrayScale)", CV_WINDOW_AUTOSIZE);
  cvNamedWindow("Sobel", CV_WINDOW_AUTOSIZE);
  cvNamedWindow("Laplacian", CV_WINDOW_AUTOSIZE);
  cvNamedWindow("Canny", CV_WINDOW_AUTOSIZE);
  cvShowImage("Original(GrayScale)", src_img);
  cvShowImage("Sobel", sobel_img);
  cvShowImage("Laplacian", laplaian_img);
  cvShowImage("Canny", canny_img);
  cvWaitKey(0);
   
  cvDestroyWindow("Original");
  cvDestroyWindow("Sobel");
  cvDestroyWindow("Laplace");
  cvDestroyWindow("Canny");
  cvReleaseImage(&src_img);
  cvReleaseImage(&sobel_img);
  cvReleaseImage(&laplaian_img);
  cvReleaseImage(&canny_img);
  cvReleaseImage(&tmp_img);
  
  return 0;
}
