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
#ifdef RUN_AS_WIN
	if (j.contains("7zip")) 	m_local_7zip.from_json(j["7zip"]);
#endif
	if (j.contains("magick")) 	m_local_magick.from_json(j["magick"]);

	DBGS("Done!");
}

bool PathingStuff::save_remote_configs()
{
	DBGS("About to export configs. Generating JSON...");
	
	nlohmann::json j;

	j["ffmpeg"] = m_ffmpeg.to_json();
#ifdef RUN_AS_WIN
	j["7zip"] 	= m_7zip.to_json();
#endif
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
#ifdef RUN_AS_WIN
		!m_7zip.version.empty()   && !m_7zip.download.empty() &&
#endif
		!m_magick.version.empty() && !m_magick.download.empty();
}

bool PathingStuff::check_local_is_good()
{
	return
		!m_local_ffmpeg.version.empty() && !m_local_ffmpeg.download.empty() &&
#ifdef RUN_AS_WIN
		!m_local_7zip.version.empty() 	&& !m_local_7zip.download.empty() &&
#endif
		!m_local_magick.version.empty() && !m_local_magick.download.empty();
}

void PathingStuff::install_updates()
{
#ifdef RUN_AS_WIN
	if (m_local_7zip.version != m_7zip.version) {
		DBGS("7zip has update! Downloading it...");

		if (!m_7zip.opt_dep) {
			Logger::print(Logger::type::T_ERROR, "Something very wrong happened on fetch links. 7zip second link is null!");
			throw std::runtime_error("7zip second link is null!");
		}

		DBGS("Downloading " + m_7zip.opt_dep->download + "...");
		if (const long res = down_fp(m_7zip.opt_dep->download, m_base_path + m_7zip.opt_dep->fpname); res < 200 || res > 300) {
			Logger::print(Logger::type::T_ERROR, "Could not correctly download " +
				std::string(m_7zip.opt_dep->fpname) + ". Error HTTP " + std::to_string(res));
			throw std::runtime_error("Failed downloading " + std::string(m_7zip.opt_dep->fpname));
		}
		DBGS("Downloading " + m_7zip.download + "...");
		if (const long res = down_fp(m_7zip.download, m_base_path + m_7zip.fpname); res < 200 || res > 300) {
			Logger::print(Logger::type::T_ERROR, "Could not correctly download " +
				std::string(m_7zip.fpname) + ". Error HTTP " + std::to_string(res));
			throw std::runtime_error("Failed downloading " + std::string(m_7zip.fpname));
		}
		// THEN NEEDS EXTRACT ON WIN
	}
	else DBGS("7zip is up to date.");
#endif

	if (m_local_ffmpeg.version != m_ffmpeg.version) {
		DBGS("FFMPEG has update! Downloading it...");

		DBGS("Downloading " + m_ffmpeg.download + "...");
		if (const long res = down_fp(m_ffmpeg.download, m_base_path + m_ffmpeg.fpname); res < 200 || res > 300) {
			Logger::print(Logger::type::T_ERROR, "Could not correctly download " +
				std::string(m_ffmpeg.fpname) + ". Error HTTP " + std::to_string(res));
			throw std::runtime_error("Failed downloading " + std::string(m_ffmpeg.fpname));
		}

		DBGS("Extracting FFMPEG...");

#ifdef RUN_AS_WIN
		// THEN NEEDS EXTRACT
#else
		std::filesystem::create_directories(m_base_path + path_ffmpeg);
		Lunaris::process_sync proc("/usr/bin/tar",
			{
				"-xvf",
				m_base_path + m_ffmpeg.fpname,
				"-C",
				m_base_path + path_ffmpeg
			}, Lunaris::process_sync::mode::READ
		);

		while (proc.is_running() || proc.has_read()) {
			while (proc.has_read()) {
				DBGS("TAR: " + proc.read());
				std::this_thread::sleep_for(std::chrono::milliseconds(10));
			}
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
		}

		DBGS("Done extracting FFMPEG.");
#endif
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
		// THEN NEEDS EXTRACT
#else
		DBGS("ImageMagick ready.");
#endif
	}
	else DBGS("ImageMagick is up to date.");
}

PathingStuff::PathingStuff()
	: m_base_path(get_app_path()), m_ffmpeg(get_ffmpeg()),
#ifdef RUN_AS_WIN
		m_7zip(get_7zip()),
#endif
		m_magick(get_magick())
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
#ifdef RUN_AS_WIN
const gh_auto_links& PathingStuff::get_own_7zip() const
{
	return m_7zip;
}
#endif




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

gh_auto_links get_7zip()
{
	constexpr char base_url[] = "https://api.github.com/repos/ip7z/7zip/releases/latest";
	gh_auto_links ref;

	DBGS("Requesting latest versions on remote...");
#ifdef RUN_AS_WIN
	ref.opt_dep = std::make_unique<gh_auto_links>();

	constexpr char key_7z[] = "-extra.7z"; // complete package
	constexpr char key_7zr[] = "7zr.exe"; // only works with .7z

	constexpr char fpname_zip_7z[] = "7zip.7z";
	constexpr char fpname_7zr_exe[] = "7zr.exe";

	const auto res = gh_asset_and_name(base_url, { 
		key_7z, key_7zr
	 }, ref.version);

	DBGS("Got version: " + ref.version);

	if (const auto it = res.find(key_7z); it != res.end()) {
		ref.download = it->second;
		ref.fpname = fpname_zip_7z;
		DBGS("Got 7zip url: " + ref.download);
	}
	if (const auto it = res.find(key_7zr); it != res.end()) {
		ref.opt_dep->download = it->second;
		ref.opt_dep->fpname = fpname_7zr_exe;
		ref.opt_dep->version = ref.version;
		DBGS("Got 7zr url: " + ref.opt_dep->download);
	}
#else
	throw std::runtime_error("Actually, there's no need to download 7zip on linux! You don't want this to be called.");
//	constexpr char key_7z[] = "-linux-x64.tar.xz"; // complete package
//	const auto res = gh_asset_and_name(base_url, { 
//		key_7z
//	 }, ref.version);
//
//	DBGS("Got version: " + ref.version);
//
//	if (const auto it = res.find(key_7z); it != res.end()) {
//		ref.download = it->second;
//		DBGS("Got 7zip url: " + ref.download);
//	}
#endif

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