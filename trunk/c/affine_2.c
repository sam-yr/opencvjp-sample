#include <cv.h>
#include <highgui.h>

int main( int argc, char** argv )
{
  double        angle = -45.0, scale = 1.0;
  IplImage      *src_img = 0, *dst_img = 0;
  CvMat         *map_matrix;
  CvPoint2D32f   center;
  CvPoint        pt1, pt2;
  CvRect         rect;
		
  // (1)画像の読み込み，出力用画像領域の確保を行なう
  if (argc >= 2)
    src_img = cvLoadImage (argv[1], CV_LOAD_IMAGE_ANYDEPTH | CV_LOAD_IMAGE_ANYCOLOR);
  if( src_img == 0 )  return -1;
	
  rect.x      = (int)( src_img->width  * 0.25 );
  rect.y      = (int)( src_img->height * 0.25 );
  rect.width  = (int)( src_img->width  * 0.5  );
  rect.height = (int)( src_img->height * 0.5  );
  cvSetImageROI( src_img, rect );
  
  dst_img = cvCloneImage( src_img );
  
  // (2)角度，スケール係数，回転中心を指定して
  //    cv2DRotationMatrixを用いてアフィン行列を求める
  map_matrix = cvCreateMat( 2, 3, CV_32FC1 );
  center = cvPoint2D32f( src_img->width * 0.25, src_img->height*0.25 );
  cv2DRotationMatrix( center, angle, scale, map_matrix );
  
  // (3)指定されたアフィン行列により，cvWarpaffineを用いて画像のROI部分を回転させる
  cvWarpAffine( src_img, dst_img, map_matrix,
		CV_INTER_LINEAR+CV_WARP_FILL_OUTLIERS, cvScalarAll(255) );
  
  // (4)結果を表示する
  cvResetImageROI( src_img );
  cvResetImageROI( dst_img );
  
  pt1 = cvPoint( rect.x, rect.y );
  pt2 = cvPoint( rect.x + rect.width, rect.y + rect.height );
  cvRectangle( src_img, pt1, pt2, CV_RGB( 255, 0 , 255 ), 2, 8, 0 );
  
  cvNamedWindow( "src", CV_WINDOW_AUTOSIZE );
  cvNamedWindow( "dst", CV_WINDOW_AUTOSIZE );
  cvShowImage( "src", src_img );
  cvShowImage( "dst", dst_img );
  cvWaitKey( 0 );
  
  cvDestroyWindow("src");
  cvDestroyWindow("dst");
  cvReleaseImage(&src_img);
  cvReleaseImage(&dst_img);
  cvReleaseMat(&map_matrix);
	
  return 1;
}
