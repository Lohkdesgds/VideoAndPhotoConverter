//#include "shared.h"
//
//#ifdef _WIN32
//#define _CRT_SECURE_NO_WARNINGS
//#endif
//
//#include <Lunaris/CURL/curl_cpp.h>
//#include <Lunaris/Process/process.h>
//#include <nlohmann/json.hpp>
//
//#include <logger.h>
//
//#include <fstream>
//#include <filesystem>
//#include <vector>
//#include <unordered_map>
//
//#ifdef _WIN32
//#define SLASH "\\"
//#else
//#define SLASH "/"
//#endif
//
///// ====== FFMPEG stuff ====== ///
//constexpr char g_ffmpeg_gh_releases_page[] = "https://api.github.com/repos/BtbN/FFmpeg-Builds/releases/latest";
//
//#ifdef _WIN32
//constexpr char g_ffmpeg_link_contains[] = "-master-latest-win64-gpl.zip";
//#else
//constexpr char g_ffmpeg_link_contains[] = "-master-latest-linux64-lgpl.tar.xz";
//#endif
//constexpr char g_ffmpeg_download_file[] = "ffmpeg_temp.7z";
//
///// ====== 7-zip stuff ====== ///
//constexpr char g_7zip_gh_releases_page[] = "https://api.github.com/repos/ip7z/7zip/releases/latest";
//
//#ifdef _WIN32
//constexpr char g_7za_download_file[] = "7za_temp.7z";
//
//constexpr char g_7za_portable_file[] = "7za.exe";
//constexpr char g_7z_portable_file[] = "7zr.exe";
//
//constexpr char g_7za_link_contains[] = "-extra.7z";
//constexpr char g_7z_link_contains[] = "7zr.exe";
//#else
//constexpr char g_7z_download_file[] = "7zz.tar.gz"; // link; assume linux instance has tar (tar -xzvf file.tar.gz)
//
//constexpr char g_7z_portable_file[] = "7zz"; // final executable
//
//constexpr char g_7z_link_contains[] = "-linux-x64.tar.xz"; // finds with this
//#endif
//
///// ====== ImageMagic stuff ====== ///
//constexpr char g_magick_gh_releases_page[] = "https://api.github.com/repos/ImageMagick/ImageMagick/releases/latest"; // ONLY FOR VERSIONING!
//
//#ifdef _WIN32
//constexpr char g_magick_download_ftp_listing_url[] = "https://imagemagick.org/archive/binaries/?C=M;O=A";
//constexpr char g_magick_download_base_url[] = "https://imagemagick.org/archive/binaries/";
//constexpr char g_magick_download_file[] = "imagemagick.zip";
//#else
//constexpr char g_magick_download_static[] = "https://imagemagick.org/archive/binaries/magick";
//#endif
//
//
//using namespace Lunaris;
//
//
//std::string get_app_path();
//std::vector<std::string> get_folders(const std::string& path);
//
//bool PathingStuff::_download_fp(const std::string& url, const std::string& file_name) const
//{
//	std::fstream fp(m_base_path + file_name, std::ios::out | std::ios::binary);
//	if (!fp || fp.bad() || !fp.is_open()) {
//        Logger::print(Logger::type::T_ERROR, "[PS] Could not download to '" + file_name + "'. Failure on file opening.");
//		return false;
//	}
//
//    std::string mem;
//
//    if (!_download_to(url, mem)) return false;
//
//    fp.write(mem.data(), mem.size());
//
//	fp.close();
//
//	Logger::print(Logger::type::T_INFO, "[PS] '" + file_name + "' installed.");
//
//	return true;
//}
//
//bool PathingStuff::_download_to(const std::string& url, std::string& target) const
//{
//    http::response res = http::do_GET(url, http::request().set_headers({
//        "User-Agent: VideoAndPhotoConverter/1.0",
//        "Accept: application/json, */*"
//    }));
//
//    if (res.code < 200 || res.code >= 300) {
//        Logger::print(Logger::type::T_ERROR, "[PS] Internet down or link broken! HTTP status: " + std::to_string(res.code));
//        Logger::print(Logger::type::T_ERROR, "[PS] Body: " + res.body);
//        return false;
//    }
//
//    target = std::move(res.body);
//
//	Logger::print(Logger::type::T_DEBUG, "[PS] Downloaded from '" + url + "'.");
//
//	return true;
//}
//
//void PathingStuff::_erase_based(const std::string& dir) const
//{
//	std::filesystem::remove_all(m_base_path + dir);
//}
//
//bool PathingStuff::download_7z() const
//{
//	const auto exp_7z_exe = this->get_7z_exe();
//#ifdef _WIN32
//	const auto exp_7za_exe = this->get_7za_exe();
//#endif
//
//	if (!std::filesystem::exists(exp_7z_exe)) {
//		Logger::print(Logger::type::T_INFO, "[PS] 7z not available. Downloading it...");
//
//#ifdef _WIN32
//		if (!_download_fp(m_dl.m_7z, g_7z_portable_file)) return false;
//#else
//
//		Logger::print(Logger::type::T_INFO, "[PS] Extracting 7z...");
//
//		if (!_download_fp(m_dl.m_7z, g_7z_download_file)) return false;
//
//		call_tar_extract(g_7z_portable_file);
//#endif
//
//		Logger::print(Logger::type::T_INFO, "[PS] 7z ready.");
//	}
//	else {
//		Logger::print(Logger::type::T_INFO, "[PS] 7z is already installed.");
//	}
//
//#ifdef _WIN32
//	if (!std::filesystem::exists(exp_7za_exe)) {
//		Logger::print(Logger::type::T_INFO, "[PS] 7za not available. Downloading it...");
//
//		if (!_download_fp(m_dl.m_7za, g_7za_download_file)) return false;
//
//		Logger::print(Logger::type::T_INFO, "[PS] Extracting 7za...");
//
//		call_7z_extract(g_7za_download_file);
//
//		Logger::print(Logger::type::T_INFO, "[PS] Cleaning up 7za download...");
//
//		_erase_based(g_7za_download_file);
//
//		Logger::print(Logger::type::T_INFO, "[PS] 7za ready.");
//	}
//	else {
//		Logger::print(Logger::type::T_INFO, "[PS] 7za is already installed.");
//	}
//#else
//
//#endif
//
//	return true;
//}
//
//bool PathingStuff::download_ffmpeg() const
//{
//	const auto exp_ffmpeg_exe = this->get_ffmpeg_exe();
//
//	if (!std::filesystem::exists(exp_ffmpeg_exe)) {
//		Logger::print(Logger::type::T_INFO, "[PS] FFMPEG not available in latest version. "
//            "Cleaning up FFMPEG and downloading latest...");
//
//		std::filesystem::remove_all(get_folder_with_name_in_dir(m_base_path, "ffmpeg-"));
//
//		if (!_download_fp(m_dl.m_ffmpeg, g_ffmpeg_download_file)) return false;
//
//		Logger::print(Logger::type::T_INFO, "[PS] Extracting FFMPEG...");
//
//		call_7z_extract(g_ffmpeg_download_file);
//
//		Logger::print(Logger::type::T_INFO, "[PS] Cleaning up FFMPEG download...");
//
//		_erase_based(g_ffmpeg_download_file);
//
//		Logger::print(Logger::type::T_INFO, "[PS] FFMPEG ready.");
//	}
//	else {
//		Logger::print(Logger::type::T_INFO, "[PS] FFMPEG is already installed.");
//	}
//
//	return true;
//}
//
//bool PathingStuff::download_magick() const
//{
////	const auto exp_magick_exe = this->get_magick_exe();
////
////	if (!std::filesystem::exists(exp_magick_exe)) {
////		Logger::print(Logger::type::T_INFO, "[PS] ImageMagick not available in latest version. "
////            "Cleaning up ImageMagick and downloading latest...");
////
////		std::filesystem::remove_all(get_folder_with_name_in_dir(m_base_path, "ImageMagick-"));
////
////		if (!_download_fp(m_dl.m_magick, g_magick_download_file)) return false;
////
////		Logger::print(Logger::type::T_INFO, "[PS] Extracting ImageMagick...");
////
////		call_7z_extract(g_magick_download_file);
////
////		Logger::print(Logger::type::T_INFO, "[PS] Cleaning up ImageMagick download...");
////
////		_erase_based(g_magick_download_file);
////
////		Logger::print(Logger::type::T_INFO, "[PS] ImageMagick ready.");
////	}
////	else {
////		Logger::print(Logger::type::T_INFO, "[PS] ImageMagick is already installed.");
////	}
//
//	return true;
//}
//
//bool PathingStuff::fetch_all_versions_and_links()
//{
//
//	const auto github_get_assets_download_url = [&](const std::string& url, const std::vector<std::string>& keys, std::string& version_name_back)
//	 	-> std::unordered_map<std::string, std::string> 
//	{
//		const size_t keys_to_find = keys.size();
//
//		std::unordered_map<std::string, std::string> out;
//		std::string buf;
//
//		Logger::print(Logger::type::T_DEBUG, "Gathering Github assets from '" + url + "'...");
//
//		if (!_download_to(url, buf)) {
//            Logger::print(Logger::type::T_ERROR, "Cannot get from '" + url + "'!");
//			return out;
//		}
//
//		const nlohmann::json jroot = nlohmann::json::parse(buf, nullptr, false);
//
//		if (jroot.is_discarded() || jroot.is_null()) {
//            Logger::print(Logger::type::T_ERROR, "'" + url + "' got an empty or invalid JSON!");
//			return out;
//		}
//
//		version_name_back = jroot["name"].get<std::string>() + "_id" + std::to_string(jroot["id"].get<unsigned long long>());
//		Logger::print(Logger::type::T_DEBUG, "Versioning for it: '" + version_name_back + "'");
//
//		if (keys_to_find == 0) return {};		
//
//		for (const auto& asset : jroot["assets"]) {
//			const auto name = asset["name"].get<std::string>();
//			Logger::print(Logger::type::T_DEBUG, "- On asset '" + name + "'...");
//
//			for (const auto& key : keys) {
//				if (name.find(key) != std::string::npos) {
//					const std::string val = asset["browser_download_url"];
//					out[key] = val;
//					Logger::print(Logger::type::T_DEBUG, "--> Found JSON key match '" + name + "' => '" + val + "'");
//					break;
//				}
//			}
//
//			if (out.size() >= keys_to_find) break;
//		}
//
//		Logger::print(Logger::type::T_DEBUG, "End of gather Github assets from '" + url + "'.");
//		return out;
//	};
//
//
//	Logger::print(Logger::type::T_DEBUG, "Working on 7-zip link related stuff...");
//	// ==== 7zip all ==== //
//	{
//
//#ifdef _WIN32
//		auto res = github_get_assets_download_url(g_7zip_gh_releases_page, {
//			g_7za_link_contains,
//			g_7z_link_contains
//		}, m_gh.m_7zip);
//
//		m_dl.m_7z = res[g_7z_link_contains];
//		m_dl.m_7za = res[g_7za_link_contains];
//#else // linux
//		auto res = github_get_assets_download_url(g_7zip_gh_releases_page, {
//			g_7zz_link_contains
//		}, m_gh.m_7zip);
//
//		m_dl.m_7z = res[g_7zz_link_contains];
//#endif
//	}
//	Logger::print(Logger::type::T_DEBUG, "Ended 7-zip link related stuff.");
//
//	// ==== FFMPEG all ==== //
//	Logger::print(Logger::type::T_DEBUG, "Working on FFMPEG link related stuff...");
//	{
//
//		auto res = github_get_assets_download_url(g_ffmpeg_gh_releases_page, {
//			g_ffmpeg_link_contains
//		}, m_gh.m_ffmpeg);
//
//		m_dl.m_ffmpeg = res[g_ffmpeg_link_contains];
//	}
//	Logger::print(Logger::type::T_DEBUG, "Ended FFMPEG link related stuff.");
//
//
//	Logger::print(Logger::type::T_DEBUG, "Working on ImageMagick link related stuff...");
//	{
//		github_get_assets_download_url(g_magick_gh_releases_page, {}, m_gh.m_magick);
//	}
//	// Windows does not have a static link to latest version, so this is needed
//#ifdef _WIN32
//	// === ImageMagick ===
//	{
//        std::string magick_body;
//        if (!_download_to(g_magick_download_ftp_listing_url, magick_body)) {
//            Logger::print(Logger::type::T_ERROR, "Cannot fetch ImageMagick version!");
//            return false;
//        }
//        
//		// for search:
//		constexpr char ref_find[] = "<a href=\"ImageMagick-";
//		constexpr char ref_find_aft[] = "-portable-Q16-x64.zip\">";
//		// for substr based on search:
//		constexpr size_t ref_find_drop_len = sizeof("<a href=\"") - 1;
//		constexpr char ref_find_end_str_on[] = "\">";
//
//		uint32_t newest_version_detected[4]{ 0,0,0,0 }; // ImageMagick-MAJOR.MINOR.REVISION-BUILD (or something like that)
//
//		const auto compare_is_greater = [&](const uint32_t(&cmp)[4]) {
//			const auto& ref = newest_version_detected;
//
//			return
//				(cmp[0] > ref[0]) || // major bump
//				(cmp[0] == ref[0] && cmp[1] > ref[1]) || // minor bump
//				(cmp[0] == ref[0] && cmp[1] == ref[1] && cmp[2] > ref[2]) || // revision bump
//				(cmp[0] == ref[0] && cmp[1] == ref[1] && cmp[2] == ref[2] && cmp[3] > ref[3]); // build bump
//			};
//
//		for (size_t p = magick_body.find(ref_find); (p = magick_body.find(ref_find, p + 1)) != std::string::npos; ) {
//			const char* ref = magick_body.c_str() + p + sizeof(ref_find) - 1;
//			const char format[] = "%u.%u.%u-%u";
//			uint32_t test_version[4]{ 0,0,0,0 };
//
//			if (sscanf(ref, format,
//				&test_version[0],
//				&test_version[1],
//				&test_version[2],
//				&test_version[3]) != 4) continue;
//
//			const int off = snprintf(nullptr, 0, format,
//				test_version[0],
//				test_version[1],
//				test_version[2],
//				test_version[3]
//			);
//
//			if (off <= 0) continue;
//
//			const char* exp_ref = ref + off;
//
//			if (strncmp(ref_find_aft, exp_ref, sizeof(ref_find_aft) - 1) != 0)
//				continue;
//
//			if (compare_is_greater(test_version)) {
//				std::copy(std::begin(test_version), std::end(test_version), std::begin(newest_version_detected));
//			}
//		}
//
//		auto m_magick_latest_version =
//			std::to_string(newest_version_detected[0]) + "." +
//			std::to_string(newest_version_detected[1]) + "." +
//			std::to_string(newest_version_detected[2]) + "-" +
//			std::to_string(newest_version_detected[3]);
//
//		char reconst[96]{};
//		snprintf(reconst, 96, "ImageMagick-%u.%u.%u-%u-portable-Q16-x64.zip",
//			newest_version_detected[0],
//			newest_version_detected[1],
//			newest_version_detected[2],
//			newest_version_detected[3]);
//
//		m_dl.m_magick = g_magick_download_base_url + std::string(reconst);
//
//        //Logger::print(Logger::type::T_DEBUG, "Fetched / rebuilt ImageMagick version: " + m_magick_latest_version);
//        Logger::print(Logger::type::T_DEBUG, "Fetched / rebuilt ImageMagick link: " + m_dl.m_magick);
//	}
//#else // linux
//	{
//		m_dl.m_magick = g_magick_download_static;
//		Logger::print(Logger::type::T_DEBUG, "On linux ImageMagick has static link to download AppImage: " + m_dl.m_magick);
//	}
//#endif
//	Logger::print(Logger::type::T_DEBUG, "Ended ImageMagick link related stuff.");
//
//	return true;
//}
//
//bool PathingStuff::create_dirs(const std::string& path) const
//{
//	std::error_code err;
//	std::filesystem::create_directories(path, err);
//	return !err;
//}
//
//std::string PathingStuff::get_folder_with_name_in_dir(const std::string& dir, const std::string& substr) const
//{
//	const auto folders_there = get_folders(m_base_path);
//	for (auto& i : folders_there) {
//		const size_t p = i.rfind(SLASH);
//		const size_t f = i.rfind(substr);
//		if (f != std::string::npos && f >= p) return i;
//	}
//	return "";
//}
//
//void PathingStuff::call_7z_extract(const std::string& zip) const
//{
////	const auto expected_cmd = (zip.rfind(".zip") == zip.length() - 4) ? get_7za_exe() : get_7z_exe();
////	const auto file_path = m_base_path + zip; // downloaded file
////
////	Logger::print(Logger::type::T_DEBUG, "[PS>7z] Extracting '" + zip + "' with '" + expected_cmd + "'...");
////
////	Lunaris::process_sync proc(
////		expected_cmd, { 
////			"x",
////			file_path,
////			"-o\"" + m_base_path + "\""
////		}, Lunaris::process_sync::mode::READ
////	);
////
////#ifdef _DEBUG
////	std::this_thread::sleep_for(std::chrono::milliseconds(200));
////
////	if (proc.has_read()) {
////		while (proc.has_read()) {
////			Logger::print(Logger::type::T_DEBUG, "[PS>7z>Proc] " +  proc.read());
////			std::this_thread::sleep_for(std::chrono::milliseconds(20));
////		}
////	}
////#endif
////
////	while (proc.is_running()) std::this_thread::sleep_for(std::chrono::milliseconds(50));
////
////	Logger::print(Logger::type::T_DEBUG, "[PS>7z] Ended extracting '" + zip + "'.");
//}
//
//PathingStuff::PathingStuff()
//	: m_base_path(get_app_path())
//{
//}
//
//bool PathingStuff::self_check_install()
//{
//	create_dirs(m_base_path);
//
//	return 
//		fetch_all_versions_and_links() && 
//		download_7z(); // && 
//		//download_ffmpeg() && 
//		//download_magick();
//}
//
//std::string PathingStuff::get_ffmpeg_exe() const
//{
//	return m_base_path;// + "ffmpeg-" + m_ffmpeg_latest_version + "-full_build" SLASH "bin" SLASH "ffmpeg.exe";
//}
//
//std::string PathingStuff::get_magick_exe() const
//{
//	return m_base_path;// + "ImageMagick-" + m_magick_latest_version + "-portable-Q16-x64" SLASH "magick.exe";
//}
//
//#ifdef _WIN32
//std::string PathingStuff::get_7z_exe() const
//{
//	return m_base_path + g_7z_portable_file; // 7z exe path
//}
//std::string PathingStuff::get_7za_exe() const
//{
//	return m_base_path + g_7za_portable_file; // 7za exe path
//}
//#else // linux
//std::string PathingStuff::get_7z_exe() const
//{
//	return m_base_path + g_7zz_portable_file; // 7z exe path
//}
//#endif
//
//std::string get_app_path()
//{
//#ifdef _WIN32
//    const char* env = "APPDATA";
//#else
//    const char* env = "HOME";
//#endif
//	std::string res = getenv(env);
//	while (res.length() > 0 && res.back() == '\0') res.pop_back();
//#ifdef _WIN32
//    return res + "\\VideoAndPhotoConverter\\";
//#elif defined __APPLE__
//    return res + "/Library/Caches/VideoAndPhotoConverter/";
//#else
//    return res + "/.videoandphotoconverter/";
//#endif
//}
//
//std::vector<std::string> get_folders(const std::string& path)
//{
//	std::vector<std::string> vec;
//	for (const auto& i : std::filesystem::directory_iterator(path))
//	{
//		if (i.is_directory()) vec.push_back(std::filesystem::canonical(i.path()).string());
//	}
//	return vec;
//}