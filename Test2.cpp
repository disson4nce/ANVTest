

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files
#include <windows.h>

#include <iostream>
#include <string>

// C RunTime Header Files
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <opencv2\opencv.hpp>
//#include "pch.h"

using namespace cv;
using namespace std;

int main(int argc, char** argv)
{
	//open the video file for reading
	VideoCapture cap("C:\\Users\\Klaudia\\source\\repos\\Test2\\Test2\\VID_2018.mp4");

	if (cap.isOpened() == false) // Check for failure
	{
		cout << "Could not open the video file" << endl;
		system("pause"); //wait for any key press
		return -1;
	}
	//Uncomment the following line if you want to start the video in the middle
	cap.set(CAP_PROP_POS_MSEC, 12000); 

	//get the frames rate of the video
	int frame_width = static_cast<int>(cap.get(CAP_PROP_FRAME_WIDTH)); //get the width of frames of the video
	int frame_height = static_cast<int>(cap.get(CAP_PROP_FRAME_HEIGHT)); //get the height of frames of the video
	Size frame_size(frame_width, frame_height);
	double fps = cap.get(CAP_PROP_FPS);
	cout << "Frames per seconds : " << fps << endl;
	cout << "Frame size : " << frame_size << endl;
	cout << "Resized to : 600 x 600 " << endl;

	//Create and initialize the VideoWriter object 
	//VideoWriter oVideoWriter("cMyVideo1.avi", VideoWriter::fourcc('M', 'J', 'P', 'G'),
		//fps, cv::Size(600,600), true);

	//If the VideoWriter object is not initialized successfully, exit the program
	/*if (oVideoWriter.isOpened() == false)
	{
		cout << "Cannot save the video to a file" << endl;
		cin.get(); //wait for any key press
		return -1;
	}*/


	String window_name = "My First Video Resized";

	//namedWindow(window_name); //create a window
	int i = 0;
	while (true)
	{
		Mat frame;
		bool bSuccess = cap.read(frame); // read a new frame from video 

		
		//Breaking the while loop if frames cannot be read from the camera
		if (bSuccess == false)
		{
			cout << "Video ended" << endl;
			cin.get(); //Wait for any key press
			break;
		}
		

		cv::resize(frame, frame, cv::Size(600,600));
		/*
	   Make changes to the frame as necessary
	   e.g.
		1. Change brightness/contrast of the image
		2. Smooth/Blur image
		3. Crop the image
		4. Rotate the image
		5. Draw shapes on the image
	   */
		std::ostringstream name;
		name << "C:\\Users\\Klaudia\\source\\repos\\Test2\\Test2\\frames\\scaled_im_" << i << ".jpg";
		bool isSuccess = imwrite(name.str(), frame);
		
	   //write the video frame to the file
		//bool isSuccess = imwrite("C:\\Users\\Klaudia\\source\\repos\\Test2\\Test2\\frames\\MyImage.jpg", frame); //write the image to a file as JPEG 
		//bool isSuccess = imwrite("D:/MyImage.png", image); //write the image to a file as PNG
		if (isSuccess == false)
		{
			cout << "Failed to save the image" << i <<endl;
			cin.get(); //wait for a key press
			return -1;
		}

		cout << "Image is succusfully saved to a file" << i<<endl;
		i++;

		//oVideoWriter.write(frame);

		//show the frame in the created window
		//imshow(window_name, frame);

		//wait for for 10 ms until any key is pressed.  
		//If the 'Esc' key is pressed, break the while loop.
		//If the any other key is pressed, continue the loop 
		//If any key is not pressed withing 10 ms, continue the loop
		if (waitKey(10) == 27)
		{
			cout << "Esc key is pressed by user. Stoppig the video" << endl;
			break;
		}
	}
	//Flush and close the video file
	//oVideoWriter.release();
	cout << "Video flushed" << endl;
	return 0;
}