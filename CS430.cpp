#include "XMP_Writer.h"
#include <cstdlib>
#include <stdio.h>
#include <cmath>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>

using namespace std;

const int TOP = 8; //1000
const int BOT = 4; //0100
const int RIGHT = 2; //0010
const int LEFT = 1; //0001

void createPixels(vector< vector<string> >& pixels, int height, int width, string c1symbol);
void clipLine(float x1, float y1, float x2, float y2, float x_min, float x_max, float y_min, float y_max, string c2symbol, vector< vector<string> >& pixels);
int createEndpointCode(float x, float y, float x_min, float x_max, float y_min, float y_max);
void adjustEndpoint(int code, int& xc, int &yc, float x1, float y1, float x2, float y2, float x_min, float x_max, float y_min, float y_max);
void drawLine(float x1, float y1, float x2, float y2, float x_min, float x_max, float y_min, float y_max, string c2symbol, vector< vector<string> >& pixels);

int main(int argc, char*argv[]) {

	float x1, y1, x2, y2;
	float x_min, x_max, y_min, y_max;
	int width, height;
	string filename, line, word;

	//color symbols, with c1 being the default white
	string c1symbol, c2symbol;
	c1symbol = "-";
	c2symbol = "#";

	//Ensure that enough arguments are present
	if (argc < 2) {
		cerr << "Program requires a file to run";
		return 1;
	}
	//Parse arguments to find filename
	for (int i = 1; i < argc; i++) {
		if (string(argv[i]) == "-f") {
			if (i + 1 < argc) {
				i++;
				filename = argv[i];
			}
			else {
				cerr << "no file found";
				return 1;
			}
		}
	}

	ifstream input;
	filename = filename + ".ps";
	input.open(filename);
	
	bool inDelimiters = false;

	vector< vector<int> > inputLines;

	if (!input) {
		cerr << "File could not be opened";
		return 1;
	} else if (input){	
		while (getline(input, line)) {
			if (line.compare("%%%END") == 0) {
				inDelimiters = false;
			} else if (inDelimiters) {
				stringstream ss(line);
				string temp;
				vector<string> tokens;

				while (ss >> temp) {
					tokens.push_back(temp);
				}

				if (tokens.size() == 5) {
					if (tokens[4].compare("Line") == 0) {
						vector<int> tempv;
						for (int i = 0; i < 4; i++) {
							tempv.push_back(stoi(tokens[i]));
						}
						inputLines.push_back(tempv);
					}
				}
			} else if (line.compare("%%%BEGIN") == 0) {
				inDelimiters = true;
			}
		}
	}

	//For now, hard code size of drawing bounds
	x_min = 0;
	x_max = 500;
	y_min = 0;
	y_max = 500;

	//Calculate width and height of drawing bounds
	height = (int)(y_max - y_min);
	width = (int)(x_max - x_min);

	vector< vector<string> > pixels;
	createPixels(pixels, height, width, c1symbol);

	for (int i = 0; i < inputLines.size(); i++) {
		clipLine(inputLines[i][0], inputLines[i][1], inputLines[i][2], inputLines[i][3], x_min, x_max, y_min, y_max, c2symbol, pixels);
	}

	XMP_Writer writer;
	writer.WriteFile(height, width, 2, 1, pixels, "Test");

	
	return 0;
}

void clipLine(float x1, float y1, float x2, float y2, float x_min, float x_max, float y_min, float y_max, string c2symbol, vector< vector<string> >& pixels) {
	int code0 = createEndpointCode(x1, y1, x_min, x_max, y_min, y_max);
	int code1 = createEndpointCode(x2, y2, x_min, x_max, y_min, y_max);

	if (code0 == 0 && code1 == 0) {
		drawLine(x1, y1, x2, y2, x_min, x_max, y_min, y_max, c2symbol, pixels);
	}
	else if (code0 & code1) {
		//line is completely outside the bounding box
	} else {
		//create new endpoints & initialize them to the original endpoints
		int xc1 = x1;
		int yc1 = y1;
		int xc2 = x2;
		int yc2 = y2;

		adjustEndpoint(code0, xc1, yc1, x1, y1, x2, y2, x_min, x_max, y_min, y_max);
		adjustEndpoint(code1, xc2, yc2, x1, y1, x2, y2, x_min, x_max, y_min, y_max);
		drawLine(xc1, yc1, xc2, yc2, x_min, x_max, y_min, y_max, c2symbol, pixels);
	}	
}

int createEndpointCode(float x, float y, float x_min, float x_max, float y_min, float y_max) {
	int code = 0000;

	if (y >= y_max) {
		code |= TOP;
	} else if (y < y_min) {
		code |= BOT;
	} 
	if (x >= x_max) {
		code |= RIGHT;
	} else if (x < x_min) {
		code |= LEFT;
	}

	return code;
}

void adjustEndpoint(int code, int& xc, int &yc, float x1, float y1, float x2, float y2, float x_min, float x_max, float y_min, float y_max) {
	if ((code & TOP) == TOP) {
		yc = y_max;
		xc = x1 + ((x2 - x1) / (y2 - y1)) * (yc - y1);
		//subtract one so not out of bounds
		yc--;
	}
	else if ((code & BOT) == BOT) {
		yc = y_min;
		xc = x1 + ((x2 - x1) / (y2 - y1)) * (yc - y1);
	}
	else if ((code & RIGHT) == RIGHT) {
		xc = x_max;
		yc = y1 + ((y2 - y1) / (x2 - x1)) * (xc - x1);
		//subtract one so not out of bounds
		xc--;
	}
	else if ((code & LEFT) == LEFT) {
		xc = x_min;
		yc = y1 + ((y2 - y1) / (x2 - x1)) * (xc - x1);
	}
}

void createPixels(vector< vector<string> >& pixels, int height, int width, string c1symbol) {
	pixels.resize(height, vector<string>(width));

	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			pixels[i][j] = c1symbol;
		}
	}
}

void drawLine(float x1, float y1, float x2, float y2, float x_min, float x_max, float y_min, float y_max, string c2symbol, vector< vector<string> >& pixels) {
	float currentPx, currentPy, slope;
	bool isVert;

	//Check if vertical line.  If not, calculate slope
	if (x2 - x1 == 0) {
		isVert = true;
	}
	else {
		isVert = false;
		slope = (y2 - y1) / (x2 - x1);
	}

	//Set current pixels to the start of the line
	currentPx = x1;
	currentPy = y1;
	pixels[currentPy][currentPx] = c2symbol;

	if (!isVert) {
		int steps;
		float deltaX = x2 - x1;
		float deltaY = y2 - y1;

		if (abs(slope) < 1) {
			steps = abs(deltaX);
		} else {
			steps = abs(deltaY);
		}

		deltaX = (deltaX / steps);
		deltaY = (deltaY / steps);
		currentPx = x1;
		currentPy = y1;

		for (int i = 0; i < steps; i++) {
			pixels[currentPy][currentPx] = c2symbol;
			x1 = (x1 + deltaX);
			currentPx = round(x1);
			y1 = (y1 + deltaY);
			currentPy = round(y1);
		}
	}
}
