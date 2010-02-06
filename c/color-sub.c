#include <cv.h>
#include <highgui.h>

#define MAX_CLUSTERS (32) /* クラスタ数 */

int main( int argc, char** argv )
{
  int i, size;
  IplImage *src_img = 0, *dst_img = 0;
  CvMat* clusters;
  CvMat* points;
  CvMat* color = cvCreateMat( MAX_CLUSTERS, 1, CV_32FC3);
  CvMat* count = cvCreateMat( MAX_CLUSTERS, 1, CV_32SC1);

  // (1)画像を読み込みます． 
  if(argc != 2 || (src_img = cvLoadImage (argv[1], CV_LOAD_IMAGE_COLOR))==0)
    return -1;
  
  size = src_img->width * src_img->height;
  dst_img  = cvCloneImage(src_img);
  clusters = cvCreateMat(size, 1, CV_32SC1 );
  points   = cvCreateMat( size, 1, CV_32FC3 );

  // (2)ピクセルの値を行列へ代入します． 
  for(i=0; i<size; i++) {
    points->data.fl[i*3+0] = (uchar)src_img->imageData[i*3+0];
    points->data.fl[i*3+1] = (uchar)src_img->imageData[i*3+1];
    points->data.fl[i*3+2] = (uchar)src_img->imageData[i*3+2];
  }

  // (3)k-means クラスタリングを行います． 
  cvKMeans2( points, MAX_CLUSTERS, clusters,
	     cvTermCriteria( CV_TERMCRIT_EPS+CV_TERMCRIT_ITER, 10, 1.0 ),
	     1, 0, 0, 0, 0 );

  // (4)各クラスタの平均値を計算
  cvSetZero(color);
  cvSetZero(count);
  for(i=0; i<size; i++) {
    int idx = clusters->data.i[i];
    int j = ++count->data.i[idx];;
    color->data.fl[idx*3+0] = color->data.fl[idx*3+0]*(j-1)/j + points->data.fl[i*3+0]/j;
    color->data.fl[idx*3+1] = color->data.fl[idx*3+1]*(j-1)/j + points->data.fl[i*3+1]/j;
    color->data.fl[idx*3+2] = color->data.fl[idx*3+2]*(j-1)/j + points->data.fl[i*3+2]/j;
  }  
  
  // (5)クラスタ毎に色を描画します． 
  for(i=0; i<size; i++) {
    int idx = clusters->data.i[i];
    dst_img->imageData[i*3+0] = (char)color->data.fl[idx*3+0];
    dst_img->imageData[i*3+1] = (char)color->data.fl[idx*3+1];
    dst_img->imageData[i*3+2] = (char)color->data.fl[idx*3+2];
  }

  // (6)画像を表示，キーが押されたときに終了します． 
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
  cvReleaseMat(&color);
  cvReleaseMat(&count);
  
  return 0;
}
