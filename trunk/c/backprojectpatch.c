#include <cv.h>
#include <highgui.h>

int
main (int argc, char **argv)
{
  int i, hist_size = 90;
  float h_ranges[] = { 0, 180 };
  float *ranges[] = { h_ranges };
  double max_val;
  CvSize dst_size;
  CvPoint max_loc;
  IplImage *search_img = 0, *tmp_img = 0, *dst_img;
  IplImage *search_hsv, *tmp_hsv;
  IplImage *search_planes[3], *tmp_planes[3];
  CvHistogram *hist = 0;
  char *imagename, *templatename;

  // (1)load a search-area image and a template image respectively
  imagename = argc > 1 ? argv[1] : "../image/room7.png";
  templatename = argc > 2 ? argv[2] : "../image/room7_temp.png";
  search_img = cvLoadImage (imagename, CV_LOAD_IMAGE_COLOR);
  tmp_img = cvLoadImage (templatename, CV_LOAD_IMAGE_COLOR);
  if(search_img == 0 || tmp_img == 0)
    return -1;

  // (2)create color planes buffer to calculate histograms
  for (i = 0; i < 3; i++) {
    search_planes[i] = cvCreateImage (cvGetSize (search_img), IPL_DEPTH_8U, 1);
    tmp_planes[i] = cvCreateImage (cvGetSize (tmp_img), IPL_DEPTH_8U, 1);
  }

  // (3)convert the color-space of input images from RGB to HSV
  search_hsv = cvCreateImage (cvGetSize (search_img), IPL_DEPTH_8U, 3);
  tmp_hsv = cvCreateImage (cvGetSize (tmp_img), IPL_DEPTH_8U, 3);
  cvCvtColor(search_img, search_hsv, CV_BGR2HSV);
  cvCvtColor(tmp_img, tmp_hsv, CV_BGR2HSV);
  cvCvtPixToPlane(search_hsv, search_planes[0], search_planes[1], search_planes[2], 0);
  cvCvtPixToPlane(tmp_hsv, tmp_planes[0], tmp_planes[1], tmp_planes[2], 0);

  // (4)calculate a histogram of hue-plane in a template image
  hist = cvCreateHist(1, &hist_size, CV_HIST_ARRAY, ranges, 1);
  cvCalcHist(&tmp_planes[0], hist, 0, 0);

  // (5)calculate distance between a template and a patch region in the source image
  //    by comparing their histograms
  dst_size = cvSize(search_img->width - tmp_img->width + 1,
		     search_img->height - tmp_img->height + 1);
  dst_img = cvCreateImage (dst_size, IPL_DEPTH_32F, 1);
  cvCalcBackProjectPatch (&search_planes[0], dst_img, cvGetSize (tmp_img), hist,
			  CV_COMP_CORREL, 1.0);

  // (6)draw a rectangle indicating template position
  cvMinMaxLoc(dst_img, NULL, &max_val, NULL, &max_loc, NULL);
  cvRectangle(search_img, max_loc,
	       cvPoint (max_loc.x + tmp_img->width,
			max_loc.y + tmp_img->height), CV_RGB (255, 0, 0), 3, 8, 0);
  
  // (7)show a matching result image and a probability image, and quit when any key pressed
  cvNamedWindow ("Match", CV_WINDOW_AUTOSIZE);
  cvNamedWindow ("Prob", CV_WINDOW_AUTOSIZE);
  cvShowImage ("Match", search_img);
  cvShowImage ("Prob", dst_img);
  cvWaitKey (0);
  
  cvDestroyWindow("Image");
  cvReleaseImage(&search_img);
  cvReleaseImage(&dst_img);
  cvReleaseImage(&tmp_img);
  cvReleaseImage(&search_hsv);
  cvReleaseImage(&tmp_hsv);
  for(i=0; i<3; i++) {
    cvReleaseImage(&search_planes[i]);
    cvReleaseImage(&tmp_planes[i]);
  }

  return 0;
}
