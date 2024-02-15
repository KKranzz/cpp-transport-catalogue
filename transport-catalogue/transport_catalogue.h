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
			Stop(std::string name, geo_calc::Coordinates coord) : name_(name), coord_(std::move(coord)) {}
			const std::string name_;
			const geo_calc::Coordinates coord_;
			std::set<std::string_view> buses_;

		};

		struct Bus
		{
			std::string name_;
			std::vector<const identity::Stop*> stops_;
		};

	}
	namespace processing {
		class TransportCatalogue {
			// Реализуйте класс самостоятельно
		public:
			void AddStop(std::string&& name, geo_calc::Coordinates coord);

			const identity::Stop* FindStop(const std::string& name_stop) const;

			void AddBus(const std::string& name, std::vector<std::string>& stopnames);

			const identity::Bus* FindBus(const std::string& name_bus) const;

		private:
			std::deque<identity::Stop> stops_;
			std::unordered_map<std::string_view, identity::Stop*> stopname_to_stops_;
			std::deque<identity::Bus> buses_;
			std::unordered_map <std::string_view, const identity::Bus*> busname_to_bus_;
		};
	}

}
