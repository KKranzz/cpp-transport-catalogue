#include <iostream>
#include <string>
#include <fstream>

#include "input_reader.h"
#include "stat_reader.h"

using namespace std;

int main() {
    transport_catalogue::processing::TransportCatalogue catalogue;
    fstream file("C:\\Users\\1655082\\Desktop\\testdata\\t1i.txt");
    
    int base_request_count;
    file >> base_request_count >> ws;
   
    {
        inreader::InputReader reader;
        for (int i = 0; i < base_request_count; ++i) {
            string line;
            getline(file, line);
            reader.ParseLine(line);
        }
        reader.ApplyCommands(catalogue);
    }
    ofstream ifile("C:\\Users\\1655082\\Desktop\\testdata\\t1o.txt");
    int stat_request_count;
    file >> stat_request_count >> ws;
    for (int i = 0; i < stat_request_count; ++i) {
        string line;
        getline(file, line);
        statreader::ParseAndPrintStat(catalogue, line, ifile);
    }
}
