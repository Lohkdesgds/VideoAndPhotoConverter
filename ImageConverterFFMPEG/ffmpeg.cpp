#include "ffmpeg.h"

#define LUNARIS_USE_WINDOWS_LIB
#include <Lunaris/downloader/downloader.h>
#include <Lunaris/process/process.h>
#include <Lunaris/console/console.h>

#include <fstream>
#include <stdexcept>
#include <filesystem>

constexpr char g_ffmpeg_path[] = "\\ImageConverter\\";
constexpr char g_ffpmeg_base_path[] = "APPDATA";

constexpr char g_7zip_portable_url[] = "https://www.7-zip.org/a/7zr.exe";
constexpr char g_7zip_portable_file[] = "7zr.exe";

constexpr char g_ffmpeg_download_url[] = "https://www.gyan.dev/ffmpeg/builds/ffmpeg-git-full.7z";
constexpr char g_ffmpeg_download_file[] = "ffmpeg_temp.7z";

constexpr char g_ffmpeg_download_ver_url[] = "https://www.gyan.dev/ffmpeg/builds/git-version";


std::string get_env(const char* env);


std::string FFMPEG::_get_ffmpeg_exe() const
{
	return m_base_path + "ffmpeg-" +  m_current_remote_ver + "-full_build\\bin\\ffmpeg.exe";
}

bool FFMPEG::_download_all_files() const
{
	using namespace Lunaris;

	cout << console::color::DARK_GRAY << "[FFMPEG] Creating necessary paths under '" << m_base_path << "'...";

	std::error_code err;
	std::filesystem::create_directories(m_base_path, err);
	if (err) return false;

	cout << console::color::DARK_GRAY << "[FFMPEG] Downloading latest FFMPEG and 7zip portable to extract it...";

	const auto download_one = [&](const std::string& url, const std::string& file_name) {
		Lunaris::downloader down;
		std::fstream fp(m_base_path + file_name, std::ios::out | std::ios::binary);
		if (!fp || fp.bad() || !fp.is_open()) return false;
		down.get_store(url, [&](const char* buf, const size_t len) { fp.write(buf, len); });
		fp.close();
		return true;
	};

	if (!download_one(g_ffmpeg_download_url, g_ffmpeg_download_file)) return false;
	if (!download_one(g_7zip_portable_url, g_7zip_portable_file)) return false;

	cout << console::color::DARK_GREEN << "[FFMPEG] Done downloading stuff.";

	return true;
}

void FFMPEG::_reset_ffmpeg_folder() const
{
	using namespace Lunaris;

	cout << console::color::DARK_GRAY << "[FFMPEG] Cleaning up folder '" << m_base_path << "' for (new) install...";

	std::filesystem::remove_all(m_base_path);

	cout << console::color::DARK_GREEN << "[FFMPEG] Cleaned up folder '" << m_base_path << "'.";
}

void FFMPEG::_cleanup_ffmpeg_folder() const
{
	using namespace Lunaris;

	const auto expected_cmd = m_base_path + g_7zip_portable_file; // 7z exe path
	const auto file_path = m_base_path + g_ffmpeg_download_file; // downloaded file

	cout << console::color::DARK_GRAY << "[FFMPEG] Cleaning up folder '" << m_base_path << "' temporary files now...";

	std::filesystem::remove(expected_cmd);
	std::filesystem::remove(file_path);

	cout << console::color::DARK_GREEN << "[FFMPEG] Cleaned up temporary files.";
}

bool FFMPEG::_check_ffmpeg_exist_and_is_latest() const
{
	using namespace Lunaris;

	const std::string expected_path = this->_get_ffmpeg_exe();
	if (!std::filesystem::exists(expected_path)) {
		cout << console::color::GOLD << "[FFMPEG] FFMPEG does not exist in latest path";
		return false;
	}

	cout << console::color::DARK_GREEN << "[FFMPEG] FFMPEG is in latest version and exists";	
	return true;
}

void FFMPEG::_call_7z_to_extract_ffmpeg()
{
	using namespace Lunaris;

	const auto expected_cmd = m_base_path + g_7zip_portable_file; // 7z exe path
	const auto file_path = m_base_path + g_ffmpeg_download_file; // downloaded file

	cout << console::color::DARK_GRAY << "[FFMPEG] Extracting FFMPEG...";

	m_proc = std::unique_ptr<Lunaris::process_sync>(
		new Lunaris::process_sync(
			expected_cmd, { "x", file_path, "-o\"" + m_base_path + "\"" }, Lunaris::process_sync::mode::READ
		)
	);
	_auto_wait_proc_end_silent();
	m_proc.reset();

	cout << console::color::DARK_GREEN << "[FFMPEG] Ended extracting latest FFMPEG.";
}

void FFMPEG::_auto_wait_proc_end_silent() const
{
	using namespace Lunaris;

	if (!m_proc) return;
	while (m_proc->is_running()) std::this_thread::sleep_for(std::chrono::milliseconds(100));
}



FFMPEG::FFMPEG()
	: m_base_path(get_env(g_ffpmeg_base_path) + g_ffmpeg_path)
{
}


bool FFMPEG::setup()
{
	using namespace Lunaris;

	downloader down;
	down.get_store(g_ffmpeg_download_ver_url, [&](const char* str, const size_t len) {m_current_remote_ver.append(str, len); });

	cout << console::color::DARK_GREEN << "[FFMPEG] Checked latest version on remote: '" << m_current_remote_ver << "'";

	// if there, ready
	if (this->_check_ffmpeg_exist_and_is_latest()) return true;

	// if does not exist, clean up first
	this->_reset_ffmpeg_folder();
	// download new
	if (!this->_download_all_files()) return false;
	// extract
	this->_call_7z_to_extract_ffmpeg();
	// clean up temporary files
	this->_cleanup_ffmpeg_folder();
	// ready

	return true;
}

void FFMPEG::call(const std::initializer_list<std::string>& cmds) const
{
	m_proc = std::unique_ptr<Lunaris::process_sync>(
		new Lunaris::process_sync(this->_get_ffmpeg_exe(), cmds, Lunaris::process_sync::mode::READ
		)
	);
	this->_auto_wait_proc_end_silent();
	m_proc.reset();
}

void FFMPEG::call(const std::initializer_list<std::string>& cmds, const std::function<void(const std::string&)>& out) const
{
	m_proc = std::unique_ptr<Lunaris::process_sync>(
		new Lunaris::process_sync(this->_get_ffmpeg_exe(), cmds, Lunaris::process_sync::mode::READ
		)
	);
	while (m_proc->is_running() || m_proc->has_read()) {
		while (m_proc->has_read()) {
			const auto did_read = m_proc->read();
			out(did_read);

			if (!m_proc->has_read()) std::this_thread::sleep_for(std::chrono::milliseconds(10));
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}	
	m_proc.reset();
}

std::string get_env(const char* env)
{
	size_t req;
	getenv_s(&req, nullptr, 0, env);
	if (req == 0) return {};
	std::string res(req, '\0');
	getenv_s(&req, res.data(), req, env);
	while (res.length() > 0 && res.back() == '\0') res.pop_back();
	return res;
}