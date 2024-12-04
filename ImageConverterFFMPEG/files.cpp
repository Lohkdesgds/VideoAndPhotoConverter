#include "files.h"

#include <algorithm>
#include <filesystem>

const std::string image_ext[] = {
	"png", "jpg", "jpeg", "bmp", "gif", "ppm", "pgm", "tiff", "tga", "svg",
	"heic", "heif", "dng"
};
const std::string video_ext[] = {
	"mp4", "avi", "mov", "mkv", "flv", "webm", "mpeg", "asf", "ogv", "mxf"
};
const std::string image_final_format = "jpg";
const std::string video_final_format = "mp4";
constexpr char trash_move_folder_name[] = ".VideoAndPhotoConverterConvertedFiles";

constexpr char common_converted_end_filter[] = "_conv.";

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

File::File(const std::string& path, const std::shared_ptr<std::unique_ptr<Parameters>>& parameters, const std::string& extension)
	: m_path(path), m_parameters(parameters), m_ext(extension)
{
	if (m_path.find(":\\") == std::string::npos) throw std::invalid_argument("Path must be canonical (e.g. not relative, like C:\\folder\\file.mp4)");
}

bool File::move_to_trash() const
{	
	std::error_code ec;
	const std::string full_path = get_trash_path();
	const std::string only_dirs = full_path.substr(0, full_path.rfind("\\"));
	std::filesystem::create_directories(only_dirs, ec);
	if (ec) return false;
	std::filesystem::rename(m_path, full_path, ec);
	return !ec;
}

const std::string& File::get_path() const
{
	return m_path;
}

std::string File::get_trash_path() const
{
	const size_t p = m_path.find(":\\");
	const std::string drive = m_path.substr(0, p + 2);
	const std::string remaining_path = m_path.substr(p + 2);
	return drive + trash_move_folder_name + "\\" + remaining_path;
}

VideoFile::VideoFile(const std::string& path, const FFMPEG& ffmpeg, const std::shared_ptr<std::unique_ptr<Parameters>>& parameters)
	: File(path, parameters, video_final_format), m_ffmpeg(ffmpeg)
{
}

void VideoFile::convert() const
{
	std::vector<std::string> params;
		
	auto extra = (*m_parameters)->to_props(m_path);

	params.insert(params.end(), std::move_iterator(extra.begin()), std::move_iterator(extra.end()));

	params.push_back("\"" + m_path + common_converted_end_filter + m_ext + "\"");

	m_ffmpeg.call(std::initializer_list<std::string>(params.data(), params.data() + params.size()), [&](const std::string& l) {
		printf_s("FFMPEG: %s\n", l.c_str());
	});
}

bool VideoFile::probably_a_converted_file() const
{
	const auto exp_end = common_converted_end_filter + m_ext;
	const size_t p = m_path.rfind(exp_end);
	return p != std::string::npos && p + exp_end.size() == m_path.size();
}

bool VideoFile::is_video() const
{
	return true;
}

bool VideoFile::is_image() const
{
	return false;
}

ImageFile::ImageFile(const std::string& path, const MAGICK& magick, const std::shared_ptr<std::unique_ptr<Parameters>>& parameters)
	: File(path, parameters, image_final_format), m_magick(magick)
{
}

void ImageFile::convert() const
{
	std::vector<std::string> params;
		
	auto extra = (*m_parameters)->to_props(m_path);

	params.insert(params.end(), std::move_iterator(extra.begin()), std::move_iterator(extra.end()));

	params.push_back("\"" + m_path + common_converted_end_filter + m_ext + "\"");

	m_magick.call(std::initializer_list<std::string>(params.data(), params.data() + params.size()), [&](const std::string& l) {
		printf_s("MAGICK: %s\n", l.c_str());
	});
}

bool ImageFile::probably_a_converted_file() const
{
	const auto exp_end = common_converted_end_filter + m_ext;
	const size_t p = m_path.rfind(exp_end);
	return p != std::string::npos && p + exp_end.size() == m_path.size();
}

bool ImageFile::is_video() const
{
	return false;
}

bool ImageFile::is_image() const
{
	return true;
}


std::shared_ptr<File> make_file_auto(const std::string& path, 
	const FFMPEG& ff, const MAGICK& mk, 
	std::shared_ptr<std::unique_ptr<Parameters>> pf, std::shared_ptr<std::unique_ptr<Parameters>> pm)
{
	const bool b_img = is_image(path);
	const bool b_vid = is_video(path);

	if (!b_img && !b_vid) return {};

	return std::shared_ptr<File>(
		b_img ?
			reinterpret_cast<File*>(new ImageFile(path, mk, pm)) :
			reinterpret_cast<File*>(new VideoFile(path, ff, pf)),
		b_img ? 
			[](File* f) { delete (ImageFile*)f; } :
			[](File* f) { delete (VideoFile*)f; }
	);
}	