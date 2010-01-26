#include <cv.h>
#include <highgui.h>

int
main(int argc, char **argv)
{
  IplImage *src_img = 0, *dst_img = 0;
  CvMat *affine_matrix = NULL;
  CvPoint2D32f src_pt[3], dst_pt[3];
  char *imagename;

  // (1)画像の読み込み（カラー），出力用画像領域の確保を行ないます． 
  imagename = argc > 1 ? argv[1] : "fruit.png";
  src_img = cvLoadImage(imagename, CV_LOAD_IMAGE_COLOR);
  if(src_img == 0)
    return -1;
  dst_img = cvCloneImage(src_img);

  // (2)三角形の回転前と回転後の対応する頂点をそれぞれセットし 
  //    cvGetAffineTransform を用いてアフィン行列を求めます． 
  src_pt[0] = cvPoint2D32f(200.0, 200.0);
  src_pt[1] = cvPoint2D32f(250.0, 200.0);
  src_pt[2] = cvPoint2D32f(200.0, 100.0);
  dst_pt[0] = cvPoint2D32f(300.0, 100.0);
  dst_pt[1] = cvPoint2D32f(300.0, 50.0);
  dst_pt[2] = cvPoint2D32f(200.0, 100.0);
  affine_matrix = cvCreateMat(2, 3, CV_32FC1);
  cvGetAffineTransform(src_pt, dst_pt, affine_matrix);

  // (3)求めたアフィン行列を引数に与え，cvWarpAffineを用いて画像を回転させます． 
  cvWarpAffine(src_img, dst_img, affine_matrix, CV_INTER_LINEAR + CV_WARP_FILL_OUTLIERS, cvScalarAll(0));


  // (4)元画像上に変換前後の三点を表す三角形を描画し，結果を表示します． 
  cvLine(src_img, cvPointFrom32f(src_pt[0]), cvPointFrom32f(src_pt[1]), CV_RGB(0,255,255), 2, 8, 0);
  cvLine(src_img, cvPointFrom32f(src_pt[1]), cvPointFrom32f(src_pt[2]), CV_RGB(0,255,255), 2, 8, 0);
  cvLine(src_img, cvPointFrom32f(src_pt[2]), cvPointFrom32f(src_pt[0]), CV_RGB(0,255,255), 2, 8, 0);
  cvLine(src_img, cvPointFrom32f(dst_pt[0]), cvPointFrom32f(dst_pt[1]), CV_RGB(255,0,255), 2, 8, 0);
  cvLine(src_img, cvPointFrom32f(dst_pt[1]), cvPointFrom32f(dst_pt[2]), CV_RGB(255,0,255), 2, 8, 0);
  cvLine(src_img, cvPointFrom32f(dst_pt[2]), cvPointFrom32f(dst_pt[0]), CV_RGB(255,0,255), 2, 8, 0);
  

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
