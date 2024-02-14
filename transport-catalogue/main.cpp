#include <iostream>
#include <string>
#include <fstream>

#include "input_reader.h"
#include "stat_reader.h"

using namespace std;

int main() {
    transport_catalogue::processing::TransportCatalogue catalogue;
    fstream file("C:\\Users\\1655082\\Desktop\\testdata\\t1i.txt");
    inreader::InputReader reader(&file);
    reader.StartParcing(catalogue);

    ofstream ifile("C:\\Users\\1655082\\Desktop\\testdata\\t1o.txt");
    statreader::StatReader sreader(&ifile, &file);
    sreader.StartRequestParcing(catalogue);

}
