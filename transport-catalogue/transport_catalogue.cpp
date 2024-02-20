#include "transport_catalogue.h"


void transport_catalogue::processing::TransportCatalogue::AddStop(std::string&& name, geo_calc::Coordinates coord)
{
	stops_.push_back({ std::move(name), std::move(coord) });
	stopname_to_stops_[stops_.back().name_] = &stops_.back();
}

const transport_catalogue::identity::Stop* transport_catalogue::processing::TransportCatalogue::FindStop(const std::string& name_stop)
const {
	if (stopname_to_stops_.find(name_stop) != stopname_to_stops_.end()) {
		return stopname_to_stops_.at(name_stop);
	}
	return nullptr;
}

void transport_catalogue::processing::TransportCatalogue::AddBus(const std::string& name, std::vector<std::string>& stopnames)
{
	transport_catalogue::identity::Bus new_bus;
	new_bus.name_ = name;
	for (auto& name_stop : stopnames) {
		new_bus.stops_.push_back(stopname_to_stops_[name_stop]);
	}

	buses_.push_back(std::move(new_bus));
	busname_to_bus_[buses_.back().name_] = &buses_.back();

	for (auto& stop : buses_.back().stops_) 
	{
		stopname_to_stops_[stop->name_]->buses_.insert(buses_.back().name_);
	}
}

const transport_catalogue::identity::Bus* transport_catalogue::processing::TransportCatalogue::FindBus(const std::string& name_bus)
const {
	if (busname_to_bus_.find(name_bus) != busname_to_bus_.end()) {
		return busname_to_bus_.at(name_bus);
	}
	return nullptr;
}

void transport_catalogue::processing::TransportCatalogue::AddDistance(std::string&& first, std::string&& second, int dist)
{
	dist_data_[{stopname_to_stops_[first], stopname_to_stops_[second]}] = dist;
}

 int transport_catalogue::processing::TransportCatalogue::FindDist( identity::Stop* stop1,   identity::Stop* stop2)
 {
	if (dist_data_.find({ stop1,stop2 })!= dist_data_.end()) 
	{
		return dist_data_.at({ stop1,stop2 });
	}
		return dist_data_.at({ stop2,stop1 });
	
	
}
