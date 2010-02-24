#include <cv.h>
#include <highgui.h>

using namespace cv;

Rect selection;
int select_object;

/* prototype declaration */
void on_mouse(int event, int x, int y, int flags, void* param);

int 
main(int argc, char *argv[])
{ 
  // (1)load a source image as is
  const char *imagename = argc > 1 ? argv[1] : "../image/bambi.png";
  Mat src_img = imread(imagename, -1);
  if(!src_img.data)
    return -1;

  // (2)create a window and set the callback function for mouse events
  namedWindow("Image", 1);
  cvSetMouseCallback("Image", (CvMouseCallback)(&on_mouse), &src_img);
  
  // (3)show the source image with an invert area, and quit when 'esc' pressed
  while(1) {
    Mat dst_img = src_img.clone();
    if(select_object && selection.width > 0 && selection.height > 0) {
      Mat roi(dst_img, selection);
      bitwise_xor(roi, Scalar::all(255), roi);
    }
    
    imshow("Image", dst_img);
    int key = waitKey(10);
    if(key==27)
      break;
  }

  return 0;
}

void
on_mouse(int event, int x, int y, int flags, void* param)
{
  static Point2i origin;
  Mat *img = static_cast<Mat*>(param);

  // (4)calculate coordinates of selected area (by Click and Drag)
  if(select_object) {
    selection.x = CV_IMIN(x, origin.x);
    selection.y = CV_IMIN(y, origin.y);
    selection.width = selection.x + CV_IABS(x - origin.x);
    selection.height = selection.y + CV_IABS(y - origin.y);

    selection.x = CV_IMAX(selection.x, 0);
    selection.y = CV_IMAX(selection.y, 0);
    selection.width = CV_IMIN( selection.width, img->cols );
    selection.height = CV_IMIN( selection.height, img->rows );
    selection.width -= selection.x;
    selection.height -= selection.y;
  }

  // (5)process a start and a finish selecting events (i.e. button-up and -down)
  switch(event) {
  case CV_EVENT_LBUTTONDOWN:
    origin = Point2i(x,y);
    selection = Rect(x, y, 0, 0);
    select_object = 1;
    break;
  case CV_EVENT_LBUTTONUP:
    select_object = 0;
    break;
  }
}

