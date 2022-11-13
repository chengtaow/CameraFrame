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
    mOutBuffer = nullptr;
}

void CameraFrame::Connect() {
    std::cout << "Connect camera!" << std::endl;
}

void CameraFrame::GetSize(int& height, int& width) {
    height = mHeight;
    width = mWidth;
}

void CameraFrame::SetImage(uint16_t* buffer) {
    mOutBuffer = buffer;
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
        mBufferQueue.push(move(rawBuffer));
        data++;
    }
}

void CameraFrame::DoAlgorithm() {
    while (mRunning) {
        algoMutex.lock();
        if (mBufferQueue.size() > 0) {
            auto buffer = move(mBufferQueue.front());
            cout << "buffer: " << to_string(buffer[0]) << endl;
            mBufferQueue.pop();
            algoMutex.unlock();
            // process
            this_thread::sleep_for(chrono::microseconds(16));
        }
        else {
            algoMutex.unlock();
            this_thread::sleep_for(chrono::milliseconds(5));
            continue;
        }
        chrono::system_clock::time_point now = chrono::system_clock::now();
        uint64_t time = chrono::duration_cast<chrono::milliseconds>(now.time_since_epoch()).count();
        if (nullptr != frameCompleted) {
            frameCompleted(time);
        }
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