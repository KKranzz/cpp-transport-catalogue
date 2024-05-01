#include "transport_router.h"

void TransportRouter::ConstructGraph(transport_catalogue::processing::TransportCatalogue& ts)
{
	
	double prev_distance = 0;
	
	graph::DirectedWeightedGraph<double> graph(stopname_id_data_.size()*2);

	for (auto& [name, id] : stopname_id_data_) 
	{
		graph.AddEdge(graph::Edge{ id, stopname_id_data_.size() + id, wait_time_, 
			graph::StatusEdge::WAIT, 0, name});
	}

	for (auto& bus : ts.GetBusData())
	{
		int count = 1;
		int span_count = 1;
		if (bus.is_roundtrip_) {
			
			for (auto& stop : bus.stops_)
			{
				prev_distance = 0;
				span_count = 1;
		
				for (int i = count; static_cast<size_t>(i) < bus.stops_.size(); ++i)// a b c b a    a b a c // b a b c / 
				{
					prev_distance += static_cast<double>(ts.FindDist(const_cast<domain::Stop*>(bus.stops_.at(i - 1)), const_cast<domain::Stop*>(bus.stops_.at(i)))) / 1000.0;
					graph::Edge<double> edge;
					edge.from = stopname_id_data_[stop->name_] + stopname_id_data_.size();
					edge.to = stopname_id_data_[bus.stops_.at(i)->name_];
					edge.weight = prev_distance / bus_velocity_;
					edge.state = graph::StatusEdge::BUS;
					edge.span_count = span_count;
					edge.data = bus.name_;
					graph.AddEdge(edge);
					span_count++;
				}
				count++;
			}
		}
		else 
		{
			count = 1;
			for (int j = 0; static_cast<size_t>(j) < bus.stops_.size() / 2; ++j)  // a b c b a
			{
				prev_distance = 0;
				span_count = 1;
				auto stop = bus.stops_.at(j);
				for (int i = count; static_cast<size_t>(i) < bus.stops_.size() / 2+1; ++i) //a b 
				{
				
					prev_distance += static_cast<double>(ts.FindDist(const_cast<domain::Stop*>(bus.stops_.at(i - 1)), const_cast<domain::Stop*>(bus.stops_.at(i)))) / 1000.0;
					graph::Edge<double> edge;
					edge.from = stopname_id_data_[stop->name_] + stopname_id_data_.size();
					edge.to = stopname_id_data_[bus.stops_.at(i)->name_];
					edge.weight = prev_distance / bus_velocity_;
					edge.state = graph::StatusEdge::BUS;
					edge.span_count = span_count;
					edge.data = bus.name_;
					graph.AddEdge(edge);
					span_count++;

				}
				count++;
			}

			count = 1;
			
			for (int j = bus.stops_.size()/2; static_cast<size_t>(j) < bus.stops_.size(); ++j) {
				span_count = 1;
				prev_distance = 0;
				auto stop = bus.stops_.at(j);
				for (int i = bus.stops_.size() / 2 + count; static_cast<size_t>(i) < bus.stops_.size(); ++i) //a b c b a
				{
					prev_distance += static_cast<double>(ts.FindDist(const_cast<domain::Stop*>(bus.stops_.at(i-1)), const_cast<domain::Stop*>(bus.stops_.at(i)))) / 1000.0;
					graph::Edge<double> edge;
					edge.from = stopname_id_data_[stop->name_] + stopname_id_data_.size();
					edge.to = stopname_id_data_[bus.stops_.at(i)->name_];
					edge.weight = prev_distance / bus_velocity_;
					edge.state = graph::StatusEdge::BUS;
					edge.span_count = span_count;
					edge.data = bus.name_;
					graph.AddEdge(edge);
					span_count++;

				}
				count++;
			}
				
		}
	}


	graph_ = std::move(graph);
	
}
