#pragma once

#include "ffmpeg.h"
#include "parameters.h"

#include <string>
#include <memory>

extern const std::string image_ext[], video_ext[];
extern const std::string image_final_format;
extern const std::string video_final_format;

bool is_image(const std::string& file_name);
bool is_video(const std::string& file_name);

class File {
	const FFMPEG& m_ffmpeg;
	const std::shared_ptr<std::unique_ptr<Parameters>> m_parameters;
	const std::string m_path, m_ext;
public:
	File(const std::string&, const FFMPEG&, const std::shared_ptr<std::unique_ptr<Parameters>>&, const std::string&);

	void convert() const;

	const std::string& get_path() const;
};

class VideoFile : public File {
public:
	VideoFile(const std::string&, const FFMPEG&, const std::shared_ptr<std::unique_ptr<Parameters>>&);
};

class ImageFile : public File {
public:
	ImageFile(const std::string&, const FFMPEG&, const std::shared_ptr<std::unique_ptr<Parameters>>&);
};