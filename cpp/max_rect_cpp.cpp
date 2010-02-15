#include <cv.h>
#include <highgui.h>
#include <vector>

using namespace cv;

int
main (int argc, char **argv)
{
  // (1)allocate and initialize an image
  Mat img(Size(640, 480), CV_8UC3, Scalar(100, 100, 100));

  // (2)generate rundom coordinates within a center space
  const int rect_num = 8; /** number of inner rectangles **/
  Mat rnum(Size(rect_num*2,1), CV_32SC2);
  RNG(getTickCount()).fill(rnum, RNG::UNIFORM, Scalar(img.cols/8, img.rows/8), Scalar(img.cols*7./8, img.rows*7./8));
  
  // (3)draw rectangles based on generated coordinates
  vector<Rect> inner;
  MatIterator_<Vec2i> itp = rnum.begin<Vec2i>(), itp_end = rnum.end<Vec2i>();
  for(; itp!=itp_end; itp+=2) {
    Point a((*itp)[0], (*itp)[1]), b((*(itp+1))[0], (*(itp+1))[1]);
    rectangle(img, a, b, Scalar(200,200,255), 2);
    inner.push_back(Rect(a, b));
  }

  // (4)find a minimum rectangle including all rectangles
  vector<Rect>::iterator itr = inner.begin();
  Rect outer = *itr;
  for(++itr; itr!=inner.end(); ++itr)
    outer |= *itr;
  rectangle(img, outer.tl(), outer.br(), Scalar(0,0,255), 3);

  // (5)show the iamge, and quit when any key pressed
  namedWindow("img", CV_WINDOW_AUTOSIZE);
  imshow("img", img);
  waitKey(0);

  return 0;
}
