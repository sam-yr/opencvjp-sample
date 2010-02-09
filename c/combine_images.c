#include <cv.h>
#include <highgui.h>

int
main (int argc, char **argv)
{
  int i, img_num;
  int total_width=0, max_height=0;
  IplImage **src_img;
  IplImage *combined_img;
  CvRect roi = cvRect(0, 0, 0, 0);

  // (1)load all images specified on the command line
  if(argc < 2) {
    return 0;
  } else {
    img_num = argc - 1;
    src_img = (IplImage**)cvAlloc(sizeof(IplImage*)*img_num);
    for(i=0; i<img_num; i++) {
      src_img[i] = cvLoadImage (argv[i+1], CV_LOAD_IMAGE_COLOR);
      if(src_img[i] == 0)
	return -1;
      total_width += src_img[i]->width;
      max_height = max_height < src_img[i]->height ? src_img[i]->height : max_height;
    }
  }

  // (2)append images one after another
  combined_img = cvCreateImage(cvSize(total_width, max_height), IPL_DEPTH_8U, 3);
  cvZero(combined_img);
  for(i=0; i<img_num; i++) {
    roi.width = src_img[i]->width;
    roi.height = src_img[i]->height;
    cvSetImageROI(combined_img, roi);
    cvCopy(src_img[i], combined_img, NULL);
    roi.x += roi.width;
  }
  cvResetImageROI(combined_img);

  // (3)show the combined image, and quit when any key pressed
  cvNamedWindow ("Image", CV_WINDOW_AUTOSIZE);
  cvShowImage ("Image", combined_img);
  cvWaitKey (0);

  cvDestroyWindow("Image");
  cvReleaseImage(&combined_img);
  for(i=0; i<img_num; i++) {
    cvReleaseImage(&src_img[i]);
  }
  cvFree(&src_img);

  return 0;
}
