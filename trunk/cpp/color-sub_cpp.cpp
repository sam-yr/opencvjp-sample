#include <cv.h>
#include <highgui.h>
#include <iostream>

using namespace cv;

int
main (int argc, char **argv)
{
  // (1)画像を読み込みます． 
  const char *imagename = argc > 1 ? argv[1] : "boat.png";
  Mat src_img = imread(imagename);
  if(!src_img.data)
    return -1;

  //Mat points(Size(src_img.rows*src_img.cols, 1), CV_32FC3);
  Mat points = src_img.reshape(3, 1);
  std::cout << points.channels() << "ch ," << points.rows << "," << points.cols << ":" << points.isContinuous() << std::endl;
  const int cluster_count = 32;
  //Mat points;
  points.convertTo(points, CV_32FC3);
  Mat clusters(points.size(), CV_32SC1);
  std::cout << clusters.channels() << "ch ," << clusters.rows << "," << clusters.cols << ":" << clusters.isContinuous() << std::endl;
  cvKMeans2(&IplImage(points), cluster_count, &IplImage(clusters), cvTermCriteria( CV_TERMCRIT_EPS+CV_TERMCRIT_ITER, 10, 1.0 ));
  

  std::cout << clusters.channels() << "," << clusters.rows << "," << clusters.cols << std::endl;
  //  for(int i=0; i<cluster_count; ++i)
  //    std::cout << clusters.at<int>(i,1) << std::endl;
  
  namedWindow("Image", CV_WINDOW_AUTOSIZE);
  imshow("Image", src_img);
  waitKey(0);

  return 0;
}
