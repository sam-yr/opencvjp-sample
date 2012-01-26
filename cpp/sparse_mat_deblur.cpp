#include <cv.h>
#include <highgui.h>
using namespace cv;

#include <iostream>
using namespace std;

SparseMat createblurBoxSparseMat32f(Mat& src, Size window);
Mat visualizeSparseMat(SparseMat& smat, Size out_imsize);

void blurSparseMat32f(Mat& src, Mat& dest, SparseMat& H);
void deblurSparseMat32f(Mat& src, Mat& dest, SparseMat& H, int iteration, float beta);
void mulSparseMat32f(SparseMat& smat, Mat& src, Mat& dest, bool isTranspose = false);

void unsharpMaskBox(Mat& src, Mat& dest,Size window);

int main(int argc, char** argv)
{
	Mat image = imread("lenna.png");//input image
	Mat bimage(image.size(),CV_8UC3);// blured image
	Mat dbimage(image.size(),CV_8UC3);// deblured blured image
	Mat dbimage2(image.size(),CV_8UC3);// deblured blured image for unsharp mask

	Size kernelwin = Size(7,7);//blur kernel is 7x7 box matrix
	int64 time;
	double f = 1000.0/getTickFrequency();

	//(1) generate sparse matrix of point spread function
	time = getTickCount();
	SparseMat psf=createblurBoxSparseMat32f(image, kernelwin);
	cout<<"generating point spread fuction time:"<<(getTickCount()-time)*f<<" ms"<<endl;

	//(2) visualize the sparse matrix
	time = getTickCount();
	Mat smatim = visualizeSparseMat(psf, Size(256,256));
	cout<<"visualizing sparse matrix time:"<<(getTickCount()-time)*f<<" ms"<<endl;
	imshow("Sparse Mat",smatim);
	waitKey(10);

	//(3) blur a input image  with the sparse matrix
	time = getTickCount();
	blurSparseMat32f(image,bimage,psf);
	cout<<"bluring time:"<<(getTickCount()-time)*f<<" ms"<<endl;

	//(4) deblur the degraded image with a steepest descent method for L2 norm minimization
	time = getTickCount();
	deblurSparseMat32f(bimage,dbimage,psf,10, 0.2f);//iteration:10, beta = 0.2
	cout<<"debluring time:"<<(getTickCount()-time)*f<<" ms"<<endl;

	//(5) sharp the degraded image with common sharpness filter for mutch up
	time = getTickCount();
	unsharpMaskBox(bimage,dbimage2,kernelwin);
	cout<<"unsharpMask time:"<<(getTickCount()-time)*f<<" ms"<<endl;

	imshow("test",image); //input image
	waitKey();
	imshow("test",bimage);//blured image
	waitKey();
	imshow("test",dbimage);//deblured image
	waitKey();
	imshow("test",dbimage2);//unsharpmasked image
	waitKey();

	//(6)evaluation by L2 norm
	cout<<"norm: ideal <-> blured                "<<norm(image,bimage)<<endl;
	cout<<"norm: ideal <-> deblured              "<<norm(image,dbimage)<<endl;
	cout<<"norm: ideal <-> unsharpMask "<<norm(image,dbimage2)<<endl;
	return 0;
}

Mat visualizeSparseMat(SparseMat& smat, Size out_imsize)
{
	Mat data = Mat::zeros(out_imsize,CV_8U);
	double inv_size = 1.0/smat.size(0); 

	SparseMatIterator it = smat.begin(),it_end = smat.end();
	for(;it!=it_end;++it)
	{
		int j = (int)(((double)it.node()->idx[0]*inv_size*out_imsize.height));
		int i = (int)(((double)it.node()->idx[1]*inv_size*out_imsize.width));
		data.at<uchar>(j,i)++;
	}

	double minv,maxv;
	minMaxLoc(data,&minv,&maxv);
	data.convertTo(data,-1,255.0/maxv);

	Mat zeromat = Mat::zeros(out_imsize,CV_8U);
	vector<Mat> image;
	image.push_back(zeromat);
	image.push_back(data);
	image.push_back(zeromat);

	Mat ret;
	cv::merge(image,ret);

	cout<<"number of non zero elements: "<<smat.nzcount()<<endl;	
	return ret;
}

SparseMat createblurBoxSparseMat32f(Mat& src, Size window)
{
	int matsize =src.cols*src.rows ;
	int size[2]={matsize,matsize};
	SparseMat H(2,size,CV_32FC1);//sparse mat: image_size x image_size

	int bsy=window.height/2;// offset for kernel window
	int bsx=window.width/2;// offset for kernel window

	Mat kernel(window, CV_32FC1);
	kernel.setTo(1);
	normalize(kernel,kernel,1,0,NORM_L1);
	for(int j=0;j<src.rows;j++)
	{
		for(int i=0;i<src.cols;i++)
		{
			int y = src.cols*j+i;
			if(i>=bsx &&i<src.cols-bsx &&j>=bsy &&j<src.rows-bsy)
			{
				for(int l=0;l<window.height;l++)
				{
					for(int k=0;k<window.height;k++)
					{
						// higher-level element access functions:
						// ref<_Tp>(i0,...[,hashval]) - equivalent to *(_Tp*)ptr(i0,...true[,hashval]).
						//    always return valid reference to the element.
						//    If it's did not exist, it is created.
						// find<_Tp>(i0,...[,hashval]) - equivalent to (_const Tp*)ptr(i0,...false[,hashval]).
						//    return pointer to the element or NULL pointer if the element is not there.
						// value<_Tp>(i0,...[,hashval]) - equivalent to
						//    { const _Tp* p = find<_Tp>(i0,...[,hashval]); return p ? *p : _Tp(); }
						//    that is, 0 is returned when the element is not there.
						// note that _Tp must match the actual matrix type -
						// the functions do not do any on-fly type conversion
						H.ref<float>(y,y+src.cols*(l-bsy)+(k-bsx))=kernel.at<float>(l,k);
					}
				}
			}
			else //exception handling
			{
				H.ref<float>(y,y)=1.0;
			}
		}
	}
	return H;
}


void mulSparseMat32f(SparseMat& smat, Mat& src, Mat& dest, bool isTranspose)
{
	dest.setTo(0);
	SparseMatIterator it = smat.begin(),it_end = smat.end();
	if(!isTranspose)
	{
		for(;it!=it_end;++it)
		{	
			float* d = dest.ptr<float>(it.node()->idx[0]);
			float* s = src.ptr<float>(it.node()->idx[1]);
			for(int color=0;color<3;color++)
			{
				d[color]+= it.value<float>() * s[color];
			}
		}
	}
	else // for transpose matrix
	{
		for(;it!=it_end;++it)
		{	
			float* d = dest.ptr<float>(it.node()->idx[1]);
			float* s = src.ptr<float>(it.node()->idx[0]);
             for(int color=0;color<3;color++)
			{
				d[color]+= it.value<float>() * s[color];
			}
		}
	}
}

void deblurSparseMat32f(Mat& src, Mat& dest, SparseMat& H, int iteration, float beta)
{
	const int matsize = src.cols*src.rows;
	Mat dstvec(matsize,1,CV_32FC3);
	Mat svec; 
	//convert Mat image structure to 1D vecor structure
	src.reshape(3,matsize).convertTo(svec,CV_32FC3);

	//steepest descent method for L2 norm minimization
	for(int i=0;i<iteration;i++)
	{
		//blur input vector
		mulSparseMat32f(H,svec,dstvec);
		//subtract b blured image from non-blured image
		Mat temp = dstvec - svec;
		//blur the subtructed vector with transposed matrix
		mulSparseMat32f(H,temp,dstvec,true);
		//creep ideal image, beta is parameter of the creeping speed.
		svec -= (beta*dstvec);
	}
	//re-convert  1D vecor structure to Mat image structure
	svec.reshape(3,src.rows).convertTo(dest,CV_8UC3);
}

void blurSparseMat32f(Mat& src, Mat& dest, SparseMat& H)
{	
	const int matsize = src.cols*src.rows;
	Mat dstvec(matsize,1,CV_32FC3);
	//convert Mat image structure to 1D vecor structure
	Mat svec = src.reshape(3,matsize);

	//bluring operation
	SparseMatIterator it = H.begin(),it_end = H.end();
	for(;it!=it_end;++it)
	{	
		float* d = dstvec.ptr<float>(it.node()->idx[0]);
		uchar* s = svec.ptr<uchar>(it.node()->idx[1]);
        
        for(int color=0;color<3;color++)
		{
			d[color]+= it.value<float>() * s[color];
		}
	}

	//re-convert  1D vecor structure to Mat image structure
	dstvec.reshape(3,src.rows).convertTo(dest,CV_8UC3);
}

//unsharp mask for Mat image; it is not matrix domain representation, just image filtering.
void unsharpMaskBox(Mat& src, Mat& dest,Size window)
{
	Mat src_f;
	Mat dest_f;
	Mat sub_f;

	src.convertTo(src_f,CV_32F);

	blur(src_f,dest_f, window);
	subtract(src_f,dest_f,sub_f);
	add(src_f,sub_f,dest_f);

	dest_f.convertTo(dest,CV_8U);
}