#define _FILE_OFFSET_BITS 64

#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <iostream>
#include <string>
#include <vector>
#include <typeinfo>

#include "handleError.h"
#include "sliceClass.h"

using namespace std;

void usage()
{
	std::cerr << "Syntax:\n";
	std::cerr << "slice file1 param1 file2 param2 ...\n";
	std::cerr << "Limited to a maximum of 255 files to process.\n\n";
	std::cerr << "With paramX like:\n";
	std::cerr << "X[]Y : skip X bytes from the beginning and skip Y bytes before the end of file\n";
	std::cerr << "X[Y] : skip X bytes from the beginning and then keep Y bytes\n";
	std::cerr << "[X]Y : skip Y bytes before the end and keep X bytes before\n\n";
	std::cerr << "Return codes:\n";
	std::cerr << " 0 = Success\n";
	std::cerr << "-1 = Error: missing parameters\n";
	std::cerr << "-2 = Error: can't allocate memory for file buffer\n";
	std::cerr << "-3 = Error: can't access to a file\n";
	std::cerr << "-4 = Error: file I/O error during initialization\n";
	std::cerr << "-5 = Error: wrong cutting parameters\n";
	std::cerr << "-6 = Error: file I/O error during reading/writing\n";
	return;
}

int main(int argc, char *argv[]) {

	uint8_t	nbfiles = 0;
	Slice myCatFile;

	string ex;

	if (argc==1 || ((argc % 2)!=1)) // There should be an odd number of arguments: executable name + x pairs of paramaters
	{
		cerr << "Error: Missing paramater(s)\n\n";
		usage();
		exit(-1);
	}

	nbfiles = argc/2; // Number of files to process (pair of file name + cutting parameters

	cerr << "Number of files to process:" << +nbfiles << endl << endl;

	for (uint8_t i=0; i<nbfiles; i++)
	{
		try
		{
			myCatFile.initialize(argv[(2*i)+1],argv[(2*i)+2]);
		}
		catch(errorInfo const& error)
		{
			handleError(error);
		}
		//if (!extractParams(argv[(2*i)+2],argv[(2*i)+1]))
/*		if ((access(argv[(2*i)+1],R_OK))!=0)
		{
			cerr << "Error: Can't access to file " << argv[(2*i)+1] << ": ";
			perror(NULL);
			exit(-2);
		}
*/
		if (!myCatFile.extractParams())
		{
			cerr << endl;
			usage();
			exit(-5);
		}

		if (myCatFile.isReadyToCat())
		{
			try
			{
				myCatFile.executeCat();
			}
			catch(errorInfo const& error)
			{
				handleError(error);
			}
		}
		cerr << "\n";

		myCatFile.clean();
	}

	cerr << "Done!\n\n";

	return 0;
}
