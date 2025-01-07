#pragma once

#include "shared.h"

#include <string>
#include <functional>
#include <vector>


class FFMPEG {
	const PathingStuff& m_pathing;

	std::string m_path_ffmpeg;
public:
	FFMPEG(const PathingStuff&);

	// Call ffmpeg with args
	void call(const std::vector<std::string>&) const;

	// Call ffmpeg with args and get output
	void call(const std::vector<std::string>&, const std::function<void(const std::string&)>&) const;
};