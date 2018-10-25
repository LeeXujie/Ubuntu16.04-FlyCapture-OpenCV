#include "FLIR.h"

using namespace FLIR;
unsigned int width=640;
unsigned int height=480;

int main(int argc, char ** argv)
{
    if(argc==3){
        width=atoi(argv[1]);
        height=atoi(argv[2]);
    }

    mGigEGrab cam(width,height);

    if(!cam.detectCam())return 0;

    if(!cam.selectCam())return 0;

    if(!cam.openCam())return 0;

    cv::Mat img;
    while(cv::waitKey(30)!=27)
    {
        cam.RetrieveBGR(img);
        cv::imshow(cam.ipStr,img);
    }

    img.release();
    cv::destroyAllWindows();

    return 0;
}
