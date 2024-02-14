#include "input_reader.h"

/*
 Парсит строку в структуру CommandDescription и сохраняет результат в commands_
 */ 
std::vector<std::string> ParserByParameters(std::string_view string_line, char type_delimeter)
{
    std::vector <std::string> res;
  
        res.reserve(std::count(string_line.begin(), string_line.end(), type_delimeter));
        std::string parameter;
        for (auto symbol : string_line)
        {
            if (symbol == type_delimeter)
            {
                parameter.erase(parameter.begin(), parameter.begin() + parameter.find_first_not_of(" "));
                parameter.erase(parameter.begin() + parameter.find_last_not_of(" ") + 1, parameter.end());

                res.push_back(std::move(parameter));
                parameter.clear();

            }
            else
            {
                parameter += symbol;
            }
        }

        if (!parameter.empty()) 
        {
            parameter.erase(parameter.begin(), parameter.begin() + parameter.find_first_not_of(" "));
            parameter.erase(parameter.begin() + parameter.find_last_not_of(" ")+1, parameter.end());
            res.push_back(parameter);
        }

    return res;
}

void inreader::InputReader::ParseLine(std::string_view line) 
{
    CommandDescription res;
    if (line.find(":") != std::string::npos)
    {
        if (line[0] == 'B')
        {
            if (line.find(">")!=std::string::npos) {
                res.command = "ACBus";
            }
            else
            {
                res.command = "ALBus";
            }
           
            res.id = std::move(std::string(line.begin() + line.find_first_of(" ") + 1, 
                line.begin() + line.find_first_of(":")));
            res.description = std::string(line.begin() + line.find_first_of(":") + 1, line.end());

        }
        else 
        {
            res.command = "AStop";
            res.id = std::move(std::string(line.begin() + line.find_first_of(" ") + 1,
                line.begin() + line.find_first_of(":")));
            res.description = std::string(line.begin() + line.find_first_of(":")+1, line.end());
        }
   
    }
    else if(line[0]=='B') 
    {
        res.command = "GBus";
        res.id = std::move(std::string(line.begin() + line.find_first_of(" ") + 1,
            line.begin() + line.find_first_of(":")));
        
    }
    else 
    {
        return;
    }
    commands_.push_back(res);

}
/**
 * Наполняет данными транспортный справочник, используя команды из commands_
 */
void inreader::InputReader::ApplyCommands(transport_catalogue::processing::TransportCatalogue& catalogue)
{
    std::sort(commands_.begin(), commands_.end(), [](const auto& lhs, const auto& rhs)
     {
          return lhs.command == "AStop" && rhs.command != "AStop";
     });

    for (auto& command : commands_) 
    {
        if (command.command == "AStop")
        {
            std::vector<std::string> params = std::move(ParserByParameters(command.description, ','));
            geo_calc::Coordinates coord(std::stod(params[0]), std::stod(params[1]));
            catalogue.AddStop(std::move(command.id), coord);
        }
        else if (command.command == "ACBus") 
        {
            std::vector<std::string> params = std::move(ParserByParameters(command.description, '>'));
            catalogue.AddBus(command.id, params);
        }
        else if (command.command == "ALBus")
        {
            std::vector<std::string> params = std::move(ParserByParameters(command.description, '-'));
            std::vector <std::string> second_part(params.begin(), params.end()-1);
            for (int i = second_part.size() - 1; i >= 0; --i) 
            {
                params.push_back(std::move(second_part[i]));
            }
            catalogue.AddBus(command.id, params);
        }
    }
}

void inreader::InputReader::StartParcing(transport_catalogue::processing::TransportCatalogue& catalogue)
{
    int base_request_count;
    *str_ >> base_request_count >> std::ws;

    {
        for (int i = 0; i < base_request_count; ++i) {
            std::string line;
            getline(*str_, line);
            this->ParseLine(line);
        }
        this->ApplyCommands(catalogue);
    }
}
