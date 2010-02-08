#include <cv.h>
#include <highgui.h>

#define MAX_CLUSTERS (10) /* クラスタ数 */

int main( int argc, char** argv )
{
  int i, size;
  IplImage *src_img = 0, *dst_img = 0;
  CvMat* clusters;
  CvMat* points;
  CvMat* count = cvCreateMat( MAX_CLUSTERS, 1, CV_32SC1);
  CvMat* centers = cvCreateMat( MAX_CLUSTERS, 3, CV_32FC1);

  // (1)画像を読み込みます． 
  if(argc != 2 || (src_img = cvLoadImage (argv[1], CV_LOAD_IMAGE_COLOR))==0)
    return -1;
  
  size = src_img->width * src_img->height;
  dst_img  = cvCloneImage(src_img);
  clusters = cvCreateMat(size, 1, CV_32SC1 );
  points   = cvCreateMat( size, 1, CV_32FC3 );

  // (2)ピクセルの値を行列へ代入します． 
#if 1
  CvMat tmp_header;
  CvMat *tmp = cvCreateMat(size, 1, CV_8UC3);
  tmp = cvReshape(src_img, &tmp_header, 0, size);
  cvConvert(tmp, points);
  cvReleaseMat(&tmp);
#else
  for(i=0; i<size; i++) {
    points->data.fl[i*3+0] = (uchar)src_img->imageData[i*3+0];
    points->data.fl[i*3+1] = (uchar)src_img->imageData[i*3+1];
    points->data.fl[i*3+2] = (uchar)src_img->imageData[i*3+2];
  }
#endif
  
  // (3)k-meansクラスタリングを実行します．
  cvKMeans2( points, MAX_CLUSTERS, clusters,
	     cvTermCriteria( CV_TERMCRIT_EPS+CV_TERMCRIT_ITER, 10, 1.0 ),
	     1, 0, 0, centers, 0);

  // (4)画素値を，それが属するクラスタの中心値で代表します． 
  for(i=0; i<size; i++) {
    int idx = clusters->data.i[i];
    dst_img->imageData[i*3+0] = (char)centers->data.fl[idx*3+0];
    dst_img->imageData[i*3+1] = (char)centers->data.fl[idx*3+1];
    dst_img->imageData[i*3+2] = (char)centers->data.fl[idx*3+2];
  }

  // (5)画像を表示，キーが押されたときに終了します． 
  cvNamedWindow( "src", CV_WINDOW_AUTOSIZE);
  cvShowImage( "src", src_img );
  cvNamedWindow( "low-color", CV_WINDOW_AUTOSIZE);
  cvShowImage( "low-color", dst_img );
  cvWaitKey(0);

  cvDestroyWindow("src");
  cvDestroyWindow("low-color");
  cvReleaseImage(&src_img);
  cvReleaseImage(&dst_img);
  cvReleaseMat(&clusters);
  cvReleaseMat(&points);
  cvReleaseMat(&centers);
  cvReleaseMat(&count);
  
  return 0;
}
