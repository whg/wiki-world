//
//  fdata.h
//  example_basic
//
//  Created by Will Gallia on 30/10/2015.
//
//

#pragma once

#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>


inline void read_fdata(std::string filename, vector< vector<float> > &data) {
    
    std::ifstream infile(filename.c_str(), std::ifstream::binary);
    
    // read the header, just two 32 bit ints
    int numRows, rowLength;
    infile.read((char*) &numRows, sizeof(int));
    infile.read((char*) &rowLength, sizeof(int));

    data.clear();
    data.resize(numRows);
    
    float *buffer = new float[rowLength];
    
    // grab all the data
    for (int i = 0; i < numRows; i++) {
        infile.read((char*) buffer, rowLength * sizeof(float));
        
        data[i].resize(rowLength);
        copy(buffer, buffer + rowLength, &data[i][0]);
    }
    
    delete buffer;
}