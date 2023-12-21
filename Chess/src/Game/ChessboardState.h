#pragma once

#include "WebgpuLib.h"

#include "Core/Base.h"

#include "Game/Move.h"

#include <vector>

namespace Chess {

	enum class PieceType : u8 // 3 bits
	{
		None = 0, Pawn, Rook, Knight, Bishop, Queen, King
	};

	enum class PieceColor : u8 // 1 bit
	{
		White = 0, Black = 1
	};

	// 3 bits for PieceType, 1 bit for PieceColor, 6 bits for position, 1 bit active or not, 3 bits for index of the same piece (0-7), 1 bit for if first move = 15 bits
	class Piece
	{
	public:
		Piece(u8 x, u8 y)
			: Piece(x, y, PieceType::None, PieceColor::White, 0) {}
		Piece(u8 x, u8 y, PieceType pieceType, PieceColor color, u8 pieceIndex)
			: m_Data((x << 13) | (y << 10) | (static_cast<u8>(pieceType) << 7) | (static_cast<u8>(color) << 6) | (pieceIndex << 2)) {}

		Position GetPosition() const
		{
			u8 x = m_Data >> 13;
			u8 y = (m_Data & Y_MASK) >> 10;
			return Position(x, y);
		}

		bool IsValid() const { return GetPieceType() != PieceType::None; }

		PieceType GetPieceType() const { return static_cast<PieceType>(static_cast<u8>((m_Data & PIECE_MASK) >> 7)); }
		PieceColor GetPieceColor() const { return static_cast<PieceColor>(static_cast<u8>(m_Data & COLOR_MASK) >> 6); }
		bool IsActive() const { return static_cast<bool>((m_Data & ACTIVE_MASK) >> 5); }
		u8 GetIndex() const { return static_cast<u8>((m_Data & INDEX_MASK) >> 2); }
		bool HasMoved() const { return static_cast<bool>((m_Data & MOVED_MASK) >> 1); }

		bool operator==(const Piece& other) const { return m_Data == other.m_Data; }
		bool operator!=(const Piece& other) const { return m_Data == other.m_Data; }
	private:
		void SetPieceType(PieceType pieceType)
		{
			m_Data = m_Data & ~PIECE_MASK;
			m_Data |= static_cast<u8>(pieceType) << 7;
		}

		void Moved()
		{
			m_Data = m_Data & ~MOVED_MASK;
			m_Data |= 1u << 1;
		}
	private:
		u16 m_Data;
	private:
		static inline constexpr u16 X_MASK =      0b1110'0000'0000'0000;
		static inline constexpr u16 Y_MASK =      0b0001'1100'0000'0000;
		static inline constexpr u16 PIECE_MASK =  0b0000'0011'1000'0000;
		static inline constexpr u16 COLOR_MASK =  0b0000'0000'0100'0000;
		static inline constexpr u16 ACTIVE_MASK = 0b0000'0000'0010'0000;
		static inline constexpr u16 INDEX_MASK =  0b0000'0000'0001'1100;
		static inline constexpr u16 MOVED_MASK =  0b0000'0000'0000'0010;
	private:
		friend class ChessboardState;
	};

	class ChessboardState
	{
	public:
		ChessboardState();

		void SetPiece(const Piece& piece)
		{
			Position position = piece.GetPosition();
			u8 index = position.GetY() * 8 + position.GetX();
			m_Pieces[index] = piece;
		}

		void RemovePieceAtPosition(Position position)
		{
			Piece& piece = GetPieceAtPosition(position);
			if (!piece.IsValid())
			{
				ASSERT(false, "Attempting to remove piece at position, where there is no valid piece");
				return;
			}

			piece.SetPieceType(PieceType::None);
		}

		const Piece& GetPieceAtPosition(Position position) const
		{
			u8 index = position.GetY() * 8 + position.GetX();
			return m_Pieces[index];
		}

		Piece& GetPieceAtPosition(Position position)
		{
			u8 index = position.GetY() * 8 + position.GetX();
			return m_Pieces[index];
		}

		bool IsPositionOccupied(Position position) const
		{
			u8 index = GetFlatIndex(position);
			return m_Pieces[index].GetPieceType() != PieceType::None;
		}

		std::vector<Position> GenerateAllMoves(Position position) const;

		const std::vector<Piece>& GetPieces() const { return m_Pieces; }
	public:
		static u8 GetFlatIndex(Position position) { return position.GetY() * 8 + position.GetX(); }
		static ChessboardState CreateDefault();
	private:
		std::vector<Position> GenerateAllMovesPawn(Position position) const;
	private:
		std::vector<Piece> m_Pieces;
	};

}
