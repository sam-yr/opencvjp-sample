#include <cv.h>

int
main (int argc, char **argv)
{
  char filename[] = "save_cv.xml";	// file name
  int i;
  int a;
  float b;
  CvMat** mat;
  CvFileStorage *cvfs;

  // (1)create sample data
  a = 10;
  b = 0.1;
  mat = (CvMat**)cvAlloc(3*sizeof(CvMat*));
  for(i=0;i<3;i++){
    mat[i] = cvCreateMat(3,3,CV_32FC1);
    cvSet(mat[i], cvScalarAll(i), NULL);
  }

  // (2)open file storage
  cvfs = cvOpenFileStorage(filename,NULL,CV_STORAGE_WRITE);

  // (3)write data to file
  cvWriteInt(cvfs, "a", a);
  cvWriteReal(cvfs, "b", b);

  cvStartWriteStruct(cvfs, "mat_array", CV_NODE_SEQ, NULL, cvAttrList(NULL, NULL));	// create node
  for(i=0; i<3; i++){
    cvWrite(cvfs, NULL, mat[i], cvAttrList(NULL, NULL));
  }
  cvEndWriteStruct(cvfs);

  // (4)close file storage
  cvReleaseFileStorage(&cvfs);

  // release mat
  for(i=0; i<3; i++){
    cvReleaseMat(mat+i);
  }
  cvFree(mat);

  return 0;
}
