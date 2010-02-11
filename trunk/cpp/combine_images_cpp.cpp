#include <cv.h>
#include <highgui.h>
#include <vector>

using namespace cv;

int
main (int argc, char **argv)
{
  int img_num = argc > 1 ? argc-1 : 2;
  const char defaultfile[2][32] = {"../image/moon.png", "../image/sunset.png"};
  int total_width=0, max_height=0;  

  // (1)load all images specified on the command line
  vector<Mat> src_img;
  for(int i=0; i<img_num; i++) {
    src_img.push_back(imread(argc-1?argv[i+1]:defaultfile[i]));
    Mat last = src_img.back();
    if(!last.data)
      return -1;
    total_width += last.cols;
    max_height = max_height < last.rows ? last.rows : max_height;
  }

  // (2)append images laterally one after another
  Mat combined_img(Size(total_width, max_height), CV_8UC3);
  vector<Mat>::iterator it = src_img.begin(), it_end = src_img.end();
  Rect roi_rect;
  for(; it!=it_end; ++it) {
    roi_rect.width = it->cols;
    roi_rect.height = it->rows;
    Mat roi(combined_img, roi_rect);
    it->copyTo(roi);
    roi_rect.x += it->cols;
  }

  // (3)show the combined image, and quit when any key pressed
  namedWindow("Combined Image", CV_WINDOW_AUTOSIZE);
  imshow("Combined Image", combined_img);
  waitKey(0);

  return 0;
}
