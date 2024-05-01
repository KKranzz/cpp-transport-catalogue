#pragma once
#include <unordered_map>
#include <string>
#include <memory>
#include "transport_catalogue.h"
#include "graph.h"
#include "domain.h"
#include "router.h"

const int MINUT_IN_HOUR = 60;

class TransportRouter
{
public:
	TransportRouter() = default;
	void SetupWaitTime(size_t wait_time)
	{
		wait_time_ = wait_time;
	}

	void SetupVelocity(double bus_velocity) 
	{
		bus_velocity_ = bus_velocity/ MINUT_IN_HOUR;
	}

	void ConstructGraph(transport_catalogue::processing::TransportCatalogue& ts);

	void CreateRouterIdentity() 
	{
		router_.reset(new graph::Router<double>(std::move(graph::Router<double>(graph_))));	
	}

	std::optional<graph::Router<double>::RouteInfo> BuildRoute(size_t from, size_t to)
	{
		return router_->BuildRoute(from, to);
	}
	
	std::unordered_map<std::string, size_t> stopname_id_data_;
	graph::DirectedWeightedGraph<double> graph_;
	std::unique_ptr<graph::Router<double>> router_;

private:
	double wait_time_ = 0;
	double bus_velocity_ = 0;
	
};


