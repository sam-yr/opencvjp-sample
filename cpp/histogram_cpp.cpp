#include <cv.h>
#include <highgui.h>
#include <iostream>

using namespace cv;

int
main (int argc, char **argv)
{

  // (1)load a source image as is
  const char *imagename = argc > 1 ? argv[1] : "../image/flower_color.png";
  Mat src_img = imread(imagename, -1);
  if(!src_img.data)
    return -1;

  // (2)allocate a histogram image
  const int ch_width = 260;
  const int sch = src_img.channels();
  Mat hist_img(Size(ch_width * sch, 200), CV_8UC3, Scalar::all(255));

  vector<MatND> hist(3);
  const int hist_size = 256;
  const int hdims[] = {hist_size};
  const float hranges[] = {0,256};
  const float* ranges[] = {hranges};
  double max_val = .0;

  if(sch==1) {
    // (3a)if the source image has single-channel, calculate its histogram
    calcHist(&src_img, 1, 0, Mat(), hist[0], 1, hdims, ranges, true, false);
    minMaxLoc(hist[0], 0, &max_val);
  } else {
    // (3b)if the souce image has multi-channel, calculate histogram of each plane
    for(int i=0; i<sch; ++i) {
      calcHist(&src_img, 1, &i, Mat(), hist[i], 1, hdims, ranges, true, false);
      double tmp_val;
      minMaxLoc(hist[i], 0, &tmp_val);
      max_val = max_val < tmp_val ? tmp_val : max_val;
    }
  }

  // (4)scale and draw the histogram(s)
  Scalar color = Scalar::all(100);
  for(int i=0; i<sch; i++) {
    if(sch==3)
      color = Scalar((0xaa<<i*8)&0x0000ff,(0xaa<<i*8)&0x00ff00,(0xaa<<i*8)&0xff0000, 0);
    hist[i].convertTo(hist[i], hist[i].type(), max_val?200./max_val:0.,0);
    for(int j=0; j<hist_size; ++j) {
      int bin_w = saturate_cast<int>((double)ch_width/hist_size);
      rectangle(hist_img, 
		Point(j*bin_w+(i*ch_width), hist_img.rows),
		Point((j+1)*bin_w+(i*ch_width), hist_img.rows-saturate_cast<int>(hist[i].at<float>(j))),
		color, -1);
    }
  }

  // (5)show the histogram iamge, and quit when any key pressed
  namedWindow("Image", CV_WINDOW_AUTOSIZE);
  namedWindow("Histogram", CV_WINDOW_AUTOSIZE);
  imshow("Image", src_img);
  imshow("Histogram", hist_img);
  waitKey(0);
  
  return 0;
}
