#include "Log.h"

namespace Base {

	std::shared_ptr<Logger> Log::s_Logger;

	void Log::Init()
	{
		s_Logger = std::make_shared<Logger>("LOG");
	}

}
