#include "files.h"

#include <algorithm>

const std::string image_ext[] = {
	"png", "jpg", "jpeg", "bmp", "gif", "ppm", "pgm", "tiff", "tga", "svg",
	"heic", "heif"
};
const std::string video_ext[] = {
	"mp4", "avi", "mov", "mkv", "flv", "webm", "mpeg", "asf", "ogv", "mxf"
};

bool is_image(const std::string& file_name)
{
	for (const auto& i : image_ext) {
		std::string lowered(file_name.size(), '\0');
		std::transform(file_name.begin(), file_name.end(), lowered.begin(), [](unsigned char c) { return std::tolower(c); });
		if (lowered.size() >= i.size() && lowered.rfind(i) == (lowered.size() - i.size())) return true;
	}
	return false;
}

bool is_video(const std::string& file_name)
{
	for (const auto& i : video_ext) {
		std::string lowered(file_name.size(), '\0');
		std::transform(file_name.begin(), file_name.end(), lowered.begin(), [](unsigned char c) { return std::tolower(c); });
		if (lowered.size() >= i.size() && lowered.rfind(i) == (lowered.size() - i.size())) return true;
	}
	return false;
}