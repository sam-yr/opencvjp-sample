#include <cv.h>
#include <highgui.h>

int
main(int argc, char **argv)
{
  double angle = -45.0, scale = 1.0;
  IplImage *src_img = 0, *dst_img = 0;
  CvMat *affine_matrix;
  CvPoint2D32f center;
  CvPoint pt1, pt2;
  CvRect rect;
  char *imagename;

  // (1)画像の読み込み（カラー），ROIの設定，出力画像の確保を行います． 
  imagename = argc > 1 ? argv[1] : "vegas.png";
  src_img = cvLoadImage(imagename, CV_LOAD_IMAGE_COLOR);
  if(src_img == 0)
    return -1;
  rect.x = cvRound(src_img->width * 0.25);
  rect.y = cvRound(src_img->height * 0.25);
  rect.width = cvRound(src_img->width * 0.5);
  rect.height = cvRound(src_img->height * 0.5);
  cvSetImageROI(src_img, rect);
  dst_img = cvCloneImage(src_img);

  // (2)角度，回転中心，スケール係数を指定して
  //    cv2DRotationMatrixを用いてアフィン変換行列を求めます． 
  affine_matrix = cvCreateMat(2, 3, CV_32FC1);
  center = cvPoint2D32f(src_img->width * 0.25, src_img->height * 0.25);
  cv2DRotationMatrix(center, angle, scale, affine_matrix);

  // (3)求めたアフィン変換行列を引数に与え，cvWarpAffineを用いて画像を回転させます． 
  cvWarpAffine(src_img, dst_img, affine_matrix, CV_INTER_LINEAR + CV_WARP_FILL_OUTLIERS, cvScalarAll(255));

  // (4)ROIをリセットし，元画像にROIを表す矩形を書き込み，最後に結果を表示します． 
  cvResetImageROI(src_img);
  cvResetImageROI(dst_img);

  pt1 = cvPoint(rect.x, rect.y);
  pt2 = cvPoint(rect.x + rect.width, rect.y + rect.height);
  cvRectangle(src_img, pt1, pt2, CV_RGB(255, 0, 255), 2, 8, 0);

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

  return 1;
}
