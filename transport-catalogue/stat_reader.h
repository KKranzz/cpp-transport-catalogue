#pragma once

#include <iosfwd>
#include <string_view>
#include <unordered_set>
#include <iomanip>

#include "transport_catalogue.h"

namespace statreader {
    void ParseAndPrintStat(const transport_catalogue::processing::TransportCatalogue& tansport_catalogue, std::string_view request,
        std::ostream& output);
}
