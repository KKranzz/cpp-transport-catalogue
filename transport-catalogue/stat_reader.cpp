#include "stat_reader.h"



void statreader::StatReader::ParseAndPrintStat(const transport_catalogue::processing::TransportCatalogue& tansport_catalogue,
	std::string_view request)
{
	if (request[0] == 'B') {
		std::string busname(request.begin() + request.find_first_of(" "), request.end());
		busname = busname.substr(1);

		auto bus = tansport_catalogue.FindBus(busname);
		if (bus == nullptr)
		{
			*os_ << "Bus " << busname << ": not found\n";
			return;
		}
		unsigned total_stops = 0, unique_stops = 0;
		double rlength = 0;

		std::unordered_set<const transport_catalogue::identity::Stop*> unique(bus->stops_.begin(), bus->stops_.end());
		unique_stops = unique.size();
		total_stops = bus->stops_.size();

		for (unsigned i = 0; i < total_stops - 1; ++i)
		{
			rlength += ComputeDistance(bus->stops_.at(i)->coord_, bus->stops_.at(i + 1)->coord_);
		}


		*os_ << "Bus " << busname << ": " << total_stops
			<< " stops on route, " << unique_stops << " unique stops, "
			<< std::setprecision(6) << rlength << " route length\n";
	}
	else 
	{
		std::string stopname(request.begin() + request.find_first_of(" "), request.end());
		stopname = stopname.substr(1);

		auto stop = tansport_catalogue.FindStop(stopname);
		if (stop == nullptr)
		{
			*os_ << "Stop " << stopname << ": not found\n";
			return;
		}

		if (stop->buses_.empty())
		{
			*os_ << "Stop " << stopname << ": no buses\n";
			return;
		}

		*os_ << "Stop " << stopname << ": buses";
		for (auto& busname : stop->buses_) 
		{
			*os_ << " " << busname;
		}
		*os_ << std::endl;
	}

}
void statreader::StatReader::StartRequestParcing(const transport_catalogue::processing::TransportCatalogue& tansport_catalogue)
{
	int stat_request_count;
	*is_ >> stat_request_count >> std::ws;
	for (int i = 0; i < stat_request_count; ++i) {
		std::string line;
		getline(*is_, line);
		ParseAndPrintStat(tansport_catalogue, line);
	}
}
