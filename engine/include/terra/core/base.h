#pragma once

#include <cstdint>

#include "terra/core/platform_detection.h"
#include "terra/core/assert.h"


#ifdef TR_DEBUG
	#if defined(TR_PLATFORM_WINDOWS)
		#define TR_DEBUGBREAK() __debugbreak()
	#elif defined(TR_PLATFORM_LINUX)
		#include <signal.h>
		#define TR_DEBUGBREAK() raise(SIGTRAP)
	#elif defined(TR_PLATFORM_MACOS)
        #include <signal.h>
        #define TR_DEBUGBREAK() raise(SIGTRAP)
	#endif

	// #define TR_ENABLE_ASSERTS
#else
	#define TR_DEBUGBREAK()
#endif

namespace terra {

using i8  = std::int8_t;
using i16 = std::int16_t;
using i32 = std::int32_t;
using i64 = std::int64_t;

using u8  = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using u64 = std::uint64_t;

using f32 = float;
using f64 = double;

}
