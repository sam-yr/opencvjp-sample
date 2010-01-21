#include "cv.h"
#include "highgui.h"

int
main(int argc, char **argv)
{
  IplImage *src_img = 0, *dst_img = 0;
  CvMat *affine_matrix = NULL;
  CvPoint2D32f src_pnt[3], dst_pnt[3];
  const char * imagename;

  // (1)画像の読み込み（カラー），出力用画像領域の確保を行ないます． 
  imagename = argc > 1 ? argv[1] : "fruit.png";
  src_img = cvLoadImage(imagename, CV_LOAD_IMAGE_COLOR);
  if(src_img == 0) return -1;
  dst_img = cvCloneImage(src_img);

  // (2)三角形の回転前と回転後の対応する頂点をそれぞれセットし 
  //    cvGetAffineTransform を用いてアフィン行列を求めます． 
  src_pnt[0] = cvPoint2D32f(200.0, 200.0);
  src_pnt[1] = cvPoint2D32f(250.0, 200.0);
  src_pnt[2] = cvPoint2D32f(200.0, 100.0);
  dst_pnt[0] = cvPoint2D32f(300.0, 100.0);
  dst_pnt[1] = cvPoint2D32f(300.0, 50.0);
  dst_pnt[2] = cvPoint2D32f(200.0, 100.0);
  affine_matrix = cvCreateMat(2, 3, CV_32FC1);
  cvGetAffineTransform(src_pnt, dst_pnt, affine_matrix);

  // (3)求めたアフィン行列を用いて，cvWarpAffineを用いて画像を回転させます． 
  cvWarpAffine(src_img, dst_img, affine_matrix, CV_INTER_LINEAR + CV_WARP_FILL_OUTLIERS, cvScalarAll(0));


  // (4)結果を表示します． 
  cvNamedWindow("src", CV_WINDOW_AUTOSIZE);
  cvNamedWindow("dst", CV_WINDOW_AUTOSIZE);
  cvShowImage("src", src_img);
  cvShowImage("dst", dst_img);
  cvWaitKey(0);

  cvDestroyWindow("src");
  cvDestroyWindow("dst");
  cvReleaseImage(&src_img);
  cvReleaseImage(&dst_img);
  cvReleaseMat(&affine_matrix);

  return 0;
}
