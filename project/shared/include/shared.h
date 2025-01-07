#pragma once

#include <string>
#include <functional>
#include <optional>
#include <memory>

#include <nlohmann/json.hpp>

#ifdef _WIN32
#define RUN_AS_WIN // used for debugging on non windows too
#endif
//#define RUN_AS_WIN


struct gh_auto_links {
	std::string download;
	std::string version;
	std::string fpname;
	std::unique_ptr<gh_auto_links> opt_dep;

	void from_json(const nlohmann::json&);
	nlohmann::json to_json() const;
};

struct directory_parsed {
	const std::string raw_current_path;
	std::vector<std::string> files;
	std::vector<std::string> directories;

	std::string find_dir(const std::string&) const;
	std::string find_file(const std::string&) const;
	directory_parsed operator[](const std::string&) const;
	directory_parsed(const std::string&);
};


class PathingStuff {
	const std::string m_base_path;

	const gh_auto_links 
		m_ffmpeg,
		m_magick;

	gh_auto_links
		m_local_ffmpeg,
		m_local_magick;

	std::string 
		m_path_ffmpeg,
		m_path_magick;

	void load_local_configs();
	bool save_remote_configs();

	bool check_remote_is_good();
	bool check_local_is_good();

	void install_updates();
public:
	PathingStuff();
	
	const std::string& get_ffmpeg_exe() const;
	const std::string& get_magick_exe() const;
};


