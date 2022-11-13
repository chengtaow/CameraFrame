// CameraFrame: Camera frame management

#ifndef CAMERA_FRAME_
#define CAMERA_FRAME_

#include <queue>

using namespace std;

class CameraFrame {
    public:
        typedef void (*FrameCompleted) (uint64_t timestamp);
        CameraFrame();
        void Connect();
        void GetSize(int& height, int& width);
        void SetImage(uint16_t* buffer);
        void Disconnect();
        void Start();
        void Stop();
        void SetFrameCompletedCallback(FrameCompleted callback);

    private:
        int mHeight;
        int mWidth;
        uint16_t* mOutBuffer;
        volatile bool mRunning;
        FrameCompleted frameCompleted;
        mutex algoMutex;
        thread exposureThread;
        thread algorithmThread1;
        thread algorithmThread2;
        queue<unique_ptr<uint8_t[]>> mBufferQueue;
        void DoExposure();
        void DoAlgorithm();
};


#endif