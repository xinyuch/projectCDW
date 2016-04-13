#include "opencv2/opencv.hpp"
#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui/highgui.hpp"

#include <iostream>
#include <math.h>
#include <string.h>

using namespace cv;
using namespace std;

static double angle( Point pt1, Point pt2, Point pt0 )
{
    double dx1 = pt1.x - pt0.x;
    double dy1 = pt1.y - pt0.y;
    double dx2 = pt2.x - pt0.x;
    double dy2 = pt2.y - pt0.y;
    return (dx1*dx2 + dy1*dy2)/sqrt((dx1*dx1 + dy1*dy1)*(dx2*dx2 + dy2*dy2) + 1e-10);
}

static void findSquares( const Mat& image, vector<vector<Point> >& squares )
{
    int thresh = 50, N = 12;
    squares.clear();

    Mat pyr, timg, gray;
    Mat gray0(image.size(), CV_8U);

    vector<vector<Point> > contours;

    for( int c = 0; c < 3; c++ )
    {
        int ch[] = {c, 0};
        mixChannels(&image, 1, &gray0, 1, ch, 1);
        
        for( int l = 0; l < N; l++ )
        {
            if( l == 0 )
            {
                Canny(gray0, gray, 0, thresh, 5);
                dilate(gray, gray, Mat(), Point(-1,-1));
            }
            else
            {
                gray = gray0 >= (l+1)*255/N;
            }

            findContours(gray, contours, RETR_LIST, CHAIN_APPROX_SIMPLE);

            vector<Point> approx;

            for( size_t i = 0; i < contours.size(); i++ )
            {
                approxPolyDP(Mat(contours[i]), approx, arcLength(Mat(contours[i]), true)*0.02, true);
                if( approx.size() == 4 &&
                    fabs(contourArea(Mat(approx))) > 1000 &&
                    isContourConvex(Mat(approx)) )
                {
                    double maxCosine = 0;

                    for( int j = 2; j < 5; j++ )
                    {
                        double cosine = fabs(angle(approx[j%4], approx[j-2], approx[j-1]));
                        maxCosine = MAX(maxCosine, cosine);
                    }
                    if( maxCosine < 0.2 )
                    {
                        squares.push_back(approx);
                    }
                }
            }
        }
    }
}

static void drawSquares( Mat& image, const vector<vector<Point> >& squares )
{
    const char* wndname = "Cell Recognition";
    for( size_t i = 0; i < squares.size(); i++ )
    {
        const Point* p = &squares[i][0];
        int n = (int)squares[i].size();
        polylines(image, &p, &n, 1, true, Scalar(0,255,0), 3, LINE_AA);
    }
    imshow(wndname, image);
}

int main(int, char**)
{   
    
    VideoCapture cap(0);
    if(!cap.isOpened()) return -1;

    Mat faces[6], frame, face;
    vector<vector<Point> > squares;
    Mat blank, src_gray, dst, canny_output;

    int num = 0;
    int k = 0;
    /*
    cap>>frame;
    Mat grid = 0*frame;
    for (int i = 120; i <= 560; i+=140)
    {
        for (int j = 500; j <= 620; j++)
        {
            grid.at<Vec3i>(i, j)[0] = 255;
            grid.at<Vec3i>(i, j)[1] = 255;
            grid.at<Vec3i>(i, j)[2] = 255;
        }
    }
    for (int i = 120; i <= 560; i+=4)
    {
        for (int j = 500; j <= 620; j+=40)
        {
            grid.at<Vec3i>(i, j)[0] = 255;
            grid.at<Vec3i>(i, j)[1] = 255;
            grid.at<Vec3i>(i, j)[2] = 255;
        }
    }
    for (int i = 120; i <= 560; i+=140)
    {
        for (int j = 340; j <= 460; j++)
        {
            grid.at<Vec3i>(i, j)[0] = 255;
            grid.at<Vec3i>(i, j)[1] = 255;
            grid.at<Vec3i>(i, j)[2] = 255;
        }
    }
    for (int i = 120; i <= 560; i+=4)
    {
        for (int j = 340; j <= 460; j+=40)
        {
            grid.at<Vec3i>(i, j)[0] = 255;
            grid.at<Vec3i>(i, j)[1] = 255;
            grid.at<Vec3i>(i, j)[2] = 255;
        }
    }
    */
    while(num < 6)
    {
        cap >> frame; 
        int threshold_value = 215;
        int threshold_type = 3;
        int const max_value = 255;
        int const max_type = 4;
        int const max_BINARY_value = 255;

        threshold( frame, dst, threshold_value, max_BINARY_value, threshold_type );
        findSquares(dst, squares);
        drawSquares(frame, squares);

        k = waitKey(30);
        if (k == 'c')
        {
            faces[num] = frame;
            num++;
            cout<<num<<" capture a fig\n";
            //while 'y' not pressed, wait for verification 
            while(k != 'y' && k != 'n') 
            {
                k = waitKey(30);
                //correct, store, scan the next face
                if(k == 'y')
                {
                    for (int i = 0; i < squares.size(); i++)
                    {
                        for (int j = 0; j < squares[i].size(); j++)
                        {
                            cout<<"("<<squares[i][j].x<<","<<squares[i][j].y<<") ";
                        }
                        cout<<endl;
                    }
                    switch(num)
                    {
                        case 1:
                            cout<<imwrite("img1.png",frame)<<endl;
                            break;
                        case 2:
                            cout<<imwrite("img2.png",frame)<<endl;
                            break;
                        case 3:
                            cout<<imwrite("img3.png",frame)<<endl;
                            break;
                        case 4:
                            cout<<imwrite("img4.png",frame)<<endl;
                            break;
                        case 5:
                            cout<<imwrite("img5.png",frame)<<endl;
                            break;
                        case 6:
                            cout<<imwrite("img6.png",frame)<<endl;
                            break;
                    }
                    cout<<num<<" store the fig\n";
                    break;
                }
                //not correct, discard, re-scan
                if(k == 'n')
                {
                    cout<<"discard the fig\n";
                    num--;
                    break;
                }
            }
        }
    }
    cap.release();
    cout<<"end capture\n";
    destroyAllWindows();
    /*
    string tmp;
    cin>>tmp;
    while(tmp != "quit")
    {
        cin>>tmp;
    }
    */
    return 0;
}