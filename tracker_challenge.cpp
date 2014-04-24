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
#undef RANDOCOL
#ifdef RANDOCOL
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

void interpreter(char* line,void* data){
    if(line==NULL || *line=='\0'){
        return;
    }
    int len=strlen(line);
    char *ptr=NULL;
    if((ptr=strchr(line,'#'))!=NULL){
        *ptr='\0';
    }else{
        ptr=line+len-1;
        if(*ptr=='\n'){
            *ptr='\0';
        }
    }
    if(*line=='\0'){
        return;
    }

    printf("real interpreter's got line %s\n",line);
    ptr = strtok (line, " \t");
    int nums[5];
    int i=0;
    for(i=0;ptr != NULL;ptr = strtok (NULL, " \t"),i++){
        nums[i]=atoi(ptr);
    }
    if(i<4){
        return;
    }

    int* total=(int*)data;
    printf("transition (%d,%d)->(%d,%d) in %d frames\n",nums[0],nums[1],nums[2],nums[3],nums[4]);
    *total+=animate(Point(nums[0],nums[1]),Point(nums[2],nums[3]),nums[4],*total);
}
void interpret(char* filename, void (*interpreter)(char*,void*),void* data){
    char buf[200]={'\0'};
    size_t n=sizeof(buf);
    FILE* fs=fopen(filename,"r");
    if(fs==NULL){
        printf("cannot open file %s\n",filename);
        exit(EXIT_FAILURE);
    }
    while((fgets(buf,sizeof(buf),fs))!=NULL){
        interpreter(buf,data);
    }
    fclose(fs);
}

int main( int argc, char ** argv)
{
    int total=0;
    if(argc==1){
        printf("usage: %s <commandfile.txt>\n",argv[0]);
        exit(0);
    }
    groundtruth=fopen("groundtruth.txt","w");
    fprintf(groundtruth,"%d %d\n",DEFSCREENH,DEFSCREENW);

    interpret(argv[1],interpreter,(void*)&total);
    //exit(0);

    /*for(int i=0;i<1;i++){
    total+=animate(p1,p2,3+25,total);
    total+=animate(p2,p1,9+25,total);
    total+=animate(p1,p2,2+25,total);
    total+=animate(p2,p1,7+25,total);
    total+=animate(p1,p2,3+25,total);
    total+=animate(p2,p1,4+25,total);
    total+=animate(p1,p2,1+25,total);
    total+=animate(p2,p1,9+25,total);
    }*/

    system("ffmpeg -i image\%03d.jpg -vcodec mpeg4 test.avi");
    system("rm -rf image*.jpg");

    printf("%d images written\n",total);
    printf("ground truth is written to groundtruth.txt\n");
    fclose(groundtruth);
}
