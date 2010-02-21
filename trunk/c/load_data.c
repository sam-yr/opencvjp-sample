#include <stdio.h>
#include <cv.h>

int
main (int argc, char **argv)
{
	char filename[] = "save_cv.xml";	// file name
	int i,j,k;
	CvFileStorage *cvfs;
	CvFileNode *node, *fn;
	CvSeq *s;
	int total;

	// (1)memory space for loading data
	int a;
	float b;
	CvMat** mat = (CvMat**)malloc(3*sizeof(CvMat*));

	// (2)open file storage
	cvfs = cvOpenFileStorage(filename,NULL,CV_STORAGE_READ);

	// (3)read data from file storage
	node = cvGetFileNodeByName(cvfs,NULL,"");	// Get Top Node

	a = cvReadIntByName(cvfs, node, "a");
	b = cvReadRealByName(cvfs, node, "b");

	fn = cvGetFileNodeByName(cvfs,node,"mat_array");

	s = fn->data.seq;
	total = s->total;
	for(i=0;i<total;i++){
		mat[i] = (CvMat*)cvRead(cvfs,(CvFileNode*)cvGetSeqElem(s,i));
	}

	// (4)close file storage
	cvReleaseFileStorage(&cvfs);

	// (5)print loaded data
	printf("a:%d\n", a);
	printf("b:%f\n", b);
	for(i=0; i<3; i++){
		printf("mat%d:\n",i);
		for(j=0;j<mat[i]->rows;j++){
			for(k=0;k<mat[i]->cols;k++){
				printf("%f,",cvmGet(mat[i],j,k));
			}
			printf("\n");
		}
	}

	// release mat
	for(i=0; i<3; i++){
		cvReleaseMat(mat+i);
	}
	free(mat);

	return 0;
}
