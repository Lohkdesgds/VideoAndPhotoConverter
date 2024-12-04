#include "shared.h"

#define LUNARIS_USE_WINDOWS_LIB
#include <Lunaris/downloader/downloader.h>
#include <Lunaris/process/process.h>
#include <Lunaris/console/console.h>
#include <nlohmann/json.hpp>

#include <fstream>
#include <filesystem>
#include <vector>

//constexpr char g_7zip_portable_url[] = "https://www.7-zip.org/a/7zr.exe";

constexpr char g_7zip_gh_releases_page[] = "https://api.github.com/repos/ip7z/7zip/releases/latest";
constexpr char g_7za_download_file[] = "7za_temp.7z";
constexpr char g_7za_portable_file[] = "7za.exe";
constexpr char g_7zr_portable_file[] = "7zr.exe";

constexpr char g_ffmpeg_download_ver_url[] = "https://www.gyan.dev/ffmpeg/builds/git-version";
constexpr char g_ffmpeg_download_url[] = "https://www.gyan.dev/ffmpeg/builds/ffmpeg-git-full.7z";
constexpr char g_ffmpeg_download_file[] = "ffmpeg_temp.7z";

constexpr char g_ffmpeg_path[] = "\\VideoAndPhotoConverter\\";
constexpr char g_ffpmeg_base_path[] = "APPDATA";

constexpr char g_magick_download_ftp_listing_url[] = "https://imagemagick.org/archive/binaries/?C=M;O=A";
constexpr char g_magick_download_base_url[] = "https://imagemagick.org/archive/binaries/";
constexpr char g_magick_download_file[] = "imagemagick.zip";


using namespace Lunaris;


std::string get_env(const char* env);
std::vector<std::string> get_folders(const std::string& path);

bool PathingStuff::_download_one(const std::string& url, const std::string& file_name) const
{
	downloader down;

	std::fstream fp(m_base_path + file_name, std::ios::out | std::ios::binary);
	if (!fp || fp.bad() || !fp.is_open()) {
		cout << console::color::DARK_RED <<
			"[PS] Could not install '" << file_name << "'. Failure on file opening.";
		return false;
	}

	down.get_store(url, [&](const char* buf, const size_t len) { fp.write(buf, len); });
	fp.close();

	cout << console::color::DARK_GREEN <<
		"[PS] '" << file_name << "' installed.";

	return true;
}

void PathingStuff::_erase_based(const std::string& dir) const
{
	std::filesystem::remove_all(m_base_path + dir);
}

bool PathingStuff::download_7zr_and_7za() const
{
	const auto exp_7zr_exe = this->get_7zr_exe();
	const auto exp_7za_exe = this->get_7za_exe();

	if (!std::filesystem::exists(exp_7zr_exe)) {
		cout << console::color::DARK_GRAY <<
			"[PS] 7zr not available. Downloading it...";

		if (!_download_one(m_7zr_fetched_link, g_7zr_portable_file)) return false;

		cout << console::color::DARK_GREEN <<
			"[PS] 7zr ready.";
	}
	else {
		cout << console::color::DARK_GREEN <<
			"[PS] 7zr is already installed.";
	}

	if (!std::filesystem::exists(exp_7za_exe)) {
		cout << console::color::DARK_GRAY <<
			"[PS] 7za not available. Downloading it...";

		if (!_download_one(m_7za_fetched_link, g_7za_download_file)) return false;

		cout << console::color::DARK_GRAY <<
			"[PS] Extracting 7za...";

		call_7z_extract(g_7za_download_file);

		cout << console::color::DARK_GRAY <<
			"[PS] Cleaning up 7za download...";

		_erase_based(g_7za_download_file);

		cout << console::color::DARK_GREEN <<
			"[PS] 7za ready.";
	}
	else {
		cout << console::color::DARK_GREEN <<
			"[PS] 7za is already installed.";
	}

	return true;
}

bool PathingStuff::download_ffmpeg() const
{
	const auto exp_ffmpeg_exe = this->get_ffmpeg_exe();

	if (!std::filesystem::exists(exp_ffmpeg_exe)) {
		cout << console::color::DARK_GRAY <<
			"[PS] FFMPEG not available in latest version. Cleaning up FFMPEG and downloading latest...";

		std::filesystem::remove_all(get_folder_with_name_in_dir(m_base_path, "ffmpeg-"));

		if (!_download_one(g_ffmpeg_download_url, g_ffmpeg_download_file)) return false;

		cout << console::color::DARK_GRAY <<
			"[PS] Extracting FFMPEG...";

		call_7z_extract(g_ffmpeg_download_file);

		cout << console::color::DARK_GRAY <<
			"[PS] Cleaning up FFMPEG download...";

		_erase_based(g_ffmpeg_download_file);

		cout << console::color::DARK_GREEN <<
			"[PS] FFMPEG ready.";
	}
	else {
		cout << console::color::DARK_GREEN <<
			"[PS] FFMPEG is already installed.";
	}

	return true;
}

bool PathingStuff::download_magick() const
{
	const auto exp_magick_exe = this->get_magick_exe();

	if (!std::filesystem::exists(exp_magick_exe)) {
		cout << console::color::DARK_GRAY <<
			"[PS] ImageMagick not available in latest version. Cleaning up ImageMagick and downloading latest...";

		std::filesystem::remove_all(get_folder_with_name_in_dir(m_base_path, "ImageMagick-"));

		if (!_download_one(m_magick_fetched_link, g_magick_download_file)) return false;

		cout << console::color::DARK_GRAY <<
			"[PS] Extracting ImageMagick...";

		call_7z_extract(g_magick_download_file);

		cout << console::color::DARK_GRAY <<
			"[PS] Cleaning up ImageMagick download...";

		_erase_based(g_magick_download_file);

		cout << console::color::DARK_GREEN <<
			"[PS] ImageMagick ready.";
	}
	else {
		cout << console::color::DARK_GREEN <<
			"[PS] ImageMagick is already installed.";
	}

	return true;
}

bool PathingStuff::fetch_all_versions_and_links()
{
	downloader down;

	// === FFMPEG ===
	{
		down.get_store(g_ffmpeg_download_ver_url,
			[&](const char* str, const size_t len) {
				m_ffmpeg_latest_version.append(str, len);
			}
		);
	}

	// === 7za ===
	{
		down.get(g_7zip_gh_releases_page);

		const std::string sevenza_json_raw(down.read().begin(), down.read().end());
		const nlohmann::json sevenza = nlohmann::json::parse(sevenza_json_raw);

		for (const auto& asset : sevenza["assets"]) {
			const auto name = asset["name"].get<std::string>();

			if (name.rfind("-extra.7z") != std::string::npos) {
				m_7za_fetched_link = asset["browser_download_url"];
			}
			if (name.find("7zr.exe") != std::string::npos) {
				m_7zr_fetched_link = asset["browser_download_url"];
			}

			if (!m_7zr_fetched_link.empty() && !m_7za_fetched_link.empty()) break;
		}
	}


	// === ImageMagick ===
	{
		down.get(g_magick_download_ftp_listing_url);

		const std::string magick_body(down.read().begin(), down.read().end());
		// for search:
		constexpr char ref_find[] = "<a href=\"ImageMagick-";
		constexpr char ref_find_aft[] = "-portable-Q16-x64.zip\">";
		// for substr based on search:
		constexpr size_t ref_find_drop_len = sizeof("<a href=\"") - 1;
		constexpr char ref_find_end_str_on[] = "\">";

		uint32_t newest_version_detected[4]{ 0,0,0,0 }; // ImageMagick-MAJOR.MINOR.REVISION-BUILD (or something like that)

		const auto compare_is_greater = [&](const uint32_t(&cmp)[4]) {
			const auto& ref = newest_version_detected;

			return
				(cmp[0] > ref[0]) || // major bump
				(cmp[0] == ref[0] && cmp[1] > ref[1]) || // minor bump
				(cmp[0] == ref[0] && cmp[1] == ref[1] && cmp[2] > ref[2]) || // revision bump
				(cmp[0] == ref[0] && cmp[1] == ref[1] && cmp[2] == ref[2] && cmp[3] > ref[3]); // build bump
			};

		for (size_t p = magick_body.find(ref_find); (p = magick_body.find(ref_find, p + 1)) != std::string::npos; ) {
			const char* ref = magick_body.c_str() + p + sizeof(ref_find) - 1;
			const char format[] = "%u.%u.%u-%u";
			uint32_t test_version[4]{ 0,0,0,0 };

			if (sscanf_s(ref, format,
				&test_version[0],
				&test_version[1],
				&test_version[2],
				&test_version[3]) != 4) continue;

			const int off = snprintf(nullptr, 0, format,
				test_version[0],
				test_version[1],
				test_version[2],
				test_version[3]
			);

			if (off <= 0) continue;

			const char* exp_ref = ref + off;

			if (strncmp(ref_find_aft, exp_ref, sizeof(ref_find_aft) - 1) != 0)
				continue;

			if (compare_is_greater(test_version)) {
				std::copy(std::begin(test_version), std::end(test_version), std::begin(newest_version_detected));
			}
		}

		m_magick_latest_version =
			std::to_string(newest_version_detected[0]) + "." +
			std::to_string(newest_version_detected[1]) + "." +
			std::to_string(newest_version_detected[2]) + "-" +
			std::to_string(newest_version_detected[3]);

		char reconst[96]{};
		snprintf(reconst, 96, "ImageMagick-%u.%u.%u-%u-portable-Q16-x64.zip",
			newest_version_detected[0],
			newest_version_detected[1],
			newest_version_detected[2],
			newest_version_detected[3]);

		m_magick_fetched_link = g_magick_download_base_url + std::string(reconst);
	}

	return true;
}

bool PathingStuff::create_dirs(const std::string& path) const
{
	std::error_code err;
	std::filesystem::create_directories(path, err);
	return !err;
}

std::string PathingStuff::get_folder_with_name_in_dir(const std::string& dir, const std::string& substr) const
{
	const auto folders_there = get_folders(m_base_path);
	for (auto& i : folders_there) {
		const size_t p = i.rfind("\\");
		const size_t f = i.rfind(substr);
		if (f != std::string::npos && f >= p) return i;
	}
	return "";
}

void PathingStuff::call_7z_extract(const std::string& zip) const
{
	const auto expected_cmd = (zip.rfind(".zip") == zip.length() - 4) ? get_7za_exe() : get_7zr_exe();
	const auto file_path = m_base_path + zip; // downloaded file

	cout << console::color::DARK_GRAY << 
		"[PS>7z] Extracting '" << zip << "' with '" << expected_cmd << "'...";

	Lunaris::process_sync proc(
		expected_cmd, { 
			"x",
			file_path,
			"-o\"" + m_base_path + "\""
		}, Lunaris::process_sync::mode::READ
	);

#ifdef _DEBUG
	std::this_thread::sleep_for(std::chrono::milliseconds(200));

	if (proc.has_read()) {
		while (proc.has_read()) {
			cout << console::color::DARK_GRAY << proc.read();
			std::this_thread::sleep_for(std::chrono::milliseconds(20));
		}
	}
#endif

	while (proc.is_running()) std::this_thread::sleep_for(std::chrono::milliseconds(50));

	cout << console::color::DARK_GREEN << "[PS>7z] Ended extracting '" << zip << "'.";
}

PathingStuff::PathingStuff()
	: m_base_path(get_env(g_ffpmeg_base_path) + g_ffmpeg_path)
{
}

bool PathingStuff::self_check_install()
{
	create_dirs(m_base_path);

	return 
		fetch_all_versions_and_links() && 
		download_7zr_and_7za() && 
		download_ffmpeg() && 
		download_magick();
}

std::string PathingStuff::get_ffmpeg_exe() const
{
	return m_base_path + "ffmpeg-" + m_ffmpeg_latest_version + "-full_build\\bin\\ffmpeg.exe";
}

std::string PathingStuff::get_magick_exe() const
{
	return m_base_path + "ImageMagick-" + m_magick_latest_version + "-portable-Q16-x64\\magick.exe";
}

std::string PathingStuff::get_7zr_exe() const
{
	return m_base_path + g_7zr_portable_file; // 7zr exe path
}

std::string PathingStuff::get_7za_exe() const
{
	return m_base_path + g_7za_portable_file; // 7za exe path
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

std::vector<std::string> get_folders(const std::string& path)
{
	std::vector<std::string> vec;
	for (const auto& i : std::filesystem::directory_iterator(path))
	{
		if (i.is_directory()) vec.push_back(std::filesystem::canonical(i.path()).string());
	}
	return vec;
}