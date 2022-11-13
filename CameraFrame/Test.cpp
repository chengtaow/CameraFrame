#include <chrono>
#include <iostream>
#include "CameraFrame.hpp"

using namespace std;

static int height = 0;
static int width = 0;
static uint16_t* buffer = nullptr;

void FrameCompleted(uint64_t timestamp, const unique_ptr<uint16_t[]>& image) {
	std::cout << "Frame completed: " << timestamp;
	std::cout << ", " << to_string(image[0]) << endl;
}

int main()
{
	cout << "Start frame." << endl;
	CameraFrame cam;
	cam.Connect();
	cam.GetSize(height, width);
	cout << "Camera height: " << height << ", width: " << width << endl;
	buffer = new uint16_t[height * width];
	cam.SetFrameCompletedCallback(FrameCompleted);
	cam.Start();
	std::this_thread::sleep_for(std::chrono::seconds(3));
	cam.Stop();
	cam.Disconnect();
	delete[] buffer;
	buffer = nullptr;

	return 0;
}
