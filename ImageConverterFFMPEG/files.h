#pragma once

#include <string>

extern const std::string image_ext[], video_ext[];

bool is_image(const std::string& file_name);
bool is_video(const std::string& file_name);

class File {
	std::string m_path;
};