#pragma once

#include <string>
#include <filesystem>
#include <optional>

namespace Base {

	class Filesystem
	{
	public:
		static std::optional<std::string> ReadFile(const std::filesystem::path& relativeFilepath);
	private:
		static void Init();
	private:
		inline static std::filesystem::path s_AssetsPath;
	private:
		friend class Application;
	};



}
