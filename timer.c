
#include "typedefine.h"
#include  "iodefine.h"
#include "misratypes.h"
#include "timer.h"
#include "key.h"
#include "crc_16.h"

extern  uint8_t alm_4;

extern  volatile uint8_t  rcv_cnt;
extern	volatile uint8_t rcv_over;

// Timer 
volatile uint8_t flg_100msec_interval;	// 100msec毎にON

volatile uint8_t timer_a_cnt;          // 　(10msec毎にカウントアップ)



//  コンペアマッチタイマ CMT0
//   10msec毎の割り込み
// 　約 22[usec]かかる。22[usec]は他の割り込みは入れない。
//
#pragma interrupt (Excep_CMT0_CMI0(vect=28))

void Excep_CMT0_CMI0(void){
	
	uint32_t i;
	
	
	
	timer_a_cnt++;	       // カウントのインクリメント
	
	if ( timer_a_cnt > 9 ) {	   // 100msec経過
		
		flg_100msec_interval = 1;  // 100msecフラグ ON
		
		timer_a_cnt = 0;	  //  カウンターのクリア
	}
	
	
	
	touch_position();		// タッチ位置の読み出し (A/D変換データを得る)
	  	
	touch_xyz_press();		// X,Y,Zの値を得る。タッチ圧を計算。
	 
	touch_key_status_check();   // タッチキーの状態を得る
	 
	for( i = 0; i < KEY_SW_NUM; i++ ) {   // タッチ→非タッチ状態となったキーを見つけて、キー入力処理要求フラグをセットする。
		     switch_input_check(i);           //  (前回押されたキーを検索している)
	}
	
}


//
//    10msec タイマ(CMT0)
//    CMTユニット0のCMT0を使用。 
//
//  PCLKB(=32MHz)の128分周でカウント 32/128 = 1/4 MHz
//      1カウント = 4/1 = 4 [usec]  
//    1*10,000 usec =  N * 4 usec 
//      N = 2500


void Timer10msec_Set(void)
{	
	IPR(CMT0,CMI0) = 3;		// 割り込みレベル = 3　　（15が最高レベル)
	IEN(CMT0,CMI0) = 1;		// CMT0 割込み許可
		
	CMT0.CMCR.BIT.CKS = 0x2;        // PCLKB/128       
	CMT0.CMCOR = 2499;		// CMCNTは0からカウント 	


}


//   CMT0 タイマ開始　
//  割り込み許可してカウント開始

void Timer10msec_Start(void)
{	
	CMT0.CMCR.BIT.CMIE = 1;		// コンペアマッチ割込み　許可
		
	CMT.CMSTR0.BIT.STR0 = 1;	// CMT0 カウント開始
	
	timer_a_cnt = 0;		//  タイマのカウント値クリア
}

