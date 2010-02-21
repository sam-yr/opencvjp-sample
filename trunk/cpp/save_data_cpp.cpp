#include <cv.h>

using namespace std;
using namespace cv;

int
main (int argc, char **argv)
{
	char filename[] = "save_cv.xml";	// file name

	// (1)create sample data
	int a = 10;
	float b = 0.1;

	cv::Mat mat[3];
	for(int i=0; i<3; i++){
		mat[i].create(3,3,CV_32FC1);
		mat[i].setTo(cv::Scalar(i));
	}

	// (2)open file storage
	cv::FileStorage	cvfs(filename,CV_STORAGE_WRITE);

	// (3)write data to file
	cv::write(cvfs,"a",a);
	cv::write(cvfs,"b",b);
	cv::WriteStructContext ws(cvfs, "mat_array", CV_NODE_SEQ);	// create node
	for(int i=0; i<3; i++){
		cv::write(cvfs,"",mat[i]);
	}

	return 0;
}
