#include <cv.h>
#include <highgui.h>
#include <iostream>
#include <stdio.h>

using namespace cv;

int
main (int argc, char **argv)
{

  // (1)画像を確保し初期化します． 
  Mat img = Mat::zeros(Size(640, 480), CV_8UC3);

  // (2)点列を生成します． 
  Mat points1(Size(50, 1), CV_32SC2);
  // randu(points1, Scalar(img.cols/4, img.rows/4), Scalar(img.cols*3./4., img.rows*3./4.));
  RNG(getTickCount()).fill(points1, RNG::UNIFORM, Scalar(img.cols/4, img.rows/4), Scalar(img.cols*3./4, img.rows*3./4));
  std::cout << points1.elemSize() << std::endl;

  // (3)点列を．描画します
#if 0
  vector<Mat> planes;
  split(points1, planes);
  MatIterator_<int> it0 = planes[0].begin<int>(), it0_end = planes[0].end<int>();
  MatIterator_<int> it1 = planes[1].begin<int>();
  for(; it0 != it0_end; ++it0, ++it1) {
    circle(img, Point(*it0, *it1), 3, Scalar(0,255,0), -1);
  }
#elif 0
  MatIterator_<Vec2i> it = points1.begin<Vec2i>(), it_end = points1.end<Vec2i>();
  for(; it!=it_end; ++it) {
    circle(img, Point((*it)[0], (*it)[1]), 3, Scalar(0,255,0), -1);
  }
#else
  int *p = points1.ptr<int>();
  int inc = points1.elemSize()/sizeof(int);
  for(int i=0; i<points1.cols; i++, p+=inc) {
    circle(img, Point(p[0], p[1]), 3, Scalar(0,255,0), -1);
  }
#endif

  // (4)点列を包含する矩形を求めて描画します． 
  Rect brect = boundingRect(points1);
  rectangle(img, brect.tl(), brect.br(), Scalar(0,0,255), 2);

  // (5)画像を表示し，キーが押されたときに終了します． 
  namedWindow("img", CV_WINDOW_AUTOSIZE);
  imshow("img", img);
  waitKey(0);
  
  return 0;
}
