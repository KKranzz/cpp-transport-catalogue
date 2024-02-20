#pragma once

#include <string_view>
#include <string>
#include <deque>
#include <vector>
#include <set>
#include <unordered_map>
#include "geo.h"

namespace transport_catalogue {
	namespace identity {
		struct Bus;

		struct Stop
		{
			Stop(std::string&& name, geo_calc::Coordinates coord) : name_(std::move(name)), coord_(std::move(coord)) {}
			const std::string name_;
			const geo_calc::Coordinates coord_;
			std::set<std::string_view> buses_;

		};

		struct Bus
		{
			std::string name_;
			std::vector<identity::Stop*> stops_;
		};

	}
	namespace processing {
		class TransportCatalogue {
			// Реализуйте класс самостоятельно
		public:
			struct TransportHasher 
			{
				size_t operator()(std::pair <identity::Stop*, identity::Stop*> pair_pointer) const  {
					return hasher(pair_pointer.first) + hasher(pair_pointer.second) * 37;
				}
				std::hash<const void*> hasher;
			};
		
			void AddStop(std::string&& name, geo_calc::Coordinates coord);

			const identity::Stop* FindStop(const std::string& name_stop) const;

			void AddBus(const std::string& name, std::vector<std::string>& stopnames);

			const identity::Bus* FindBus(const std::string& name_bus) const;

			void AddDistance(std::string&& first, std::string&& second, int dist);

			int FindDist(identity::Stop* stop1, identity::Stop* stop2);

		private:
			std::deque<identity::Stop> stops_;
			std::unordered_map<std::string_view, identity::Stop*> stopname_to_stops_;
			std::deque<identity::Bus> buses_;
			std::unordered_map <std::string_view, const identity::Bus*> busname_to_bus_;
			std::unordered_map <std::pair<identity::Stop*, identity::Stop*>, int, TransportHasher> dist_data_;
		};
	}

}
