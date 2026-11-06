#ifndef loader_hpp_
#define loader_hpp_

#include <istream>
#include "simulationsystem.hpp"
#include "simulationclasses.hpp"

class basic_istream_loader
{
    const static char coordDelim = ';';
    const static char tubeEndDelim = '|';
    const static char materialDelim = 'M';
    const static char escapeChar = '#';
	
	tb_str_builder builder;

    tube_material* parseMaterialString(const std::string& s)
    {
        if (s == "0") return &DEFAULT_M;
        if (s == "1") return &STRONG_M;
        if (s == "2") return &SLIPPERY_M;
        if (s == "3") return &ELASTIC_M;
        throw std::runtime_error("unknown material");
    }

	vector2f parseSingleCoords(const std::string& s)
	{
		std::size_t sep_idx = s.find(coordDelim);
		if (sep_idx == std::string::npos)
			throw std::runtime_error("expecting coordinates in format x;y");

		return {
			std::stof(s.substr(0, sep_idx)),
			std::stof(s.substr(sep_idx + 1))
		};
	}
	void parseCoords(const std::string& s, tube_material* m)
	{
		std::size_t sep_idx = s.find(tubeEndDelim);
		if (sep_idx == std::string::npos) {
			builder.insertFirmNode(parseSingleCoords(s));
		}
		else {
			builder.insertTube(
				parseSingleCoords(s.substr(0, sep_idx)),
				parseSingleCoords(s.substr(sep_idx + 1)),
				m
			);
		}
	}
	void parseLine(const std::string& l)
	{
		if (l.length() == 0 || l[0] == escapeChar) return;
		std::size_t sep_idx = l.find(materialDelim);
		if (sep_idx == std::string::npos) {
			parseCoords(l, &DEFAULT_M);
		}
		else {
			parseCoords(
				l.substr(sep_idx + 1),
				parseMaterialString(l.substr(0, sep_idx))
			);
		}
	}

public:
	friend tb_str_simulator loadFromInput(std::istream& input);
};

inline tb_str_simulator loadFromInput(std::istream& input)
{
	basic_istream_loader ld;
	std::string line;
	while (std::getline(input, line)) {
		ld.parseLine(line);
	}
	return ld.builder.createSimulationFromCurrentState();
}

#endif
