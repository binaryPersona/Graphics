CXX=g++
CXXFLAGS=-Wall -std=c++11
main: CS430.o XMP_Writer.o
	$(CXX) $(FLAG)-o main CS430.o XMP_Writer.o
main.o: CS430.cpp XMP_Writer.h
	$(CXX) $(FLAG) -c CS430.cpp
XML_Writer.o: XML_Writer.cpp XML_Writer.h
	$(CXX) $(FLAG) -c XML_Writer.cpp
