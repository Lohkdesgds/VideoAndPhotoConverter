#pragma once

#include <string>
#include <functional>
#include <optional>
#include <memory>

#include <nlohmann/json.hpp>

struct gh_auto_links {
	std::string download;
	std::string version;
	std::unique_ptr<gh_auto_links> opt_dep;

	void from_json(const nlohmann::json&);
	nlohmann::json to_json() const;
};


class PathingStuff {
	const std::string m_base_path;

	const gh_auto_links 
		m_ffmpeg,
		m_7zip,
		m_magisk;

	gh_auto_links
		m_local_ffmpeg,
		m_local_7zip,
		m_local_magisk;

	void load_local_configs();
	void save_remote_configs();
public:
	PathingStuff();

	const std::string& get_base_path() const;

	const gh_auto_links& get_own_magisk() const;
	const gh_auto_links& get_own_ffmpeg() const;
	const gh_auto_links& get_own_7zip() const;
};

//class PathingStuff {
//	const std::string m_base_path; // app path to save stuff
//
//	struct download_links {
//		std::string m_magick;
//		std::string m_ffmpeg;
//#ifdef _WIN32
//		std::string m_7za;
//#endif
//		std::string m_7z;
//
//	} m_dl;
//
//	struct gh_versioning {
//		std::string m_magick;
//		std::string m_ffmpeg;
//		std::string m_7zip;
//	} m_gh;
//
//
//
//	bool _download_fp(const std::string& url, const std::string& file_name) const;
//	bool _download_to(const std::string& url, std::string& target) const;
//	void _erase_based(const std::string&) const;
//
//	bool download_7z() const;
//	bool download_ffmpeg() const;
//	bool download_magick() const;
//
//	bool fetch_all_versions_and_links(); // GOOD
//
//	bool create_dirs(const std::string&) const;
//	std::string get_folder_with_name_in_dir(const std::string& dir, const std::string& substr) const;
//
//	void call_7z_extract(const std::string& zip) const;
//public:
//	PathingStuff();
//
//	bool self_check_install();
//
//	std::string get_ffmpeg_exe() const;
//	std::string get_magick_exe() const;
//	std::string get_7z_exe() const;
//#ifdef _WIN32
//	std::string get_7za_exe() const;
//#endif
//};


gh_auto_links get_magisk();
gh_auto_links get_ffmpeg();
gh_auto_links get_7zip();