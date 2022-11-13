#include <chrono>
#include <iostream>
#include "CameraFrame.hpp"

using namespace std;

static uint16_t* buffer = nullptr;

void FrameCompleted(uint64_t timestamp) {
	std::cout << "Frame completed: " << timestamp << std::endl;
}

int main()
{
	cout << "Start frame." << endl;
	int height;
	int width;
	CameraFrame cam;
	cam.Connect();
	cam.GetSize(height, width);
	cout << "Camera height: " << height << ", width: " << width << endl;
	buffer = new uint16_t[height * width];
	cam.SetImage(buffer);
	cam.SetFrameCompletedCallback(FrameCompleted);
	cam.Start();
	std::this_thread::sleep_for(std::chrono::seconds(3));
	cam.Stop();
	cam.Disconnect();
	delete[] buffer;
	buffer = nullptr;

	return 0;
}
