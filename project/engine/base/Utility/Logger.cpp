#include "Logger.h"
#include "externals/DirectXTex/DirectXTex.h"

namespace MyEngine {
	namespace Logger {
		void Log(const std::string& message) {
			OutputDebugStringA(message.c_str());;
		}
	}
}