#include<vector>
#include<string>

#pragma once
#ifndef XMP_Writer_H
#define XMP_Writer_H

class XMP_Writer
{
public:
	XMP_Writer();
	~XMP_Writer();
	void WriteFile(int height, int width, int numColors, int cpp, std::vector< std::vector<std::string> > pixels, std::string fileName);
	std::vector< std::vector<std::string> > ChangeOrigin(std::vector < std::vector<std::string> > pixels, int height, int width);

private:

};

#endif
