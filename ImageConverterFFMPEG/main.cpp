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
	ffmpeg.setup();

	cout << console::color::GREEN << "[Main] Checking parameters...";

	if (argc > 1) {

	}
	else { // explore
		int opt;
		cout << console::color::WHITE << "Hey, what do you want to do? (Enter number)";
		cout << console::color::WHITE << "1. Recursively find all files in this folder and subfolders";
		cout << console::color::WHITE << "2. Find all files in this folder only";
		

		std::cin >> opt;

		switch (opt) {
		case 1:
		{
			for (const auto& p : std::filesystem::recursive_directory_iterator{ "." }) {
				const std::string path = p.path().string();
				if (p.is_regular_file() && (is_image(path) || is_video(path))) {
					cout << path;
				}
			}
		}
			break;
		case 2:
		{
			for (const auto& p : std::filesystem::directory_iterator{ "." }) {
				const std::string path = p.path().string();
				if (p.is_regular_file() && (is_image(path) || is_video(path))) {
					cout << path;
				}
			}
		}
			break;
		default:
			return 0;
		}
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