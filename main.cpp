
#include "opencv2/opencv.hpp"
#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui/highgui.hpp"

#include <iostream>
#include <math.h>
#include <string>
#include <vector>
#include <algorithm>
#include "animation.h"

using namespace cv;
using namespace std;

enum Color {YELLOW, GREEN, RED, ORANGE, WHITE, BLUE};

static double angle( Point pt1, Point pt2, Point pt0 )
{
    double dx1 = pt1.x - pt0.x;
    double dy1 = pt1.y - pt0.y;
    double dx2 = pt2.x - pt0.x;
    double dy2 = pt2.y - pt0.y;
    return (dx1*dx2 + dy1*dy2)/sqrt((dx1*dx1 + dy1*dy1)*(dx2*dx2 + dy2*dy2) + 1e-10);
}

static void findsquare( const Mat& image, vector<vector<Point> >& square )
{
    int thresh = 10;
    square.clear();
    
    Mat pyr, timg, gray;
    Mat gray0(image.size(), CV_8U);
    
    vector<vector<Point> > contours;
    
    for( int c = 0; c < 3; c++ )
    {
        int ch[] = {c, 0};
        mixChannels(&image, 1, &gray0, 1, ch, 1);
        
        Canny(gray0, gray, 0, thresh, 5);
        dilate(gray, gray, Mat(), Point(-1,-1));
        
        findContours(gray, contours, RETR_LIST, CHAIN_APPROX_SIMPLE);
        
        vector<Point> approx;
        
        for( size_t i = 0; i < contours.size(); i++ )
        {
            approxPolyDP(Mat(contours[i]), approx, arcLength(Mat(contours[i]), true)*0.02, true);
            if( approx.size() == 4 &&
               fabs(contourArea(Mat(approx))) > 20000 &&
               isContourConvex(Mat(approx)) )
            {
                double maxCosine = 0;
                
                for( int j = 2; j < 5; j++ )
                {
                    double cosine = fabs(angle(approx[j%4], approx[j-2], approx[j-1]));
                    maxCosine = MAX(maxCosine, cosine);
                }
                if( maxCosine < 0.3 )
                {
                    square.push_back(approx);
                }
            }
        }
    }
    if (square.size() != 1) {
        square.clear();
    }
}

static void drawsquare( Mat& image, const vector<vector<Point> >& square )
{
    const char* wndname = "Cell Recognition";
    for( size_t i = 0; i < square.size(); i++ )
    {
        const Point* p = &square[i][0];
        int n = (int)square[i].size();
        polylines(image, &p, &n, 1, true, Scalar(0,255,0), 3, LINE_AA);
    }
    imshow(wndname, image);
    //    cout<<square.size()<<endl;
}

//*********************color detection helper functions*************************//
//h1 and s1 corresponds to testing color
//h2 and s2 corresponds to comparison color

double calculateDis(double h1, double s1, double h2, double s2) {
    double dh = min(abs(h1 - h2), 360 - abs(h1 - h2)) / 180.0;
    double ds = abs(s1 - s2);
    return sqrt(dh * dh + ds * ds);
}
//calculate 6 distance and find the smallest one to be the color
int findColor(double h, double s) {
    double colors[6];
    colors[YELLOW] = calculateDis(h, s, 60, 1.0);
    colors[GREEN] = calculateDis(h, s, 120, 1.0);
    colors[RED] = calculateDis(h, s, 0, 1.0);
    colors[ORANGE] = calculateDis(h, s, 30, 1.0);
    colors[WHITE] = calculateDis(h, s, 0, 0.0);
    colors[BLUE] = calculateDis(h, s, 240, 1.0);
    double min_dis = colors[0];
    int color_index = 0;
    for (int i = 1; i < 6; ++i) {
        if (min_dis > colors[i]) {
            min_dis = colors[i];
            color_index = i;
        }
    }
    return color_index;
}

//the first parameter is an image in RGB
//squares is a vector of size 9 which represents the center of
//each small square of a certain face
vector<int> ColorDetect(Mat img, vector<Point> squares) {
    vector<int> face_color(9, 0);
    Mat imgHSV;
    //img *= 1./255;
    cvtColor(img, imgHSV, CV_BGR2HSV);
    for (int i = 0; i < squares.size(); ++i) {
        Vec3b hsv = imgHSV.at<Vec3b>(squares[i].y, squares[i].x);
        int h = hsv.val[0] * 2;
        double s = hsv.val[1] / 255.0;
        double v = hsv.val[2] / 255.0;
        //int h = imgHSV.at<Vec3b>(squares[i].y, squares[i].x).val[0];
        //int s = imgHSV.at<Vec3b>(squares[i].y, squares[i].x).val[1];
        //int v = imgHSV.at<Vec3b>(squares[i].y, squares[i].x).val[2];
//        cout << "the color of face " << i << " is h = " << h << " s = " << s << " v = " << v << endl;
        face_color[i] = findColor(h, s);
    }
    return face_color;
}

vector<int> printFaceColor(Mat img, vector<Point> squares) {
    //Mat img = imread(filename, CV_LOAD_IMAGE_COLOR);
    // cout << img.type() << endl;
    //cout << img.cols << endl;
    //cout << img.rows << endl;
//    if(!img.data) {
//        cout << "image not loaded!" << endl;
//        exit(1);
//    }
    //    namedWindow("win1");
    //    imshow("win1", img);
    //    waitKey();
    
    //  vector<Square> squares;
    //  squares.push_back(Square(790, 211)); //brown (h should equal to 26)
    //  squares.push_back(Square(787, 284)); //pink...ish
    //  squares.push_back(Square(782, 357)); //green
    //  squares.push_back(Square(719, 213)); //brown-red (h = 28)
    //  squares.push_back(Square(717, 284)); //orange - yellow (h = 30)
    //  squares.push_back(Square(714, 354)); //red-orange (h = 20)
    //  squares.push_back(Square(649, 215)); //yellow (h = 48)
    //  squares.push_back(Square(647, 283)); //yellow (h = 58)
    //  squares.push_back(Square(645, 351)); // brown - red
    
    //test for bgr value
//    for (int i = 0; i < squares.size(); ++i) {
//        int b = img.at<Vec3b>(squares[i].y, squares[i].x)[0];
//        int g = img.at<Vec3b>(squares[i].y, squares[i].x)[1];
//        int r = img.at<Vec3b>(squares[i].y, squares[i].x)[2];
//        cout << "TEST: " << i << " B = " << b << " G = " << g << " R = " << r << endl;
//    }
    vector<int> face_color = ColorDetect(img, squares);
    //the order of cube color is vertical, but cube solver algorithm needs horizontal input
//    vector<int> face_color_ordered(9, 0);
//    face_color_ordered[0] = face_color[6];
//    face_color_ordered[1] = face_color[3];
//    face_color_ordered[2] = face_color[0];
//    face_color_ordered[3] = face_color[7];
//    face_color_ordered[4] = face_color[4];
//    face_color_ordered[5] = face_color[1];
//    face_color_ordered[6] = face_color[8];
//    face_color_ordered[7] = face_color[5];
//    face_color_ordered[8] = face_color[2];


//    for(int i = 0; i < 9; ++i) {
//        switch(face_color[i]) {
//            case 0:
//                cout << "YELLOW" << endl;
//                break;
//            case 1:
//                cout << "GREEN" << endl;
//                break;
//            case 2:
//                cout << "RED" << endl;
//                break;
//            case 3:
//                cout << "ORANGE" << endl;
//                break;
//            case 4:
//                cout << "WHITE" << endl;
//                break;
//            case 5:
//                cout << "BLUE" << endl;
//                break;
//        }
//    }
    return face_color;
}



int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    VideoCapture cap(0);
    if(!cap.isOpened()) return -1;
    
    Mat faces[6], frame, face;
    vector<vector<Point> > square;
    Mat blank, src_gray, dst, canny_output;
    
    int num = 0;
    int k = 0;
    
    //color of cells 6*9
    vector<vector<int>> cell_colors(6, vector<int>(9, 0));
 
    while(num < 6)
    {
        cap >> frame;
        int threshold_value = 215;
        int threshold_type = 3;
        int const max_BINARY_value = 255;
        mean(frame);
        threshold( frame, dst, threshold_value, max_BINARY_value, threshold_type );
        findsquare(dst, square);
        
        vector<Point> cells;
        
        if (square.size()) {
            Point a,b,c,d;
            Point mean = (square[0][0]+square[0][1]+square[0][2]+square[0][3])/4;
            for (int i = 0; i < 4; i++) {
                if (square[0][i].x < mean.x && square[0][i].y > mean.y) {
                    a = square[0][i];
                }
                else if (square[0][i].x > mean.x && square[0][i].y > mean.y)
                {
                    b = square[0][i];
                }
                else if (square[0][i].x > mean.x && square[0][i].y < mean.y)
                {
                    c = square[0][i];
                }
                else
                {
                    d = square[0][i];
                }
            }

            cells.push_back(Point(b.x/6+5*d.x/6, b.y/6+5*d.y/6));
            cells.push_back(Point((a.x+b.x)/12+5*(c.x+d.x)/12, (a.y+b.y)/12+5*(c.y+d.y)/12));
            cells.push_back(Point(a.x/6+5*c.x/6, a.y/6+5*c.y/6));
            cells.push_back(Point((b.x+c.x)/12+5*(a.x+d.x)/12, (b.y+c.y)/12+5*(a.y+d.y)/12));
            cells.push_back(Point((a.x+b.x+c.x+d.x)/4, (a.y+b.y+c.y+d.y)/4));
            cells.push_back(Point(5*(b.x+c.x)/12+(a.x+d.x)/12, 5*(b.y+c.y)/12+(a.y+d.y)/12));
            cells.push_back(Point(5*a.x/6+c.x/6, 5*a.y/6+c.y/6));
            cells.push_back(Point(5*(a.x+b.x)/12+(c.x+d.x)/12, 5*(a.y+b.y)/12+(c.y+d.y)/12));
            cells.push_back(Point(5*b.x/6+d.x/6, 5*b.y/6+d.y/6));
            for (int i = 0; i < 9; i++) {
                circle(frame, cells[i], 10, Scalar(0,255,0));
            }
        }
        drawsquare(frame, square);
        
        k = waitKey(30);
        if (k == 'c')
        {
            faces[num] = frame;
            num++;
            cout<<num<<" capture a fig\n";
            //color detection starts here
            vector<int> temp = printFaceColor(frame, cells);
//            for (int i = 0; i < 4; i++) {
//                cout<<square[0][i].x<<","<<square[0][i].y<<endl;
//            }
            for (int i = 0; i < cells.size(); i++) {
                cout<<cells[i].x<<","<<cells[i].y<<endl;
                switch(temp[i]) {
                    case YELLOW:
                        circle(frame, cells[i] - Point(200, 0), 10, Scalar(0,255,255), CV_FILLED);
                        break;
                    case GREEN:
                        circle(frame, cells[i] - Point(200, 0), 10, Scalar(0,255,0), CV_FILLED);
                        break;
                    case RED:
                        circle(frame, cells[i] - Point(200, 0), 10, Scalar(0,0,255), CV_FILLED);
                        break;
                    case ORANGE:
                        circle(frame, cells[i] - Point(200, 0), 10, Scalar(20,100,240), CV_FILLED);
                        break;
                    case WHITE:
                        circle(frame, cells[i] - Point(200, 0), 10, Scalar(255,255,255), CV_FILLED);
                        break;
                    case BLUE:
                        circle(frame, cells[i] - Point(200, 0), 10, Scalar(255,0,0), CV_FILLED);
                        break;
                }
            }
            imshow("Cell Recognition", frame);
            vector<int> temp_ordered = temp;
            
//            if (temp_ordered[4] == 2 || temp_ordered[4] == 4) {
//                vector<int> tmp = temp_ordered;
//                temp_ordered= {tmp[2], tmp[5], tmp[8], tmp[1], tmp[4], tmp[7], tmp[0], tmp[3], tmp[6]};
//            }
            for(int i = 0; i < 9; i++) {
                switch(temp_ordered[i]) {
                    case 0:
                        temp_ordered[i] = 4;
                        break;
                    case 1:
                        temp_ordered[i] = 3;
                        break;
                    case 2:
                       temp_ordered[i] = 5;
                        break;
                    case 3:
                        temp_ordered[i] = 1;
                        break;
                    case 4:
                        temp_ordered[i] = 0;
                        break;
                    case 5:
                        temp_ordered[i] = 2;
                        break;
                }
            }
            

            
            //while 'y' not pressed, wait for verification
            while(k != 'y' && k != 'n')
            {
                k = waitKey(30);
                //correct, store, scan the next face
                if(k == 'y')
                {
                    string filename = "../img" + to_string(num) + ".png";
                    imwrite(filename, frame);
                    cout << filename<< endl;
                    cout<< num <<" store the fig\n";
//                    for (int i = 0; i < cells.size(); ++i) {
//                        cout << i << " x = " << cells[i].x << " y = " << cells[i].y << endl;
//                    }
                    
                    //temp[4] is the color of central
//                    for (int i = 0; i < temp_ordered.size(); i++) {
//                        temp_ordered[i] = temp_ordered[8-i];
//                    }
                    cout<<temp[4]<<endl;
                    for(int i = 0; i < 9; i++)
                    {
                        switch(temp_ordered[i])
                        {
                            case 4:
                                cout << "YELLOW" << endl;
                                break;
                            case 3:
                                cout << "GREEN" << endl;
                                break;
                            case 5:
                                cout << "RED" << endl;
                                break;
                            case 1:
                                cout << "ORANGE" << endl;
                                break;
                            case 0:
                                cout << "WHITE" << endl;
                                break;
                            case 2:
                                cout << "BLUE" << endl;
                                break;
                        }
                    }
                    
                    cell_colors[temp[4]] = temp_ordered;
                
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
    
    cout<<"animation\n";
    vector<int> tmp = cell_colors[0];
//    cell_colors[0] = {tmp[2], tmp[5], tmp[8], tmp[1], tmp[4], tmp[7], tmp[0], tmp[3], tmp[6]};
//    for (int k = 0; k < 6; k++) {
//        for(int i = 0; i < 9; ++i) {
//            switch(cell_colors[k][i]) {
//                case 0:
//                    cout << "YELLOW" << endl;
//                    break;
//                case 1:
//                    cout << "GREEN" << endl;
//                    break;
//                case 2:
//                    cout << "RED" << endl;
//                    break;
//                case 3:
//                    cout << "ORANGE" << endl;
//                    break;
//                case 4:
//                    cout << "WHITE" << endl;
//                    break;
//                case 5:
//                    cout << "BLUE" << endl;
//                    break;
//            }
//        }
//        cout<<"-----------------------\n";
//    }
    animation(argc, argv, cell_colors);

    return 0;
}

