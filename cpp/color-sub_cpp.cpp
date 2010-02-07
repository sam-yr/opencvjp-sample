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
  const int cluster_count = 10;

  // (2)画像の形状を変形します． 
  src_img.convertTo(points, CV_32FC3);
  points = points.reshape(3, src_img.rows*src_img.cols);

    // (3)k-meansクラスタリングを実行します．
  Mat_<int> clusters(points.size(), CV_32SC1);
  Mat centers;
  kmeans(points, cluster_count, clusters, 
	 cvTermCriteria(CV_TERMCRIT_EPS+CV_TERMCRIT_ITER, 10, 1.0), 1, KMEANS_PP_CENTERS, &centers);


  // (4)画素値を，それが属するクラスタの中心値で代表します． 
  Mat dst_img(src_img.size(), src_img.type());
  MatIterator_<Vec3f> itf = centers.begin<Vec3f>();
  MatIterator_<Vec3b> itd = dst_img.begin<Vec3b>(), itd_end = dst_img.end<Vec3b>();
  for(int i=0; itd != itd_end; ++itd, ++i) {
    (*itd)[0] = saturate_cast<uchar>(itf[clusters(1,i)][0]);
    (*itd)[1] = saturate_cast<uchar>(itf[clusters(1,i)][1]);
    (*itd)[2] = saturate_cast<uchar>(itf[clusters(1,i)][2]);
  }

  // (5)画像を表示，キーが押されたときに終了します．   
  namedWindow("src_img", CV_WINDOW_AUTOSIZE);
  imshow("src_img", src_img);
  namedWindow("dst_img", CV_WINDOW_AUTOSIZE);
  imshow("dst_img", dst_img);
  waitKey(0);

  return 0;
}
