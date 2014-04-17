#include <opencv2/core.hpp>
#include <opencv2/core/core_c.h>
#include "opencv2/core/utility.hpp"
#include <opencv/cv.h>
#include <opencv2/highgui.hpp>
#include <cstring>
#include <cstdlib>

using namespace cv;

/*TODO: read transitions from config file
 */
/*TODO: write this-> update contrib & opencv -> build contrib & opencv -> automate initial frame (contrib)
 */

FILE* groundtruth=NULL;

#define DEFSCREENW 832
#define DEFSCREENH 468

int animate(Point initPos, Point finalPos,int framenum,int frameOffset,
        int screenWidth=DEFSCREENW,int screenHeight=DEFSCREENH,int objectWidth=100,int objectHeight=100){

    Mat image(screenHeight,screenWidth, CV_8U, 1);

    char filename[50];
    float increment_x=(finalPos.x-initPos.x)*1.0/framenum,
          increment_y=(finalPos.y-initPos.y)*1.0/framenum;
    
    for(int i=0,px=initPos.x,py=initPos.y;i<framenum;i++,px+=increment_x,py+=increment_y){
        image=255;
        rectangle(image,Point(px,py),Point(px+objectWidth,py+objectHeight),0,CV_FILLED);
#define RANDO
#ifdef RANDO
        int dec=5;
        RNG rng;
        for(int ii=0;ii<(objectWidth/dec);ii++){
            for(int jj=0;jj<(objectHeight/dec);jj++){
                //rectangle(image,Point(px+ii*dec,py+jj*dec),Point(px+(ii+1)*dec,py+(jj+1)*dec),((ii+jj)%2)*255.0/2.0,CV_FILLED);
                rectangle(image,Point(px+ii*dec,py+jj*dec),Point(px+(ii+1)*dec,py+(jj+1)*dec),rng.next()%255,CV_FILLED);
            }
        }
#endif
        fprintf(groundtruth,"%d %d %d %d\n",px,py,px+objectWidth,py+objectHeight);
        sprintf(filename,"image%03d.jpg",i+frameOffset);
        imwrite(filename,image);
    }
    return framenum;
}

int main( int arc, char ** argv)
{
    int total=0;
    Point p1(25,468/2-100/2),
          p2(832-25-200,468/2-100/2);
    groundtruth=fopen("groundtruth.txt","w");
    fprintf(groundtruth,"%d %d\n",DEFSCREENH,DEFSCREENW);

    for(int i=0;i<1;i++){
    total+=animate(p1,p2,3+25,total);
    total+=animate(p2,p1,9+25,total);
    total+=animate(p1,p2,2+25,total);
    total+=animate(p2,p1,7+25,total);
    total+=animate(p1,p2,3+25,total);
    total+=animate(p2,p1,4+25,total);
    total+=animate(p1,p2,1+25,total);
    total+=animate(p2,p1,9+25,total);
    }

    system("ffmpeg -i image\%03d.jpg -vcodec mpeg4 test.avi");
    system("rm -rf image*.jpg");

    printf("%d images written\n",total);
    printf("ground truth is written to groundtruth.txt\n");
    fclose(groundtruth);
}
