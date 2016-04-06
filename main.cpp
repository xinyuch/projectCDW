#include "opencv2/opencv.hpp"
//#include "types.hpp"

using namespace cv;

int main(int, char**)
{
    VideoCapture cap(0);
    if(!cap.isOpened())
        return -1;

    Mat edges;
    Mat red, orange, green, blue, white, yellow;
    namedWindow("Scan your Magic Cube",1);
    namedWindow("display",1);

        bool flag = 1;
        while(flag)
        {
            Mat frame;
            cap >> frame; 
            imshow("Scan your Magic Cube", frame);
            char k = waitKey(30);
            if (k == 'Y')
            {
                cap >> red;
                imshow("display", red);
                /*switch(i)
                {
                    case 0:
                        cap >> red;
                        imshow("display", red);
                        break;
                    case 1:
                        cap >> orange;
                        imshow("display", orange);
                        break;
                    case 2:
                        cap >> green;
                        imshow("display", green);
                        break;
                    case 3:
                        cap >> blue;
                        imshow("display", blue);
                        break;
                    case 4:
                        cap >> white;
                        imshow("display", white);
                        break;
                    case 5:
                        cap >> yellow;
                        imshow("display", yellow);
                        break;
                }
                */
                flag = 0;
            }
        
    }

    return 0;
}