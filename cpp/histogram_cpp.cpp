#include <cv.h>
#include <highgui.h>
#include <iostream>

using namespace cv;

int
main (int argc, char **argv)
{

  const char *imagename = argc > 1 ? argv[1] : "flower.png";
  Mat src_img = imread(imagename, -1);
  if(!src_img.data)
    return -1;

  const int channels[] = {0,1,2};
  MatND hist;
  const int hdims[] = {256, 256, 256};
  int hist_size = 256;
  float hranges[] = {0,256};
  const float* ranges[] = {hranges, hranges, hranges};

  calcHist(&src_img, 1, channels, Mat(), hist, 1, hdims, ranges, true, false);
  double min_val = .0, max_val = .0;
  minMaxLoc(hist, &min_val, &max_val);
  hist.convertTo(hist, hist.type(), max_val?200./max_val:0.,0);

  int sch = 3, ch_width = 260;
  Mat histimg(Size(ch_width * sch, 200), CV_8UC3, Scalar::all(255));

  int i=0;
  for(int i=0; i<3; i++) {
  for(int j=0; j<hist_size; ++j) {
    int bin_w = saturate_cast<int>((double)ch_width/hist_size);
    std::cout << "j=" << j << std::endl;
    std::cout << "ch=" << hist.channels() << " dims=" << hist.dims << std::endl;
    //std::cout << j*bin_w+(i*ch_width) << ","<< histimg.rows << "(fixed)" <<std::endl;
    //std::cout << (j+1)*bin_w+(i*ch_width)<<","<< histimg.rows-saturate_cast<int>(hist.at<Vec3f>(j)[0])<<std::endl;
    rectangle(histimg, 
	      Point(j*bin_w+(i*ch_width), histimg.rows),
	      Point((j+1)*bin_w+(i*ch_width), histimg.rows-saturate_cast<int>(hist.at<Vec3f>(j)[i])),
	      Scalar(255,0,0), -1);
  }
}
  namedWindow("src", CV_WINDOW_AUTOSIZE);
  imshow("src", histimg);
  waitKey(0);
  
  return 0;
}
