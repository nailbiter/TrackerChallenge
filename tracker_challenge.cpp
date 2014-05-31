#include <opencv2/core.hpp>
#include <opencv2/core/core_c.h>
#include "opencv2/core/utility.hpp"
#include <opencv/cv.h>
#include <opencv2/highgui.hpp>
#include <cstring>
#include <cstdlib>

using namespace cv;

/*TODO: two rectangles --> disappear
 */
/*TODO: write this-> update contrib & opencv -> build contrib & opencv -> automate initial frame (contrib)
 */

FILE* groundtruth=NULL;

int DEFSCREENW=0;
int DEFSCREENH=0;

struct interpreter_data{
    interpreter_data():total(0),first_time(true){}
    int total;
    Point prev_pt;
    bool first_time;
};

int animate(Point initPos, Point finalPos,int framenum,int frameOffset,
        int screenWidth=DEFSCREENW,int screenHeight=DEFSCREENH,int objectWidth=20,int objectHeight=20){

    printf("line %d with %d %d\n",__LINE__,screenWidth,screenHeight);fflush(stdout);
    Mat image(screenHeight,screenWidth, CV_8U, 1);
    printf("line %d\n",__LINE__);fflush(stdout);
    char filename[50];
    float increment_x=(finalPos.x-initPos.x)*1.0/framenum,
          increment_y=(finalPos.y-initPos.y)*1.0/framenum;

    if(objectWidth<0 || objectHeight<0){
        image=255;
        for(int i=0;i<framenum;i++){
            fprintf(groundtruth,"NaN,NaN,NaN,NaN\n");
            sprintf(filename,"image%03d.jpg",i+frameOffset);
            imwrite(filename,image);
        }
        return framenum;
    }
    
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
        fprintf(groundtruth,"%d,%d,%d,%d\n",px,py,px+objectWidth,py+objectHeight);
        sprintf(filename,"image%03d.jpg",i+frameOffset);
        imwrite(filename,image);
    }
    return framenum;
}

void interpreter(char* line,void* data){
    interpreter_data* mydata=(interpreter_data*)data;

    if(line==NULL || *line=='\0'){
        return;
    }

    if(mydata->first_time){
        mydata->first_time=false;
        char *ptr = strtok (line, " \t");
        DEFSCREENW=atoi(ptr);
        ptr=strtok (NULL, " \t");
        DEFSCREENH=atoi(ptr);
        printf("resolution: %dx%d\n",DEFSCREENW,DEFSCREENH);
        if(DEFSCREENW<=0 || DEFSCREENH<=0){
            exit(EXIT_FAILURE);
        }
        fprintf(groundtruth,"%d %d\n",DEFSCREENH,DEFSCREENW);
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

    printf("interpreter's got line %s\n",line);

    if(strncmp(line,"- - - -",7)==0){
        printf("disappear for %s frames\n",line+7);
        mydata->total+=animate(Point(0,0),Point(0,0),atoi(line+7),mydata->total,DEFSCREENW,DEFSCREENH,-1,-1);
        return;
    }

    ptr = strtok (line, " \t");
    int nums[5];
    int i=0;
    for(i=0;ptr != NULL;ptr = strtok (NULL, " \t"),i++){
        if(strcmp(ptr,"*")==0 && i!=4){
            nums[i]=((i%2)==0)?(mydata->prev_pt.x):(mydata->prev_pt.y);
        }else{
            nums[i]=atoi(ptr);
        }
        if(i!=4){
            if((i%2)==0){
                mydata->prev_pt.x=nums[i];
            }else{
                mydata->prev_pt.y=nums[i];
            }
        }
    }
    if(i<4){
        return;
    }

    printf("transition (%d,%d)->(%d,%d) in %d frames\n",nums[0],nums[1],nums[2],nums[3],nums[4]);
    mydata->total+=animate(Point(nums[0],nums[1]),Point(nums[2],nums[3]),nums[4],mydata->total);
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

int main( int argc, char ** argv){
    interpreter_data data;
    data.total=0;
    if(argc==1){
        printf("usage: %s <commandfile.txt>\n",argv[0]);
        exit(0);
    }
    groundtruth=fopen("groundtruth.txt","w");

    interpret(argv[1],interpreter,(void*)&data);

    system("ffmpeg -i image\%03d.jpg -vcodec mpeg4 test.avi");
    system("rm -rf image*.jpg");

    printf("%d images written\n",data.total);
    printf("ground truth is written to groundtruth.txt\n");
    fclose(groundtruth);
}
