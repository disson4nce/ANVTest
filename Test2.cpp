#define WIN32_LEAN_AND_MEAN // Exclude rarely-used stuff from Windows headers. Copies content of stdafx due to issues with configuration on Windows 
//Windows Header Files: 
#include <windows.h>
#include <iostream>
#include <string>
// C RunTime Header Files:
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <opencv2\opencv.hpp>
#include <mpi.h>
#include <sstream>

#define SETUPTAG 0 //MPI tag for sending the size of the path
#define WORKTAG 1 // MPI tag for sending the path
#define DIETAG  2 // MPI tag for killing the slave processes

using namespace cv;
using namespace std;

//List of functions:
int videoSplitter(int* fullPathNameSize, queue<string>* someQueue, char* videoPath); // Splits video into frames
int slaveProcessor(int* pathSize, char *path); //processes frame
void master(char* videoPath);
void slave(int rank);
void assignJob(queue<string>* queueOfPaths); 


int main(int argc, char** argv) // Main, run on win with "mpiexec -n 4 Test2.exe videoPath"
{  
	// Example of arg[1] videoPath: C:\\Users\\Klaudia\\source\\repos\\Test2\\Test2\\VID_2018.mp4 
	if (argc < 2) {
		std::cerr << "Usage: " << argv[0] << " Videopath:" << std::endl; // Tell the user how to run the program
		return 1;
	}
	
	MPI_Init(&argc, &argv); //Initialize Mpi
	int rank;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank); // Process rank, from 0 to N-1
	if (rank == 0) {
		cout << "I am the master" << endl;
		master(argv[1]); // Feeding master the video path
	}
	else {	
		slave(rank); // Feedig slaves the rank
	}
	MPI_Finalize(); // Cleaning up the MPI
}


int slaveProcessor(int* pathSize, char *path) // Frames processor
{
	size_t pathSize_sizeT = (size_t)(*pathSize); // Conversion from int* to size_t
	int pathSize_int = (int)(pathSize_sizeT);  // Conversion from size_t to int
	std::string spareString(pathSize_sizeT, '*'); // Generate a string of lenght pathSize_sizeT
	int n = spareString.length(); // String length
	char* char_array = new char[n+1]; // creating a target array
	strcpy_s(char_array, n+1,spareString.c_str()); // Copy the string content to the char array
	memcpy(char_array, path, 64 /* Length */); // Trimming too long path
	
	Mat image = imread(char_array); // Read the image
	if (image.empty()) { // Check for failure of reading image
		cout << "Could not open / find the image file" << endl;
		return -1;
	}

	int frame_width = 100; // Define the target width of the frame
	int frame_height = 800; // Define the target height of the frame
	Mat frame;
	cv::resize(image, frame, cv::Size(frame_width, frame_height)); // Apply resize

	std::ostringstream name; // Create custom name
	name << char_array << "resized.jpg";
	bool isSuccess = imwrite(name.str(), frame); // Save the frame with a custom name
	if (isSuccess == false) {// Check for failure of saving the frame
		cout << "Failed to save the image" << endl;
		return -1;
	}
	return 1;
}

int videoSplitter(int* fullPathNameSize,queue<string>* queueOfPaths, char* videoPath) //split the video into frames
{
	VideoCapture cap(videoPath); //open the video file for reading from the path: videoPath

	if (cap.isOpened() == false) { // Check for failure
		cout << "Could not open the video file" << endl;
		return -1;
	}

	int frame_width = static_cast<int>(cap.get(CAP_PROP_FRAME_WIDTH)); // Get the width of frames of the video
	int frame_height = static_cast<int>(cap.get(CAP_PROP_FRAME_HEIGHT)); // Get the height of frames of the video
	Size frame_size(frame_width, frame_height);
	double fps = cap.get(CAP_PROP_FPS); // Get the frame rate per second for video
	cout << "Frames per seconds : " << fps << endl;
	cout << " Original frame size : " << frame_size << endl;
	int i = 0; // iterator for naming the frames

	while (true)
	{
		Mat frame;
		bool bSuccess = cap.read(frame); // Read a new frame from video 
		if (bSuccess == false) {
				cout << "Video processing has ended" << endl; // Breaking the while loop if frames cannot be read from the camera
				break;
		}
		
		std::ostringstream name; // Save each frame with an iterator
		if (i < 10) {
			name << "C:\\Users\\Klaudia\\source\\repos\\Test2\\Test2\\frames\\image_0000" << i << ".jpg";
		}
		else if (i < 100) {
			name << "C:\\Users\\Klaudia\\source\\repos\\Test2\\Test2\\frames\\image_000" << i << ".jpg";
		}
		else if (i < 1000) {
			name << "C:\\Users\\Klaudia\\source\\repos\\Test2\\Test2\\frames\\image_00" << i << ".jpg";
		}
		else if(i < 10000) {
			name << "C:\\Users\\Klaudia\\source\\repos\\Test2\\Test2\\frames\\image_0" << i << ".jpg";
		}
		else if (i<100000) {
				name << "C:\\Users\\Klaudia\\source\\repos\\Test2\\Test2\\frames\\image_" << i << ".jpg";
		}
		else {
			cout << "We do not rescale more than  100000 frames" << endl; // Set a limit on number of frames to process (arbitrary)
			break;
		}

		*fullPathNameSize = static_cast<int> (name.str().size()); // Define the size of the frame path
		bool isSuccess = imwrite(name.str(), frame); // Save the frame to a path
		queueOfPaths->push(name.str()); // Add path to the queue
		if (isSuccess == false) { // Check for failure
			cout << "Failed to save the image" << i << endl;
			return -1;
		}

	i++; 
}
	return 0;
}

void master(char* videoPath) // Defining master process
{
	queue<string> queueOfPaths; 
	int fullPathNameSize;
	int result = videoSplitter(&fullPathNameSize, &queueOfPaths, videoPath);
	int messageResult;
	MPI_Status status;
	
	MPI_Barrier(MPI_COMM_WORLD);
	int size;
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	for (int i = 1; i < size; i++) {
		MPI_Send(&fullPathNameSize, 1, MPI_INT, i, SETUPTAG, MPI_COMM_WORLD); // Send the size of the paths for trimming 
	}
	for (int i = 1; i < size; i++) {
		MPI_Recv(&messageResult, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status); //  Receive the result
	}

	for (int i = 1; i < size; i++) { // Assign first jobs
		if (!queueOfPaths.empty()) {
			std::string fullPathName = queueOfPaths.front();
			queueOfPaths.pop(); // Unload the queue
			MPI_Send(fullPathName.data(), fullPathName.size(), MPI_CHAR, i, WORKTAG, MPI_COMM_WORLD); //pass path (strings), in future it can pass urls
		}
	}

	while (!queueOfPaths.empty()) { // Assign more jobs
		MPI_Recv(&messageResult, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD,&status);
		std::string fullPathName = queueOfPaths.front();
		queueOfPaths.pop(); // Unload the queue
		MPI_Send(fullPathName.data(), fullPathName.size(), MPI_CHAR, status.MPI_SOURCE, WORKTAG, MPI_COMM_WORLD); //pass path (strings)
	}
	// Send message to kill slave processes
	for (int i = 1; i < size; i++) {
		MPI_Send(&fullPathNameSize, 1, MPI_INT, i, DIETAG, MPI_COMM_WORLD);
	}
}

void slave(int rank) // Slave process
{
	MPI_Barrier(MPI_COMM_WORLD);
	int pathSize;
	MPI_Status status;
	MPI_Recv(&pathSize, 1, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE); // Receive the path size
	MPI_Send(&pathSize, 1, MPI_INT, 0, 0, MPI_COMM_WORLD); // Send confirmation of receiving
	for (;;) { // Send frame paths
		char arr[200]; // Array to store the path (too large, needs trimming later)
		MPI_Recv(arr, _countof(arr), MPI_CHAR, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
		if (status.MPI_TAG == DIETAG) { // DIETAG received
			break;
		}
		int sum = slaveProcessor(&pathSize, arr); // Resize the frames
		MPI_Send(&sum, 1, MPI_INT, 0, 0, MPI_COMM_WORLD); // Send slave process confirmation message
	}
	printf("Rank %d revceived string from Rank 0\n", rank); // Slave process summary
}
