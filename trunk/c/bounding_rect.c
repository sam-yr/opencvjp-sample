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
  
  // (1)画像を確保し初期化します． 
  img = cvCreateImage(cvSize(640, 480), IPL_DEPTH_8U, 3);
  cvZero(img);

  // (2)点列を生成して，描画します．  
  points = cvCreateSeq(CV_SEQ_ELTYPE_POINT, sizeof(CvSeq), sizeof(CvPoint), storage);
  for( i = 0 ;i < 50; i++ ) {
    pt.x = cvRandInt(&rng) % (img->width/2) + img->width/4;
    pt.y = cvRandInt(&rng) % (img->height/2) + img->height/4;
    cvSeqPush( points, &pt);
    cvCircle( img, pt, 3, CV_RGB( 0, 255, 0 ), CV_FILLED, 8, 0);
  }
  
  // (3)点列を包含する矩形を求めて描画します． 
  rect = cvBoundingRect( points, 0 );
  cvRectangle( img, cvPoint(rect.x, rect.y),
	       cvPoint(rect.x+rect.width, rect.y+rect.height), CV_RGB(255,0,0), 2, 8, 0);

  // (4)画像を表示し，キーが押されたときに終了します． 
  cvNamedWindow ("BoundingRect", CV_WINDOW_AUTOSIZE);
  cvShowImage ("BoundingRect", img);
  cvWaitKey (0);

  cvDestroyWindow("BoundingRect");
  cvReleaseImage(&img);
  cvReleaseMemStorage(&storage);
  
  return 0;
}
