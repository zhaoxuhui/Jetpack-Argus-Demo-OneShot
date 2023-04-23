#include <stdio.h>
#include <stdlib.h>
#include <Argus/Argus.h>
#include <EGLStream/EGLStream.h>
#include <iostream>

int main(int argc, char** argv)
{

    // step1 创建Argus环境
	Argus::UniqueObj<Argus::CameraProvider> cameraProvider(Argus::CameraProvider::create());
	Argus::ICameraProvider *iCameraProvider = Argus::interface_cast<Argus::ICameraProvider>(cameraProvider);
    
	// 信息输出
    std::cout<<"Step1: Create Argus Session and Get Camera Provider"<<std::endl;
    if(!iCameraProvider){
        std::cout<<"==>Cannot get core camera provider interface"<<std::endl<<std::endl;
    }else{
        std::cout<<"==>Argus Version:"<<iCameraProvider->getVersion().c_str()<<std::endl<<std::endl;
    }
	
	
    // step2 获取可用相机
	std::vector<Argus::CameraDevice*> cameraDevices;
	iCameraProvider->getCameraDevices(&cameraDevices);
	
	// 信息输出
    std::cout<<"Step2: Get Available Camera Devices by Camera Provider"<<std::endl;
    if(cameraDevices.size()==0){
        std::cout<<"==>No cameras available"<<std::endl<<std::endl;
    }else{
        std::cout<<"==>"<<cameraDevices.size()<<" Camera device(s) avaiable"<<std::endl<<std::endl;
    }
	
	
    // step3 获取相机属性接口
	Argus::ICameraProperties *iCameraProperties = Argus::interface_cast<Argus::ICameraProperties>(cameraDevices[0]);

    // 信息输出
    std::cout<<"Step3: Get Properties of Available Camera"<<std::endl;
    if(!iCameraProperties){
        std::cout<<"==>Failed to get iCameraProperties interface"<<std::endl<<std::endl;
    }else{
        std::cout<<"==>Succeed to get iCameraProperties interface"<<std::endl<<std::endl;
    }


    // step4 获取相机支持的拍摄模式并默认使用第一个拍摄
	std::vector<Argus::SensorMode*> sensorModes;
	iCameraProperties->getAllSensorModes(&sensorModes);
	Argus::ISensorMode *iSensorMode = Argus::interface_cast<Argus::ISensorMode>(sensorModes[0]);
	
	// 信息输出
    std::cout<<"Step4: Get Available Camera Modes"<<std::endl;
    if(sensorModes.size()==0){
        std::cout<<"==>Failed to get sensor modes"<<std::endl<<std::endl;
    }else{
        std::cout<<"==>Succeed to get sensor modes"<<std::endl;
        for (int i = 0; i < sensorModes.size(); ++i)
        {
            Argus::ISensorMode *tmpSensorMode = Argus::interface_cast<Argus::ISensorMode>(sensorModes[i]);
            Argus::Size2D<uint32_t> resolution = tmpSensorMode->getResolution();
            std::cout<<"\tMode "<<i<<": Width="<<resolution.width()<<" Height="<<resolution.height()<<std::endl;
        }
        std::cout<<std::endl;
    }
	

    // Step5 创建Capture Session并获得控制接口
	Argus::Status status;
	Argus::UniqueObj<Argus::CaptureSession> captureSession(iCameraProvider->createCaptureSession(cameraDevices[0], &status));
	Argus::ICaptureSession *iSession = Argus::interface_cast<Argus::ICaptureSession>(captureSession);
	
	// 信息输出
    std::cout<<"Step5: Get the Interface of Capture Session"<<std::endl;
    if(status!=Argus::STATUS_OK){
        std::cout<<"==>Failed to get the interface of capture session"<<std::endl<<std::endl;
    }else{
        std::cout<<"==>Succeed to get the interface of capture session"<<std::endl<<std::endl;
    }
	

    // Step6 设置输出流参数
	Argus::UniqueObj<Argus::OutputStreamSettings> streamSettings(iSession->createOutputStreamSettings(Argus::STREAM_TYPE_EGL));
	Argus::IEGLOutputStreamSettings *iEGLStreamSettings = Argus::interface_cast<Argus::IEGLOutputStreamSettings>(streamSettings);
	iEGLStreamSettings->setPixelFormat(Argus::PIXEL_FMT_YCbCr_420_888);
	iEGLStreamSettings->setResolution(iSensorMode->getResolution());
	iEGLStreamSettings->setMetadataEnable(true);
	
	// 信息输出
    std::cout<<"Step6: Set Parameters of Output Stream"<<std::endl;
    if(!iEGLStreamSettings){
        std::cout<<"==>Failed to set the parameters of output stream"<<std::endl<<std::endl;
    }else{
        std::cout<<"==>Succeed to set the parameters of output stream"<<std::endl<<std::endl;
    }
	

    // Step7 创建输出流
	Argus::UniqueObj<Argus::OutputStream> stream(iSession->createOutputStream(streamSettings.get()));
	
	// 信息输出
    std::cout<<"Step7: Create Output Stream Object"<<std::endl;
    if(!stream){
        std::cout<<"==>Failed to create output stream object"<<std::endl<<std::endl;
    }else{
        std::cout<<"==>Succeed to create output stream object"<<std::endl<<std::endl;
    }
	

    // Step8 创建Consumer对象并获取控制接口
	Argus::UniqueObj<EGLStream::FrameConsumer> consumer(EGLStream::FrameConsumer::create(stream.get()));
	EGLStream::IFrameConsumer *iFrameConsumer = Argus::interface_cast<EGLStream::IFrameConsumer>(consumer);
	
	// 信息输出
    std::cout<<"Step8: Create Consumer Object and Interface"<<std::endl;
    if(!iFrameConsumer){
        std::cout<<"==>Failed to create consumer object"<<std::endl<<std::endl;
    }else{
        std::cout<<"==>Succeed to create consumer object"<<std::endl<<std::endl;
    }
	

    // Step9 创建请求并与Stream关联
	Argus::UniqueObj<Argus::Request> request(iSession->createRequest(Argus::CAPTURE_INTENT_STILL_CAPTURE));
	Argus::IRequest *iRequest = Argus::interface_cast<Argus::IRequest>(request);
	status = iRequest->enableOutputStream(stream.get());
	
	// 信息输出
    std::cout<<"Step9: Create Request"<<std::endl;
    if(status!=Argus::STATUS_OK){
        std::cout<<"==>Failed to create request"<<std::endl<<std::endl;
    }else{
        std::cout<<"==>Succeed to create request"<<std::endl<<std::endl;
    }
	

    // Step10 设置拍摄的模式
    Argus::ISourceSettings *iSourceSettings = Argus::interface_cast<Argus::ISourceSettings>(request);
    iSourceSettings->setSensorMode(sensorModes[0]);
    uint32_t requestId = iSession->capture(request.get());
	
	// 信息输出
    std::cout<<"Step10: Set Sensor Mode"<<std::endl;
    if(!iSourceSettings){
        std::cout<<"==>Failed to set sensor mode"<<std::endl<<std::endl;
    }else{
        std::cout<<"==>Succeed to set sensor mode"<<std::endl<<std::endl;
    }


    // Step11 构造Frame对象并接收数据
	const uint64_t FIVE_SECONDS_IN_NANOSECONDS = 5000000000;
	Argus::UniqueObj<EGLStream::Frame> frame(iFrameConsumer->acquireFrame(FIVE_SECONDS_IN_NANOSECONDS, &status));
	EGLStream::IFrame *iFrame = Argus::interface_cast<EGLStream::IFrame>(frame);
	EGLStream::Image *image = iFrame->getImage();
	
	// 信息输出
    std::cout<<"Step11: Create Frame Object and Receive Data"<<std::endl;
    if(!image){
        std::cout<<"==>Failed to create frame object and receive data"<<std::endl<<std::endl;
    }else{
        std::cout<<"==>Succeed to create frame object and receive data"<<std::endl<<std::endl;
    }
	
    
    // Step12 构造JPEG对象
	EGLStream::IImageJPEG *iImageJPEG = Argus::interface_cast<EGLStream::IImageJPEG>(image);
	status = iImageJPEG->writeJPEG("argus_oneShot.jpg");
	
	// 信息输出
    std::cout<<"Step12: Create JPEG Object and Save Data"<<std::endl;
    if(status!=Argus::STATUS_OK){
        std::cout<<"==>Failed to create jpeg object and save data"<<std::endl<<std::endl;
    }else{
        std::cout<<"==>Succeed to create jpeg object and save data"<<std::endl<<std::endl;
    }
	

    // Step13 关闭Argus
	cameraProvider.reset();
	
	// 信息输出
    std::cout<<"Step13: Shut Down Argus"<<std::endl;

    return 0;
}