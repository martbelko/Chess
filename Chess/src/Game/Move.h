#pragma once

#include <WebgpuLib.h>

#include "Core/Base.h"

#include <glm/glm.hpp>

#include <string>

namespace Chess {

	class Position
	{
	public:
		constexpr Position(u8 x = 0, u8 y = 0)
		{
			m_Data = (x << 4) | y;
		}

		constexpr u8 GetX() const { return m_Data >> 4; }
		constexpr u8 GetY() const { return (m_Data & 0x0F); }
		constexpr std::pair<u8, u8> Get() const { return std::make_pair(GetX(), GetY()); }

		bool IsWhite() const { return (GetX() + GetY()) % 2 == 1; }
		bool IsValid() const { return *this != InvalidPosition; }

		std::string ToString() const
		{
			char ch = GetX() + 'a';
			return ch + std::to_string(GetY() + 1);
		}

		constexpr bool operator==(const Position& other) const { return m_Data == other.m_Data; }
		constexpr bool operator!=(const Position& other) const { return m_Data != other.m_Data; }
	public:
		static constexpr Position StringToPosition(const std::string& pos)
		{
			ASSERT(pos.length() == 2, "String must contain exactly 2 characters, e.g. c5");
			u8 x = pos[0] - 'a';
			u8 y = pos[1] - '1';
			return Position(x, y);
		}
	public:
		static const Position InvalidPosition;
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
