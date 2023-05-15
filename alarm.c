#include "iodefine.h"
#include "misratypes.h"

#include "alarm.h"

uint8_t	alm_1;		// ROM-CRCエラー
uint8_t alm_2;		// E2 データフラッシュエラー (CRCエラーまたは書き込み失敗)
uint8_t alm_3;		// 熱電対の断線エラー
uint8_t alm_4;          // パソコンとの通信エラー  b0: CRCエラー,b1:通信電文の長さが短い, b2:未使用, b3:先頭バイトがコマンド以外, b4-b7:未使用

// アラームLED出力処理
//  alm_2～alm_3 発生時のLED出力処理
void alarm_led(void)
{
	if ( alm_2 == 1 ) {  
		ALM_2_PODR = 1; 	//  High(ALM2_LED点灯)
	}
	else {
		ALM_2_PODR = 0; 	//  Low(ALM2_LED消灯)
	}
	
	if ( alm_3 == 1 ) {  
		ALM_3_PODR = 1; 	//  High(ALM3_LED点灯)
	}
	else {
		ALM_3_PODR = 0; 	//  Low(ALM3_LED消灯)
	}
	
	if ( alm_4 > 0 ) {  
		ALM_4_PODR = 1; 	//  High(ALM4_LED点灯)
	}
	else {
		ALM_4_PODR = 0; 	//  Low(ALM4_LED消灯)
	}
}


// アラーム LED ポートの定義
void alarm_port_ini(void)
{
	ALM_1_PMR = 0;	// 汎用入出力ポート
	ALM_1_PDR = 1;	// 出力ポートに指定
	
	ALM_2_PMR = 0;
	ALM_2_PDR = 1;
	
	ALM_3_PMR = 0;
	ALM_3_PDR = 1;
	
	ALM_4_PMR = 0;
	ALM_4_PDR = 1;
	
	
}