#pragma once

#include <cstdint>
#include <memory>

#define BIT(x) (1 << x)

namespace Chess {

	using i8 = std::int8_t;
	using u8 = std::uint8_t;
	using i8fast = std::int_fast8_t;
	using u8fast = std::uint_fast8_t;
	using i8small = std::int_least8_t;
	using u8small = std::uint_least8_t;

	using i16 = std::int16_t;
	using u16 = std::uint16_t;
	using i16fast = std::int_fast16_t;
	using u16fast = std::uint_fast16_t;
	using i16small = std::int_least16_t;
	using u16small = std::uint_least16_t;

	using i32 = std::int32_t;
	using u32 = std::uint32_t;
	using i32fast = std::int_fast32_t;
	using u32fast = std::uint_fast32_t;
	using i32small = std::int_least32_t;
	using u32small = std::uint_least32_t;

	using i64 = std::int64_t;
	using u64 = std::uint64_t;
	using i64fast = std::int_fast64_t;
	using u64fast = std::uint_fast64_t;
	using i64small = std::int_least64_t;
	using u64small = std::uint_least64_t;

	using f32 = float;
	using f64 = double;

	using byte = u8;

	template<typename T>
	using Scope = std::unique_ptr<T>;
	template<typename T, typename ... Args>
	constexpr Scope<T> CreateScope(Args&& ... args)
	{
		return std::make_unique<T>(std::forward<Args>(args)...);
	}

	template<typename T>
	using Ref = std::shared_ptr<T>;
	template<typename T, typename ... Args>
	constexpr Ref<T> CreateRef(Args&& ... args)
	{
		return std::make_shared<T>(std::forward<Args>(args)...);
	}

}
