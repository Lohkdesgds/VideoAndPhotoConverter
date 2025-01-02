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

#ifdef _WIN32
#define RUN_AS_WIN // used for debugging on non windows too
#endif
//#define RUN_AS_WIN

constexpr char cfg_name[] = "installed.json";


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
	std::fstream fp(m_base_path + cfg_name, std::ios::in | std::ios::binary);

	if (!fp || fp.bad()) return;

	auto j = nlohmann::json::parse(fp, nullptr, false);
	if (j.is_discarded() || j.is_null()) return;

	if (j.contains("ffmpeg")) 	m_local_ffmpeg.from_json(j["ffmpeg"]);
	if (j.contains("7zip")) 	m_local_7zip.from_json(j["7zip"]);
	if (j.contains("magisk")) 	m_local_magisk.from_json(j["magisk"]);
}

void PathingStuff::save_remote_configs()
{
	nlohmann::json j;

	j["ffmpeg"] = m_local_ffmpeg.to_json();
	j["7zip"] 	= m_local_7zip.to_json();
	j["magisk"] = m_local_magisk.to_json();

	std::fstream fp(m_base_path + cfg_name, std::ios::out | std::ios::binary);

	if (!fp || fp.bad()) return;

	fp << j;	
}

PathingStuff::PathingStuff()
	: m_base_path(get_app_path()), m_ffmpeg(get_ffmpeg()), m_7zip(get_7zip()), m_magisk(get_magisk())
{	
	std::filesystem::create_directories(m_base_path);
}

const std::string& PathingStuff::get_base_path() const
{
	return m_base_path;
}

const gh_auto_links& PathingStuff::get_own_magisk() const
{
	return m_magisk;
}
const gh_auto_links& PathingStuff::get_own_ffmpeg() const
{
	return m_ffmpeg;
}
const gh_auto_links& PathingStuff::get_own_7zip() const
{
	return m_7zip;
}





http::response easy_down_to(const std::string& url)
{
	return http::do_GET(url, http::request().set_headers({
		"User-Agent: VideoAndPhotoConverter/1.0",
		"Accept: application/json, */*"
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
	if (res.code < 200 || res.code >= 300) return -2;	
	ou.write(res.body.data(), res.body.size());	
	return res.code;
}

std::unordered_map<std::string, std::string> 
gh_asset_and_name(const std::string& url, const std::vector<std::string>& keys, std::string& version_name)
{
	const size_t keys_to_find = keys.size();
	std::unordered_map<std::string, std::string> out;
	std::string buf;

	Logger::print(Logger::type::DEBUG, "Gathering Github assets from '" + url + "'...");

	if (const long rt = down_mem(url, buf); rt < 200 || rt >= 300) {
		Logger::print(Logger::type::ERROR, "Cannot get from '" + url + "'!");
		return out;
	}

	const nlohmann::json jroot = nlohmann::json::parse(buf, nullptr, false);

	if (jroot.is_discarded() || jroot.is_null()) {
           Logger::print(Logger::type::ERROR, "'" + url + "' got an empty or invalid JSON!");
		return out;
	}

	version_name = jroot["name"].get<std::string>();

	Logger::print(Logger::type::DEBUG, "Versioning for it: '" + version_name + "'");

	if (keys_to_find == 0) return {};		

	for (const auto& asset : jroot["assets"]) {
		const auto name = asset["name"].get<std::string>();

		Logger::print(Logger::type::DEBUG, "- On asset '" + name + "'...");

		for (const auto& key : keys) {
			if (name.find(key) != std::string::npos) {
				const std::string val = asset["browser_download_url"];
				out[key] = val;
				Logger::print(Logger::type::DEBUG, "--> Found JSON key match '" + name + "' => '" + val + "'");
				break;
			}
		}
		if (out.size() >= keys_to_find) break;
	}

	Logger::print(Logger::type::DEBUG, "End of gather Github assets from '" + url + "'.");
	return out;
};


gh_auto_links get_magisk()
{
	constexpr char base_url[] = "https://api.github.com/repos/ImageMagick/ImageMagick/releases/latest";
	gh_auto_links ref;

	gh_asset_and_name(base_url, {}, ref.version);
#ifdef RUN_AS_WIN
	ref.download = "https://imagemagick.org/archive/binaries/ImageMagick-" + ref.version + "-portable-Q16-x64.zip";
#else
	ref.download = "https://imagemagick.org/archive/binaries/magick"; // static on linux
#endif
	
	return ref;
}

gh_auto_links get_ffmpeg()
{
	constexpr char base_url[] = "https://api.github.com/repos/BtbN/FFmpeg-Builds/releases/latest";
	
	gh_auto_links ref;

#ifdef RUN_AS_WIN
	constexpr char key[] = "-master-latest-win64-gpl.zip";	
#else
	constexpr char key[] = "-master-latest-linux64-lgpl.tar.xz";
#endif
	const auto res = gh_asset_and_name(base_url, { key }, ref.version);

	if (const auto it = res.find(key); it != res.end()) {
		ref.download = it->second;
	}

	return ref;
}

gh_auto_links get_7zip()
{
	constexpr char base_url[] = "https://api.github.com/repos/ip7z/7zip/releases/latest";

	gh_auto_links ref;

#ifdef RUN_AS_WIN
	ref.opt_dep = std::make_unique<gh_auto_links>();

	constexpr char key_7z[] = "-extra.7z"; // complete package
	constexpr char key_7zr[] = "7zr.exe"; // only works with .7z
	const auto res = gh_asset_and_name(base_url, { 
		key_7z, key_7zr
	 }, ref.version);

	if (const auto it = res.find(key_7z); it != res.end()) {
		ref.download = it->second;
	}
	if (const auto it = res.find(key_7zr); it != res.end()) {
		ref.opt_dep->download = it->second;
		ref.opt_dep->version = ref.version;
	}
#else
	constexpr char key_7z[] = "-linux-x64.tar.xz"; // complete package
	const auto res = gh_asset_and_name(base_url, { 
		key_7z
	 }, ref.version);

	if (const auto it = res.find(key_7z); it != res.end()) {
		ref.download = it->second;
	}
#endif

	return ref;
}

std::string get_app_path()
{
#ifdef _WIN32
    const char* env = "APPDATA";
#else
    const char* env = "HOME";
#endif
	std::string res = getenv(env);
	while (res.length() > 0 && res.back() == '\0') res.pop_back();
#ifdef _WIN32
    return res + "\\VideoAndPhotoConverter\\";
#elif defined __APPLE__ // not working with it, but it is here anyway
    return res + "/Library/Caches/VideoAndPhotoConverter/";
#else
    return res + "/.videoandphotoconverter/";
#endif
}