#include <cv.h>
#include <highgui.h>

int
main(int argc, char **argv)
{
  IplImage *src_img = 0, *dst_img = 0;
  CvMat *affine_matrix = NULL;
  CvPoint2D32f src_pt[3], dst_pt[3];
  char *imagename;

  // (1)load a specified file as a 3-channel color image,
  //    allocate a destination image
  imagename = argc > 1 ? argv[1] : "fruit.png";
  src_img = cvLoadImage(imagename, CV_LOAD_IMAGE_COLOR);
  if(src_img == 0)
    return -1;
  dst_img = cvCloneImage(src_img);

  // (2)setting three points before and after transformation, 
  //    an affine transformation matrix was calculated by getAffineTransform
  src_pt[0] = cvPoint2D32f(200.0, 200.0);
  src_pt[1] = cvPoint2D32f(250.0, 200.0);
  src_pt[2] = cvPoint2D32f(200.0, 100.0);
  dst_pt[0] = cvPoint2D32f(300.0, 100.0);
  dst_pt[1] = cvPoint2D32f(300.0, 50.0);
  dst_pt[2] = cvPoint2D32f(200.0, 100.0);
  affine_matrix = cvCreateMat(2, 3, CV_32FC1);
  cvGetAffineTransform(src_pt, dst_pt, affine_matrix);

  // (3)rotate the image by warpAffine taking the affine matrix
  cvWarpAffine(src_img, dst_img, affine_matrix, CV_INTER_LINEAR + CV_WARP_FILL_OUTLIERS, cvScalarAll(0));


  // (4)show source and destination images with two triangles representing the transformation
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
