#ifndef _FLIR_H_
#define _FLIR_H_

#include <iomanip>
#include <iostream>
#include <sstream>
#include "FlyCapture2.h"
#include "opencv2/highgui/highgui.hpp"

using namespace std;
using namespace FlyCapture2;

namespace FLIR {

class mGigEGrab
{
public:
    mGigEGrab(unsigned int w,unsigned int h):numCameras(10),width(w),height(h) {}
    ~mGigEGrab() {}

    bool detectCam();
    bool selectCam();
    bool openCam();
    bool RetrieveBGR(cv::Mat &mat);
    bool closeCam();

    void PrintCameraInfo(CameraInfo *pCamInfo,unsigned int i);
    void PrintError(FlyCapture2::Error error) { error.PrintErrorTrace(); }

private:
    FlyCapture2::Error error;
    BusManager busMgr;
    PGRGuid guid;
    GigECamera cam;

    unsigned int numCameras,width,height;
    CameraInfo camInfo[10];

public:
    char ipStr[100];

};


bool mGigEGrab::detectCam()
{
    error = BusManager::DiscoverGigECameras(camInfo, &numCameras);
    if (error != PGRERROR_OK)
    {
        PrintError(error);
        return false;
    }

    cout << "\033[1m\033[32mNumber of GigE cameras discovered: " << numCameras << endl;

    for (unsigned int i = 0; i < numCameras; i++)
    {
        PrintCameraInfo(&camInfo[i],i);
    }

    if (numCameras == 0)
    {
        cout << "No suitable GigE cameras found. Press Enter to exit..." << endl;
        return false;
    }

    return true;
}

bool mGigEGrab::selectCam()
{
    cout << "Input which camera you want open: " ;

    int camIndex;
    string inputData;
    getline(cin,inputData);
    camIndex=atoi(inputData.c_str());
    while (camIndex<1||camIndex>numCameras) {
        cout<<"Please input the right number which camera you want open: ";
        getline(cin,inputData);
        camIndex=atoi(inputData.c_str());
    }

    IPAddress ipAddress=camInfo[camIndex-1].ipAddress;
    sprintf(ipStr,"%d.%d.%d.%d",ipAddress.octets[0],ipAddress.octets[1],ipAddress.octets[2],ipAddress.octets[3]);

    error=busMgr.GetCameraFromIPAddress(ipAddress,&guid);
    if (error != PGRERROR_OK)
    {
        PrintError(error);
        return false;
    }

    return true;
}

bool mGigEGrab::openCam()
{
    cout << "Connecting to camera: " << ipStr << endl;

    error = cam.Connect(&guid);
    if (error != PGRERROR_OK)
    {
        PrintError(error);
        return false;
    }

    GigEImageSettingsInfo imageSettingsInfo;
    error = cam.GetGigEImageSettingsInfo(&imageSettingsInfo);
    if (error != PGRERROR_OK)
    {
        PrintError(error);
        return false;
    }

    GigEImageSettings imageSettings;
    imageSettings.offsetX = (unsigned int)(imageSettingsInfo.maxWidth-width)/2;
    imageSettings.offsetY = (unsigned int)(imageSettingsInfo.maxHeight-height)/2;
    imageSettings.height = height;
    imageSettings.width = width;
    imageSettings.pixelFormat = PIXEL_FORMAT_RGB;

    error = cam.SetGigEImageSettings(&imageSettings);
    if (error != PGRERROR_OK)
    {
        PrintError(error);
        return false;
    }

    cout << "Starting image capture..." << endl;

    error = cam.StartCapture();
    if (error != PGRERROR_OK)
    {
        PrintError(error);
        return false;
    }

    return true;
}


bool mGigEGrab::RetrieveBGR(cv::Mat &mat)
{
    Image rawImage;

    error = cam.RetrieveBuffer(&rawImage);
    if (error != PGRERROR_OK)
    {
        PrintError(error);
        return false;
    }

    Image convertedImage;

    error = rawImage.Convert(PIXEL_FORMAT_BGR, &convertedImage);
    if (error != PGRERROR_OK)
    {
        PrintError(error);
        return false;
    }

    unsigned int rowBytes = (double)convertedImage.GetReceivedDataSize()/(double)convertedImage.GetRows();
    cv::Mat cvImage = cv::Mat( convertedImage.GetRows(), convertedImage.GetCols(), CV_8UC3, convertedImage.GetData(), rowBytes );
    cvImage.copyTo(mat);

    return true;
}

bool mGigEGrab::closeCam()
{
    cout << "Stopping capture" << endl;

    error = cam.StopCapture();
    if (error != PGRERROR_OK)
    {
        PrintError(error);
        return false;
    }

    error = cam.Disconnect();
    if (error != PGRERROR_OK)
    {
        PrintError(error);
        return false;
    }

    return true;
}

void mGigEGrab::PrintCameraInfo(CameraInfo *pCamInfo,unsigned int i)
{
    ostringstream macAddress;
    macAddress << hex << setw(2) << setfill('0')
               << (unsigned int)pCamInfo->macAddress.octets[0] << ":" << hex
               << setw(2) << setfill('0')
               << (unsigned int)pCamInfo->macAddress.octets[1] << ":" << hex
               << setw(2) << setfill('0')
               << (unsigned int)pCamInfo->macAddress.octets[2] << ":" << hex
               << setw(2) << setfill('0')
               << (unsigned int)pCamInfo->macAddress.octets[3] << ":" << hex
               << setw(2) << setfill('0')
               << (unsigned int)pCamInfo->macAddress.octets[4] << ":" << hex
               << setw(2) << setfill('0')
               << (unsigned int)pCamInfo->macAddress.octets[5];

    ostringstream ipAddress;
    ipAddress << (unsigned int)pCamInfo->ipAddress.octets[0] << "."
                                                             << (unsigned int)pCamInfo->ipAddress.octets[1] << "."
                                                             << (unsigned int)pCamInfo->ipAddress.octets[2] << "."
                                                             << (unsigned int)pCamInfo->ipAddress.octets[3];

    ostringstream subnetMask;
    subnetMask << (unsigned int)pCamInfo->subnetMask.octets[0] << "."
                                                               << (unsigned int)pCamInfo->subnetMask.octets[1] << "."
                                                               << (unsigned int)pCamInfo->subnetMask.octets[2] << "."
                                                               << (unsigned int)pCamInfo->subnetMask.octets[3];

    ostringstream defaultGateway;
    defaultGateway << (unsigned int)pCamInfo->defaultGateway.octets[0] << "."
                                                                       << (unsigned int)pCamInfo->defaultGateway.octets[1] << "."
                                                                       << (unsigned int)pCamInfo->defaultGateway.octets[2] << "."
                                                                       << (unsigned int)pCamInfo->defaultGateway.octets[3];

    cout << endl;
    cout << "*** CAMERA \033[7m"<< i+1 << "\033[0m\033[1m\033[32m INFORMATION ***" << endl;
//    cout << "Camera model - " << pCamInfo->modelName << endl;
//    cout << "Camera vendor - " << pCamInfo->vendorName << endl;
//    cout << "MAC address - " << macAddress.str() << endl;
    cout << "\033[4mIP address - " << ipAddress.str() << "\033[0m\033[1m\033[32m" << endl;
//    cout << "Subnet mask - " << subnetMask.str() << endl;
//    cout << "Default gateway - " << defaultGateway.str() << endl;
    cout << endl;
}

}

#endif//_FLIR_H_
