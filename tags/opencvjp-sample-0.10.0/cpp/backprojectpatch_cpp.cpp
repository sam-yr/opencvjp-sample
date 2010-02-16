#include <cv.h>
#include <highgui.h>

using namespace cv;

int
main (int argc, char **argv)
{
  // (1)load a source image and a template image respectively
  const char *imagename = argc > 1 ? argv[1] : "../image/room7.png";
  const char *templatename = argc > 2 ? argv[2] : "../image/room7_temp.png";
  Mat search_img = imread(imagename);
  Mat tmp_img = imread(templatename);
  if(!search_img.data || !tmp_img.data)
    return -1;

  // (2)convert the color-space of input images from RGB to HSV
  Mat search_hsv, tmp_hsv;
  cvtColor(search_img, search_hsv, CV_BGR2HSV);
  cvtColor(tmp_img, tmp_hsv, CV_BGR2HSV);

  // (3)calculate a histogram of hue-plane in a template image
  int channels[] = {0};
  MatND search_hist, tmp_hist;
  int hbins = 90;
  int hist_size[] = {hbins};
  float hranges[] = {0, 180};
  const float *ranges[] = {hranges};
  calcHist(&tmp_hsv, 1, channels, Mat(), tmp_hist, 1, hist_size, ranges, true);


  // (4)calculate distance between a template and a patch region in the source image
  //    by comparing their histograms
  Size search_area(search_img.cols-tmp_img.cols+1, search_img.rows-tmp_img.rows+1);
  Mat dst_img(search_area, CV_32FC1);
  Rect roi_rect(0, 0, tmp_img.cols, tmp_img.rows);

  normalize(tmp_hist, tmp_hist, 1, 0, NORM_L1);
  for(int j=0; j<search_area.height; ++j) {
    for(int i=0; i<search_area.width; ++i) {
      roi_rect.x = i;
      roi_rect.y = j;
      Mat roi(search_hsv, roi_rect);
      calcHist(&roi, 1, channels, Mat(), search_hist, 1, hist_size, ranges, true);
      normalize(search_hist, search_hist, 1, 0, NORM_L1);
      dst_img.at<float>(j, i) = saturate_cast<float>(compareHist(search_hist, tmp_hist, CV_COMP_CORREL));
    }
  }
  
  // (5)draw a rectangle indicating template position
  Point max_pt;
  minMaxLoc(dst_img, NULL, NULL, NULL, &max_pt);
  roi_rect.x = max_pt.x;
  roi_rect.y = max_pt.y;
  rectangle(search_img, roi_rect.tl(), roi_rect.br(), Scalar(0,0,255), 3);

  // (6)show a matching result image and a probability image, and quit when any key pressed
  namedWindow("src", CV_WINDOW_AUTOSIZE);
  namedWindow("dst", CV_WINDOW_AUTOSIZE);
  imshow("src", search_img);
  imshow("dst", dst_img);
  waitKey(0);

  return 0;
}
