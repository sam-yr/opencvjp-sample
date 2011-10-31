#include <opencv2/opencv.hpp>
#include <iostream>
using namespace cv;
using namespace std;

// additional functions/////////////////////////////////////
void addNoiseSoltPepperMono(Mat& src, Mat& dest,double per)
{
	cv::RNG rng;
#pragma omp parallel for
	for(int j=0;j<src.rows;j++)
	{
		uchar* s=src.ptr(j);
		uchar* d=dest.ptr(j);
		for(int i=0;i<src.cols;i++)
		{
			double a1 = rng.uniform((double)0, (double)1);

			if(a1>per)
				d[i]=s[i];
			else
			{
				double a2 = rng.uniform((double)0, (double)1);
				if(a2>0.5)d[i]=0;
				else d[i]=255;
			}
		}
	}
}
void addNoiseMono(Mat& src, Mat& dest,double sigma)
{
	Mat s;
	src.convertTo(s,CV_16S);
	Mat n(s.size(),CV_16S);
	randn(n,0,sigma);
	Mat temp = s+n;
	temp.convertTo(dest,CV_8U);
}
void addNoise(Mat&src, Mat& dest, double sigma,double sprate=0.0)
{
	if(src.channels()==1)
	{
		addNoiseMono(src,dest,sigma);
		if(sprate!=0)addNoiseSoltPepperMono(dest,dest,sprate);
		return;
	}
	else
	{
		vector<Mat> s;
		vector<Mat> d(src.channels());
		split(src,s);
		for(int i=0;i<src.channels();i++)
		{
			addNoiseMono(s[i],d[i],sigma);
			if(sprate!=0)addNoiseSoltPepperMono(d[i],d[i],sprate);
		}
		cv::merge(d,dest);
	}
}
static double getPSNR(Mat& src, Mat& dest)
{
	int i,j;
	double sse,mse,psnr;
	sse = 0.0;


	for(j=0;j<src.rows;j++)
	{
		uchar* d=dest.ptr(j);
		uchar* s=src.ptr(j);
		for(i=0;i<src.cols;i++)
		{
			sse += ((d[i] - s[i])*(d[i] - s[i]));
		}
	}
	if(sse == 0.0)
	{
		return 0;
	}
	else
	{
		mse =sse /(double)(src.cols*src.rows);
		psnr = 10.0*log10((255*255)/mse);
		return psnr;
	}
}

double calcPSNR(Mat& src, Mat& dest)
{  
	Mat ssrc;
	Mat ddest;
	if(src.channels()==1)
	{
		src.copyTo(ssrc);
		dest.copyTo(ddest);
	}
	else
	{
		cvtColor(src,ssrc,CV_BGR2YUV);
		cvtColor(dest,ddest,CV_BGR2YUV);
	}
	double sn	= getPSNR(ssrc,ddest);
	return sn;
}
////////////////////////////////////////////////////////////////////////////////

//main implementaion
void nonlocalMeansFilter(Mat& src, Mat& dest, int templeteWindowSize, int searchWindowSize, double h, double sigma=0.0)
{
	if(templeteWindowSize>searchWindowSize)
	{
		cout<<"searchWindowSize should be larger than templeteWindowSize"<<endl;
		return;
	}
	if(dest.empty())dest=Mat::zeros(src.size(),src.type());

	const int tr = templeteWindowSize>>1;
	const int sr = searchWindowSize>>1;
	const int bb = sr+tr;
	const int D = searchWindowSize*searchWindowSize;
	const int H=D/2+1;
	const double div = 1.0/(double)D;//search area div
	const int tD = templeteWindowSize*templeteWindowSize;
	const double tdiv = 1.0/(double)(tD);//templete square div

	//create large size image for bounding box;
	Mat im;
	copyMakeBorder(src,im,bb,bb,bb,bb,cv::BORDER_DEFAULT);

	//weight computation;
	vector<double> weight(256*256*src.channels());
	double* w = &weight[0];
	const double gauss_sd = (sigma == 0.0) ? h :sigma;
	double gauss_color_coeff = -(1.0/(double)(src.channels()))*(1.0/(h*h));
	int nxt;
	for(int i = 0; i < 256*256*src.channels(); i++ )
	{
		double v = std::exp( max(i-2.0*gauss_sd*gauss_sd,0.0)*gauss_color_coeff);
		w[i] = v;
		if(v<0.001)
		{
			nxt=i;
			break;
		}
	}
	for(int i = nxt; i < 256*256*src.channels(); i++ )w[i] = 0.0;

	if(src.channels()==3)
	{
#pragma omp parallel for
		for(int j=0;j<src.rows;j++)
		{
			uchar* d = dest.ptr(j);
			uchar* ss = im.ptr(j);
			vector<int> ww(D);
			vector<double> nw(D);
			for(int i=0;i<src.cols;i++)
			{
				double tweight=0.0;
				//search loop

				const int bbj = bb+j;
				const int bbi = bb+i;
				for(int l=-sr,count=0;l<=sr;l++)
				{
					for(int k=-sr;k<=sr;k++)
					{
						//templete loop
						int e=0;
						for(int n=-tr;n<=tr;n++)
						{
							uchar* s = im.ptr(bbj+l+n); s+=3*(bbi+k);
							uchar* t = im.ptr(bbj+n);t+=3*(bbi);
							for(int m=-tr;m<=tr;m++)
							{
								// computing color L2 norm
								e += (s[0]-t[0])*(s[0]-t[0])+(s[1]-t[1])*(s[1]-t[1])+(s[2]-t[2])*(s[2]-t[2]);//L2 norm
								s+=3,t+=3;
							}
						}
						const int ediv = e*tdiv;
						ww[count++]=ediv;
						//get weighted Euclidean distance
						tweight+=w[ediv];
					}
				}
				//weight normalization
				if(tweight==0.0)
				{
					for(int z=0;z<D;z++) nw[z]=0;
					nw[H]=1;
				}
				else 
				{
					double itweight=1.0/(double)tweight;
					for(int z=0;z<D;z++) nw[z]=w[ww[z]]*itweight;
				}

				double r=0.0,g=0.0,b=0.0;
				for(int l=0,count=0;l<searchWindowSize;l++)
				{
					uchar* s = im.ptr(bbj+l-sr); s+=3*(bbi-sr);
					for(int k=0;k<searchWindowSize;k++)
					{
						r += s[0]*nw[count];
						g += s[1]*nw[count];
						b += s[2]*nw[count++];
						s+=3;
					}
				}
				d[0] = saturate_cast<uchar>(r);
				d[1] = saturate_cast<uchar>(g);
				d[2] = saturate_cast<uchar>(b);
				d+=3;
			}
		}
	}
	else if(src.channels()==1)
	{
#pragma omp parallel for
		for(int j=0;j<src.rows;j++)
		{
			uchar* d = dest.ptr(j);
			uchar* ss = im.ptr(j);
			vector<int> ww(D);
			vector<double> nw(D);
			for(int i=0;i<src.cols;i++)
			{
				double tweight=0.0;
				//search loop

				const int bbj = bb+j;
				const int bbi = bb+i;
				for(int l=-sr,count=0;l<=sr;l++)
				{
					for(int k=-sr;k<=sr;k++)
					{
						//templete loop
						int e=0;
						for(int n=-tr;n<=tr;n++)
						{
							uchar* s = im.ptr(bbj+l+n); s+=(bbi+k);
							uchar* t = im.ptr(bbj+n);t+=(bbi);
							for(int m=-tr;m<=tr;m++)
							{							
								e += (*s-*t)*(*s-*t);
								s++,t++;
							}
						}
						const int ediv = e*tdiv;
						ww[count++]=ediv;
						tweight+=w[ediv];
					}
				}
				//weight normalization
				if(tweight==0.0)
				{
					for(int z=0;z<D;z++) nw[z]=0;
					nw[H]=1;
				}
				else 
				{
					double itweight=1.0/(double)tweight;
					for(int z=0;z<D;z++) nw[z]=w[ww[z]]*itweight;
				}

				double v=0.0;
				for(int l=0,count=0;l<searchWindowSize;l++)
				{
					uchar* s = im.ptr(bbj+l-sr); s+=(bbi-sr);
					for(int k=0;k<searchWindowSize;k++)
					{
						v += *(s++)*nw[count++];
					}
				}
				*(d++) = saturate_cast<uchar>(v);
			}
		}
	}
}

int main(int argc, char** argv)
{
	//(1) Reading image and add noise(standart deviation = 15)
	const double noise_sigma = 15.0;
	Mat src = imread("lenna.png");
	Mat snoise;
	Mat dest;
	addNoise(src,snoise,noise_sigma);

	//(2) preview conventional method with PSNR
	//(2-1) RAW
	cout<<"RAW: "<<calcPSNR(src,snoise)<<endl<<endl;
	imwrite("noise.png",snoise);

	//(2-2) Gaussian Filter (7x7) sigma = 5
	int64 pre = getTickCount();
	GaussianBlur(snoise,dest,Size(7,7),5);
	cout<<"time: "<<1000.0*(getTickCount()-pre)/(getTickFrequency())<<" ms"<<endl;
	cout<<"gaussian: "<<calcPSNR(src,dest)<<endl<<endl;
	imwrite("gaussian.png",dest);

	//(2-3) median Filter (3x3)
	pre = getTickCount();
	medianBlur(snoise,dest,3);
	cout<<"time: "<<1000.0*(getTickCount()-pre)/(getTickFrequency())<<" ms"<<endl;
	cout<<"median: "<<calcPSNR(src,dest)<<endl<<endl;
	imwrite("median.png",dest);

	//(2-4) Bilateral Filter (7x7) color sigma = 35, space sigma = 5 
	pre = getTickCount();
	bilateralFilter(snoise,dest,7,35,5);
	cout<<"time: "<<1000.0*(getTickCount()-pre)/(getTickFrequency())<<" ms"<<endl;
	cout<<"bilateral: "<<calcPSNR(src,dest)<<endl<<endl;
	imwrite("bilateral.png",dest);


	//(3) analizing of performance of Nonlocal means filter
	pre = getTickCount();
	nonlocalMeansFilter(snoise,dest,3,7,noise_sigma,noise_sigma);
	cout<<"time: "<<1000.0*(getTickCount()-pre)/(getTickFrequency())<<" ms"<<endl;
	cout<<"nonlocal: "<<calcPSNR(src,dest)<<endl<<endl;
	imwrite("nonlocal.png",dest);

	imshow("noise", snoise);
	imshow("Non-local Means Filter", dest);
	waitKey();
	return 0;
}
