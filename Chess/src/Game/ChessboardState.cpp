#include "ChessboardState.h"

namespace Chess {

	ChessboardState ChessboardState::CreateDefault()
	{
		ChessboardState state;
		for (u8 i = 0; i < 8; ++i)
		{
			Piece pawnWhite = Piece(i, 1, PieceType::Pawn, PieceColor::White, i);
			Piece pawnBlack = Piece(i, 6, PieceType::Pawn, PieceColor::Black, i);
			state.AddPiece(pawnWhite);
			state.AddPiece(pawnBlack);
		}

		Piece rook1white = Piece(0, 0, PieceType::Rook, PieceColor::White, 0);
		Piece rook2white = Piece(7, 0, PieceType::Rook, PieceColor::White, 1);
		Piece rook1black = Piece(0, 7, PieceType::Rook, PieceColor::Black, 0);
		Piece rook2black = Piece(7, 7, PieceType::Rook, PieceColor::Black, 1);
		state.AddPiece(rook1white);
		state.AddPiece(rook2white);
		state.AddPiece(rook1black);
		state.AddPiece(rook2black);

		Piece knight1white = Piece(1, 0, PieceType::Knight, PieceColor::White, 0);
		Piece knight2white = Piece(6, 0, PieceType::Knight, PieceColor::White, 1);
		Piece knight1black = Piece(1, 7, PieceType::Knight, PieceColor::Black, 0);
		Piece knight2black = Piece(6, 7, PieceType::Knight, PieceColor::Black, 1);
		state.AddPiece(knight1white);
		state.AddPiece(knight2white);
		state.AddPiece(knight1black);
		state.AddPiece(knight2black);

		Piece bishop1white = Piece(2, 0, PieceType::Bishop, PieceColor::White, 0);
		Piece bishop2white = Piece(5, 0, PieceType::Bishop, PieceColor::White, 1);
		Piece bishop1black = Piece(2, 7, PieceType::Bishop, PieceColor::Black, 0);
		Piece bishop2black = Piece(5, 7, PieceType::Bishop, PieceColor::Black, 1);
		state.AddPiece(bishop1white);
		state.AddPiece(bishop2white);
		state.AddPiece(bishop1black);
		state.AddPiece(bishop2black);

		Piece queenWhite = Piece(3, 0, PieceType::Queen, PieceColor::White, 0);
		Piece kingWhite = Piece(4, 0, PieceType::King, PieceColor::White, 1);
		Piece queenBlack = Piece(3, 7, PieceType::Queen, PieceColor::Black, 0);
		Piece kingBlack = Piece(4, 7, PieceType::King, PieceColor::Black, 1);
		state.AddPiece(queenWhite);
		state.AddPiece(kingWhite);
		state.AddPiece(queenBlack);
		state.AddPiece(kingBlack);

		return state;
	}

}
