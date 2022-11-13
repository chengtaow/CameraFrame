// CameraFrame: Camera frame management

#ifndef CAMERA_FRAME_
#define CAMERA_FRAME_

#include <queue>

using namespace std;

class CameraFrame {
    public:
        typedef void (*FrameCompleted) (uint64_t timestamp, const unique_ptr<uint16_t[]>& image);
        CameraFrame();
        void Connect();
        void GetSize(int& height, int& width);
        void Disconnect();
        void Start();
        void Stop();
        void SetFrameCompletedCallback(FrameCompleted callback);

    private:
        int mHeight;
        int mWidth;
        volatile bool mRunning;
        FrameCompleted frameCompleted;
        mutex mQueueMutex;
        mutex mOutputMutex;
        thread exposureThread;
        thread algorithmThread1;
        thread algorithmThread2;
        queue<uint64_t> mTimeQueue;
        queue<unique_ptr<uint8_t[]>> mBufferQueue;
        void DoExposure();
        void DoAlgorithm();
        void Process(const unique_ptr<uint8_t[]>& input, unique_ptr<uint16_t[]>& output);
};


#endif