#include <cv.h>
#include <highgui.h>

#define RECT_NUM (8) /** number of inner rectangles **/

int
main (int argc, char **argv)
{
  IplImage *img = 0;
  CvRNG rng = cvRNG(cvGetTickCount());
  CvRect inner[RECT_NUM], outer;
  int i;
  CvMat *rnum = cvCreateMat(RECT_NUM*2, 1, CV_32SC2);

  // (1)allocate and initialize an image
  img = cvCreateImage(cvSize(640, 480), IPL_DEPTH_8U, 3);
  cvRectangle(img, cvPoint(0,0), cvPoint(640,480), CV_RGB(100,100,100), CV_FILLED, 8, 0);

  // (2)generate rundom coordinates within a center space
  cvRandArr(&rng, rnum, CV_RAND_UNI, cvScalar(img->width/8, img->height/8, 0, 0), cvScalar(img->width*7./8, img->height*7./8,0,0));

  // (3)draw rectangles based on generated coordinates
  for(i=0; i<RECT_NUM; i++) {
    int x1 = rnum->data.i[i*4+0];
    int y1 = rnum->data.i[i*4+1];
    int x2 = rnum->data.i[i*4+2];
    int y2 = rnum->data.i[i*4+3];
    CvPoint tl = cvPoint(MIN(x1, x2), MIN(y1,y2));
    CvPoint br = cvPoint(MAX(x1, x2), MAX(y1,y2));
    cvRectangle(img, tl, br, CV_RGB(255,200,200), 2, 8, 0);
    inner[i] = cvRect(tl.x, tl.y, br.x-tl.x, br.y-tl.y);
  }

  // (4)find a minimum rectangle including all rectangles
  outer = inner[0];
  for(i=1; i<RECT_NUM; i++)
    outer = cvMaxRect(&outer, &inner[i]);
  cvRectangle(img, cvPoint(outer.x, outer.y),
	      cvPoint(outer.x+outer.width, outer.y+outer.height), CV_RGB(255,0,0), 3, 8, 0);
  
  // (5)show the iamge, and quit when any key pressed
  cvNamedWindow ("BoundingRect", CV_WINDOW_AUTOSIZE);
  cvShowImage ("BoundingRect", img);
  cvWaitKey (0);

  cvDestroyWindow("BoundingRect");
  cvReleaseImage(&img);
  
  return 0;
}
