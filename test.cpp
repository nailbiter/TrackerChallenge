#include <opencv2/core.hpp>
#include <opencv2/core/core_c.h>
#include <opencv2/highgui.hpp>
#include <cstring>
#include <cstdlib>

using namespace cv;

#define RECT_H 100
#define RECT_W 200
#define BORDER 25

void animate(int frames_per_period,int frames_total){
    Mat image(468,832, CV_8U, 1);

    char filename[50];
    int increment=(image.cols-2*BORDER-RECT_W)/(frames_per_period-1);
    
    for(int i=0,pos=BORDER;i<frames_total;i++){
        //printf("offset: %d\n",offset);
        image=255;
        rectangle(image,Point(pos,image.rows/2-RECT_H/2),Point(pos+RECT_W,image.rows/2+RECT_H/2),0,CV_FILLED);
        sprintf(filename,"image%03d.jpg",i);
        imwrite(filename,image);

        if(i%(2*frames_per_period-2)<(frames_per_period-1)){
            pos+=increment;
        }else{
            pos-=increment;
        }
    }
}

#define LINE(X,Y) for(int y=0;y<=11;y++){image.at<Vec3b>(X,Y+y)=black;}
#define CROSS(X,Y) {for(int y=-5;y<=5;y++){image.at<Vec3b>(X,Y+y)=black;}for(int y=-5;y<=5;y++){image.at<Vec3b>(X+y,Y)=black;}}
int main( int arc, char ** argv)
{
    /*int total=200;
    animate(30,total);
    printf("%d images written\n",total);*/

    Mat image;
    image=imread("test.png");
    printf("dimensions: %d and %d\n",image.cols,image.rows);
    Vec3b black(0,0,0);
    LINE(813,11);
    LINE(713,11);
    LINE(613,11);
    LINE(513,11);
    LINE(413,11);
    LINE(311,11);
    LINE(211,11);
    LINE(111,11);
    LINE(011,11);

    LINE(813,1150);
    LINE(713,1150);
    LINE(613,1150);
    LINE(513,1150);
    LINE(413,1150);
    LINE(311,1150);
    LINE(211,1150);
    LINE(111,1150);
    LINE(11,1150);

    CROSS(11,590);
    CROSS(111,590);
    CROSS(211,590);
    CROSS(311,590);
    CROSS(413,590);
    CROSS(513,590);
    CROSS(613,590);
    CROSS(713,590);
    CROSS(813,590);

    if(false){
        imwrite("mytest.png",image);
    }else{
        imshow("cut image",image);
        waitKey(0);
    }

    return 0;
}
//ffmpeg -i image%03d.jpg -vcodec mpeg4 test.avi
