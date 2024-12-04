#include <Lunaris/console/console.h>

#include <filesystem>

#include "shared.h"
#include "ffmpeg.h"
#include "parameters.h"
#include "files.h"

using namespace Lunaris;

int main(int argc, char* argv[])
{
	cout << console::color::GREEN << "[Main] Starting...";

	PathingStuff ps;
	ps.self_check_install();

	FFMPEG ffmpeg(ps);
	MAGICK magick(ps);
	std::vector<std::shared_ptr<File>> files;
	bool has_video_files = false;
	bool has_image_files = false;
	bool flag_move_trash = true;
	bool is_nvenc = true;
	
	// yes, I am committing a crime here.
	std::shared_ptr<std::unique_ptr<Parameters>> props_video = 
		std::shared_ptr<std::unique_ptr<Parameters>>(new std::unique_ptr<Parameters>(new NVENC_parameters{}));
	
	std::shared_ptr<std::unique_ptr<Parameters>> props_image =
		std::shared_ptr<std::unique_ptr<Parameters>>(new std::unique_ptr<Parameters>(new JPEG_parameters{}));

	const auto auto_push_back_flag = [&](const std::filesystem::directory_entry& item) {
		if (!item.is_regular_file()) return;
	
		const std::string path = std::filesystem::canonical(item.path()).string();
		const auto s_file = make_file_auto(path, ffmpeg, magick, props_video, props_image);

		if (!s_file) return;

		if (s_file->probably_a_converted_file()) {
			cout << console::color::DARK_GRAY << "[Main] File '" << path << "' looks like a converted one. Skipping it.";
			return;
		}

		has_video_files |= s_file->is_video();
		has_image_files |= s_file->is_image();

		cout << console::color::DARK_GRAY << "[Main] Added file: '" << path << "'";
		files.push_back(s_file);
	};
	
	
	cout << console::color::GREEN << "[Main] Checking parameters...";
	
	if (argc > 1) {
		for (int k = 1; k < argc; ++k) {
			auto_push_back_flag(std::filesystem::directory_entry(argv[k]));
		}
	}
	else { // explore
		int opt;
	
		cout << console::color::YELLOW << "\n\n[?] Hey, what do you want to do? (Enter number)";
		cout << console::color::WHITE << "1. Recursively find all files in this folder and subfolders";
		cout << console::color::WHITE << "2. Find all files in this folder only";		
		
		std::cout << "> ";
		std::cin >> opt;
	
		switch (opt) {
		case 1:
		{
			for (const auto& p : std::filesystem::recursive_directory_iterator{ "." })
				auto_push_back_flag(p);
		}
			break;
		case 2:
		{
			for (const auto& p : std::filesystem::directory_iterator{ "." })
				auto_push_back_flag(p);
		}
			break;
		default:
			return 0;
		}
	}

	if (files.size() == 0) {
		cout << console::color::RED << "No files found to convert! Quitting...";
		return 0;
	}
	
	int opt;
	
	if (has_video_files) {
		cout << console::color::YELLOW << "\n\n[?] So, do you want NVENC or x264 to convert the video files?";
		cout << console::color::WHITE << "1. NVENC (GPU)";
		cout << console::color::WHITE << "2. x264 (CPU)";

		std::cout << "> ";
		std::cin >> opt;
	
		if (opt == 2) {
			props_video->reset(new x264_parameters{});
			is_nvenc = false;
		}
	}
	
	
	const auto print_pretty_lines = [&](const std::string& title, const std::vector<std::string>& lines) {
		cout << console::color::YELLOW << title;
		for (const auto& i : lines) {
			cout << console::color::GOLD << "- " << i;
		}
	};
	const auto auto_print_pretty_props = [&] {
		cout << console::color::DARK_GRAY << "==============================";
		if (has_video_files) print_pretty_lines("===> Current video properties <===", (*props_video)->to_pretty_lines());
		if (has_image_files) print_pretty_lines("===> Current image properties <===", (*props_image)->to_pretty_lines());
		cout << console::color::YELLOW << "===> Flags <===";
		cout << console::color::GOLD << "Auto move files to trash-like folder in root of file's disk? " 
			<< (flag_move_trash ? console::color::GREEN : console::color::DARK_RED) << (flag_move_trash ? "YES (they'll be moved)" : "NO (they're kept in their place)");
		cout << console::color::DARK_GRAY << "==============================";
	};

	std::cout << "\n\n\n\n\n\n\n";
	auto_print_pretty_props();
	opt = -1;
	while (opt != 0) {

		cout << console::color::YELLOW << "\n[?] Do you want to change settings? Send \"0\" to start encoding.";
		//if (has_video_files) cout << console::color::WHITE << "1. Show video encoder settings";
		//cout << console::color::WHITE << "2. Show image encoder settings";

		if (has_video_files) cout << console::color::WHITE << "1. Set a video encoder value";
		if (has_image_files) cout << console::color::WHITE << "2. Set an image encoder value";

		cout << console::color::WHITE << "3. List all files added";
		cout << console::color::WHITE << "4. Toggle auto move to trash folder";
		cout << console::color::WHITE << "5. Show specific string properties values reference";
		cout << console::color::WHITE << "0. Work with current settings";

		std::cout << "> ";
		std::cin >> opt;

		std::cout << "\n\n\n\n\n\n\n";
	
		switch (opt) {
		case 0:
			continue;
		case 1: // video encoder set
		{
			if (!has_video_files) {
				cout << console::color::RED << "Not an option.";
				auto_print_pretty_props();
				break;
			}

			std::string opt, val;
			cout << console::color::LIGHT_PURPLE << "Which property do you want to change (single word) and to what value (single word/number/decimal/true/false)?";

			std::cin >> opt >> val;

			if ((*props_video)->set(opt.c_str(), val.c_str())) {
				cout << console::color::DARK_PURPLE << "Mapped \"" << opt << "\" property with value \"" << val << "\" (check if correctly set manually, depending on value you may have removed the property).";
			}
			else {
				cout << console::color::DARK_PURPLE << "Mapping \"" << opt << "\" to \"" << val << "\" failed. Invalid key?";
			}
			auto_print_pretty_props();
		}
		break;
		case 2: // image encoder set
		{
			if (!has_image_files) {
				cout << console::color::RED << "Not an option.";
				auto_print_pretty_props();
				break;
			}

			std::string opt, val;
			cout << console::color::LIGHT_PURPLE << "Which property do you want to change (single word) and to what value (single word/number/decimal/true/false)?";

			std::cin >> opt >> val;

			if ((*props_image)->set(opt.c_str(), val.c_str())) {
				cout << console::color::DARK_PURPLE << "Mapped \"" << opt << "\" property with value \"" << val << "\" (check if correctly set manually, depending on value you may have removed the property).";
			}
			else {
				cout << console::color::DARK_PURPLE << "Mapping \"" << opt << "\" to \"" << val << "\" failed. Invalid key?";
			}
			auto_print_pretty_props();
		}
		break;
		case 3: // show files
		{
			cout << console::color::LIGHT_PURPLE << "Files ready to be converted: ";
			for (const auto& i : files)
				cout << console::color::DARK_PURPLE << "- \"" << i->get_path() << "\"";

		}
		break;
		case 4: // toggle move to trash
		{
			if (flag_move_trash = !flag_move_trash) {
				cout << console::color::LIGHT_PURPLE << "Now MOVING to trash folder.";
			}
			else {
				cout << console::color::LIGHT_PURPLE << "Now NOT MOVING to trash folder.";
			}

			auto_print_pretty_props();
		}
		break;
		case 5: // print static listings
		{
			if (has_video_files) {
				if (is_nvenc) {
					cout << console::color::LIGHT_PURPLE << "=== NVIDIA SETTINGS ===";

					cout << console::color::LIGHT_PURPLE << "# Preset:";
					for (const auto& i : NVENC_parameters::m_preset_values) 
						cout << console::color::DARK_PURPLE << "- \"" << i << "\"";
					cout << console::color::LIGHT_PURPLE << "# Tune:";
					for (const auto& i : NVENC_parameters::m_tune_values)
						cout << console::color::DARK_PURPLE << "- \"" << i << "\"";
					cout << console::color::LIGHT_PURPLE << "# Profile:";
					for (const auto& i : NVENC_parameters::m_profile_values)
						cout << console::color::DARK_PURPLE << "- \"" << i << "\"";
				}
				else {
					cout << console::color::LIGHT_PURPLE << "=== x264 SETTINGS ===";

					cout << console::color::LIGHT_PURPLE << "# Preset:";
					for (const auto& i : x264_parameters::m_preset_values)
						cout << console::color::DARK_PURPLE << "- \"" << i << "\"";
				}
			}
			if (has_image_files) {
				cout << console::color::LIGHT_PURPLE << "=== IMAGE SETTINGS ===";
				cout << console::color::DARK_PURPLE << "There is no specific string-based setting for images in this version.";
			}
		}
		break;
		default:
			cout << console::color::RED << "Not an option.";
			auto_print_pretty_props();
			break;
		}
	}

	std::cout << "\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n";
	
	for (size_t p = 0; p < files.size(); ++p) {
		const auto& each = files[p];

		cout << console::color::GOLD << "[" << (p + 1) << "/" << files.size() << "] "
			"Converting \"" << each->get_path() << "\"...";
		each->convert();
		if (flag_move_trash) {
			cout << console::color::GOLD << "[" << (p + 1) << "/" << files.size() << "] " 
				"Moving to \"" << each->get_trash_path() << "\"...";
			each->move_to_trash();
		}
	}

	cout << console::color::GREEN << "List done! Please close the window.";

	std::cout << "> ";
	std::cin >> opt;
	
	return 0;
}