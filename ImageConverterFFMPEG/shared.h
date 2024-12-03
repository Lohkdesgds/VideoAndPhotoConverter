#pragma once

#include <string>

class PathingStuff {
	const std::string m_base_path; // %appdata%/<appname>/ << here
	std::string m_ffmpeg_latest_version, m_magick_latest_version;

	std::string m_magick_fetched_link;
	std::string m_7za_fetched_link;
	std::string m_7zr_fetched_link;


	bool _download_one(const std::string& url, const std::string& file_name) const;
	void _erase_based(const std::string&) const;

	bool download_7zr_and_7za() const;
	bool download_ffmpeg() const;
	bool download_magick() const;

	bool fetch_all_versions_and_links(); // GOOD

	bool create_dirs(const std::string&) const;
	std::string get_folder_with_name_in_dir(const std::string& dir, const std::string& substr) const;

	void call_7z_extract(const std::string& zip) const;
public:
	PathingStuff();

	bool self_check_install();

	std::string get_ffmpeg_exe() const;
	std::string get_magick_exe() const;
	std::string get_7zr_exe() const;
	std::string get_7za_exe() const;
};