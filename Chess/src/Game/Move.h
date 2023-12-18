#pragma once

#include "Core/Base.h"

#include <glm/glm.hpp>

namespace Chess {

	class Position
	{
	public:
		Position(u8 x = 0, u8 y = 0)
		{
			m_Data = (x << 4) | y;
		}

		u8 GetX() const { return m_Data >> 4; }
		u8 GetY() const { return (m_Data & 0x0F); }
		std::pair<u8, u8> Get() const { return std::make_pair(GetX(), GetY()); }
	private:
		u8 m_Data;
	};

	class Move
	{
	public:
		Move(Position from, Position to)
			: m_From(from), m_To(to) {}
	private:
		Position m_From;
		Position m_To;
	};

}
