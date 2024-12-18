#pragma once

#include "ffmpeg.h"

#include <string>
#include <optional>
#include <vector>

class Parameters {
protected:
	std::optional<int> m_cqp{ 37 };
public:
	virtual std::vector<std::string> to_props(const std::string&) const = 0;
	virtual std::vector<std::string> to_pretty_lines() const = 0;

	Parameters& set_cqp(const int);

	virtual bool set(const char*, const char*);
};

class VideoParameters : public Parameters {
protected:
	std::optional<int> m_audio_bitrate{ 192 };
	std::optional<float> m_scale{ 720.0f }; // < 1.0f == scale, > 1.0f == max res limit
	bool m_mix{ false }; // 5.1
public:
	virtual std::vector<std::string> to_props(const std::string&) const = 0;
	virtual std::vector<std::string> to_pretty_lines() const = 0;

	VideoParameters& set_mix(const bool);
	VideoParameters& set_audio_bitrate(const int);
	VideoParameters& set_scale(const float);
	VideoParameters& set_maxres(const int);

	virtual bool set(const char*, const char*);
};

// h265
class NVENC_parameters : public VideoParameters {
public:
	static const std::string m_preset_values[19];
	static const std::string m_tune_values[4];
	static const std::string m_profile_values[3];
private:
	std::optional<std::string> m_preset{ "slow" };
	std::optional<std::string> m_tune{ "hq" };
	std::optional<std::string> m_profile{ "main" };
public:
	virtual std::vector<std::string> to_props(const std::string&) const;
	virtual std::vector<std::string> to_pretty_lines() const;

	NVENC_parameters& set_preset(const std::string&);
	NVENC_parameters& set_tune(const std::string&);
	NVENC_parameters& set_profile(const std::string&);
	// [0..51]
	NVENC_parameters& set_cqp(const int);

	// replaced so return type is correct.

	NVENC_parameters& set_mix(const bool);
	NVENC_parameters& set_audio_bitrate(const int);
	NVENC_parameters& set_scale(const float);
	NVENC_parameters& set_maxres(const int);

	virtual bool set(const char*, const char*);
};

class x264_parameters : public VideoParameters {
public:
	static const std::string m_preset_values[9];
private:
	std::optional<std::string> m_preset{ "veryslow" };
public:
	virtual std::vector<std::string> to_props(const std::string&) const;
	virtual std::vector<std::string> to_pretty_lines() const;

	x264_parameters& set_preset(const std::string&);
	// [15..50]
	x264_parameters& set_cqp(const int);

	// replaced so return type is correct.

	x264_parameters& set_mix(const bool);
	x264_parameters& set_audio_bitrate(const int);
	x264_parameters& set_scale(const float);
	x264_parameters& set_maxres(const int);

	virtual bool set(const char*, const char*);
};

// call with magick
class JPEG_parameters : public Parameters {
public:
	JPEG_parameters();

	virtual std::vector<std::string> to_props(const std::string&) const;
	virtual std::vector<std::string> to_pretty_lines() const;

	// [2..31]
	JPEG_parameters& set_cqp(const int);

	virtual bool set(const char*, const char*);
};