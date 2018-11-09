#include <iostream>
#include <fstream>
#include <cstring>
#include <vector>
#include "XMP_Writer.h"

using namespace std;

XMP_Writer::XMP_Writer()
{
}

XMP_Writer::~XMP_Writer()
{
}

void XMP_Writer::WriteFile(int height, int width, int numColors, int cpp, vector <vector<string> > pixels, string fileName) {
	ofstream outfile;
	outfile.open(fileName + ".xpm");

	outfile << "static char * drawing[] = {" << endl;
	outfile << "/* width height num_colors chars_per_pixel */" << endl;
	outfile << "\"" << width << " " << height << " " << numColors << " " << cpp << "\"," << endl;

	outfile << "/* colors */" << endl;
	outfile << "\"- c #ffffff\"," << endl;
	outfile << "\"# c #000000\"," << endl;

	outfile << "/* pixels */" << endl;
	vector<vector<string>> reOrdered = ChangeOrigin(pixels, height, width);
	for (int i = 0; i < height; i++) {
		outfile << "\"";
		for (int j = 0; j < width; j++) {
			outfile << reOrdered[i][j];
		}
		outfile << "\"";
		if (i + 1 < height) {
			outfile << ",";
		}
		outfile << endl;
	}
	outfile << "};";
	outfile.close();
}

vector< vector<string> > XMP_Writer::ChangeOrigin(vector <vector<string> > pixels, int height, int width) {
	vector< vector<string> > reOrdered;
	reOrdered.resize(height, vector<string>(width));
	int u = 0;
	for (int j = height - 1; j >= 0; j--) {
		for (int i = 0; i < width; i++) {
			reOrdered[u][i] = pixels[j][i];
		}
		u++;
	}
	return reOrdered;
}

