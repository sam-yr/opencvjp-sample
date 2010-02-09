#include <cv.h>
#include <highgui.h>

int
main (int argc, char **argv)
{
  int i;
  IplImage *img = 0;
  CvMemStorage* storage = cvCreateMemStorage(0);
  CvSeq *points;
  CvRNG rng = cvRNG(cvGetTickCount());
  CvPoint pt;
  CvRect rect;
  
  // (1)allocate and initialize an image
  img = cvCreateImage(cvSize(640, 480), IPL_DEPTH_8U, 3);
  cvZero(img);

  // (2)generate and draw a sequence of random 2d points
  points = cvCreateSeq(CV_SEQ_ELTYPE_POINT, sizeof(CvSeq), sizeof(CvPoint), storage);
  for( i = 0 ;i < 50; i++ ) {
    pt.x = cvRandInt(&rng) % (img->width/2) + img->width/4;
    pt.y = cvRandInt(&rng) % (img->height/2) + img->height/4;
    cvSeqPush( points, &pt);
    cvCircle( img, pt, 3, CV_RGB( 0, 255, 0 ), CV_FILLED, 8, 0);
  }
  
  // (3)calculate and draw a bounding rectangle of points
  rect = cvBoundingRect( points, 0 );
  cvRectangle( img, cvPoint(rect.x, rect.y),
	       cvPoint(rect.x+rect.width, rect.y+rect.height), CV_RGB(255,0,0), 2, 8, 0);

  // (4)show the iamge, and quit when any key pressed
  cvNamedWindow ("BoundingRect", CV_WINDOW_AUTOSIZE);
  cvShowImage ("BoundingRect", img);
  cvWaitKey (0);

  cvDestroyWindow("BoundingRect");
  cvReleaseImage(&img);
  cvReleaseMemStorage(&storage);
  
  return 0;
}
