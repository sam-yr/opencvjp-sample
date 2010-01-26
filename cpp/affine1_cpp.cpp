#include <cv.h>
#include <highgui.h>

using namespace cv;

int
main(int argc, char **argv)
{
  // (1)画像を読み込みます（カラー）． 
  const char *imagename = argc > 1 ? argv[1] : "fruit.png";
  Mat src_img = imread(imagename);
  if(!src_img.data)
    return -1;

  // (2)三角形の回転前と回転後の対応する頂点をそれぞれセットし，
  //    getAffineTransform を用いてアフィン行列を求めます． 
  const Point2f src_pt[] = { Point2f(200.0, 200.0), Point2f(250.0, 200.0), Point2f(200.0, 100.0) };
  const Point2f dst_pt[] = { Point2f(300.0, 100.0), Point2f(300.0, 50.0), Point2f(200.0, 100.0) };
  const Mat affine_matrix = getAffineTransform(src_pt, dst_pt);

  // (3)求めたアフィン変換行列を引数に与え，warpAffineを用いて画像を回転させます． 
  Mat dst_img;
  warpAffine(src_img, dst_img, affine_matrix, src_img.size());

  // (4)元画像上に変換前後の三点を表す三角形を描画し，結果を表示します． 
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
