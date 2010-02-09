#include <cv.h>
#include <highgui.h>

using namespace cv;

Mat src_img, disp_img, bin_img;

void on_trackbar1(int val, void* userdata);

/** main function **/
int
main(int argc, char *argv[])
{
  // (1)load a specified file as a 3-channel color image 
  //    and convert to agrayscale image
  const char *imagename = argc > 1 ? argv[1] : "../image/vegas.png";
  src_img = imread(imagename);
  if(!src_img.data)
    return -1;
  
  disp_img = src_img.clone();
  cvtColor(src_img, bin_img, CV_RGB2GRAY);

  // (2)smooth and threshold the grayscale image
  int level = 125;
  GaussianBlur(bin_img, bin_img, Size(3,3), 0);
  threshold(bin_img, bin_img, level, 255, THRESH_BINARY_INV);

  // (3)reduce a binary image to a vector as summation of all of the image rows/columns
  Mat xsum, ysum;
  reduce(bin_img, xsum, 0, CV_REDUCE_SUM, CV_32F);
  reduce(bin_img, ysum, 1, CV_REDUCE_SUM, CV_32F);

  // (4)find max location of each vector
  Point xloc, yloc;
  minMaxLoc(xsum, 0, 0, 0, &xloc);
  minMaxLoc(ysum, 0, 0, 0, &yloc);
  
  // (5)draw lines and a circle to indicate found location
  line(disp_img, Point(0, yloc.y), Point(disp_img.cols-1, yloc.y), Scalar(0,100,255), 2);
  line(disp_img, Point(xloc.x, 0), Point(xloc.x, disp_img.cols-1), Scalar(0,100,255), 2);
  circle(disp_img, Point(xloc.x, yloc.y), 10, Scalar(0,0,255), 2);

  // (6)show the binary and the result image respectively
  namedWindow("Image", CV_WINDOW_AUTOSIZE);
  namedWindow("BinImage", CV_WINDOW_AUTOSIZE);
  createTrackbar("Trackbar1", "Image", &level, 255, on_trackbar1);
  imshow("Image", disp_img);
  imshow("BinImage", bin_img);
  waitKey(0);

  return 0;
}

/** callback function for trackbar events **/
void
on_trackbar1(int val, void* userdata)
{
  // (7)every trackbar event, run processes (2)-(6) as above
  cvtColor(src_img, bin_img, CV_RGB2GRAY);  
  GaussianBlur(bin_img, bin_img, Size(3,3), 0);
  threshold(bin_img, bin_img, val, 255, THRESH_BINARY_INV);

  Point xloc, yloc;
  Mat xsum, ysum;
  reduce(bin_img, xsum, 0, CV_REDUCE_SUM, CV_32F);
  reduce(bin_img, ysum, 1, CV_REDUCE_SUM, CV_32F);
  minMaxLoc(xsum, 0, 0, 0, &xloc);
  minMaxLoc(ysum, 0, 0, 0, &yloc);
  
  src_img.copyTo(disp_img);
  line(disp_img, Point(0, yloc.y), Point(disp_img.cols-1, yloc.y), Scalar(0,100,255), 2);
  line(disp_img, Point(xloc.x, 0), Point(xloc.x, disp_img.cols-1), Scalar(0,100,255), 2);
  circle(disp_img, Point(xloc.x, yloc.y), 10, Scalar(0,0,255), 2);
  imshow("Image", disp_img);
  imshow("BinImage", bin_img);
}
