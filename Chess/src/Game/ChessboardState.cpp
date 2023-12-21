#include "ChessboardState.h"

namespace Chess {

	ChessboardState::ChessboardState()
	{
		m_Pieces.reserve(8 * 8);
		for (u8 i = 0; i < 8 * 8; i++)
		{
			u8 x = i % 8;
			u8 y = i / 8;
			m_Pieces.emplace_back(x, y);
		}
	}

	std::vector<Chess::Position> ChessboardState::GenerateAllMoves(Position position) const
	{
		const Piece& piece = GetPieceAtPosition(position);
		ASSERT(piece.IsValid(), "Invalid piece for generating possible moves");
		switch (piece.GetPieceType())
		{
		case PieceType::Pawn:
			return GenerateAllMovesPawn(position);
		}

		return {};
	}

	ChessboardState ChessboardState::CreateDefault()
	{
		ChessboardState state;
		for (u8 i = 0; i < 8; ++i)
		{
			Piece pawnWhite = Piece(i, 1, PieceType::Pawn, PieceColor::White, i);
			Piece pawnBlack = Piece(i, 6, PieceType::Pawn, PieceColor::Black, i);
			state.SetPiece(pawnWhite);
			state.SetPiece(pawnBlack);
		}

		Piece rook1white = Piece(0, 0, PieceType::Rook, PieceColor::White, 0);
		Piece rook2white = Piece(7, 0, PieceType::Rook, PieceColor::White, 1);
		Piece rook1black = Piece(0, 7, PieceType::Rook, PieceColor::Black, 0);
		Piece rook2black = Piece(7, 7, PieceType::Rook, PieceColor::Black, 1);
		state.SetPiece(rook1white);
		state.SetPiece(rook2white);
		state.SetPiece(rook1black);
		state.SetPiece(rook2black);

		Piece knight1white = Piece(1, 0, PieceType::Knight, PieceColor::White, 0);
		Piece knight2white = Piece(6, 0, PieceType::Knight, PieceColor::White, 1);
		Piece knight1black = Piece(1, 7, PieceType::Knight, PieceColor::Black, 0);
		Piece knight2black = Piece(6, 7, PieceType::Knight, PieceColor::Black, 1);
		state.SetPiece(knight1white);
		state.SetPiece(knight2white);
		state.SetPiece(knight1black);
		state.SetPiece(knight2black);

		Piece bishop1white = Piece(2, 0, PieceType::Bishop, PieceColor::White, 0);
		Piece bishop2white = Piece(5, 0, PieceType::Bishop, PieceColor::White, 1);
		Piece bishop1black = Piece(2, 7, PieceType::Bishop, PieceColor::Black, 0);
		Piece bishop2black = Piece(5, 7, PieceType::Bishop, PieceColor::Black, 1);
		state.SetPiece(bishop1white);
		state.SetPiece(bishop2white);
		state.SetPiece(bishop1black);
		state.SetPiece(bishop2black);

		Piece queenWhite = Piece(3, 0, PieceType::Queen, PieceColor::White, 0);
		Piece kingWhite = Piece(4, 0, PieceType::King, PieceColor::White, 1);
		Piece queenBlack = Piece(3, 7, PieceType::Queen, PieceColor::Black, 0);
		Piece kingBlack = Piece(4, 7, PieceType::King, PieceColor::Black, 1);
		state.SetPiece(queenWhite);
		state.SetPiece(kingWhite);
		state.SetPiece(queenBlack);
		state.SetPiece(kingBlack);

		return state;
	}

	std::vector<Position> ChessboardState::GenerateAllMovesPawn(Position position) const
	{
		const Piece& pawn = GetPieceAtPosition(position);
		i8 yDir = pawn.GetPieceColor() == PieceColor::White ? 1 : -1;
		Position nextPos = Position(position.GetX(), position.GetY() + yDir);
		if (IsPositionOccupied(nextPos))
		{
			return {};
		}

		std::vector<Position> positions;
		positions.push_back(nextPos);

		if (!pawn.HasMoved())
		{
			Position nextNextPos = Position(position.GetX(), position.GetY() + 2 * yDir);
			if (!IsPositionOccupied(nextNextPos))
			{
				positions.push_back(nextNextPos);
			}
		}

		// TODO: Add en-passant, and check check
		return positions;
	}

}
