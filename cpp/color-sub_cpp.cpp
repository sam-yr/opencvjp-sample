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

  // (2)画像（行列）の形状を変形して， 1 行の行列にします． 
  const int cluster_count = 3;
  Mat points;
  src_img.convertTo(points, CV_32FC3);
  points = points.reshape(3, src_img.rows*src_img.cols);

  // (3)k-means クラスタリングを行います． 
  Mat_<int> clusters(points.size(), CV_32SC1);
  Mat centers;
  kmeans(points, cluster_count, clusters, 
	 cvTermCriteria(CV_TERMCRIT_EPS+CV_TERMCRIT_ITER, 10, 1.0), 1, KMEANS_PP_CENTERS, &centers);

  // (4)クラスタ毎に色を描画します． 
  MatIterator_<Vec3f> itf = centers.begin<Vec3f>();
  Mat dst_img(src_img.size(), src_img.type());
  uchar *p = dst_img.ptr<uchar>();
  int inc = dst_img.elemSize()/sizeof(uchar);
  for(int i=0; i<dst_img.cols*dst_img.rows; i++, p+=inc) {
    p[0] = saturate_cast<uchar>(itf[clusters(1,i)][0]);
    p[1] = saturate_cast<uchar>(itf[clusters(1,i)][1]);
    p[2] = saturate_cast<uchar>(itf[clusters(1,i)][2]);
  }

  // (5)画像を表示，キーが押されたときに終了します．   
  namedWindow("Image", CV_WINDOW_AUTOSIZE);
  imshow("Image", src_img);
  namedWindow("dst_img", CV_WINDOW_AUTOSIZE);
  imshow("dst_img", dst_img);
  waitKey(0);

  return 0;
}
