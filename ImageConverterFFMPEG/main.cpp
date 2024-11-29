#include <Lunaris/console/console.h>

#include <filesystem>

#include "ffmpeg.h"
#include "parameters.h"
#include "files.h"

using namespace Lunaris;

int main(int argc, char* argv[])
{
	cout << console::color::GREEN << "[Main] Starting...";

	FFMPEG ffmpeg;
	std::vector<std::unique_ptr<File>> files;
	bool has_video_files = false;

	// yes, I am committing a crime here.
	std::shared_ptr<std::unique_ptr<Parameters>> props_video = 
		std::shared_ptr<std::unique_ptr<Parameters>>(new std::unique_ptr<Parameters>(new NVENC_parameters{}));

	std::shared_ptr<std::unique_ptr<Parameters>> props_image =
		std::shared_ptr<std::unique_ptr<Parameters>>(new std::unique_ptr<Parameters>(new JPEG_parameters{}));

	ffmpeg.setup();

	cout << console::color::GREEN << "[Main] Checking parameters...";

	if (argc > 1) {
		for (int k = 1; k < argc; ++k) {
			std::filesystem::directory_entry p(argv[k]);

			if (!p.is_regular_file()) continue;

			const std::string path = std::filesystem::canonical(p.path()).string();

			const bool b_img = is_image(path);
			const bool b_vid = is_video(path);
			has_video_files |= b_vid;

			if (b_img || b_vid) {
				cout << console::color::DARK_GRAY << "[Main] Added file: '" << path << "'";
				files.push_back(std::unique_ptr<File>(
					b_img ?
					reinterpret_cast<File*>(new ImageFile(path, ffmpeg, props_image)) :
					reinterpret_cast<File*>(new VideoFile(path, ffmpeg, props_video))
				));
			}

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
			for (const auto& p : std::filesystem::recursive_directory_iterator{ "." }) {
				if (!p.is_regular_file()) continue;

				const std::string path = std::filesystem::canonical(p.path()).string();

				const bool b_img = is_image(path);
				const bool b_vid = is_video(path);
				has_video_files |= b_vid;

				if (b_img || b_vid) {
					cout << console::color::DARK_GRAY << "[Main] Added file: '" << path << "'";
					files.push_back(std::unique_ptr<File>(
						b_img ?
							reinterpret_cast<File*>(new ImageFile(path, ffmpeg, props_image)) :
							reinterpret_cast<File*>(new VideoFile(path, ffmpeg, props_video))
					));
				}
			}
		}
			break;
		case 2:
		{
			for (const auto& p : std::filesystem::directory_iterator{ "." }) {
				if (!p.is_regular_file()) continue;

				const std::string path = std::filesystem::canonical(p.path()).string();

				const bool b_img = is_image(path);
				const bool b_vid = is_video(path);
				has_video_files |= b_vid;

				if (b_img || b_vid) {
					cout << console::color::DARK_GRAY << "[Main] Added file: '" << path << "'";
					files.push_back(std::unique_ptr<File>(
						b_img ?
						reinterpret_cast<File*>(new ImageFile(path, ffmpeg, props_image)) :
						reinterpret_cast<File*>(new VideoFile(path, ffmpeg, props_video))
					));
				}
			}
		}
			break;
		default:
			return 0;
		}
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
		cout << console::color::WHITE << "3. Set a video encoder value";
		cout << console::color::WHITE << "4. Set an image encoder value";
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
		case 3: // video encoder set
		{
			cout << console::color::RED << "Not implemented yet";
		}
		break;
		case 4: // image encoder set
		{
			cout << console::color::RED << "Not implemented yet";
		}
		break;
		}
	}

	for (const auto& each : files) {
		each->convert();
	}

	//x264_parameters pm;
	//pm.set_audio_bitrate(128)
	//	.set_cqp(20)
	//	.set_scale(0.5f);
	//	//.set_tune("hq");
	//
	//const VideoParameters& cast = pm;
	//
	//const auto vec = cast.to_ffmpeg_props();
	//
	//for (const auto& i : vec) {
	//	cout << i;
	//}
	/* 
	Steps:
	1. Check if FFMPEG exists in appdata
	2. Download if it doesn't exist
	3. Extract FFMPEG
	4. Get list from argument list OR scan every folder and subfolder
	5. Settings
	6. Work
	*/

	return 0;
}