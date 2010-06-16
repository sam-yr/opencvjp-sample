#include <iostream>
#include <cv.h>
#include <highgui.h>

using namespace cv;
using namespace std;

class DftIdftApp {
private:
  Mat src_img, mag_img;
  Mat Re_img, Im_img, Complex_img;
  Mat zero, dft_src, dft_dst, dft_dst_p;
  Mat idft_img;
  vector<Mat> mv;
  double min, max;
  string org_win, mag_win, idft_win;
public:
  DftIdftApp(const string filename);
  ~DftIdftApp(){};
  void calcMagImage();
  void showOrgImage();
  void showMagImage();
  void showIDFTImage();
  void shiftDFT(Mat &src, Mat &dst);
  void calcIDFT(bool all=false);
  void clear();
  static void onMouse(int event, int x, int y, int flags, void* param);
};

DftIdftApp::DftIdftApp(const string filename)
  :org_win("Original"), mag_win("Magnitude"), idft_win("IDTF")
{
  src_img = imread(filename, 0);
  if(!src_img.data) return;

  Size s_size = src_img.size();
  Re_img = Mat(s_size, CV_64F);
  Im_img = Mat::zeros(s_size, CV_64F);
  Complex_img = Mat(s_size, CV_64FC2);
  
  src_img.convertTo(Re_img, CV_64F);
  mv.push_back(Re_img);
  mv.push_back(Im_img);
  merge(mv, Complex_img);
  idft_img = zero = Mat::zeros(s_size, CV_64F);

  namedWindow(org_win, CV_WINDOW_AUTOSIZE);
  namedWindow(mag_win, CV_WINDOW_AUTOSIZE);
  namedWindow(idft_win, CV_WINDOW_AUTOSIZE);
  cvSetMouseCallback(mag_win.c_str(), DftIdftApp::onMouse, this);
}

void
DftIdftApp::clear()
{
  idft_img.setTo(0);
  calcMagImage();
}

void
DftIdftApp::calcMagImage()
{
  int dft_M = getOptimalDFTSize(src_img.rows-1);
  int dft_N = getOptimalDFTSize(src_img.cols-1);
  dft_src = Mat(dft_M, dft_N, CV_64FC2);
  
  Mat roi(dft_src, Rect(0, 0, src_img.cols, src_img.rows));
  Complex_img.copyTo(roi);
  if(dft_src.cols > src_img.cols) {
    roi = Mat(dft_src, Rect(src_img.cols, 0, dft_src.cols-src_img.cols, src_img.rows));
    roi.setTo(0);
  }

  dft(dft_src, dft_dst);
  dft_dst_p = dft_dst.clone();
  dft_dst_p.setTo(0);

  pow(dft_dst, 2.0, dft_src);
  split(dft_src, mv);
  pow(mv[0]+mv[1], 0.5, mv[0]); // mag=sqrt(Re^2+Im^2)
  log(mv[0]+1, mv[0]);          // log(mag+1)

  shiftDFT(mv[0], mv[0]);

  minMaxLoc(mv[0], &min, &max);
  mag_img = mv[0]*1.0/(max-min) - 1.0*min/(max-min);
}

void
DftIdftApp::shiftDFT(Mat &src, Mat& dst)
{
  Mat tmp;
  int cx = src.cols/2;
  int cy = src.rows/2;
  
  Mat q1(src, Rect(0,0,cx,cy));
  Mat q2(src, Rect(cx,0,cx,cy));
  Mat q3(src, Rect(cx,cy,cx,cy));
  Mat q4(src, Rect(0,cy,cx,cy));
  Mat d1(dst, Rect(0,0,cx,cy));
  Mat d2(dst, Rect(cx,0,cx,cy));
  Mat d3(dst, Rect(cx,cy,cx,cy));
  Mat d4(dst, Rect(0,cy,cx,cy));
  
  q3.copyTo(tmp);
  q1.copyTo(d3);
  tmp.copyTo(d1);
  q4.copyTo(tmp);
  q2.copyTo(d4);
  tmp.copyTo(d2);
}

void
DftIdftApp::showMagImage()
{
  imshow(mag_win, mag_img);
}

void
DftIdftApp::showOrgImage()
{
  imshow(org_win, src_img);
}

void
DftIdftApp::showIDFTImage()
{
  imshow(idft_win, idft_img);
}

void
DftIdftApp::onMouse(int event, int x, int y, int flags, void* param)
{
  DftIdftApp *app = static_cast<DftIdftApp*>(param);
  Mat &dft_src = app->dft_src;
  Mat &dft_dst = app->dft_dst;
  Mat &dft_dst_p = app->dft_dst_p;
  Mat &idft_img = app->idft_img;
  Mat &mag_img = app->mag_img;
  vector<Mat> &mv = app->mv;
  int cx = app->src_img.cols/2;
  int cy = app->src_img.rows/2;
  int mx=x, my=y;
  int w = dft_src.cols;
  int h = dft_src.rows;

  switch(event) {
  case CV_EVENT_MOUSEMOVE:
    if((flags&CV_EVENT_FLAG_LBUTTON)==0) return;
  case CV_EVENT_LBUTTONUP:
    if(flags&CV_EVENT_FLAG_CTRLKEY) {
      int step = 5;
      for(int j=-step; j<=step; j++) {
        my = y+j;
        my += (y+j)<cy ? cy:-cy;
        if(y+j<0 || y+j>=h) continue;
        double *from = dft_dst.ptr<double>(my);
        double *to = dft_dst_p.ptr<double>(my);
        double *mag = mag_img.ptr<double>(y+j);
        for(int i=-step; i<=step; i++) {
          mx = x+i;
          mx += (x+i)<cx ? cx:-cx;
          if(x+i<0 || x+i>=w) break;
          to[(mx)*2+0] = from[(mx)*2+0];
          to[(mx)*2+1] = from[(mx)*2+1];
          mag[x+i] = 0;
        }
      }
    } else {
      mx += x<cx ? cx:-cx;
      my += y<cy ? cy:-cy;
      double *from = dft_dst.ptr<double>(my);
      double *to = dft_dst_p.ptr<double>(my);
      double *mag = mag_img.ptr<double>(y);
      to[(mx)*2+0] = from[(mx)*2+0];
      to[(mx)*2+1] = from[(mx)*2+1];
      mag[x] = 0;
    }
    break;
  default:
    return;
  }
  app->calcIDFT();
  
}

void
DftIdftApp::calcIDFT(bool all)
{
  if(all) {
    dft_dst.copyTo(dft_dst_p);
    mag_img.setTo(0);
  }

  double min, max;
  idft(dft_dst_p, dft_src);
  split(dft_src, mv);
  minMaxLoc(mv[0], &min, &max);
  idft_img = Mat(mv[0]*1.0/max, Rect(0, 0, src_img.cols, src_img.rows));
}

int
main(int argc, char *argv[])
{
  const string filename = argc > 1 ? argv[1] : "../image/Fourier.png";
  DftIdftApp app(filename);

  cout << "Usage: click and drag on Magnitude Image.\n" <<
    "Hot keys: \n"
    "\tESC - quit the program\n"
    "\ta - select all pixels\n"
    "\tr - restore original images\n"
    "\tleft mouse button - select one pixel\n"
    "\tCTRL+left mouse button - select a pixel and neighbors\n";

  app.showOrgImage();
  app.calcMagImage();
  while(1) {
    int c = waitKey(10);
    switch((char)c) {
    case '\x1b':
      return 0;
    case 'a':
      app.calcIDFT(true);
      break;
    case 'r':
      app.clear();
      break;
    }
    app.showIDFTImage();
    app.showMagImage();
  }

  return 0;
}

