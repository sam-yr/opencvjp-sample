#include <cv.h>
#include <highgui.h>

using namespace cv;

int
main(int argc, char **argv)
{
  // (1)画像の読み込み（カラー），ROIの設定，出力画像の確保を行います． 
  const char *imagename = argc > 1 ? argv[1] : "vegas.png";
  Mat src_img = imread(imagename);
  if(!src_img.data)
    return -1;

  Mat dst_img = src_img.clone();
  Rect roi_rect(cvRound(src_img.cols*0.25), cvRound(src_img.rows*0.25), cvRound(src_img.cols*0.5), cvRound(src_img.rows*0.5));
  Mat src_roi(src_img, roi_rect);
  Mat dst_roi(dst_img, roi_rect);
  
  // (2)角度，回転中心，スケール係数を指定して
  //    cv2DRotationMatrixを用いてアフィン変換行列を求めます． 
  double angle = -45.0, scale = 1.0;
  Point2d center(src_roi.cols*0.5, src_roi.rows*0.5);
  const Mat affine_matrix = getRotationMatrix2D( center, angle, scale );

  // (3)求めたアフィン変換行列を引数に与え，cvWarpAffineを用いて画像を回転させます． 
  warpAffine(src_roi, dst_roi, affine_matrix, dst_roi.size(), INTER_LINEAR, BORDER_CONSTANT, Scalar::all(255));

  // (4)元画像にROIを表す矩形を書き込み，結果を表示します． 
  rectangle(src_img, roi_rect.tl(), roi_rect.br(), Scalar(255,0,255), 2);

  namedWindow("src", CV_WINDOW_AUTOSIZE);
  namedWindow("dst", CV_WINDOW_AUTOSIZE);
  imshow("src", src_img);
  imshow("dst", dst_img);
  waitKey(0);

  return 0;
}
