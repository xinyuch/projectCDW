#include "opencv2/opencv.hpp"
#include <sys/time.h>
//#include "types.hpp"

using namespace cv;

int main(int, char**)
{
    VideoCapture cap(0);
    if(!cap.isOpened()) return -1;

    Mat edges;
    Mat faces[6];
    namedWindow("Scan your Magic Cube",1);
    int num = 0;
    Mat frame, face;
    while(num < 6)
    {
        cap >> frame; 
        imshow("Scan your Magic Cube", frame);
        if (waitKey(30) == 'Y')
        {
            cap >> face;
            faces[num] = face;
            num++;
        }
    }
    destroyWindow("Scan your Magic Cube");
    namedWindow("Faces",1);
    Mat allFace;
    for (int i = 0; i < 6; i++)
    {
        allFace += faces[i];
    }
    imshow("Faces", allFace);
    return 0;
}