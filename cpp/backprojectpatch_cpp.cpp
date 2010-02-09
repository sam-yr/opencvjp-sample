#include <cv.h>
#include <highgui.h>

using namespace cv;

int
main (int argc, char **argv)
{
  // (1)load a source image and a template image respectively
  const char *imagename = argc > 1 ? argv[1] : "room7.png";
  const char *templatename = argc > 2 ? argv[2] : "room7_temp.png";
  Mat src_img = imread(imagename);
  Mat tmp_img = imread(templatename);
  if(!src_img.data || !tmp_img.data)
    return -1;

  // (2)convert the color-space of input images from RGB to HSV
  Mat src_hsv, tmp_hsv;
  cvtColor(src_img, src_hsv, CV_BGR2HSV);
  cvtColor(tmp_img, tmp_hsv, CV_BGR2HSV);

  // (3)calculate a histogram of hue-plane in a template image
  int channels[] = {0};
  MatND src_hist, tmp_hist;
  int hbins = 90;
  int hist_size[] = {hbins};
  float hranges[] = {0, 180};
  const float *ranges[] = {hranges};
  calcHist(&tmp_hsv, 1, channels, Mat(), tmp_hist, 1, hist_size, ranges, true);


  // (4)calculate distance between a template and a patch region in the source image
  //    by comparing their histograms
  Size search_area(src_img.cols-tmp_img.cols+1, src_img.rows-tmp_img.rows+1);
  Mat dst_img(search_area, CV_32FC1);
  Rect roi_rect(0, 0, tmp_img.cols, tmp_img.rows);

  normalize(tmp_hist, tmp_hist, 1, 0, NORM_L1);
  for(int j=0; j<search_area.height; ++j) {
    for(int i=0; i<search_area.width; ++i) {
      roi_rect.x = i;
      roi_rect.y = j;
      Mat roi(src_hsv, roi_rect);
      calcHist(&roi, 1, channels, Mat(), src_hist, 1, hist_size, ranges, true);
      normalize(src_hist, src_hist, 1, 0, NORM_L1);
      dst_img.at<float>(j, i) = compareHist(src_hist, tmp_hist, CV_COMP_CORREL);
    }
  }
  
  // (5)draw a rectangle indicating template position and show the image
  Point max_pt;
  minMaxLoc(dst_img, NULL, NULL, NULL, &max_pt);
  roi_rect.x = max_pt.x;
  roi_rect.y = max_pt.y;
  rectangle(src_img, roi_rect.tl(), roi_rect.br(), Scalar(0,0,255), 3);

  namedWindow("src", CV_WINDOW_AUTOSIZE);
  namedWindow("dst", CV_WINDOW_AUTOSIZE);
  imshow("src", src_img);
  imshow("dst", dst_img);
  waitKey(0);

  return 0;
}
