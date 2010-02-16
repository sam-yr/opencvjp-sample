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

  // (1)load a specified file as a 3-channel color image,
  //    set its ROI, and allocate a destination image
  imagename = argc > 1 ? argv[1] : "../image/vegas.png";
  src_img = cvLoadImage(imagename, CV_LOAD_IMAGE_COLOR);
  if(src_img == 0)
    return -1;
  rect.x = cvRound(src_img->width * 0.25);
  rect.y = cvRound(src_img->height * 0.25);
  rect.width = cvRound(src_img->width * 0.5);
  rect.height = cvRound(src_img->height * 0.5);
  cvSetImageROI(src_img, rect);
  dst_img = cvCloneImage(src_img);

  // (2)With specified three parameters (angle, rotation center, scale)
  //    calculate an affine transformation matrix by cv2DRotationMatrix
  affine_matrix = cvCreateMat(2, 3, CV_32FC1);
  center = cvPoint2D32f(src_img->width * 0.25, src_img->height * 0.25);
  cv2DRotationMatrix(center, angle, scale, affine_matrix);

  // (3)rotate the image by warpAffine taking the affine matrix
  cvWarpAffine(src_img, dst_img, affine_matrix, CV_INTER_LINEAR + CV_WARP_FILL_OUTLIERS, cvScalarAll(255));

  // (4)reset ROI, then show source and destination image with a rectangle indicating ROI
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
