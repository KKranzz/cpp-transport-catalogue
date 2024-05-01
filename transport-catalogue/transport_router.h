#pragma once
#include <unordered_map>
#include <string>
#include "transport_catalogue.h"
#include "graph.h"
#include "domain.h"
#include "router.h"

const int MINUT_IN_HOUR = 60;

class GraphConstructor 
{
public:
	GraphConstructor() = default;
	void SetupWaitTime(size_t wait_time)
	{
		wait_time_ = wait_time;
	}

	void SetupVelocity(double bus_velocity) 
	{
		bus_velocity_ = bus_velocity/ MINUT_IN_HOUR;
	}

	void ConstructGraph(transport_catalogue::processing::TransportCatalogue& ts);
	
	std::unordered_map<std::string, size_t> stopname_id_data_;
	graph::DirectedWeightedGraph<double> graph_;

private:
	double wait_time_ = 0;
	double bus_velocity_ = 0;
	
};


class ProcessRoute 
{
public:
	ProcessRoute(graph::DirectedWeightedGraph<double>& graph) : router_(graph::Router<double>(graph)){}
	graph::Router<double> router_;
};