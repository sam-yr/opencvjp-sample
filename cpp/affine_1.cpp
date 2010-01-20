#include <iostream>
#include <stdio.h>
#include "cv.h"
#include "highgui.h"

using namespace cv;

int main( int argc, char** argv )
{
  // (1)画像の読み込み，出力用画像領域の確保を行なう
  const char* imagename = argc > 1 ? argv[1] : "lena.jpg";
  Mat src_img = imread(imagename);
  //fprintf(stderr, "src_img:%p,%d\n", &src_img, src_img.data);
  //Ptr<IplImage> iplimg = cvLoadImage(imagename);
  //Mat src_img(iplimg);
  if( !src_img.data ) // 画像が適切に読み込まれたかどうかをチェックします．
    return -1;

  // (2)三角形の回転前と回転後の対応する頂点をそれぞれセットし
  //    cvGetAffineTransformを用いてアフィン行列を求める
  const Point2f src_pnt[] = {Point2f( 200.0, 200.0 ), Point2f( 250.0, 200.0 ), Point2f( 200.0, 100.0 )};
  const Point2f dst_pnt[] = {Point2f( 300.0, 100.0 ), Point2f( 300.0,  50.0 ), Point2f( 200.0, 100.0 )};
  
  const Mat map_matrix = getAffineTransform( src_pnt, dst_pnt );
  
  // (3)指定されたアフィン行列により，cvWarpAffineを用いて画像を回転させる
  Mat dst_img;
  warpAffine(src_img, dst_img, map_matrix, src_img.size());

  namedWindow("src", CV_WINDOW_AUTOSIZE);
  namedWindow("dst", CV_WINDOW_AUTOSIZE);
  imshow("src", src_img);
  imshow("dst", dst_img);
  waitKey(0);

  return 1;
}
