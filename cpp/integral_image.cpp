#include <iostream>
#include <fstream>
#include <opencv2/opencv.hpp>
using namespace std;
using namespace cv;

void myboxBlur(Mat& src, Mat& dest, Size ksize);
void myintegral(Mat& src, Mat& dest);
void boxBlurIntegral(Mat& src, Mat& dest, Size ksize);

void speedTest(Mat& src);

int main(int argc, char** argv)
{
	Mat src = imread("garden.png");
	if(src.empty())cout<<"please input valid file"<<endl;

	Mat destMY;//my blur function
	Mat destII;//integral image
	Mat destCV;//OpenCV blur function
	Size kernel = Size(15,15);

	//(1) my basic implimentaion of box blur filtering
	int64 t = getTickCount();
	myboxBlur(src,destMY,kernel);
	cout<<"my box"<<(getTickCount()-t)*1000.0/getTickFrequency()<<"ms"<<endl;
	imshow("test",destMY);
	waitKey();

	//(3)integral image based implimentaion
	t = getTickCount();
	boxBlurIntegral(src,destII,kernel);
	cout<<"integral box"<<(getTickCount()-t)*1000.0/getTickFrequency()<<"ms"<<endl;
	imshow("test",destII);
	waitKey();

	t = getTickCount();
	//(5) OpenCV original implimentation
	boxFilter(src,destCV,-1,kernel);
	cout<<"cv box"<<(getTickCount()-t)*1000.0/getTickFrequency()<<"ms"<<endl;
	imshow("test",destCV);
	waitKey();

	//graph plot test
	speedTest(src);

	return 0;
}

void myboxBlur(Mat& src, Mat& dest, Size ksize)
{
	if(dest.empty())dest.create(src.size(),src.type());

	const int kh = ksize.height/2;
	const int kw = ksize.width/2;
	const double div  = 1.0/ksize.area();
	Mat im;
	//(2) make border image for outof range of kernel window
	copyMakeBorder(src,im,kh,kh,kw,kw,BORDER_REPLICATE);

#pragma omp parallel for
	for(int j=0;j<dest.rows;j++)
	{
		uchar* d = dest.ptr<uchar>(j);
		for(int i=0;i<dest.cols;i++)
		{
			int r=0;
			int g=0;
			int b=0;
			for(int l=0;l<ksize.height;l++)
			{
				uchar* s = im.ptr<uchar>(j+l);
				s+=3*i;
				for(int k=0;k<ksize.width;k++)
				{
					r += s[0];
					g += s[1];
					b += s[2];
					s+=3;
				}
			}
			d[0]=saturate_cast<uchar>(r*div);
			d[1]=saturate_cast<uchar>(g*div);
			d[2]=saturate_cast<uchar>(b*div);
			d+=3;
		}
	}
}

void boxBlurIntegral(Mat& src, Mat& dest, Size ksize)
{
	if(dest.empty())dest.create(src.size(),src.type());	
	const int kh = ksize.height/2;
	const int kw = ksize.width/2;
	const double div  = 1.0/ksize.area();
	Mat im;
	copyMakeBorder(src,im,kh,kh,kw,kw,BORDER_REPLICATE);

	Mat sum;
	integral(im,sum,CV_32SC3);
	const int step = 3*ksize.width;
#pragma omp parallel for
	for(int j=0;j<dest.rows;j++)
	{
		uchar* d = dest.ptr<uchar>(j);
		int* sp=sum.ptr<int>(j+ksize.height);
		int*  s=sum.ptr<int>(j);
		for(int i=0;i<dest.cols;i++)
		{
			//(4) get box blur value from integral image
			d[0]=saturate_cast<uchar>((s[0]-s[step+0]-sp[0]+sp[step+0])*div);
			d[1]=saturate_cast<uchar>((s[1]-s[step+1]-sp[1]+sp[step+1])*div);
			d[2]=saturate_cast<uchar>((s[2]-s[step+2]-sp[2]+sp[step+2])*div);
			d+=3;
			s+=3;
			sp+=3;
		}
	}
}

void speedTest(Mat& src)
{
	ofstream plot("plot");
	const int maxiter=10;

	Mat dest;

	double t1=0.0;
	double t2=0.0;
	double t3=0.0;
	for(int k=0;k<20;k++)
	{
		cout<<2*k+1<<endl;
		Size kernel = Size(2*k+1,2*k+1);
		for(int i=0;i<maxiter;i++)
		{
			int64 t = getTickCount();
			myboxBlur(src,dest,kernel);
			t1 += (getTickCount()-t)*1000.0/getTickFrequency();

			t = getTickCount();
			boxBlurIntegral(src,dest,kernel);
			t2 += (getTickCount()-t)*1000.0/getTickFrequency();

			t = getTickCount();
			boxFilter(src,dest,-1,kernel);
			t3 += (getTickCount()-t)*1000.0/getTickFrequency();

		}
		cout<<"my       box"<<t1/maxiter<<"ms"<<endl;
		cout<<"integral box"<<t2/maxiter<<"ms"<<endl;
		cout<<"cv       box"<<t3/maxiter<<"ms"<<endl;


		plot<<2*k+1<<" "<<t1<<" "<<t2<<" "<<t3<<endl;
	}
}
