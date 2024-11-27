#include "ffmpeg.h"
#include <Lunaris/console/console.h>

using namespace Lunaris;

int main()
{
	cout << console::color::GREEN << "Starting...";

	FFMPEG ffmpeg;
	ffmpeg.check_installation();
	ffmpeg.install();

	/* 
	Steps:
	1. Check if FFMPEG exists in appdata
	2. Download if it doesn't exist
	3. Extract FFMPEG
	4. Get list from argument list OR scan every folder and subfolder
	5. Settings
	6. Work
	*/
}