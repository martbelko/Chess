#pragma once

#include "WebgpuLib.h"

#include "Core/Base.h"

#include "Game/Move.h"

#include <vector>

namespace Chess {

	enum class PieceType : u8 // 3 bits
	{
		Pawn = 0, Rook, Knight, Bishop, Queen, King
	};

	enum class PieceColor : u8 // 1 bit
	{
		White = 0, Black = 1
	};

	// 3 bits for PieceType, 1 bit for PieceColor, 6 bits for position, 1 bit active or not = 11 bits
	class Piece
	{
	public:
		constexpr Piece(u8 x, u8 y, PieceType pieceType, PieceColor color, bool active = true)
			: m_Data((x << 8) | (y << 5) | (static_cast<u8>(pieceType) << 2) | (static_cast<u8>(color) << 1) | static_cast<u8>(active)) {}

		constexpr Position GetPosition() const
		{
			u8 x = m_Data >> 8;
			u8 y = (m_Data & Y_MASK) >> 5;
			return Position(x, y);
		}

		constexpr PieceType GetPieceType() const { return static_cast<PieceType>(static_cast<u8>((m_Data & PIECE_MASK) >> 2)); }
		constexpr PieceColor GetPieceColor() const { return static_cast<PieceColor>(static_cast<u8>(m_Data & COLOR_MASK) >> 1); }
		constexpr bool IsActive() const { return static_cast<bool>(m_Data & ACTIVE_MASK); }

		constexpr bool operator==(const Piece& other) const { return m_Data == other.m_Data; }
		constexpr bool operator!=(const Piece& other) const { return m_Data != other.m_Data; }
	private:
		u16 m_Data;
	private:
		static inline constexpr u16 X_MASK = 0b00000'1'11'0000'0000;
		static inline constexpr u16 Y_MASK = 0b00000'000'1110'0000;
		static inline constexpr u16 PIECE_MASK = 0b00000'000'0001'1100;
		static inline constexpr u16 COLOR_MASK = 0b00000'000'0000'0010;
		static inline constexpr u16 ACTIVE_MASK = 0b00000'000'0000'0001;
	};

	class ChessboardState
	{
	public:
		constexpr void AddPiece(const Piece& activePiece)
		{
			ASSERT(std::find(m_Pieces.begin(), m_Pieces.end(), activePiece) == m_Pieces.end(), "Chessboard already contains identical piece");
			Piece.push_back(activePiece);
		}

		constexpr void RemovePieceAtPosition(Position position)
		{
			const Piece* piece = GetPieceAtPosition(position);
			if (piece != nullptr)
			{
				ASSERT(false, "Attempting to remove piece at position, where there is no active piece");
			}

			auto it = std::find(m_Pieces.begin(), m_Pieces.end(), *piece);
			m_Pieces.erase(it);
		}

		constexpr const Piece* GetPieceAtPosition(Position position) const
		{
			for (const Piece& piece : m_Pieces)
			{
				if (piece.GetPosition() == position)
				{
					return &piece;
				}
			}

			return nullptr;
		}
	private:
		std::vector<Piece> m_Pieces;
	};

}
