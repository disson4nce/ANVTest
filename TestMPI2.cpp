// TestMPI2.cpp : This file contains the 'main' function. Program execution begins and ends there.
//


#include <string>
#include "pch.h"
#include <mpi.h>
#include <iostream>
#include <sstream>



using namespace std;

int main(int argsc, char *argv[])
{
	
	MPI_Init(&argsc, &argv);

	int rank;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	if (rank == 0) {
		
		int size;
		MPI_Comm_size(MPI_COMM_WORLD, &size);
		cout << "size"<<size << "\n";
		
		
		for (int i = 1; i < size;i++) {
		
			
			std::string path = "C:\\Users\\Klaudia\\source\\repos\\Test2\\Test2\\frames\\scaled_im_";
			std::string ID = std::to_string(i);
			std::string fileType = ".jpg";

			std::stringstream ss;
			ss << path << ID << fileType;
			std::string s = ss.str();

			std::cout << s;
			
			
	
			MPI_Send(s.c_str(),s.size(), MPI_CHAR, i, 0, MPI_COMM_WORLD);//pass strings because in future it will pass the path, M, frame size 
		}
		//

	}
	else {
		char arr[100];
		MPI_Recv(arr,_countof(arr), MPI_CHAR, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		printf("Rank %d revceived string from Rank 0\n", rank);
	}
	

	MPI_Finalize();
	//return 0;

}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
