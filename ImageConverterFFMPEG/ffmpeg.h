#pragma once

#include <string>

class FFMPEG {
	std::string m_path_ffmpeg;
public:
	// Check if installation exists (exe check)
	bool check_installation();

	// Download and install FFMPEG
	bool install();

	// Call ffmpeg with args
	void call(const std::string&);
};