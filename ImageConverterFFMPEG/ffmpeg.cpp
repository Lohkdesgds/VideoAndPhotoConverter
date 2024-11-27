#include "ffmpeg.h"

#define LUNARIS_USE_WINDOWS_LIB
#include <Lunaris/downloader/downloader.h>
#include <Lunaris/process/process.h>
#include <Lunaris/console/console.h>

#include <fstream>
#include <stdexcept>
#include <filesystem>

constexpr char g_ffmpeg_path[] = "\\ImageConverter\\FFMPEG\\";
constexpr char g_ffpmeg_base_path[] = "APPDATA";

constexpr char g_7zip_portable_url[] = "https://www.7-zip.org/a/7zr.exe";
constexpr char g_7zip_portable_file[] = "7zr.exe";

constexpr char g_ffmpeg_download_url[] = "https://www.gyan.dev/ffmpeg/builds/ffmpeg-git-full.7z";
constexpr char g_ffmpeg_download_file[] = "ffmpeg_temp.7z";

std::string get_env(const char* env);
bool download_file_to(const std::string& url, const std::string& full_file_path, const std::string& file_name);
void call_7z_extract(const std::string& path, const std::string& exe_name, const std::string& what_to_extract);

bool FFMPEG::check_installation()
{
	const auto path = get_env(g_ffpmeg_base_path) + g_ffmpeg_path;

	return false;
}

bool FFMPEG::install()
{
	const auto path = get_env(g_ffpmeg_base_path) + g_ffmpeg_path; // base path as %appdata%/.../ << here

	if (!download_file_to(g_ffmpeg_download_url, path, g_ffmpeg_download_file)) return false;
	if (!download_file_to(g_7zip_portable_url, path, g_7zip_portable_file)) return false;
	call_7z_extract(path, g_7zip_portable_file, g_ffmpeg_download_file);
	

	return true;
}

void FFMPEG::call(const std::string&)
{
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

bool download_file_to(const std::string& url, const std::string& full_file_path, const std::string& file_name)
{
	std::error_code err;
	std::filesystem::create_directories(full_file_path, err);
	if (err) return false;

	Lunaris::downloader down;
	std::fstream fp(full_file_path + file_name, std::ios::out | std::ios::binary);
	if (!fp || fp.bad() || !fp.is_open()) return false;

	down.get_store(url, [&](const char* buf, const size_t len) {
		fp.write(buf, len);
	});

	fp.close();
	return true;
}

void call_7z_extract(const std::string& path, const std::string& exe_name, const std::string& what_to_extract)
{
	using namespace Lunaris;


	const auto print_if_has_read = [](Lunaris::process_sync& p) {
		while (p.has_read()) {
			const auto did_read = p.read();
			cout << console::color::DARK_AQUA << "@FFMPEG: " << did_read;
			std::this_thread::sleep_for(std::chrono::milliseconds(10));
			if (!p.has_read()) std::this_thread::sleep_for(std::chrono::milliseconds(150));
		}
	};

	const auto expected_cmd = path + exe_name;
	const auto file_path = path + what_to_extract;

	cout << console::color::DARK_AQUA << "Extracting '.\\" << what_to_extract << "'... Running '" << expected_cmd << "'";

	Lunaris::process_sync proc(expected_cmd, {"x", file_path, "-o\"" + path + "\"" }, Lunaris::process_sync::mode::READ);
	
	while (proc.is_running()) {
		print_if_has_read(proc);
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}

	print_if_has_read(proc);

	cout << console::color::DARK_AQUA << "Ended extracting process.";
}