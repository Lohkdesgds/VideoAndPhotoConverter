#pragma once

#include "shared.h"

#include <string>
#include <functional>


class FFMPEG {
	const PathingStuff& m_pathing;

	std::string m_path_ffmpeg;
public:
	FFMPEG(const PathingStuff&);

	// Call ffmpeg with args
	void call(const std::initializer_list<std::string>&) const;

	// Call ffmpeg with args and get output
	void call(const std::initializer_list<std::string>&, const std::function<void(const std::string&)>&) const;
};