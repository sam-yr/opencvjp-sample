#include <cvaux.h>
#include <highgui.h>

using namespace cv;

int
main (int argc, char **argv)
{
  // (1)load a specified file as a 3-channel color image
  const char *imagename = argc > 1 ? argv[1] : "../image/library.png";
  Mat eigen_img = imread(imagename);
  if(!eigen_img.data)
    return -1;
  Mat harris_img = eigen_img.clone();
  Mat fast_img = eigen_img.clone();

  // (2)convert to a grayscale image and normalize it
  Mat gray_img;
  cvtColor(eigen_img, gray_img, CV_BGR2GRAY);
  normalize(gray_img, gray_img, 0, 255, NORM_MINMAX);

  // (3)detect and draw strong corners on the image based on Eigen Value
  vector<Point2f> corners;
  goodFeaturesToTrack(gray_img, corners, 80, 0.01, 5);
  vector<Point2f>::iterator it_corner = corners.begin();
  for(; it_corner!=corners.end(); ++it_corner) {
    circle(eigen_img, Point(it_corner->x, it_corner->y), 1, Scalar(0,200,255), -1);
    circle(eigen_img, Point(it_corner->x, it_corner->y), 8, Scalar(0,200,255));
  }

  // (4)detect and draw strong corners on the image using Harris detector
  goodFeaturesToTrack(gray_img, corners, 80, 0.01, 3, Mat(), 3, true);
  it_corner = corners.begin();
  for(; it_corner!=corners.end(); ++it_corner) {
    circle(harris_img, Point(it_corner->x, it_corner->y), 1, Scalar(0,255,0), -1);
    circle(harris_img, Point(it_corner->x, it_corner->y), 8, Scalar(0,255,0 ));
  }

  // (5)detect corners using high-speed corner detection; FAST
  int threshold = 100;
  bool nonmax = true;
  vector<KeyPoint> keypoints;
  FAST(gray_img, keypoints, threshold, nonmax);
  vector<KeyPoint>::iterator it_kp = keypoints.begin();
  for(; it_kp!=keypoints.end(); ++it_kp) {
    circle(fast_img, Point(it_kp->pt.x, it_kp->pt.y), 1, Scalar(50,0,255), -1);
    circle(fast_img, Point(it_kp->pt.x, it_kp->pt.y), 8, Scalar(50,0,255));
  }

  // (6)show destination images, and quit when any key pressed
  namedWindow("EigenValue",CV_WINDOW_AUTOSIZE);
  namedWindow("Harris",CV_WINDOW_AUTOSIZE);
  namedWindow("Fast",CV_WINDOW_AUTOSIZE);
  imshow("EigenValue", eigen_img);
  imshow("Harris", harris_img);
  imshow("Fast", fast_img);
  waitKey(0);

  return 0;
}
