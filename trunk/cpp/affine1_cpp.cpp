#include <cv.h>
#include <highgui.h>

using namespace cv;

int
main(int argc, char **argv)
{
  // (1)load a specified file as a 3-channel color image
  const string imagename = argc > 1 ? argv[1] : "../image/fruit.png";
  Mat src_img = imread(imagename);
  if(!src_img.data)
    return -1;

  // (2)setting three points before and after transformation, 
  //    an affine transformation matrix was calculated by getAffineTransform
  const Point2f src_pt[] = { Point2f(200.0, 200.0), Point2f(250.0, 200.0), Point2f(200.0, 100.0) };
  const Point2f dst_pt[] = { Point2f(300.0, 100.0), Point2f(300.0, 50.0), Point2f(200.0, 100.0) };
  const Mat affine_matrix = getAffineTransform(src_pt, dst_pt);

  // (3)rotate the image by warpAffine taking the affine matrix
  Mat dst_img;
  warpAffine(src_img, dst_img, affine_matrix, src_img.size());

  // (4)show source and destination image with two triangles representing the transformation
  line(src_img, src_pt[0], src_pt[1], Scalar(255,255,0), 2);
  line(src_img, src_pt[1], src_pt[2], Scalar(255,255,0), 2);
  line(src_img, src_pt[2], src_pt[0], Scalar(255,255,0), 2);
  line(src_img, dst_pt[0], dst_pt[1], Scalar(255,0,255), 2);
  line(src_img, dst_pt[1], dst_pt[2], Scalar(255,0,255), 2);
  line(src_img, dst_pt[2], dst_pt[0], Scalar(255,0,255), 2);

  namedWindow("src", CV_WINDOW_AUTOSIZE);
  namedWindow("dst", CV_WINDOW_AUTOSIZE);
  imshow("src", src_img);
  imshow("dst", dst_img);
  waitKey(0);

  return 0;
}
