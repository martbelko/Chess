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

	// 3 bits for PieceType, 1 bit for PieceColor, 6 bits for position, 1 bit active or not, 3 bits for index of the same piece (0-7), 1 bit for if first move = 15 bits
	class Piece
	{
	public:
		constexpr Piece(u8 x, u8 y, PieceType pieceType, PieceColor color, u8 pieceIndex)
			: m_Data((x << 13) | (y << 10) | (static_cast<u8>(pieceType) << 7) | (static_cast<u8>(color) << 6) | 1u << 5 | (pieceIndex << 1)) {}

		constexpr Position GetPosition() const
		{
			u8 x = m_Data >> 13;
			u8 y = (m_Data & Y_MASK) >> 10;
			return Position(x, y);
		}

		constexpr PieceType GetPieceType() const { return static_cast<PieceType>(static_cast<u8>((m_Data & PIECE_MASK) >> 7)); }
		constexpr PieceColor GetPieceColor() const { return static_cast<PieceColor>(static_cast<u8>(m_Data & COLOR_MASK) >> 6); }
		constexpr bool IsActive() const { return static_cast<bool>((m_Data & ACTIVE_MASK) >> 5); }
		constexpr u8 GetIndex() const { return static_cast<u8>((m_Data & INDEX_MASK) >> 2); }
		constexpr bool HasMoved() const { return static_cast<bool>((m_Data & FIRST_MOVE_MASK) >> 1); }

		constexpr bool operator==(const Piece& other) const { return m_Data == other.m_Data; }
		constexpr bool operator!=(const Piece& other) const { return m_Data == other.m_Data; }
	private:
		u16 m_Data;
	private:
		static inline constexpr u16 X_MASK =          0b1110'0000'0000'0000;
		static inline constexpr u16 Y_MASK =          0b0001'1100'0000'0000;
		static inline constexpr u16 PIECE_MASK =      0b0000'0011'1000'0000;
		static inline constexpr u16 COLOR_MASK =      0b0000'0000'0100'0000;
		static inline constexpr u16 ACTIVE_MASK =     0b0000'0000'0010'0000;
		static inline constexpr u16 INDEX_MASK =      0b0000'0000'0001'1100;
		static inline constexpr u16 FIRST_MOVE_MASK = 0b0000'0000'0000'0010;
	};

	class ChessboardState
	{
	public:
		constexpr void AddPiece(const Piece& activePiece)
		{
			ASSERT(std::find(m_Pieces.begin(), m_Pieces.end(), activePiece) == m_Pieces.end(), "Chessboard already contains identical piece");
			m_Pieces.push_back(activePiece);
		}

		constexpr void RemovePieceAtPosition(Position position)
		{
			const Piece* piece = GetPieceAtPosition(position);
			if (piece == nullptr)
			{
				ASSERT(false, "Attempting to remove piece at position, where there is no active piece");
				return;
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

		constexpr const std::vector<Piece>& GetPieces() const { return m_Pieces; }
	private:
		std::vector<Piece> m_Pieces;
	};

}
