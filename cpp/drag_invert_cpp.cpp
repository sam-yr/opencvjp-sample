#include <cv.h>
#include <highgui.h>
#include <iostream>

using namespace cv;

Rect selection;
int select_object;

void
on_mouse(int event, int x, int y, int flags, void* param)
{
  static Point2i origin;
  Mat *img = (Mat*)param;

  /// 選択領域計算（左ドラッグ中）
  if(select_object) {
    selection.x = CV_IMIN(x, origin.x);
    selection.y = CV_IMIN(y, origin.y);
    selection.width = selection.x + CV_IABS(x - origin.x);
    selection.height = selection.y + CV_IABS(y - origin.y);

    selection.x = CV_IMAX(selection.x, 0);
    selection.y = CV_IMAX(selection.y, 0);
    selection.width = CV_IMIN( selection.width, img->cols );
    selection.height = CV_IMIN( selection.height, img->rows );
    selection.width -= selection.x;
    selection.height -= selection.y;

  }

  /// マウスイベント（左ドラッグによる領域選択）開始・終了
  switch(event) {
  case CV_EVENT_LBUTTONDOWN: // 選択開始
    origin = Point2i(x,y);
    selection = Rect(x, y, 0, 0);
    select_object = 1;
    break;
  case CV_EVENT_LBUTTONUP: // 選択終了
    select_object = 0;
    break;
  }
}


int 
main(int argc, char *argv[])
{ 
  // (1)画像の読み込み（カラーorグレースケール）
  const char *imagename = argc > 1 ? argv[1] : "lenna.png";
  Mat src_img = imread(imagename, -1);
  if(!src_img.data)
    return -1;

  namedWindow("Image", 1);
  cvSetMouseCallback("Image", (CvMouseCallback)(&on_mouse), &src_img);

  while(1) {
    Mat dst_img = src_img.clone();
    if(select_object && selection.width > 0 && selection.height > 0) {
      Mat roi(dst_img, selection);
      bitwise_xor(roi, Scalar::all(255), roi);
    }
    
    imshow("Image", dst_img);
    char key = waitKey(10);
    if(key==27 || key=='q')
      break;
  }

  return 0;
}
