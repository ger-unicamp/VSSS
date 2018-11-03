#include "program_settings.hpp"
#include <nlohmann/json.hpp>
#include <iostream>
#include <string>
#include <set>

using json = nlohmann::json;

ProgramSettings::ProgramSettings(std::string json_path)
{
	std::ifstream in_file(json_path);
	json json_config;
	in_file >> json_config;

	string arr_valid_colors[8] = {"blue", "yellow", "orange", "pink", "green", "purple", "red", "brown"};
	std::set<std::string> valid_colors;
	valid_colors.insert(arr_valid_colors, arr_valid_colors + 8);

	for (auto color_name : valid_colors)
	{
		if (!json_config["colors"][color_name].is_null())
		{
			auto tmp = json_config["colors"][color_name];
			this->colors[color_name] = Color(tmp["R_min"], tmp["R_max"], tmp["G_min"], tmp["G_max"], tmp["B_min"], tmp["B_max"]);
		}
	}

	for (int i = 0; i < 4; i++)
	{
		this->borders.push_back(Point2f(json_config["points"][i]["x"], json_config["points"][i]["y"]));
		cout << this->borders[i] << std::endl;
	}

	this->camera_parameters = json_config["camera_parameters"];
}