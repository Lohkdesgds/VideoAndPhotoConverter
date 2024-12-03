#pragma once

#include "shared.h"

#include <string>
#include <functional>


class MAGICK {
	const PathingStuff& m_pathing;

	std::string m_path_magick;
public:
	MAGICK(const PathingStuff&);

	// Call magick with args
	void call(const std::initializer_list<std::string>&) const;

	// Call magick with args and get output
	void call(const std::initializer_list<std::string>&, const std::function<void(const std::string&)>&) const;
};