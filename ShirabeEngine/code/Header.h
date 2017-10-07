#pragma once

#include <stdint.h>

enum class ESetState {
	Unset = 0,
	Black,
	White
};

// 0 ------------ 1 ------------ 2
// |              |              |
// |   3 -------- 4 -------- 5   |
// |   |          |          |   |
// |   |   6 ---- 7 ---- 8   |   |
// |   |   |             |   |   |
// 9 -10 - 11           12 - 13- 14
// |   |   |             |   |   |
// |   |   15----16 ----17   |   |
// |   |          |          |   |
// |   18 -------19 ------- 20   |
// |              |              |
// 21 -----------22 ----------- 23

ESetState fields[24];
uint32_t  combinations[16][3] ={
	{ 0, 1, 2 }, { 3, 4, 5 }, { 6, 7, 8 }, { 9, 10, 11 }, { 12, 13, 14 }, { 15, 16, 17 }, { 18, 19, 20 }, { 21, 22, 23 },
	{ 0, 9, 21 }, { 3, 10, 18 }, { 6, 11, 15 }, { 1, 4, 7 }, { 16, 19, 22 }, { 8, 12, 17 }, { 5, 13, 20 }, { 2, 14, 23 }
};

ESetState myState = ESetState::Black;

bool findPossibleWinningCombination(ESetState *fields, uint32_t fieldCount, uint32_t **combinations, uint32_t combinationCount, ESetState myState) {
	for( uint32_t c=0; c < combinationCount; ++c ) {
		int setCount = 0;
		int unsetIndices[3];

		for( uint32_t i=0; i < 3; ++i ) {
			unsetIndices[i] = combinations[c][i];

			ESetState state = fields[combinations[c][i]];
			if( state != ESetState::Unset ) {
				unsetIndices[i] = -1;

				if( state != myState )
					break; // Early return, since for this combination at least one field is occupied by the enemy.
				else
					++setCount;
			}

			if( setCount == 2 ) {
				// At least 2 fields of myState set. Find the last one.
				for( uint32_t j=0; j < 3; ++j ) {
					if( unsetIndices[j] > -1 ) {
						fields[unsetIndices[j]] = myState;
						return true;
					}
				}
			}
		}
	}

	return false;
}