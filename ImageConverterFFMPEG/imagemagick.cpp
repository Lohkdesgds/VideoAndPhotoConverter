#include "imagemagick.h"

#include <Lunaris/process/process.h>


MAGICK::MAGICK(const PathingStuff& pathing)
	: m_pathing(pathing)
{
}

void MAGICK::call(const std::initializer_list<std::string>& cmds) const
{
	Lunaris::process_sync proc(m_pathing.get_magick_exe(), cmds, Lunaris::process_sync::mode::READ);
	while (proc.is_running()) std::this_thread::sleep_for(std::chrono::milliseconds(20));
}

void MAGICK::call(const std::initializer_list<std::string>& cmds, const std::function<void(const std::string&)>& out) const
{
	Lunaris::process_sync proc(m_pathing.get_magick_exe(), cmds, Lunaris::process_sync::mode::READ);

	while (proc.is_running() || proc.has_read()) {
		while (proc.has_read()) {
			const auto did_read = proc.read();
			out(did_read);

			if (!proc.has_read()) std::this_thread::sleep_for(std::chrono::milliseconds(10));
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}
}