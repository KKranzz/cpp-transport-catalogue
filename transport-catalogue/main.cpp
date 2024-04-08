#include <iostream>
#include <string>
#include <fstream>

#include "json_reader.h"

using namespace std;

int main() {
   transport_catalogue::processing::TransportCatalogue catalogue;
   JsonReader jreader;
   jreader.JsonIn(catalogue, std::cin);
   jreader.JsonOut(catalogue, std::out);
}
