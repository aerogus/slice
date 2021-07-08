#ifndef SLICECLASS_H_
#define SLICECLASS_H_

#include <string>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <iostream>

#include "handleError.h"

class Slice {

	private:

		char *m_filename = nullptr;
		char *m_param = nullptr;
		FILE *m_pFile = nullptr;
		off_t m_toSkip = 0, m_toKeep = 0;
		bool m_bIsReady = false, m_bIsInitialized = false;
		off_t m_filesize = 0;
		uint8_t *m_buffer = nullptr;
		const uint32_t maxBufSize = 10000000;
		errorInfo m_errorInfo;

	public:

		Slice();
		Slice(char *filename, char *param);
		virtual ~Slice();
		void initialize(char *filename, char *param);
		bool extractParams();
		bool isReadyToCat();
		bool executeCat();
		void clean();
};

#endif /* SLICECLASS_H_ */
