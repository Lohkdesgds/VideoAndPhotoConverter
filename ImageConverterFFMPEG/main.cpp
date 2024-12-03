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
	
		cout << console::color::YELLOW << "[?] Hey, what do you want to do? (Enter number)";
		cout << console::color::WHITE << "1. Recursively find all files in this folder and subfolders";
		cout << console::color::WHITE << "2. Find all files in this folder only";		
	
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
		cout << console::color::YELLOW << "[?] So, do you want NVENC or x264 to convert the video files?";
		cout << console::color::WHITE << "1. NVENC (GPU)";
		cout << console::color::WHITE << "2. x264 (CPU)";
	
		std::cin >> opt;
	
		if (opt == 2) props_video->reset(new x264_parameters{});
	}
	
	opt = -1;
	while (opt != 0) {
		cout << console::color::YELLOW << "[?] Do you want to change settings? Send \"0\" to start encoding.";
		cout << console::color::WHITE << "1. Show video encoder settings";
		cout << console::color::WHITE << "2. Show image encoder settings";
#ifdef _DEBUG
		cout << console::color::WHITE << "3. Set a video encoder value";
		cout << console::color::WHITE << "4. Set an image encoder value";
#endif
		cout << console::color::WHITE << "5. List all files added";
		cout << console::color::WHITE << "0. Work with current settings";
	
		std::cin >> opt;
	
		const auto print_pretty_lines = [&](const std::vector<std::string>& lines) {
			cout << console::color::LIGHT_PURPLE << "Properties: ";
			for (const auto& i : lines) {
				cout << console::color::DARK_PURPLE << "- " << i;
			}
		};
	
		switch (opt) {
		case 0:
			continue;
		case 1: // video encoder show
		{
			print_pretty_lines((*props_video)->to_pretty_lines());
		}
			break;
		case 2: // image encoder show
		{
			print_pretty_lines((*props_image)->to_pretty_lines());
		}
		break;
#ifdef _DEBUG
		case 3: // video encoder set
		{
			cout << console::color::RED << "Not implemented yet.";
		}
		break;
		case 4: // image encoder set
		{
			cout << console::color::RED << "Not implemented yet.";
		}
		break;
#endif
		case 5: // show files
		{
			cout << console::color::LIGHT_PURPLE << "Files ready to be converted: ";
			for (const auto& i : files)
				cout << console::color::DARK_PURPLE << "- \"" << i->get_path() << "\"";
		}
		break;
		}
	}
	
	for (const auto& each : files) {
		cout << console::color::GOLD << "Converting \"" << each->get_path() << "\"...";
		each->convert();
	}

	cout << console::color::GREEN << "List done! Please close the window.";
	std::cin >> opt;
	
	return 0;
}