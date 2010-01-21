#include "cv.h"
#include "highgui.h"

using namespace cv;

int
main(int argc, char **argv)
{
  // (1)画像を読み込みます（カラー）． 
  const char *imagename = argc > 1 ? argv[1] : "fruit.png";
  Mat src_img = imread(imagename);
  if(!src_img.data) return -1;

  // (2)三角形の回転前と回転後の対応する頂点をそれぞれセットし，
  //    cvGetAffineTransform を用いてアフィン行列を求めます． 
  const Point2f src_pnt[] = { Point2f(200.0, 200.0), Point2f(250.0, 200.0), Point2f(200.0, 100.0) };
  const Point2f dst_pnt[] = { Point2f(300.0, 100.0), Point2f(300.0, 50.0), Point2f(200.0, 100.0) };
  const Mat affine_matrix = getAffineTransform(src_pnt, dst_pnt);

  // (3)求めたアフィン行列を用いて，cvWarpAffineを用いて画像を回転させます． 
  Mat dst_img;
  warpAffine(src_img, dst_img, affine_matrix, src_img.size());

  // (4)結果を表示します． 
  namedWindow("src", CV_WINDOW_AUTOSIZE);
  namedWindow("dst", CV_WINDOW_AUTOSIZE);
  imshow("src", src_img);
  imshow("dst", dst_img);
  waitKey(0);

  return 0;
}
