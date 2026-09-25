#pragma once

//==========================================================
// ミッション制御関連定義
//==========================================================
enum{
	MT_HARA = 0,		// ザコ敵ミッション
	MT_FLET	= 1,		// 艦隊ミッション
	MT_BOSS = 2,		// ボス敵ミッション
	MT_EVNT = 3,		// イベント

	MT_MAX  = 4,		// ミッションタイプMAX

//---- 特殊ミッション制御 ----
	MT_ATK_NO	= 0xFB,	// ミッション攻撃番号セット		;///< Control: set the mission attack number.
	MT_ANM_NO   = 0xFC,	// ミッションアニメ番号セット
	MT_BOSSJMP	= 0xFD,	// デモ時の開始ボスミッションにジャンプ
	MT_DEMOJMP  = 0xFE,	// デモ時の開始ミッションにジャンプ
	MT_END		= 0xFF,	// ミッション終了
};


//----------------------------------------------
// ミッションコントロールコード
//----------------------------------------------
enum{
	_MC_END			= 0x00,		///< Opcode: halt the script.
	_MC_CALL	 	= 0xFF,		///< MC_CALL, 呼び出し先アドレスラベル　　コールは先からコールは不可		;///< Opcode: call another script. A called script may not itself call.
	_MC_RET			= 0xFE,		///< Opcode: return from a call.
	_MC_LOOP_CNT	= 0xFD,		///< MC_LOOP_CNT, （ループ回数）		;///< Opcode: load the loop counter.
	_MC_JMP			= 0xFC,		///< MC_JMP, ジャンプ条件, ジャンプ先アドレス		;///< Opcode: conditional jump.
	_MC_ZAKO		= 0xFB,		///< 雑魚敵セット		;///< Opcode: spawn a minion.
	_MC_MEMCPYN		= 0xFA,		///< メモリーコピー Nバイト版		;///< Opcode: copy N bytes.
	_MC_MEMCPY2		= 0xF9,		///< メモリーコピー 2バイト版		;///< Opcode: copy 2 bytes.
	_MC_BOSS_NS		= 0xF8,		///< ボス通常弾発射		;///< Opcode: boss fires a normal shot.
	_MC_PALSET		= 0xF7,		///< パレット書き換え		;///< Opcode: rewrite part of the palette.
	_MC_MEMSET		= 0xF6,		///< メモリーセット 1バイト版		;///< Opcode: store a byte.
	_MC_MEMSET2		= 0xF5,		///< メモリーセット 2バイト版		;///< Opcode: store a word.
	_MC_PGCALL		= 0xF4,		///< プログラムコール プログラムを呼び出す		;///< Opcode: call a 6502 subroutine.
	_MC_MEMCLR		= 0xF3,		///< メモリークリアー		;///< Opcode: clear a region.

	_MC_BG_ANIME	= 0xF2,		///<  BGにアニメーションデータをセットする		;///< Opcode: install background animation data.
	_MC_PGCALL2		= 0xF1,		///<  バンク付きプログラムコール プログラムを呼び出す		;///< Opcode: call a 6502 subroutine in a named bank.
	//_MC_VRAMSET		EQU  $F0	;  VRAMセット　アドレス、値
	_MC_MEMADD		= 0xEF,		///<  メモリー加算 1バイト版 アドレス、値		;///< Opcode: add to a byte.
	_MC_MEMCMP		= 0xEE,		///<  メモリー比較 1バイト版 アドレス、値		;///< Opcode: compare a byte, leaving the result in #MISSON_CMP_P.
	_MC_MEMPUSH		= 0xED,		///<  メモリー 1バイト PUSH		;///< Opcode: push a byte onto the mission stack.
	_MC_MEMPOP		= 0xEC,		///<  メモリー 1バイト POP		;///< Opcode: pop a byte from the mission stack.


	_MC_BASE	= 0xEC		///< コマンド予約の開始番号		;///< First reserved opcode number.
};


//----------------------------------------------
// ミッションコントロールコード　ジャンプ条件
//----------------------------------------------
enum{
	MCJ_JMP			=  0,		// 無条件ジャンプ		;///< Jump condition: always.
	MCJ_LOOP_CNT	=  1,		// ループカウンターをマイナス１してゼロでなければジャンプ		;///< Jump condition: decrement the loop counter and jump while non-zero.
	MCJ_CMP_Z		=  2,		// メモリー比較結果がZならジャンプ		;///< Jump condition: last compare was equal.
	MCJ_CMP_NZ		=  3,		// メモリー比較結果がNZならジャンプ		;///< Jump condition: last compare was not equal.
	MCJ_CMP_C		=  4,		// メモリー比較結果がCならジャンプ		;///< Jump condition: last compare set carry.
	MCJ_CMP_NC		=  5,		// メモリー比較結果がNCならジャンプ		;///< Jump condition: last compare cleared carry.

	MCJ_ENEMY_Z		= 10,		// 敵の残りがゼロならジャンプ
	MCJ_ENEMY_NZ	= 11,		// 敵の残りがゼロでないならジャンプ

};

//=====================================================
//
//		ミッション処理
//
//=====================================================

class rp_mission {

public:
	void initMission();
	void getMission();
	void updateMission();

	uint8_t MISSON_TYPE;		///< Mission opcode. `0xff` means the stage is cleared, which is how this side detects it. // B ミッションタイプ
	uint8_t MISSON_TYPE_SUB;	///< Mission opcode qualifier. // B ミッションタイプ

private:
	void mtMain();
	void initMissionControl();
	bool mainMissionControl();
	bool mcj_sub( uint8_t mcj );

	void pushMstack_areg( uint8_t a );
	uint8_t popMstack_areg();
	void pushMISSON_PC();
	void popMISSON_PC();


	uint8_t MISSON_NO;			///< Mission index within the stage. // B ステージ内ミッション番号
	uint8_t MISSON_WAIT;		///< Frames until the mission script advances. // B MISSON_PCの次の処理までのウェイト
	uint16_t MISSON_PC;			///< Mission script program counter, 16-bit. // W ミッションPC
	uint8_t MISSON_STEP;		///< Mission step within the current opcode. // B ミッション 処理ステップ

	uint8_t MISSON_LOOP_CNT;		///< Loop counter for the mission script. // B ミッション ループカウンタ
	uint8_t MISSON_FLG;			///< Mission flags. // B ミッション フラグ
	uint8_t MISSON_TMP;			///< Mission scratch byte. // B ミッション 汎用
	uint8_t MISSON_PC_SP;		///< ミッションPCスタックポインタ
	uint8_t MISSON_CMP_P;		///< cmp Flag

	uint8_t MISSON_STACK[0x10];	///< ミッション用スタック

	const uint8_t *mission;


};


enum {
	PRM_0			= 0x00,		///< Subroutine parameter 0.
	PRM_1			= 0x01,		///< Subroutine parameter 1.
	PRM_2			= 0x02,		///< Subroutine parameter 2.

	MISSON_ANM_NO	= 0x03,		///< Mission animation number.

	ENEMY_ATK_LV	= 0x04,		///< Enemy attack level: 0 none, 1 aimed, 2 homing, 3 both, 4 fast homing.

	MISSON_ATK_NO   = 0x05,
	MISSON_ATK_CNT  = 0x06,
	MISSON_ANM_CNT  = 0x07,
};

extern uint8_t gMI_RAM[0x20];	///< ミッション用グローバルRAM

