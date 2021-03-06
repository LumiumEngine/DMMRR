#pragma once
#include <leptonica/allheaders.h>
#include <tesseract/baseapi.h>
#include <Windows.h>
#include <string>

namespace Data
{
    class Recorder
    {
    public:
        Recorder();
        ~Recorder();

		bool getMMR();
    private:
        void createDataFile();
		std::string screenToBmp(std::string windowTitle);
		std::string getMMRFromBMP(std::string bmpFile);
		bool writeMMRToFile(std::string text);
		bool m_firstCreation;
    };
};