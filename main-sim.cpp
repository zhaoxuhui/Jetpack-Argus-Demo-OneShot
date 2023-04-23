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
	
    // step2 获取可用相机
	std::vector<Argus::CameraDevice*> cameraDevices;
	iCameraProvider->getCameraDevices(&cameraDevices);
	
    // step3 获取相机属性接口
	Argus::ICameraProperties *iCameraProperties = Argus::interface_cast<Argus::ICameraProperties>(cameraDevices[0]);

    // step4 获取相机支持的拍摄模式并默认使用第一个拍摄
	std::vector<Argus::SensorMode*> sensorModes;
	iCameraProperties->getAllSensorModes(&sensorModes);
	Argus::ISensorMode *iSensorMode = Argus::interface_cast<Argus::ISensorMode>(sensorModes[0]);

    // Step5 创建Capture Session并获得控制接口
	Argus::Status status;
	Argus::UniqueObj<Argus::CaptureSession> captureSession(iCameraProvider->createCaptureSession(cameraDevices[0], &status));
	Argus::ICaptureSession *iSession = Argus::interface_cast<Argus::ICaptureSession>(captureSession);

    // Step6 设置输出流参数
	Argus::UniqueObj<Argus::OutputStreamSettings> streamSettings(iSession->createOutputStreamSettings(Argus::STREAM_TYPE_EGL));
	Argus::IEGLOutputStreamSettings *iEGLStreamSettings = Argus::interface_cast<Argus::IEGLOutputStreamSettings>(streamSettings);
	iEGLStreamSettings->setPixelFormat(Argus::PIXEL_FMT_YCbCr_420_888);
	iEGLStreamSettings->setResolution(iSensorMode->getResolution());
	iEGLStreamSettings->setMetadataEnable(true);

    // Step7 创建输出流
	Argus::UniqueObj<Argus::OutputStream> stream(iSession->createOutputStream(streamSettings.get()));

    // Step8 创建Consumer对象并获取控制接口
	Argus::UniqueObj<EGLStream::FrameConsumer> consumer(EGLStream::FrameConsumer::create(stream.get()));
	EGLStream::IFrameConsumer *iFrameConsumer = Argus::interface_cast<EGLStream::IFrameConsumer>(consumer);

    // Step9 创建请求并与Stream关联
	Argus::UniqueObj<Argus::Request> request(iSession->createRequest(Argus::CAPTURE_INTENT_STILL_CAPTURE));
	Argus::IRequest *iRequest = Argus::interface_cast<Argus::IRequest>(request);
	status = iRequest->enableOutputStream(stream.get());

    // Step10 设置拍摄的模式
    Argus::ISourceSettings *iSourceSettings = Argus::interface_cast<Argus::ISourceSettings>(request);
    iSourceSettings->setSensorMode(sensorModes[0]);
    uint32_t requestId = iSession->capture(request.get());

    // Step11 构造Frame对象并接收数据
	const uint64_t FIVE_SECONDS_IN_NANOSECONDS = 5000000000;
	Argus::UniqueObj<EGLStream::Frame> frame(iFrameConsumer->acquireFrame(FIVE_SECONDS_IN_NANOSECONDS, &status));
	EGLStream::IFrame *iFrame = Argus::interface_cast<EGLStream::IFrame>(frame);
	EGLStream::Image *image = iFrame->getImage();

    // Step12 构造JPEG对象
	EGLStream::IImageJPEG *iImageJPEG = Argus::interface_cast<EGLStream::IImageJPEG>(image);
	status = iImageJPEG->writeJPEG("argus_oneShot.jpg");

    // Step13 关闭Argus
	cameraProvider.reset();

    return 0;
}