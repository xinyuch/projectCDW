#include "opencv2/opencv.hpp"

using namespace cv;
using namespace std;

int main(int, char**)
{
    VideoCapture cap(0);
    if(!cap.isOpened()) return -1;

    Mat faces[6];
    namedWindow("Scan your Magic Cube", 1);
    namedWindow("Faces", 1);
    int num = 0;
    int k = 0;
    Mat frame, face;
    cap>>frame;
    Mat grid = 0*frame;
    for (int i = 120; i <= 480; i+=120)
    {
        for (int j = 480; j <= 600; j++)
        {
            grid.at<Vec3i>(i, j)[0] = 255;
            grid.at<Vec3i>(i, j)[1] = 255;
            grid.at<Vec3i>(i, j)[2] = 255;
        }
    }
    for (int i = 120; i <= 480; i++)
    {
        for (int j = 480; j <= 600; j+=40)
        {
            grid.at<Vec3i>(i, j)[0] = 255;
            grid.at<Vec3i>(i, j)[1] = 255;
            grid.at<Vec3i>(i, j)[2] = 255;
        }
    }
    //cout<<grid.at<int>(120, 120, 2);
    while(num < 6)
    {
        cap >> frame; 
        imshow("Scan your Magic Cube", frame+grid);
        k = waitKey(30);
        if (k >= 0)
        {
            cap >> face;
            faces[num] = face;
            num++;
            cout<<num<<"capture a fig\n";
            imshow("Faces",face);
            //while 'y' not pressed, wait for verification 
            while(k != 121) 
            {
                cap >> frame; 
                imshow("Scan your Magic Cube", frame+grid);
                k = waitKey(30);
                //not correct, discard, re-scan
                if(k == 110)
                {
                    num--;
                    break;
                }
            }
        }
    }
    cap.release();
    destroyWindow("Scan your Magic Cube");
    cout<<"end capture\n";
    return 0;
}