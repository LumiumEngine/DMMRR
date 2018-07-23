#include "Recorder.hpp"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>

Data::Recorder::Recorder()
{
	m_firstCreation = false;
}

Data::Recorder::~Recorder()
{

}

bool Data::Recorder::getMMR()
{
	auto fileDir = screenToBmp("Dota 2");
	auto mmrData = getMMRFromBMP(fileDir);
	auto outData = writeMMRToFile(mmrData);
	if (outData)
		return true;
	else
		return false;
}

std::string Data::Recorder::screenToBmp(std::string windowTitle)
{
	auto hwnd = FindWindow(nullptr, windowTitle.c_str());
	if (hwnd != NULL)
	{
		if (GetForegroundWindow() == hwnd)
		{
			HDC hdcWindow;
			HDC hdcMemory;
			HBITMAP hBitmap;
			BYTE *lpvBits;
			BITMAPINFO bi;
			BITMAPINFOHEADER bih;
			BITMAPFILEHEADER bifh;
			HANDLE hFile;
			DWORD dwBytesWritten = 0;
			DWORD dwSize;
			int xSize, ySize;
			RECT rect;
			hdcWindow = GetDC(NULL);

			GetWindowRect(hwnd, &rect);

			xSize = rect.right - rect.left;
			ySize = rect.bottom - rect.top;
			hBitmap = CreateCompatibleBitmap(hdcWindow, xSize, ySize);
			hdcMemory = CreateCompatibleDC(hdcWindow);
			SelectObject(hdcMemory, hBitmap);
			BitBlt(hdcMemory, 0, 0, xSize, ySize, hdcWindow, rect.left, rect.top, SRCCOPY);

			bih.biSize = sizeof
			(BITMAPINFOHEADER);
			bih.biWidth = xSize;
			bih.biHeight = ySize;
			bih.biPlanes = 1;
			bih.biBitCount = 32;
			bih.biCompression = BI_RGB;
			bih.biSizeImage = 0;
			bih.biXPelsPerMeter = 0;
			bih.biYPelsPerMeter = 0;
			bih.biClrUsed = 0;
			bih.biClrImportant = 0;

			dwSize = (((xSize * 32 + 31) / 32) * 4 * ySize);
			lpvBits = (BYTE*)malloc(dwSize);

			bifh.bfType = 0x4D42;
			bifh.bfSize = sizeof
			(BITMAPFILEHEADER) +sizeof
			(BITMAPINFOHEADER) +dwSize;
			bifh.bfOffBits = (DWORD)sizeof
			(BITMAPFILEHEADER) +(DWORD)sizeof
			(BITMAPINFOHEADER);
			bifh.bfReserved1 = 0;
			bifh.bfReserved2 = 0;

			bi.bmiHeader = bih;
			
			GetDIBits(hdcMemory, hBitmap, 0, (UINT)ySize, lpvBits, &bi, DIB_RGB_COLORS);

			auto tempPath = std::filesystem::temp_directory_path();
			std::string fileName = tempPath.string() + "mmr.bmp";
			hFile = CreateFile(fileName.c_str(), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

			WriteFile(hFile, (LPSTR)&bifh, sizeof
			(BITMAPFILEHEADER), &dwBytesWritten, NULL);
			WriteFile(hFile, (LPSTR)&bih, sizeof
			(BITMAPINFOHEADER), &dwBytesWritten, NULL);
			WriteFile(hFile, (LPSTR)lpvBits, dwSize, &dwBytesWritten, NULL);

			DeleteDC(hdcMemory);
			DeleteObject(hBitmap);
			CloseHandle(hFile);
			free(lpvBits);
			ReleaseDC(hwnd, hdcWindow);
			return fileName;
		}
	}
	return std::string();
}

std::string Data::Recorder::getMMRFromBMP(std::string bmpFile)
{
	char *outText;

	auto api = new tesseract::TessBaseAPI();
	// Initialize tesseract-ocr with English, without specifying tessdata path
	if (api->Init(nullptr, "eng"))
	{
		std::cerr << "Could not initialize tesseract.\n";
		api->End();
		return std::string();
	}
	Pix* image = pixRead(bmpFile.c_str());
	if (image == NULL)
	{
		std::cerr << "Could not get the required image to load\n";
		api->End();
		return std::string();
	}
	api->SetImage(image);
	api->SetRectangle(static_cast<int>((image->w / 4) * 3), static_cast<int>(image->h / 8), static_cast<int>(image->w / 4), static_cast<int>(image->h / 8));
	// Get OCR result
	outText = api->GetUTF8Text();
	api->End();
	pixDestroy(&image);
	auto returnVal = std::string(outText);
	delete[] outText;
	delete[] api;
	return returnVal;
}

bool Data::Recorder::writeMMRToFile(std::string text)
{
    // output the text to the file
    auto soloLoc = text.find("Solo");
    auto partyLoc = text.find("Party");
    if(soloLoc == std::string::npos || partyLoc == std::string::npos)
    {
        std::cerr << "Could not find solo or party mmr\n";
		return false;
    }
    else
    {
		// create the mmr file if it doesn't exist
		createDataFile();

        std::string lastSolo, lastParty;
        std::string soloMMR = text.substr(soloLoc + 5, text.find("\n", soloLoc) - (soloLoc + 5));
        std::string partyMMR = text.substr(partyLoc + 6, text.find("\n", partyLoc) - (partyLoc + 6));
        soloMMR.replace(soloMMR.find(","), 1, "");
        partyMMR.replace(partyMMR.find(","), 1, "");
		std::string originalSolo = soloMMR;
		std::string originalParty = partyMMR;
        auto date = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());

        std::ifstream lastMMR;
        lastMMR.open(std::filesystem::current_path().string() + "\\data\\lastMMR.txt", std::ifstream::in);
        if(lastMMR.is_open())
        {
            std::string lastLine;
            std::getline(lastMMR, lastLine);
            auto mmrBreak = lastLine.find("-");
            lastSolo = lastLine.substr(0, mmrBreak);
            lastParty = lastLine.substr(mmrBreak + 1, lastLine.size() - mmrBreak - 1);
        }

		if (lastSolo.size() > 0 && lastParty.size() > 0)
		{
			auto soloOffset = std::stoi(soloMMR) - std::stoi(lastSolo);
			auto partyOffset = std::stoi(partyMMR) - std::stoi(lastParty);

			if (soloOffset < 0)
			{
				soloMMR.append(" ");
				soloMMR.append(std::to_string(soloOffset));
			}
			else if (soloOffset > 0)
			{
				soloMMR.append(" +");
				soloMMR.append(std::to_string(soloOffset));
			}
			else
			{
				soloMMR.append(" ---");
			}

			if (partyOffset < 0)
			{
				partyMMR.append(" ");
				partyMMR.append(std::to_string(partyOffset));
			}
			else if (partyOffset > 0)
			{
				partyMMR.append(" +");
				partyMMR.append(std::to_string(partyOffset));
			}
			else
			{
				partyMMR.append(" ---");
			}
		}
		if (m_firstCreation)
		{
			soloMMR.append(" ---");
			partyMMR.append(" ---");
			m_firstCreation = false;
		}
        std::string output;
        output = "Solo: ";
        output += soloMMR;
        output += " Party: ";
        output += partyMMR;
        output += " ";
        output += std::ctime(&date);

        std::ofstream mmrFile;
        mmrFile.open(std::filesystem::current_path().string() + "\\data\\dotaMMR.txt", std::ofstream::app | std::ofstream::out);
        if(mmrFile.is_open())
        {
            mmrFile << output;
        }
        mmrFile.close();

        mmrFile.open(std::filesystem::current_path().string() + "\\data\\lastMMR.txt", std::ofstream::trunc | std::ofstream::out);
        if(mmrFile.is_open())
        {
            mmrFile << originalSolo << "-" << originalParty;
        }
        mmrFile.close();
		return true;
    }
}

void Data::Recorder::createDataFile()
{
    std::filesystem::path mmrFile = std::string(std::filesystem::current_path().string() + "\\data\\dotaMMR.txt");
    if(!std::filesystem::exists(mmrFile))
    {
		std::filesystem::create_directory(std::filesystem::current_path().string() + "\\data");
        std::ofstream file;
        file.open(std::filesystem::current_path().string() + "\\data\\dotaMMR.txt");
        file.close();
		m_firstCreation = true;
    }
}