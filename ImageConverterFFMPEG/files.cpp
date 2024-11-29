#include "files.h"

#include <algorithm>

const std::string image_ext[] = {
	"png", "jpg", "jpeg", "bmp", "gif", "ppm", "pgm", "tiff", "tga", "svg",
	"heic", "heif"
};
const std::string video_ext[] = {
	"mp4", "avi", "mov", "mkv", "flv", "webm", "mpeg", "asf", "ogv", "mxf"
};
const std::string image_final_format = "png";
const std::string video_final_format = "mp4";

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

File::File(const std::string& path, const FFMPEG& ffmpeg, const std::shared_ptr<std::unique_ptr<Parameters>>& parameters, const std::string& extension)
	: m_ffmpeg(ffmpeg), m_path(path), m_parameters(parameters), m_ext(extension)
{
}

void File::convert() const
{
	std::vector<std::string> params;
		
	auto extra = (*m_parameters)->to_ffmpeg_props(m_path);

	params.insert(params.end(), std::move_iterator(extra.begin()), std::move_iterator(extra.end()));

	params.push_back("\"" + m_path + "_conv." + m_ext + "\"");

	m_ffmpeg.call(std::initializer_list<std::string>(params.data(), params.data() + params.size()), [&](const std::string& l) {
		printf_s("FFMPEG: %s\n", l.c_str());
	});
}

const std::string& File::get_path() const
{
	return m_path;
}

VideoFile::VideoFile(const std::string& path, const FFMPEG& ffmpeg, const std::shared_ptr<std::unique_ptr<Parameters>>& parameters)
	: File(path, ffmpeg, parameters, video_final_format)
{
}

ImageFile::ImageFile(const std::string& path, const FFMPEG& ffmpeg, const std::shared_ptr<std::unique_ptr<Parameters>>& parameters)
	: File(path, ffmpeg, parameters, image_final_format)
{
}