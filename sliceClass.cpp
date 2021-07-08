#include "sliceClass.h"

Slice::Slice() = default;

Slice::Slice(char *filename, char *param) {
	initialize(filename,param);
}

Slice::~Slice() {
	clean();
}

void Slice::initialize(char *filename, char *param)
{
	if (!m_bIsInitialized)
	{
		m_filename = filename;
		m_param = param;

		m_buffer = new (std::nothrow) uint8_t[maxBufSize];
		if (!m_buffer)
		{
			m_errorInfo.errorCode = -2;
			m_errorInfo.errorMessage = std::string("Error: can't allocate buffer for file I/O\n");
			throw m_errorInfo;
		}

		std::cerr << "Processing " << m_filename << "..." << std::endl;

		m_pFile = fopen(m_filename,"r");
		if (m_pFile == nullptr)
		{
			m_errorInfo.errorCode = -3;
			m_errorInfo.errorMessage = std::string("Error: can't open file ");
			throw m_errorInfo;
		}

		if (setvbuf(m_pFile, NULL , _IOFBF, maxBufSize) != 0)
		{
			m_errorInfo.errorCode = -4;
			m_errorInfo.errorMessage = std::string("Error: File I/O error, can't define file buffering method.\n");
			throw m_errorInfo;
		}

		if (fseeko(m_pFile, 0, SEEK_END) != 0)
		{
			m_errorInfo.errorCode = -4;
			m_errorInfo.errorMessage = std::string("Error: File I/O error, can't seek to end of file.\n");
			throw m_errorInfo;
		}

		m_filesize = ftello(m_pFile);
		if (m_filesize == -1)
		{
			m_errorInfo.errorCode = -4;
			m_errorInfo.errorMessage = std::string("Error: File I/O error, can't get file size\n");
			throw m_errorInfo;
		}

		std::cerr << "Original file size: " << m_filesize << " bytes" << std::endl;
		m_bIsInitialized = true;
	}
/*	else
	{
		// ERROR already initialized
	}
*/
}

bool Slice::extractParams()
{
	std::string parameters = std::string(m_param);
	std::size_t firstBracketPos = 0, secondBracketPos = 0;
	int64_t firstValue = 0, middleValue = 0, lastValue = 0;
	bool bFirstVal = false, bMiddleVal = false, bLastVal = false;

	firstBracketPos = parameters.find_first_of("[");
	secondBracketPos = parameters.find_first_of("]");

	if (firstBracketPos != 0)
	{
		if (parameters.substr(0,firstBracketPos).find_first_not_of("0123456789") == std::string::npos)
		{
			firstValue = std::stoull(parameters.substr(0,firstBracketPos));
			//std::cerr << "param1 = " << firstValue << std::endl;
			bFirstVal = true;
		}
		else
		{
			std::cerr << "Error: Invalid value " << parameters.substr(0,firstBracketPos) << " in " << parameters << std::endl;
			return false;
		}
	}

	if (secondBracketPos != (firstBracketPos + 1))
	{
		if (parameters.substr(firstBracketPos + 1,secondBracketPos - firstBracketPos - 1).find_first_not_of("0123456789") == std::string::npos)
		{
			middleValue = std::stoull(parameters.substr(firstBracketPos + 1,secondBracketPos - firstBracketPos - 1));
			//std::cerr << "param2 = " << middleValue << std::endl;
			bMiddleVal = true;
		}
		else
		{
			std::cerr << "Error: Invalid value " << parameters.substr(firstBracketPos + 1,secondBracketPos - firstBracketPos - 1) << " in " << parameters << std::endl;
			return false;
		}
	}

	if (secondBracketPos != (parameters.length() - 1))
	{
		if (parameters.substr(secondBracketPos + 1,parameters.length() - 1).find_first_not_of("0123456789") == std::string::npos)
		{
			lastValue = std::stoull(parameters.substr(secondBracketPos + 1,parameters.length() - 1));
			//std::cerr << "param3 = " << lastValue << std::endl;
			bLastVal = true;
		}
		else
		{
			std::cerr << "Error: Invalid value " << parameters.substr(secondBracketPos + 1,parameters.length() - 1) << " in " << parameters << std::endl;
			return false;
		}
	}

	if (bFirstVal && bMiddleVal && bLastVal)
	{
		std::cerr << "Error: Too many values in " << parameters << ", you must provide only 2 values." << std::endl;
		return false;
	}

	if ((((bFirstVal)?1:0) + ((bMiddleVal)?1:0) + ((bLastVal)?1:0)) != 2) // Counting number of values: we must have exactly 2
	{
		std::cerr << "Error: Not enough values in " << parameters << ", you must provide 2 values." << std::endl;
		return false;
	}

	if ((firstValue > m_filesize) || (middleValue > m_filesize) || (lastValue > m_filesize))
	{
		std::cerr << "Error: One or more value in " << parameters << " exceeds input file size!\n";
		return false;
	}

	if (bFirstVal) // parameters like X[Y] or X[]Y
	{
		m_toSkip = firstValue;
		if (bMiddleVal)
		{
			if ((firstValue + middleValue)>m_filesize)
			{
				std::cerr << "Error: Given values in " << parameters << " = cumulated size of " << firstValue + middleValue << " bytes exceeding input file size!\n";
				return false;
			}
			m_toKeep = middleValue;
		}
		else
		{
			if ((firstValue + lastValue)>m_filesize)
			{
				std::cerr << "Error: Given values in " << parameters << " = cumulated size of " << firstValue + lastValue << " bytes exceeding input file size!\n";
				return false;
			}
			m_toKeep = m_filesize - firstValue - lastValue;
		}
	}
	else // parameters like [X]Y
	{
		if ((middleValue + lastValue)>m_filesize)
		{
			std::cerr << "Error: Given values in " << parameters << " = cumulated size of " << middleValue + lastValue << " bytes exceeding input file size!\n";
			return false;
		}
		m_toSkip = m_filesize - (middleValue + lastValue);
		m_toKeep = middleValue;
	}

	std::cerr << "Skeeping first " << m_toSkip << " bytes, keeping " << m_toKeep << " bytes..." << std::endl;
	m_bIsReady = true;
	return true;
}

bool Slice::isReadyToCat()
{
	return m_bIsInitialized && m_bIsReady;
}

bool Slice::executeCat()
{
	size_t bufsize = maxBufSize;
	off_t counter = m_toKeep;

	if (isReadyToCat())
	{
		if (fseeko(m_pFile, m_toSkip, SEEK_SET) != 0)
		{

		}

		while(counter > 0)
		{
			if (counter < maxBufSize)
			{
				bufsize = counter;
			}

			if (fread(m_buffer,1,bufsize,m_pFile) != bufsize)
			{
				m_errorInfo.errorCode = -6;
				m_errorInfo.errorMessage = std::string("Error: File I/O error while reading\n");
				throw m_errorInfo;
			}
			if ((fwrite(m_buffer,1,bufsize,stdout) != bufsize) && ferror(m_pFile))
			{
				m_errorInfo.errorCode = -6;
				m_errorInfo.errorMessage = std::string("Error: File I/O error while writing\n");
				throw m_errorInfo;
			}
			counter-=bufsize;
		}
		fflush(stdout);

	}
	else
	{
		std::cerr << "Error: parameters have not been parsed and validated, not ready to slice.\n";
		return false;
	}
	return true;
}

void Slice::clean()
{
	m_bIsInitialized = false;
	m_bIsReady = false;

	if (m_pFile)
	{
		fclose(m_pFile);
		m_pFile = nullptr;
	}

	if (m_buffer)
	{
		delete[] m_buffer;
		m_buffer = nullptr;
	}
}
