#pragma once

#include "Core/Base.h"

namespace Chess {

	class UUID
	{
	public:
		UUID()
			: m_UUID(s_NextId++) {}
		UUID(uint64_t uuid)
			: m_UUID(uuid) {}
		UUID(UUID&) = default;

		operator u64() const { return m_UUID; }
	private:
		u64 m_UUID;
	private:
		static inline u64 s_NextId = 0;
	};

}

namespace std {

	template<typename T>
	struct hash;

	template<>
	struct hash<Chess::UUID>
	{
		std::size_t operator()(const Chess::UUID& uuid) const
		{
			return static_cast<Chess::u64>(uuid);
		}
	};

}