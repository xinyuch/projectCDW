#include "opencv2/opencv.hpp"

using namespace cv;
using namespace std;

int main(int, char**)
{
    VideoCapture cap(0);
    if(!cap.isOpened()) return -1;

    Mat edges;
    Mat faces[6];
    namedWindow("Scan your Magic Cube", WINDOW_NORMAL);
    resizeWindow("Scan your Magic Cube",400,400);
    int num = 0;
    Mat frame, face;
    while(num < 6)
    {
        cap >> frame; 
        imshow("Scan your Magic Cube", frame);
        int k = waitKey(30);
        if (k >= 0)
        {
            cap >> face;
            faces[num] = face;
            num++;
            cout<<k<<"capture a fig\n";
        }
    }
    cap.release();
    destroyWindow("Scan your Magic Cube");
    cout<<"end capture\n";

    namedWindow("Faces",1);
    for (int i = 0; i < 6; i++)
    {
        imshow("Faces",faces[i]);
        waitKey(1000);
        cout<<i<<endl;
    }
    return 0;
}