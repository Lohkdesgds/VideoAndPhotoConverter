#pragma once

#include <string>
#include <memory>
#include <functional>

#include <Lunaris/process/process.h>

class FFMPEG {
	const std::string m_base_path; // %appdata%/<appname>/ << here
	std::string m_current_remote_ver;

	std::string m_path_ffmpeg;
	mutable std::unique_ptr<Lunaris::process_sync> m_proc;

	std::string _get_ffmpeg_exe() const;
	bool _download_all_files() const;
	void _reset_ffmpeg_folder() const;
	void _cleanup_ffmpeg_folder() const;
	bool _check_ffmpeg_exist_and_is_latest() const;
	void _call_7z_to_extract_ffmpeg();
	void _auto_wait_proc_end_silent() const;
public:
	FFMPEG();

	// Download and install FFMPEG if needed
	bool setup();

	// Call ffmpeg with args
	void call(const std::initializer_list<std::string>&) const;

	// Call ffmpeg with args and get output
	void call(const std::initializer_list<std::string>&, const std::function<void(const std::string&)>&) const;
};