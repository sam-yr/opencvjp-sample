#include <cv.h>
#include <highgui.h>

int
main (int argc, char **argv)
{
  int i, j, bin_w;
  int hist_size = 256;
  int sch = 0, ch_width = 260;
  float max_value = 0;
  float range_0[] = { 0, 256 };
  float *ranges[] = { range_0 };
  IplImage *src_img = 0, *dst_img[4] = { 0, 0, 0, 0 }, *hist_img;
  CvHistogram *hist[3];
  char *imagename;

  // (1)画像を読み込み，入力画像のチャンネル数分の画像領域を確保します． 
  imagename = argc > 1 ? argv[1] : "flower.png";
  src_img = cvLoadImage(imagename, CV_LOAD_IMAGE_ANYCOLOR);
  if(src_img == 0)
    return -1;
  sch = src_img->nChannels;
  for (i = 0; i < sch; i++) {
    dst_img[i] = cvCreateImage(cvSize(src_img->width, src_img->height), src_img->depth, 1);
    hist[i] = cvCreateHist(1, &hist_size, CV_HIST_ARRAY, ranges, 1);
  }

  // (2)ヒストグラム構造体，ヒストグラム画像領域を確保します． 
  hist_img = cvCreateImage(cvSize(ch_width * sch, 200), 8, 3);

  if (sch == 1) {
    // (3a)入力画像がシングルチャンネルの場合，そのチャンネルのヒストグラムを計算します． 
    cvCopy(src_img, dst_img[0], NULL);
    cvCalcHist (&dst_img[0], hist[0], 0, NULL);
    cvGetMinMaxHistValue (hist[0], 0, &max_value, 0, 0);
  } else {
    // (3b)入力画像がマルチチャンネルの場合，画像をチャンネル毎に分割してヒストグラムを計算します． 
    cvSplit(src_img, dst_img[0], dst_img[1], dst_img[2], dst_img[3]);
    for (i = 0; i < sch; i++) {
      float tmp_val;
      cvCalcHist (&dst_img[i], hist[i], 0, NULL);
      cvGetMinMaxHistValue (hist[i], 0, &tmp_val, 0, 0);
      max_value = max_value < tmp_val ? tmp_val : max_value;
    }
  }

  // (4)ヒストグラムをスケーリングして，描画します． 
  cvSet(hist_img, cvScalarAll (255), 0);
  CvScalar color = cvScalarAll(100);
  for (i = 0; i < sch; i++) {
    if(sch==3)
      color = cvScalar((0xaa<<i*8)&0x0000ff,(0xaa<<i*8)&0x00ff00,(0xaa<<i*8)&0xff0000, 0);
    cvScale (hist[i]->bins, hist[i]->bins, ((double) hist_img->height) / max_value, 0);
    bin_w = cvRound ((double) ch_width / hist_size);
    for (j = 0; j < hist_size; j++)
      cvRectangle (hist_img,
		   cvPoint (j * bin_w + (i * ch_width), hist_img->height),
		   cvPoint ((j + 1) * bin_w + (i * ch_width),
			    hist_img->height -
			    cvRound (cvGetReal1D (hist[i]->bins, j))),
		   color, -1, 8, 0);
  }

  // (5)ヒストグラム画像を表示，キーが押されたときに終了します． 
  cvNamedWindow ("Image", CV_WINDOW_AUTOSIZE);
  cvShowImage ("Image", src_img);
  cvNamedWindow ("Histogram", CV_WINDOW_AUTOSIZE);
  cvShowImage ("Histogram", hist_img);
  cvWaitKey (0);

  cvDestroyWindow("Histogram");
  cvReleaseImage(&src_img);
  cvReleaseImage(&hist_img);
  for(i=0; i<sch; i++) {
    cvReleaseImage(&dst_img[i]);
    cvReleaseHist(&hist[i]);
  }

  return 0;
}
