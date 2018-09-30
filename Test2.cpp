

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
#include <mpi.h>
#include <sstream>
#include <algorithm> //for trimming
#include <cctype> //for trimming
#include <locale> //for trimming


using namespace cv;
using namespace std;





int slaveProcessor(char *pathName);

int main(int argc, char** argv)
{
	//string pathName = "C:\\Users\\Klaudia\\source\\repos\\Test2\\Test2\\frames\\scaled_im_0.jpg";
	//int N, M, sum;
	std::string pathMock = "C:\\Users\\Klaudia\\source\\repos\\Test2\\Test2\\frames\\scaled_im_0.jpg";
	cout << pathMock.size() << "this is length";

	MPI_Init(&argc, &argv);

	int rank;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	if (rank == 0) {

		int size;
		MPI_Comm_size(MPI_COMM_WORLD, &size);
		cout << "size" << size << "\n";

		for (int i = 1; i < size; i++) {


			std::string path = "C:\\Users\\Klaudia\\source\\repos\\Test2\\Test2\\frames\\scaled_im_";
			std::string ID = std::to_string(i);
			std::string fileType = ".jpg";
			
			
			std::stringstream pathNameStream;
			pathNameStream << path << ID << fileType;
			std::string fullPathName = pathNameStream.str();

			std::cout << fullPathName<< fullPathName.size();



			MPI_Send(fullPathName.data(), fullPathName.size(), MPI_CHAR, i, 0, MPI_COMM_WORLD);//pass strings because in future it will pass the path, M, frame size 
		}
		//

	}
	else {
		char arr[100];
		MPI_Recv(arr, _countof(arr), MPI_CHAR, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		int sum = slaveProcessor(arr);
		printf("Rank %d revceived string from Rank 0\n", rank);
	}


	MPI_Finalize();

}


int slaveProcessor(char *path)
{
	
	char strPath[] = "C:\\Users\\Klaudia\\source\\repos\\Test2\\Test2\\frames\\scaled_im_x.jpg";
	memcpy(strPath, path, 64 /* Length */);
	/* Add terminator */


	cout << "This is our path" << strPath << endl;


	Mat image = imread(strPath);
	if (image.empty()) // Check for failure of reading image
	{
		cout << "Could not open/find the image file" << endl;
		system("pause"); //wait for any key press
		return -1;
	}

	//get the frames rate of the video
	int frame_width = 100; // define the width of frames of the video
	int frame_height = 800; //define the height of frames of the video
	Size frame_size(frame_width, frame_height);
	cout << "Resize to : " << frame_width << ","<<frame_height << endl; // target size

	// Make changes to the frames: resize
	Mat frame;
	cv::resize(image, frame, cv::Size(frame_width, frame_height));
	cout << "Image is succusfully processed" << endl;

	std::ostringstream name; //create custom name
	name << strPath <<"resized.jpg";
	bool isSuccess = imwrite(name.str(), frame); //save the frame with a custom name
	cout << "Image is succusfully saved to a file" << endl;
	if (isSuccess == false)
	{
		cout << "Failed to save the image" << endl;
		cin.get(); //wait for a key press
		return -1;
	}




	return 1;
}

