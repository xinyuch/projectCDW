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
    //row 720, col 1280
    float grid[720][1280][3] = {0};
    for (int i = 120; i <= 480; i+=120)
    {
        for (int j = 240; j <= 720; j++)
        {
            grid[i][j][0] = 255;
            grid[i][j][1] = 255;
            grid[i][j][2] = 255;
        }
    }
    for (int i = 120; i <= 480; i++)
    {
        for (int j = 240; j <= 720; j+=160)
        {
            grid[i][j][0] = 255;
            grid[i][j][1] = 255;
            grid[i][j][2] = 255;
        }
    }
    
    cap>>frame;
    Mat gridMat(720, 1280, frame.type(), &grid);
    imshow("Faces",gridMat);
    waitKey();
    /*
    int t = frame.type();
    for (int i = 120; i <= 480; i+=120)
    {
        for (int j = 240; j <= 720; j++)
        {
            grid.at<t>(i, j)[0] = 255;
            grid.at<t>(i, j)[1] = 255;
            grid.at<t>(i, j)[2] = 255;
        }
    }
    for (int i = 120; i <= 480; i++)
    {
        for (int j = 240; j <= 720; j+=160)
        {
            grid.at<t>(i, j)[0] = 255;
            grid.at<t>(i, j)[1] = 255;
            grid.at<t>(i, j)[2] = 255;
        }
    }
    cout<<grid.at<t>(120, 120)[2];
    */
    while(num < 6)
    {
        cap >> frame; 
        imshow("Scan your Magic Cube", frame);
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
                imshow("Scan your Magic Cube", frame);
                k = waitKey(30);
                //'n' pressed, not correct, discard, re-scan
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