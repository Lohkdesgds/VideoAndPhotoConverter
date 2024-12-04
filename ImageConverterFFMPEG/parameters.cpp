#include "parameters.h"

#include <Lunaris/console/console.h>

#include <algorithm>

const std::string NVENC_parameters::m_preset_values[] = {
	"default", "slow", "medium", "fast", "hp", "hq", "bd",
	"ll", "llhq", "llhp", "lossless", "losslesshp", "p1",
	"p2", "p3", "p4", "p5", "p6", "p7"
};

const std::string NVENC_parameters::m_tune_values[] = {
	"hq", "ll", "ull", "lossless"
};

const std::string NVENC_parameters::m_profile_values[] = {
	"main", "main10", "rext"
};


const std::string x264_parameters::m_preset_values[] = {
	"veryslow", "slower", "slow", "medium", "fast",
	"faster", "veryfast", "superfast", "ultrafast"
};

bool atob(const char* s) { return s && (s[0] == 't' || s[0] == 'T' || s[0] == '1'); }

Parameters& Parameters::set_cqp(const int v)
{
	if (v < 0 || v > 100) m_cqp.reset();
	else m_cqp = v;
	return *this;
}

bool Parameters::set(const char* nam, const char* val)
{
	if (strcmp(nam, "cqp") == 0) { set_cqp(std::atoi(val)); return true; }

	return false;
}


VideoParameters& VideoParameters::set_mix(const bool v)
{
	m_mix = v;
	return *this;
}

VideoParameters& VideoParameters::set_audio_bitrate(const int v)
{
	if (v <= 0 || v > 4096) m_audio_bitrate.reset();
	else m_audio_bitrate = v;
	return *this;
}

VideoParameters& VideoParameters::set_scale(const float v)
{
	if (v <= 0.0f || v >= 1.0f) m_scale.reset();
	else m_scale = v;
	return *this;
}

VideoParameters& VideoParameters::set_maxres(const int v)
{
	if (v <= 1) m_scale.reset();
	else m_scale = static_cast<float>(v);
	return *this;
}

bool VideoParameters::set(const char* nam, const char* val)
{
	if (strcmp(nam, "cqp") == 0) { set_cqp(std::atoi(val)); return true; }

	if (strcmp(nam, "mix") == 0) { set_mix(atob(val)); return true; }

	if (strcmp(nam, "ab") == 0) { set_audio_bitrate(std::atoi(val)); return true; }
	if (strcmp(nam, "audiobitrate") == 0) { set_audio_bitrate(std::atoi(val)); return true; }
	if (strcmp(nam, "audio_bitrate") == 0) { set_audio_bitrate(std::atoi(val)); return true; }

	if (strcmp(nam, "scale") == 0) { set_scale(static_cast<float>(std::atof(val))); return true; }

	if (strcmp(nam, "maxres") == 0) { set_maxres(std::atoi(val)); return true; }

	return false;
}



std::vector<std::string> NVENC_parameters::to_props(const std::string& input) const
{
	std::vector<std::string> vec;

	vec.push_back("-hide_banner");
	vec.push_back("-loglevel");			vec.push_back("error");
	vec.push_back("-y");
	vec.push_back("-i");
	vec.push_back("\"" + input + "\"");

	//vec.push_back("-hwaccel");					vec.push_back("cuda");
	//vec.push_back("-hwaccel_output_format");	vec.push_back("cuda");

	vec.push_back("-vcodec");					vec.push_back("hevc_nvenc");
	vec.push_back("-vsync");					vec.push_back("0");

	if (m_mix) {
		vec.push_back("-vol");
		vec.push_back("256");
		vec.push_back("-af");
		vec.push_back("^\"pan=stereo^|c0=0.5^*c2^+0.707^*c0^+0.707^*c4^+0.5^*c3^|c1=0.5^*c2^+0.707^*c1^+0.707^*c5^+0.5^*c3^\"");
	}

	if (m_audio_bitrate.has_value())	{ vec.push_back("-ab");			vec.push_back(std::to_string(m_audio_bitrate.value()) + "k"); }
	if (m_preset.has_value())			{ vec.push_back("-preset");		vec.push_back(m_preset.value()); }
	if (m_tune.has_value())				{ vec.push_back("-tune");		vec.push_back(m_tune.value()); }
	if (m_profile.has_value())			{ vec.push_back("-profile");	vec.push_back(m_profile.value()); }
	if (m_cqp.has_value())				{ vec.push_back("-qp");			vec.push_back(std::to_string(m_cqp.value())); }

	vec.push_back("-rc");				vec.push_back("constqp");
	vec.push_back("-rc-lookahead");		vec.push_back("40");
	vec.push_back("-2pass");			vec.push_back("1");
	vec.push_back("-gpu");				vec.push_back("any");
	vec.push_back("-spatial-aq");		vec.push_back("1");
	vec.push_back("-temporal-aq");		vec.push_back("1");
	vec.push_back("-aq-strength");		vec.push_back("15");
	vec.push_back("-multipass");		vec.push_back("fullres");

	if (m_scale.has_value()) { 
		const float v = m_scale.value();

		vec.push_back("-filter:v");
		char buf[64]{};

		if (v < 1.0f) {
			snprintf(buf, std::size(buf), "\"scale=in_w*%.3f:in_h*%.3f\"", v, v);
		}
		else {
			const int c = static_cast<int>(v);
			snprintf(buf, std::size(buf), "\"scale=h='if(gt(iw\\,ih)\\,%d\\,-2)':w='if(gt(iw\\,ih)\\,-2\\,%d)\"", c, c);
		}

		vec.push_back(buf);
	}

	return vec;
}

std::vector<std::string> NVENC_parameters::to_pretty_lines() const
{
	std::vector<std::string> vec;

	vec.push_back("Video codec: hevc_nvenc");
	//vec.push_back("Hardware accel: cuda");

	if (m_mix)	vec.push_back("[mix] Audio config: downsample 5.1");
	else		vec.push_back("[mix] Audio config: original");

	if (m_audio_bitrate.has_value())	vec.push_back("[ab] Audio bitrate: " + std::to_string(m_audio_bitrate.value()) + "k");
	else								vec.push_back("[ab] Audio bitrate: undefined");

	if (m_preset.has_value())			vec.push_back("[preset] Preset: " + m_preset.value());
	else								vec.push_back("[preset] Preset: undefined");

	if (m_tune.has_value())				vec.push_back("[tune] Tune: " + m_tune.value());
	else								vec.push_back("[tune] Tune: undefined");

	if (m_profile.has_value())			vec.push_back("[profile] Profile: " + m_profile.value());
	else								vec.push_back("[profile] Profile: undefined");

	if (m_cqp.has_value())				vec.push_back("[cqp] Constant of quality: " + std::to_string(m_cqp.value()));
	else								vec.push_back("[cqp] Constant of quality: undefined");

	if (m_scale.has_value()) {
		const float v = m_scale.value();

		char buf[64]{};

		if (v < 1.0f) {
			snprintf(buf, std::size(buf), "[scale] Scale: %.3f", v);
		}
		else {
			const int c = static_cast<int>(v);
			snprintf(buf, std::size(buf), "[maxres] Shorter side length: %d pixel(s)", c);
		}

		vec.push_back(buf);
	}
	else vec.push_back("[scale|maxres] Scale/Shorter side length: undefined");

	return vec;
}

NVENC_parameters& NVENC_parameters::set_preset(const std::string& v)
{
	if (std::find(std::begin(m_preset_values), std::end(m_preset_values), v) == std::end(m_preset_values)) m_preset.reset();
	else m_preset = v;
	return *this;
}

NVENC_parameters& NVENC_parameters::set_tune(const std::string& v)
{
	if (std::find(std::begin(m_tune_values), std::end(m_tune_values), v) == std::end(m_tune_values)) m_tune.reset();
	else m_tune = v;
	return *this;
}

NVENC_parameters& NVENC_parameters::set_profile(const std::string& v)
{
	if (std::find(std::begin(m_profile_values), std::end(m_profile_values), v) == std::end(m_profile_values)) m_profile.reset();
	else m_profile = v;
	return *this;
}

NVENC_parameters& NVENC_parameters::set_cqp(const int v)
{
	if (v < 0 || v > 51) m_cqp.reset();
	else m_cqp = v;
	return *this;
}

NVENC_parameters& NVENC_parameters::set_mix(const bool v) { this->VideoParameters::set_mix(v); return *this; }
NVENC_parameters& NVENC_parameters::set_audio_bitrate(const int v) { this->VideoParameters::set_audio_bitrate(v); return *this; }
NVENC_parameters& NVENC_parameters::set_scale(const float v) { this->VideoParameters::set_scale(v); return *this; }
NVENC_parameters& NVENC_parameters::set_maxres(const int v) { this->VideoParameters::set_maxres(v); return *this; }

bool NVENC_parameters::set(const char* nam, const char* val)
{
	if (strcmp(nam, "cqp") == 0) { set_cqp(std::atoi(val)); return true; }

	if (strcmp(nam, "mix") == 0) { set_mix(atob(val)); return true; }

	if (strcmp(nam, "ab") == 0) { set_audio_bitrate(std::atoi(val)); return true; }
	if (strcmp(nam, "audiobitrate") == 0) { set_audio_bitrate(std::atoi(val)); return true; }
	if (strcmp(nam, "audio_bitrate") == 0) { set_audio_bitrate(std::atoi(val)); return true; }

	if (strcmp(nam, "scale") == 0) { set_scale(static_cast<float>(std::atof(val))); return true; }

	if (strcmp(nam, "maxres") == 0) { set_maxres(std::atoi(val)); return true; }

	if (strcmp(nam, "preset") == 0) { set_preset(val); return true; }

	if (strcmp(nam, "tune") == 0) { set_tune(val); return true; }

	if (strcmp(nam, "profile") == 0) { set_profile(val); return true; }

	return false;
}



std::vector<std::string> x264_parameters::to_props(const std::string& input) const
{
	std::vector<std::string> vec;

	vec.push_back("-hide_banner");
	vec.push_back("-loglevel");			vec.push_back("error");
	vec.push_back("-y");
	vec.push_back("-i");
	vec.push_back("\"" + input + "\"");

	vec.push_back("-vcodec");			vec.push_back("libx264");
	vec.push_back("-vsync");			vec.push_back("0");

	if (m_mix) {
		vec.push_back("-vol");
		vec.push_back("256");
		vec.push_back("-af");
		vec.push_back("^\"pan=stereo^|c0=0.5^*c2^+0.707^*c0^+0.707^*c4^+0.5^*c3^|c1=0.5^*c2^+0.707^*c1^+0.707^*c5^+0.5^*c3^\"");
	}

	if (m_audio_bitrate.has_value())	{ vec.push_back("-ab");			vec.push_back(std::to_string(m_audio_bitrate.value()) + "k"); }
	if (m_preset.has_value())			{ vec.push_back("-preset");		vec.push_back(m_preset.value()); }
	if (m_cqp.has_value())				{ vec.push_back("-crf");		vec.push_back(std::to_string(m_cqp.value())); }

	vec.push_back("-g");				vec.push_back("120");
	vec.push_back("-keyint_min");		vec.push_back("40");
	vec.push_back("-bf");				vec.push_back("4");
	vec.push_back("-qcomp");			vec.push_back("0.8");
	vec.push_back("-rc-lookahead");		vec.push_back("80");

	if (m_scale.has_value()) {
		const float v = m_scale.value();

		vec.push_back("-filter:v");
		char buf[64]{};

		if (v < 1.0f) {
			snprintf(buf, std::size(buf), "\"scale=in_w*%.3f:in_h*%.3f\"", v, v);
		}
		else {
			const int c = static_cast<int>(v);
			snprintf(buf, std::size(buf), "\"scale=h='if(gt(iw\\,ih)\\,%d\\,-2)':w='if(gt(iw\\,ih)\\,-2\\,%d)\"", c, c);
		}

		vec.push_back(buf);
	}

	return vec;
}

std::vector<std::string> x264_parameters::to_pretty_lines() const
{
	std::vector<std::string> vec;

	vec.push_back("Video codec: libx264");

	if (m_mix)	vec.push_back("[mix] Audio config: downsample 5.1");
	else		vec.push_back("[mix] Audio config: original");
	
	if (m_audio_bitrate.has_value())	vec.push_back("[ab] Audio bitrate: " + std::to_string(m_audio_bitrate.value()) + "k");
	else								vec.push_back("[ab] Audio bitrate: undefined");

	if (m_preset.has_value())			vec.push_back("[preset] Preset: " + m_preset.value());
	else								vec.push_back("[preset] Preset: undefined");

	if (m_cqp.has_value())				vec.push_back("[cqp] Constant of quality: " + std::to_string(m_cqp.value()));
	else								vec.push_back("[cqp] Constant of quality: undefined");

	if (m_scale.has_value()) {
		const float v = m_scale.value();

		char buf[64]{};

		if (v < 1.0f) {
			snprintf(buf, std::size(buf), "[scale] Scale: %.3f", v);
		}
		else {
			const int c = static_cast<int>(v);
			snprintf(buf, std::size(buf), "[maxres] Shorter side length: %d pixel(s)", c);
		}

		vec.push_back(buf);
	}
	else vec.push_back("[scale|maxres] Scale/Shorter side length: undefined");

	return vec;
}

x264_parameters& x264_parameters::set_preset(const std::string& v)
{
	if (std::find(std::begin(m_preset_values), std::end(m_preset_values), v) == std::end(m_preset_values)) m_preset.reset();
	else m_preset = v;
	return *this;
}

x264_parameters& x264_parameters::set_cqp(const int v)
{
	if (v < 15 || v > 50) m_cqp.reset();
	else m_cqp = v;
	return *this;
}

x264_parameters& x264_parameters::set_mix(const bool v) { this->VideoParameters::set_mix(v); return *this; }
x264_parameters& x264_parameters::set_audio_bitrate(const int v) { this->VideoParameters::set_audio_bitrate(v); return *this; }
x264_parameters& x264_parameters::set_scale(const float v) { this->VideoParameters::set_scale(v); return *this; }
x264_parameters& x264_parameters::set_maxres(const int v) { this->VideoParameters::set_maxres(v); return *this; }

bool x264_parameters::set(const char* nam, const char* val)
{
	if (strcmp(nam, "cqp") == 0) { set_cqp(std::atoi(val)); return true; }

	if (strcmp(nam, "mix") == 0) { set_mix(atob(val)); return true; }

	if (strcmp(nam, "ab") == 0) { set_audio_bitrate(std::atoi(val)); return true; }
	if (strcmp(nam, "audiobitrate") == 0) { set_audio_bitrate(std::atoi(val)); return true; }
	if (strcmp(nam, "audio_bitrate") == 0) { set_audio_bitrate(std::atoi(val)); return true; }

	if (strcmp(nam, "scale") == 0) { set_scale(static_cast<float>(std::atof(val))); return true; }

	if (strcmp(nam, "maxres") == 0) { set_maxres(std::atoi(val)); return true; }

	if (strcmp(nam, "preset") == 0) { set_preset(val); return true; }

	return false;
}

JPEG_parameters::JPEG_parameters()
{
	set_cqp(40);
}

std::vector<std::string> JPEG_parameters::to_props(const std::string& input) const
{
	std::vector<std::string> vec;

	vec.push_back("\"" + input + "\"");
	
	if (m_cqp.has_value()) { vec.push_back("-quality");		vec.push_back(std::to_string(m_cqp.value()) + "%"); }

	return vec;
}

std::vector<std::string> JPEG_parameters::to_pretty_lines() const
{
	std::vector<std::string> vec;

	if (m_cqp.has_value())	vec.push_back("[cqp] Quality: " + std::to_string(m_cqp.value()) + "%");
	else					vec.push_back("[cqp] Quality: undefined");

	return vec;
}

JPEG_parameters& JPEG_parameters::set_cqp(const int v)
{
	if (v < 1 || v > 100) m_cqp.reset();
	else m_cqp = v;
	return *this;
}

bool JPEG_parameters::set(const char* nam, const char* val)
{
	if (strcmp(nam, "cqp") == 0) { set_cqp(std::atoi(val)); return true; }

	return false;
}
