#pragma once
#include <unordered_map>
#include <string>
#include <memory>
#include "transport_catalogue.h"
#include "graph.h"
#include "domain.h"
#include "router.h"

const int MINUT_IN_HOUR = 60;

struct RouterSet 
{
	double wait_time_ = 0;
	double bus_velocity_ = 0;
};

class TransportRouter
{
public:
	TransportRouter() = default;
	void SetupRouterSettings(RouterSet set)
	{
		set_ = set;
	}
	void CreateRouter(transport_catalogue::processing::TransportCatalogue& ts)
	{
		size_t id = 0;
		for (auto& stop : ts.GetStopNames())
		{
			stopname_id_data_[stop] = id;
			id++;
		}
		ConstructGraph(ts);
		CreateRouterIdentity();
	}
	const graph::DirectedWeightedGraph<double>& GetGraph() 
	const {
		return graph_;
	}

	std::optional<graph::Router<double>::RouteInfo> FindRoute(std::string from, std::string to)
	{
		return router_->BuildRoute(stopname_id_data_[from], stopname_id_data_[to]);
	}
private:
	void CreateWaitEdges(graph::DirectedWeightedGraph<double>& graph);

	void CreateBusEdges(graph::DirectedWeightedGraph<double>& graph, transport_catalogue::processing::TransportCatalogue& ts);

	void ConstructGraph(transport_catalogue::processing::TransportCatalogue& ts);

	void CreateRouterIdentity() 
	{
		router_.reset(new graph::Router<double>(std::move(graph::Router<double>(graph_))));	
	}
	
	std::unordered_map<std::string, size_t> stopname_id_data_;
	graph::DirectedWeightedGraph<double> graph_;
	std::unique_ptr<graph::Router<double>> router_;

private:
	RouterSet set_;
	
};


