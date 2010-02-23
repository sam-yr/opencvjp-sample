#include <iostream>
#include <cv.h>

using namespace std;
using namespace cv;

int
main (int argc, char **argv)
{
	char filename[] = "save_cv.xml";	// file name
	int i,j,k;

	// (1)memory space for loading data
	int a;
	float b;
	cv::Mat mat[3];

	// (2)open file storage
	cv::FileStorage	cvfs(filename,CV_STORAGE_READ);

	// (3)read data from file storage
	cv::FileNode node(cvfs.fs, NULL);	// Get Top Node
	a = node["a"];
	b = node["b"];
	cv::FileNode fn = node["mat_array"];
	
	for(i=0;i<fn.size();i++){
		cv::read(fn[i], mat[i]);
	}

	// (4)print loaded data
	cout << "a:" << a << endl;
	cout << "b:" << b << endl;

	for(i=0; i<3; i++){
		cout << "mat" << i << ":" << endl;
		for(j=0;j<mat[i].rows;j++){
			for(k=0;k<mat[i].cols;k++){
				cout << mat[i].at<float>(j,k) << ",";
			}
			cout << endl;
		}
	}

	return 0;
}
