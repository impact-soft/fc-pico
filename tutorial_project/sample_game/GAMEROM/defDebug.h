;/// @file defDebug.h
;/// @brief Build switches: debug features, mapper number and cheat toggles.
;/// @ingroup gamerom
;///
;/// Included first by PG_main.asm so the rest of the assembly can test these.
;/// `DEBUG_BUILD` and `DEBUG_MODE` are both left on in the shipped source.

;==========================================================
; デバッグコントロール定義
;==========================================================
; デバッグビルドコントロール
DEBUG_BUILD EQU  1		;///< Left on in the shipped source.

; マッパー番号
MAPPER_NO	EQU 0		;///< iNES mapper number. 0, NROM.

; デバッグ機能ON
DEBUG_MODE = 1		;///< Enables the in-game debug features.


DEBUG_NO_GAME_OVER = 0		;///< Set to 1 to make the player unkillable.


AUTO_SHOT_OFF = 0			; =1 の時、通常弾のオートショットをＡボタン押しっぱなしで停止出来る		;///< Set to 1 to let a held A button suppress autofire.


SINGLE_SHOT_TEST = 0		; =1 自機の通常弾1発のみテスト		;///< Set to 1 to limit the player to a single normal shot, for testing.
STAGE_TEST		 = 0		; !=0  指定ステージからスタート =8 テスト		;///< Non-zero starts from that stage; 8 selects the test stage.

MMC_TYPE = 0				; =0 MMC3  =1 AX-A1 =2 INL-SWAP		;///< Cartridge board type: 0 MMC3, 1 AX-A1, 2 INL-SWAP. This build assembles as NROM regardless. @see #MAPPER_NO
NO_COPY_PROTECT =  1		; =1 コピープロテクト無し		;///< Set to 1 to leave the copy protection out.
FLASH_DEV_CODE = $A4		;///< Expected flash device code, for the self-flashing path.
FLASH_MAN_CODE = $C2		;///< Expected flash manufacturer code.
WRAM_PROTECT_CODE = 0		;///< Write-protect code for battery-backed WRAM.

ARDUINO_MODE = 0			; =1 ARDUINO 搭載モード		;///< Set to 1 for the Arduino-hosted build. Left at 0 here; the cartridge drives the ROM through `$E000` instead. @see @ref sample_game

