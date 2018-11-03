#include "program_settings.hpp"
#include <nlohmann/json.hpp>
#include <iostream>
#include <string>
#include <set>
#include <getopt.h>

using json = nlohmann::json;

void ProgramSettings::print_usage(std::string name)
{
	std::cerr << "Usage: " << name
			  << " options:\n"
			  << "\t-h,--help\t\tShow this help message\n"
			  << "\t-t,--team STRING\tTeam color: yellow or blue\n"
			  << "\t-c,--cam INT\t\tCamera number\n"
			  << "\t-f,--flip STRING\tFlip field: y or n\n"
			  << "\nExample:\n"
			  << "\t./game.exe --team=yellow --cam=0 --flip=n\n"
			  << std::endl;
}

ProgramSettings::ProgramSettings(std::string json_path, int argc, char *argv[])
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

	/*	for (int i = 0; i < 4; i++)
	{
		this->borders.push_back(Point2f(json_config["points"][i]["x"], json_config["points"][i]["y"]));
		cout << this->borders[i] << std::endl;
	}
*/
	this->camera_parameters = json_config["camera_parameters"];

	this->parse_command_line(argc, argv);

	std::cout << "Settings:" << endl
			  << "Camera number: " << this->camera_number << endl
			  << "Flip: " << (this->flip ? "yes" : "no") << endl
			  << "Team color: " << this->team_color << endl
			  << endl;
}

void ProgramSettings::parse_command_line(int argc, char *argv[])
{
	this->flip = false;
	this->camera_number = 0;
	this->team_color = "blue";

	int c;
	// int digit_optind = 0;
	while (1)
	{
		// int this_option_optind = optind ? optind : 1;
		int option_index = 0;
		static struct option long_options[] = {
			{"team", required_argument, 0, 't'},
			{"flip", required_argument, 0, 'f'},
			{"cam", required_argument, 0, 'c'},
			{"help", no_argument, 0, 'h'},
			{0, 0, 0, 0}};

		c = getopt_long(argc, argv, "ht:f:c:",
						long_options, &option_index);
		if (c == -1)
			break;

		std::string mode_name;
		switch (c)
		{
		case 0:
			fprintf(stderr, "option %s", long_options[option_index].name);
			if (optarg)
				fprintf(stderr, " with arg %s", optarg);
			fprintf(stderr, "\n");

			this->print_usage(argv[0]);
			exit(EXIT_FAILURE);
			break;

		case '?':
		case 'h':
			this->print_usage(argv[0]);
			exit(EXIT_FAILURE);
			break;

		case 'c':
			this->camera_number = atol(optarg);

			break;
		case 'f':
			this->flip = ((optarg[0] | 32) == 'y');

			break;
		case 't':
			this->team_color = optarg;
			if (this->team_color != "yellow" && this->team_color != "blue")
			{
				fprintf(stderr, "Invalid team color\n");
				this->print_usage(argv[0]);
				exit(EXIT_FAILURE);
			}

			break;
		default:
			fprintf(stderr, "?? getopt returned character code 0%o ??\n", c);
			this->print_usage(argv[0]);
			exit(EXIT_FAILURE);
		}
	}

	if (optind < argc)
	{
		fprintf(stderr, "non-option ARGV-elements: ");
		while (optind < argc)
			fprintf(stderr, "%s ", argv[optind++]);
		fprintf(stderr, "\n");
		this->print_usage(argv[0]);
		exit(EXIT_FAILURE);
	}
}