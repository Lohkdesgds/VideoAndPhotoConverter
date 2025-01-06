#include "shared.h"

#ifdef _WIN32
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <Lunaris/CURL/curl_cpp.h>
#include <Lunaris/Process/process.h>
#include <nlohmann/json.hpp>

#include <logger.h>

#include <fstream>
#include <filesystem>
#include <vector>
#include <unordered_map>

#ifdef _WIN32
#define SLASH "\\"
#else
#define SLASH "/"
#endif



constexpr char cfg_name[] = "installed.json";
constexpr char path_ffmpeg[] = "ffmpeg";
#ifdef RUN_AS_WIN
constexpr char path_magick[] = "imagemagick";
#endif


/* = = = = = = RAW CALLS = = = = = = */
http::response easy_down_to(const std::string& url);
long down_mem(const std::string& url, std::string& mem);
long down_fp(const std::string& url, const std::string& fp);
std::unordered_map<std::string, std::string> gh_asset_and_name(const std::string& url, const std::vector<std::string>& keys, std::string& version_name);
std::string get_app_path();


void gh_auto_links::from_json(const nlohmann::json& j)
{
	if (j.contains("download")) download = j["download"];
	if (j.contains("version")) version = j["version"];
	if (j.contains("opt_dep")) {
		opt_dep = std::make_unique<gh_auto_links>();
		opt_dep->from_json(j["opt_dep"]);
	}
}

nlohmann::json gh_auto_links::to_json() const
{
	nlohmann::json j;
	j["download"] = download;
	j["version"] = version;
	if (opt_dep) {
		j["opt_dep"] = opt_dep->to_json();
	}
	return j;
}



//bool directory_parsed::has_dir(const std::string& s) const
//{
//	for(const auto& i : directories) {
//		if (const auto f = i.find(s); f == 0) return true;
//	}
//	return false;
//}
//
//bool directory_parsed::has_file(const std::string& s) const
//{
//	for(const auto& i : files) {
//		if (const auto f = i.find(s); f == 0) return true;
//	}
//	return false;
//}

std::string directory_parsed::find_dir(const std::string& s) const
{
	for(const auto& i : directories) {
		if (const auto f = i.find(s); f != std::string::npos) return i;
	}
	return {};
}
std::string directory_parsed::find_file(const std::string& s) const
{
	for(const auto& i : files) {
		if (const auto f = i.find(s); f != std::string::npos) return i;
	}
	return {};
}

directory_parsed directory_parsed::operator[](const std::string& path) const 
{
	return directory_parsed(raw_current_path + (path.find(SLASH) == 0 ? "" : SLASH) + path);
}

directory_parsed::directory_parsed(const std::string& path)
	: raw_current_path(std::filesystem::canonical(std::filesystem::directory_entry(path)).string())
{
	for (const auto& entry : std::filesystem::directory_iterator(raw_current_path))
	{
		if (entry.is_directory()) directories.push_back(entry.path().string().substr(raw_current_path.length() + 1));
		else if (entry.is_regular_file()) files.push_back(entry.path().string().substr(raw_current_path.length() + 1));
	}
}




void PathingStuff::load_local_configs()
{
	DBGS("Opening config file...");

	std::fstream fp(m_base_path + cfg_name, std::ios::in | std::ios::binary);

	if (!fp || fp.bad()) {
		DBGS("No file found or invalid. Considering new installation then.");
		return;
	}

	DBGS("Parsing...");
	auto j = nlohmann::json::parse(fp, nullptr, false);

	if (j.is_discarded() || j.is_null()) {
		DBGS("Failed to parse. Considering new installation then.");
		return;
	}

	if (j.contains("ffmpeg")) 	m_local_ffmpeg.from_json(j["ffmpeg"]);
	if (j.contains("magick")) 	m_local_magick.from_json(j["magick"]);

	DBGS("Done!");
}

bool PathingStuff::save_remote_configs()
{
	DBGS("About to export configs. Generating JSON...");
	
	nlohmann::json j;

	j["ffmpeg"] = m_ffmpeg.to_json();
	j["magick"] = m_magick.to_json();

	DBGS("Opening file to write...");
	std::fstream fp(m_base_path + cfg_name, std::ios::out | std::ios::binary);

	if (!fp || fp.bad()) {
		Logger::print(Logger::type::T_ERROR, "Cannot open file to write save on.");
		return false;
	}

	DBGS("Writing file...");
	fp << j;

	DBGS("Done.");
	return true;
}

bool PathingStuff::check_remote_is_good()
{
	return
		!m_ffmpeg.version.empty() && !m_ffmpeg.download.empty() &&
		!m_magick.version.empty() && !m_magick.download.empty();
}

bool PathingStuff::check_local_is_good()
{
	return
		!m_local_ffmpeg.version.empty() && !m_local_ffmpeg.download.empty() &&
		!m_local_magick.version.empty() && !m_local_magick.download.empty();
}

void PathingStuff::install_updates()
{
	if (m_local_ffmpeg.version != m_ffmpeg.version) {
		DBGS("FFMPEG has update! Downloading it...");

		DBGS("Downloading " + m_ffmpeg.download + "...");
		if (const long res = down_fp(m_ffmpeg.download, m_base_path + m_ffmpeg.fpname); res < 200 || res > 300) {
			Logger::print(Logger::type::T_ERROR, "Could not correctly download " +
				std::string(m_ffmpeg.fpname) + ". Error HTTP " + std::to_string(res));
			throw std::runtime_error("Failed downloading " + std::string(m_ffmpeg.fpname));
		}

		DBGS("Extracting FFMPEG...");
		std::filesystem::create_directories(m_base_path + path_ffmpeg);

#ifdef RUN_AS_WIN
		Lunaris::process_sync proc("tar",
			{
				"xzfv",
				m_base_path + m_ffmpeg.fpname,
				"-C",
				m_base_path + path_ffmpeg
			}, Lunaris::process_sync::mode::READ
		);

#else
		Lunaris::process_sync proc("/usr/bin/tar",
			{
				"-xvf",
				m_base_path + m_ffmpeg.fpname,
				"-C",
				m_base_path + path_ffmpeg
			}, Lunaris::process_sync::mode::READ
		);
#endif
		while (proc.is_running()) std::this_thread::sleep_for(std::chrono::milliseconds(100));

		directory_parsed dp(m_base_path + path_ffmpeg);
		const auto root = dp[dp.find_dir("ffmpeg")]["bin"];
		const auto search = root.find_file("ffmpeg");

		if (!search.empty()) {
			m_path_ffmpeg = root.raw_current_path + "/" + search;
			DBGS("Found FFMPEG: " + m_path_ffmpeg);
		}
		
		DBGS("Done extracting FFMPEG.");
	}
	else DBGS("FFMPEG is up to date.");

	if (m_local_magick.version != m_magick.version) {
		DBGS("ImageMagick has update! Downloading it...");

		DBGS("Downloading " + m_magick.download + "...");
		if (const long res = down_fp(m_magick.download, m_base_path + m_magick.fpname); res < 200 || res > 300) {
			Logger::print(Logger::type::T_ERROR, "Could not correctly download " +
				std::string(m_magick.fpname) + ". Error HTTP " + std::to_string(res));
			throw std::runtime_error("Failed downloading " + std::string(m_magick.fpname));
		}

#ifdef RUN_AS_WIN
		DBGS("Extracting FFMPEG...");
		std::filesystem::create_directories(m_base_path + path_magick);

		Lunaris::process_sync proc("tar",
			{
				"xzfv",
				m_base_path + m_ffmpeg.fpname,
				"-C",
				m_base_path + path_magick
			}, Lunaris::process_sync::mode::READ
		);

		while (proc.is_running()) std::this_thread::sleep_for(std::chrono::milliseconds(100));

		directory_parsed dp(m_base_path + path_magick);
		const auto root = dp[dp.find_dir("ImageMagick")];
		const auto search = root.find_file("agick.exe");

		if (!search.empty()) {
			m_path_magick = dp.raw_current_path + "/" + search;
			DBGS("Found ImageMagick: " + m_path_magick);
		}
		
		DBGS("Done extracting ImageMagick.");

#else
		DBGS("ImageMagick ready.");
		m_path_magick = m_base_path + m_magick.fpname;
		DBGS("ImageMagick at: " + m_path_magick);
#endif
	}
	else DBGS("ImageMagick is up to date.");
}

PathingStuff::PathingStuff()
	: m_base_path(get_app_path()), m_ffmpeg(get_ffmpeg()), m_magick(get_magick())
{	
	DBGS("Setting up paths...");
	std::filesystem::create_directories(m_base_path);
	DBGS("Loading local data...");
	load_local_configs();
	DBGS("Checking remote is available...");
	if (check_remote_is_good()) {
		DBGS("Installing updates or missing files (based on config)...");
		install_updates();
		DBGS("Saving current state...");
		save_remote_configs();
		DBGS("Done.");
	}
	else {
		DBGS("Checking if there is local content...");
		if (!check_local_is_good()) {
			Logger::print(Logger::type::T_ERROR, "Remote is offline and there is no local stored configuration! Cannot continue!");
			throw std::runtime_error("Remote is offline and there is no local stored configuration!");
		}
		DBGS("There is local content. Hoping that this is enough.");
	}
}

const std::string& PathingStuff::get_base_path() const
{
	return m_base_path;
}

const gh_auto_links& PathingStuff::get_own_magick() const
{
	return m_magick;
}
const gh_auto_links& PathingStuff::get_own_ffmpeg() const
{
	return m_ffmpeg;
}




http::response easy_down_to(const std::string& url)
{
	return http::do_GET(url, http::request().set_headers({
		"User-Agent: VideoAndPhotoConverter/1.0"
	}));
}

long down_mem(const std::string& url, std::string& mem)
{
	auto res = easy_down_to(url);
	mem = res.body;
	return res.code;
}

long down_fp(const std::string& url, const std::string& fp)
{
	std::fstream ou(fp, std::ios::out | std::ios::binary);
	if (!ou || ou.bad() || !ou.is_open()) return -1;
	auto res = easy_down_to(url);
	if (res.code < 200 || res.code >= 300) {
		DBGS("DOWNLOAD ERROR " + std::to_string(res.code));
		DBGS("- BODY: " + res.body);
		DBGS("- HEADERS: ");
		for(const auto& i : res.head) {
			DBGS("- | " + i);
		}
		return res.code;			
	}
	ou.write(res.body.data(), res.body.size());	
	return res.code;
}

std::unordered_map<std::string, std::string> 
gh_asset_and_name(const std::string& url, const std::vector<std::string>& keys, std::string& version_name)
{
	const size_t keys_to_find = keys.size();
	std::unordered_map<std::string, std::string> out;
	std::string buf;

	DBGS("Gathering Github assets from '" + url + "'...");

	if (const long rt = down_mem(url, buf); rt < 200 || rt >= 300) {
		Logger::print(Logger::type::T_ERROR, "Cannot get from '" + url + "'!");
		return out;
	}

	const nlohmann::json jroot = nlohmann::json::parse(buf, nullptr, false);

	if (jroot.is_discarded() || jroot.is_null()) {
           Logger::print(Logger::type::T_ERROR, "'" + url + "' got an empty or invalid JSON!");
		return out;
	}

	version_name = jroot["name"].get<std::string>();

	DBGS("Versioning for it: '" + version_name + "'");

	if (keys_to_find == 0) return {};		

	for (const auto& asset : jroot["assets"]) {
		const auto name = asset["name"].get<std::string>();

		DBGS("- On asset '" + name + "'...");

		for (const auto& key : keys) {
			if (name.find(key) != std::string::npos) {
				const std::string val = asset["browser_download_url"];
				out[key] = val;
				DBGS("--> Found JSON key match '" + name + "' => '" + val + "'");
				break;
			}
		}
		if (out.size() >= keys_to_find) break;
	}

	DBGS("End of gather Github assets from '" + url + "'.");
	return out;
};


gh_auto_links get_magick()
{
	constexpr char base_url[] = "https://api.github.com/repos/ImageMagick/ImageMagick/releases/latest";
	gh_auto_links ref;

	DBGS("Requesting latest versions on remote...");

	gh_asset_and_name(base_url, {}, ref.version);

	DBGS("Got version: " + ref.version);

#ifdef RUN_AS_WIN
	ref.download = "https://imagemagick.org/archive/binaries/ImageMagick-" + ref.version + "-portable-Q16-x64.zip";
	ref.fpname = "imagemagick.zip";

	DBGS("Reconstructed ImageMagick url: " + ref.download);
#else
	ref.download = "https://imagemagick.org/archive/binaries/magick"; // static on linux
	DBGS("Retrieved static ImageMagick url: " + ref.download);
	ref.fpname = "imagemagick";
#endif
	
	return ref;
}

gh_auto_links get_ffmpeg()
{
	constexpr char base_url[] = "https://api.github.com/repos/BtbN/FFmpeg-Builds/releases/latest";	
	gh_auto_links ref;

	DBGS("Requesting latest versions on remote...");

#ifdef RUN_AS_WIN
	constexpr char key[] = "-master-latest-win64-gpl.zip";
	constexpr char fpname[] = "ffmpeg.zip";
#else
	constexpr char key[] = "-master-latest-linux64-lgpl.tar.xz";
	constexpr char fpname[] = "ffmpeg.tar.xz";
#endif

	const auto res = gh_asset_and_name(base_url, { key }, ref.version);
	ref.fpname = fpname;

	DBGS("Got version: " + ref.version);

	if (const auto it = res.find(key); it != res.end()) {
		ref.download = it->second;
		DBGS("Got FFMPEG url: " + ref.download);
	}

	return ref;
}

std::string get_app_path()
{
	DBGS("Getting app path...");
#ifdef _WIN32
    const char* env = "APPDATA";
#else
    const char* env = "HOME";
#endif
	DBGS("Searching for '" + std::string(env) + "'");
	std::string res = getenv(env);
	while (res.length() > 0 && res.back() == '\0') res.pop_back();

#ifdef _WIN32
    const auto app_path = res + "\\VideoAndPhotoConverter\\";
//#elif defined __APPLE__ // not working with it, but it is here anyway
//    const auto app_path = res + "/Library/Caches/VideoAndPhotoConverter/";
#else
    const auto app_path = res + "/.videoandphotoconverter/";
#endif

	DBGS("Got app path: " + app_path);
	return app_path;
}