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

int main( int arc, char ** argv)
{
    int total=200;
    animate(30,total);
    printf("%d images written\n",total);
}
//ffmpeg -i image%03d.jpg -vcodec mpeg4 test.avi
