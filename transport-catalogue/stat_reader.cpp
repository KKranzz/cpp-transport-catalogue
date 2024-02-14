#include "stat_reader.h"

void statreader::ParseAndPrintStat(const transport_catalogue::processing::TransportCatalogue& tansport_catalogue,
	std::string_view request, std::ostream& output)
{
	if (request[0] == 'B') {
		std::string busname(request.begin() + request.find_first_of(" "), request.end());
		busname = busname.substr(1);

		auto bus = tansport_catalogue.FindBus(busname);
		if (bus == nullptr)
		{
			output << "Bus " << busname << ": not found\n";
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


		output << "Bus " << busname << ": " << total_stops
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
			output << "Stop " << stopname << ": not found\n";
			return;
		}

		if (stop->buses_.empty())
		{
			output << "Stop " << stopname << ": no buses\n";
			return;
		}

		output << "Stop " << stopname << ": buses";
		for (auto& busname : stop->buses_) 
		{
			output << " " << busname;
		}
		output << std::endl;
	}
}
