#pragma once

#include <utility>
#include <vector>
#include <algorithm>

#include "svg.h"
#include "transport_catalogue.h"
#include "json.h"



inline const double EPSILON = 1e-6;
bool IsZero(double value);

class SphereProjector {
public:
	// points_begin и points_end задают начало и конец интервала элементов geo::Coordinates
	template <typename PointInputIt>
	SphereProjector(PointInputIt points_begin, PointInputIt points_end,
		double max_width, double max_height, double padding)
		: padding_(padding) //
	{
		// Если точки поверхности сферы не заданы, вычислять нечего
		if (points_begin == points_end) {
			return;
		}

		// Находим точки с минимальной и максимальной долготой
		const auto [left_it, right_it] = std::minmax_element(
			points_begin, points_end,
			[](auto lhs, auto rhs) { return lhs.lng < rhs.lng; });
		min_lon_ = left_it->lng;
		const double max_lon = right_it->lng;

		// Находим точки с минимальной и максимальной широтой
		const auto [bottom_it, top_it] = std::minmax_element(
			points_begin, points_end,
			[](auto lhs, auto rhs) { return lhs.lat < rhs.lat; });
		const double min_lat = bottom_it->lat;
		max_lat_ = top_it->lat;

		// Вычисляем коэффициент масштабирования вдоль координаты x
		std::optional<double> width_zoom;
		if (!IsZero(max_lon - min_lon_)) {
			width_zoom = (max_width - 2 * padding) / (max_lon - min_lon_);
		}

		// Вычисляем коэффициент масштабирования вдоль координаты y
		std::optional<double> height_zoom;
		if (!IsZero(max_lat_ - min_lat)) {
			height_zoom = (max_height - 2 * padding) / (max_lat_ - min_lat);
		}

		if (width_zoom && height_zoom) {
			// Коэффициенты масштабирования по ширине и высоте ненулевые,
			// берём минимальный из них
			zoom_coeff_ = std::min(*width_zoom, *height_zoom);
		}
		else if (width_zoom) {
			// Коэффициент масштабирования по ширине ненулевой, используем его
			zoom_coeff_ = *width_zoom;
		}
		else if (height_zoom) {
			// Коэффициент масштабирования по высоте ненулевой, используем его
			zoom_coeff_ = *height_zoom;
		}
	}

	// Проецирует широту и долготу в координаты внутри SVG-изображения
	svg::Point operator()(geo_calc::Coordinates coords) const {
		return {
			(coords.lng - min_lon_) * zoom_coeff_ + padding_,
			(max_lat_ - coords.lat) * zoom_coeff_ + padding_
		};
	}

private:
	double padding_;
	double min_lon_ = 0;
	double max_lat_ = 0;
	double zoom_coeff_ = 0;
};






class MapRenderer 
{
public:
	void GetStatFromJson(json::Document& data)
	{
	
		json::Node render_sett = data.GetRoot().AsMap().at("render_settings");
		
		widght = render_sett.AsMap().at("width").AsDouble();
		height = render_sett.AsMap().at("height").AsDouble();
		padding = render_sett.AsMap().at("padding").AsDouble();
		line_width = render_sett.AsMap().at("line_width").AsDouble();
		stop_radius = render_sett.AsMap().at("stop_radius").AsDouble();
		bus_label_font_size = render_sett.AsMap().at("bus_label_font_size").AsInt();
		bus_label_offset.emplace_back(render_sett.AsMap().at("bus_label_offset").AsArray().at(0).AsDouble());
		bus_label_offset.emplace_back(render_sett.AsMap().at("bus_label_offset").AsArray().at(1).AsDouble());
		stop_label_font_size = render_sett.AsMap().at("stop_label_font_size").AsInt();
		stop_label_offset.emplace_back(render_sett.AsMap().at("stop_label_offset").AsArray().at(0).AsDouble());
		stop_label_offset.emplace_back(render_sett.AsMap().at("stop_label_offset").AsArray().at(1).AsDouble());
		
		json::Node u_color = render_sett.AsMap().at("underlayer_color");
		if (u_color.IsArray()) 
		{
			if (u_color.AsArray().size() == 3) 
			{
				underlayer_color = svg::Rgb({ static_cast<uint8_t>(u_color.AsArray().at(0).AsInt()),
					static_cast<uint8_t>(u_color.AsArray().at(1).AsInt()), static_cast<uint8_t>(u_color.AsArray().at(2).AsInt()) });
			}
			else 
			{
				underlayer_color = svg::Rgba({ static_cast<uint8_t>(u_color.AsArray().at(0).AsInt()),
					static_cast<uint8_t>(u_color.AsArray().at(1).AsInt()), static_cast<uint8_t>(u_color.AsArray().at(2).AsInt()), u_color.AsArray().at(3).AsDouble() });
			}
		}
		else if(u_color.IsString())
		{
			underlayer_color = u_color.AsString();
		}
		else 
		{
			underlayer_color = svg::Color("none");
		}

		underlayer_width = render_sett.AsMap().at("underlayer_width").AsDouble();

		for (auto& color : render_sett.AsMap().at("color_palette").AsArray()) 
		{
			if (color.IsArray())
			{
				if (color.AsArray().size() == 3)
				{
					color_palette.emplace_back(svg::Rgb({ static_cast<uint8_t>(color.AsArray().at(0).AsInt()),
						static_cast<uint8_t>(color.AsArray().at(1).AsInt()), static_cast<uint8_t>(color.AsArray().at(2).AsInt())}));
				}
				else
				{
					color_palette.emplace_back(svg::Rgba({ static_cast<uint8_t>(color.AsArray().at(0).AsInt()),
						static_cast<uint8_t>(color.AsArray().at(1).AsInt()), static_cast<uint8_t>(color.AsArray().at(2).AsInt()), color.AsArray().at(3).AsDouble() }));
				}
			}
			else if(color.IsString())
			{
				color_palette.emplace_back(color.AsString());
			}
			else 
			{
				color_palette.emplace_back(svg::Color("none"));
			}
		}
		
	}
	

	void SetTs(transport_catalogue::processing::TransportCatalogue& ts) 
	{
		ts_ = &ts;
	}

	void CreateSvg(std::ostream& os)
	{
		svg::Document res;
		std::vector<std::string> name_bus = ts_->GetBusNames();
		std::sort(name_bus.begin(), name_bus.end());
		std::vector<geo_calc::Coordinates> coord;
		std::vector<int> sizes_pol;
		int current_color = 0;
		int modul_color = color_palette.size();

		for (auto& name : name_bus) 
		{
			int c_size = 0;
			for (auto& stop : ts_->FindBus(name)->stops_) 
			{
				coord.emplace_back(stop->coord_);
				c_size++;
			}
			sizes_pol.emplace_back(c_size);
		

		}

		if (!coord.empty()) {
			
			SphereProjector sph{ coord.begin(), coord.end(), widght, height, padding };

			int c_number = 0;
			int bus_counter = 0;
			for (auto& num : sizes_pol)
			{
				if (num != 0) {
					svg::Polyline pol;
				
					for (int i = 0; i < num; ++i, ++c_number)
					{
						pol.AddPoint(sph(coord.at(c_number)));
					}

					pol.SetStrokeWidth(line_width);
					pol.SetStrokeLineCap(svg::StrokeLineCap::ROUND);
					pol.SetStrokeLineJoin(svg::StrokeLineJoin::ROUND);
					pol.SetFillColor(svg::Color("none"));
					pol.SetStrokeColor(color_palette.at(current_color % modul_color));

					current_color++;
					res.Add(pol);


				}
				bus_counter++;
			}
			
			current_color = 0;

			for (auto& name : name_bus) 
			{
				if (ts_->FindBus(name)->stops_.empty()) { continue; }

				svg::Text text;
				svg::Text sub_text;
				text.SetPosition(sph(ts_->FindBus(name)->stops_.at(0)->coord_));
				sub_text.SetPosition(sph(ts_->FindBus(name)->stops_.at(0)->coord_));
				text.SetOffset({ bus_label_offset.at(0), bus_label_offset.at(1) });
				sub_text.SetOffset({ bus_label_offset.at(0), bus_label_offset.at(1) });
				text.SetFontSize(bus_label_font_size);
				sub_text.SetFontSize(bus_label_font_size);
				text.SetFontFamily("Verdana");
				sub_text.SetFontFamily("Verdana");
				text.SetFontWeight("bold");
				sub_text.SetFontWeight("bold");
				text.SetData(name);
				sub_text.SetData(name);

				sub_text.SetStrokeColor(underlayer_color);

				sub_text.SetFillColor(underlayer_color);
				sub_text.SetStrokeWidth(underlayer_width);
				sub_text.SetStrokeLineCap(svg::StrokeLineCap::ROUND);
				sub_text.SetStrokeLineJoin(svg::StrokeLineJoin::ROUND);

				text.SetFillColor(color_palette.at(current_color % modul_color));
				res.Add(sub_text);
				res.Add(text);


				if (!ts_->FindBus(name)->is_roundtrip_ && ts_->FindBus(name)->stops_.at(ts_->FindBus(name)->stops_.size() / 2) != ts_->FindBus(name)->stops_.at(0))
				{
					text.SetPosition(sph(ts_->FindBus(name)->stops_.at(ts_->FindBus(name)->stops_.size() / 2)->coord_));
					sub_text.SetPosition(sph(ts_->FindBus(name)->stops_.at(ts_->FindBus(name)->stops_.size() / 2)->coord_));
					res.Add(sub_text);
					res.Add(text);
				}

			
				current_color++;

			}

			std::vector<std::string> stopname = ts_->GetStopNames();
			std::sort(stopname.begin(), stopname.end());

			for (auto& name : stopname)
			{
				if (ts_->FindStop(name)->buses_.empty()) { continue; }

				svg::Circle cr;
				cr.SetCenter(sph(ts_->FindStop(name)->coord_));
				cr.SetRadius(stop_radius);
				cr.SetFillColor(svg::Color("white"));
				res.Add(cr);

			}

			for (auto& name : stopname)
			{
				if (ts_->FindStop(name)->buses_.empty()) { continue; }

				svg::Text text;
				svg::Text sub_text;

				text.SetPosition(sph(ts_->FindStop(name)->coord_));
				sub_text.SetPosition(sph(ts_->FindStop(name)->coord_));
				text.SetOffset(svg::Point(stop_label_offset.at(0), stop_label_offset.at(1)));
				sub_text.SetOffset(svg::Point(stop_label_offset.at(0), stop_label_offset.at(1)));
				text.SetFontSize(stop_label_font_size);
				sub_text.SetFontSize(stop_label_font_size);
				text.SetFontFamily("Verdana");
				sub_text.SetFontFamily("Verdana");
				text.SetData(name);
				sub_text.SetData(name);
				sub_text.SetStrokeColor(underlayer_color);
				sub_text.SetFillColor(underlayer_color);
				sub_text.SetStrokeWidth(underlayer_width);
				sub_text.SetStrokeLineCap(svg::StrokeLineCap::ROUND);
				sub_text.SetStrokeLineJoin(svg::StrokeLineJoin::ROUND);
				text.SetFillColor(svg::Color("black"));
				res.Add(sub_text);
				res.Add(text);
			
			}


		}


		res.Render(os);
	}
	

private:
	double widght = 0;
	double height = 0;
	double padding = 0;
	double line_width = 0;
	double stop_radius = 0;
	int bus_label_font_size = 0;
	std::vector<double> bus_label_offset;
	int stop_label_font_size = 0;
	std::vector<double> stop_label_offset;
	svg::Color underlayer_color;
	double underlayer_width = 0;
	std::vector<svg::Color> color_palette;

	transport_catalogue::processing::TransportCatalogue* ts_ = nullptr;
};