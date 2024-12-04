#pragma once

#include "ffmpeg.h"
#include "imagemagick.h"
#include "parameters.h"

#include <string>
#include <memory>

extern const std::string image_ext[], video_ext[];
extern const std::string image_final_format;
extern const std::string video_final_format;

bool is_image(const std::string& file_name);
bool is_video(const std::string& file_name);

class File {
protected:
	const std::shared_ptr<std::unique_ptr<Parameters>> m_parameters;
	const std::string m_path, m_ext;
public:
	File(const std::string&, const std::shared_ptr<std::unique_ptr<Parameters>>&, const std::string&);

	virtual void convert() const = 0;

	bool move_to_trash() const;

	const std::string& get_path() const;
	std::string get_trash_path() const;

	virtual bool probably_a_converted_file() const = 0;

	virtual bool is_video() const = 0;
	virtual bool is_image() const = 0;
};

class VideoFile : public File {
	const FFMPEG& m_ffmpeg;
public:
	VideoFile(const std::string&, const FFMPEG&, const std::shared_ptr<std::unique_ptr<Parameters>>&);

	virtual void convert() const;

	virtual bool probably_a_converted_file() const;

	virtual bool is_video() const;
	virtual bool is_image() const;
};

class ImageFile : public File {
	const MAGICK& m_magick;
public:
	ImageFile(const std::string&, const MAGICK&, const std::shared_ptr<std::unique_ptr<Parameters>>&);

	virtual void convert() const;

	virtual bool probably_a_converted_file() const;

	virtual bool is_video() const;
	virtual bool is_image() const;
};

std::shared_ptr<File> make_file_auto(const std::string&,
	const FFMPEG&, const MAGICK&,
	std::shared_ptr<std::unique_ptr<Parameters>>, std::shared_ptr<std::unique_ptr<Parameters>>);