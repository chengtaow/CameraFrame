// CameraFrame.cpp: camera frame implementation.
//
#include <chrono>
#include <thread>
#include <iostream>

#include "CameraFrame.hpp"

CameraFrame::CameraFrame()
{
    mHeight = 160;
    mWidth = 210;
    mRunning = false;
    frameCompleted = nullptr;
}

void CameraFrame::Connect() {
    std::cout << "Connect camera!" << std::endl;
}

void CameraFrame::GetSize(int& height, int& width) {
    height = mHeight;
    width = mWidth;
}

void CameraFrame::Disconnect() {
    std::cout << "Disconnect camera!" << std::endl;
}

void CameraFrame::Start() {
    std::cout << "Frame started." << std::endl;
    mRunning = true;
    exposureThread = std::thread(&CameraFrame::DoExposure, this);
    algorithmThread1 = std::thread(&CameraFrame::DoAlgorithm, this);
    algorithmThread2 = std::thread(&CameraFrame::DoAlgorithm, this);
}

void CameraFrame::DoExposure() {
    uint8_t data = 0;
    while (mRunning) {
        this_thread::sleep_for(chrono::microseconds(10));
        auto rawBuffer = make_unique<uint8_t[]>(mHeight * mWidth);
        memset(rawBuffer.get(), data, sizeof(uint8_t) * mHeight * mWidth);
        chrono::system_clock::time_point now = chrono::system_clock::now();
        uint64_t time = chrono::duration_cast<chrono::milliseconds>(now.time_since_epoch()).count();
        mQueueMutex.lock();
        mBufferQueue.push(move(rawBuffer));
        mTimeQueue.push(time);
        mQueueMutex.unlock();
        data++;
    }
}

void CameraFrame::DoAlgorithm() {
    while (mRunning) {
        uint64_t time = 0;
        unique_ptr<uint16_t[]> image = make_unique<uint16_t[]>(mHeight * mWidth);
        mQueueMutex.lock();
        if (mBufferQueue.size() > 0) {
            auto buffer = move(mBufferQueue.front());
            time = mTimeQueue.front();
            mTimeQueue.pop();
            mBufferQueue.pop();
            mQueueMutex.unlock();
            // process
            Process(buffer, image);
        }
        else {
            mQueueMutex.unlock();
            this_thread::sleep_for(chrono::milliseconds(5));
            continue;
        }
        
        if (nullptr != frameCompleted) {
            std::unique_lock<std::mutex>lock(mOutputMutex);
            frameCompleted(time, image);
        }
    }
}

void CameraFrame::Process(const unique_ptr<uint8_t[]>& input, unique_ptr<uint16_t[]>& output) {
    // time consuming algorithm.
    this_thread::sleep_for(chrono::microseconds(16));
    for (int i = 0; i < mHeight * mWidth; i++) {
        output[i] = static_cast<uint16_t>(input[i]);
    }
}

void CameraFrame::Stop() {
    mRunning = false;
    exposureThread.join();
    algorithmThread1.join();
    algorithmThread2.join();
    frameCompleted = nullptr;
    while (!mBufferQueue.empty()) {
        mBufferQueue.pop();
        std::cout << "Clean queue." << std::endl;
    }
    std::cout << "Frame stopped." << std::endl;
}

void CameraFrame::SetFrameCompletedCallback(FrameCompleted callback) {
    if (nullptr != callback) {
        frameCompleted = callback;
    }
}