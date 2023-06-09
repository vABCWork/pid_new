#include "iodefine.h"
#include  "misratypes.h"
#include  "mtu.h"
#include  "pid.h"

#define D_CNT_200_MSEC   (25000 - 1)  


//
//  PWM処理
//
// 　 MV出力に対する PWMのON/OFF時間を設定
//　pid_mv = 0% ならば、OFF時間は、200msec, ON時間は 0msec
//         =10%           :        180msec     :    10msec
//           :
//         =80%                     60msec      :   140msec
//
// カウント間隔:  32/1024 MHz でカウント。　1カウントの時間は、 1024/32 usec = 32 [usec]
//    周期 200 msec,  200 000 usec = 32 * A, A = 6250
//    TGRA = 6250 - 1
//
//  
//  加熱制御時:MTU4_TGRBへOFF時間を設定 
//  冷却制御時:MTU4_TGRDへOFF時間を設定 
//
// 注: pid_mv =　0% や0.01%の場合、　i_duty = 0 となる。
//     i_duty = 0になると、 TGRB=TGRAとなり、コンペアマッチで出力が変化しない。 (22.3.5 PWMモード　(1) PWMモード1参照)
//    　(pid_mv = 100% から　0%にした場合に、出力が変化しないことになる。)
//   このため、i_duty = 0の場合、一度カウンタを停止して、初期出力 Lowを出力する。
//
//     また、pid_mv=100% で、TGRB=0の場合、初期出力=Lowのままでは、周期(200msec)毎に、1カウント(=32usec)分　Lowに落ちてしまう。
//　　　100%では常にHighとしたいため、初期値=highとしている。
//
// マニュアルより: 22.3.5 PWM モード
//(a) PWM モード1
//   ...
//  ペアで使用するTGR レジスタの設定値が同一の場合、コンペアマッチが発生しても出力値は変化しません。
//

void	Set_PWM_duty(float out_mv)
{
	float duty;
	uint16_t i_duty;
	
	duty = ( out_mv  * 0.01 ) * D_CNT_200_MSEC;	// ON時間 
	
	i_duty = duty;
	
	 
	 if ( heat_cool == 0 )  {			//　加熱制御(逆動作)の場合
	                                                // 加熱制御用 (MTIOC4A端子)の PWM出力設定	
	 	if ( i_duty == 0 ) {		        // 出力　0%の場合、初期出力(Low)を出力する処理	
			MTU.TSTR.BIT.CST4 = 0;		// MTU4.TCNT　カウント停止
			MTU4.TCNT = 0;		 	// MTU4 タイマカウンタ= 0x0000
							// MTIOC4A端子の動作:
			MTU4.TIORH.BIT.IOA = 1;		//  初期出力はLow,TCNTとTGRAのコンペアマッチでLow (表22.26)
			MTU4.TIORH.BIT.IOB = 2;		//  初期出力はLow,TCNTとTGRBのコンペアマッチでHigh (表22.18)
		}
		else if ( i_duty >=  D_CNT_200_MSEC) {	// 出力 100%以上の場合、初期出力(High)とする処理
			MTU.TSTR.BIT.CST4 = 0;		// MTU4.TCNT　カウント停止
			MTU4.TCNT = 0;		 	// MTU4 タイマカウンタ= 0x0000
							// MTIOC4A端子の動作:
			MTU4.TIORH.BIT.IOA = 5;		//  初期出力はHigh,TCNTとTGRAのコンペアマッチでLow (表22.26)
			MTU4.TIORH.BIT.IOB = 6;		//  初期出力はHigh,TCNTとTGRBのコンペアマッチでHigh (表22.18)
		
		}
		else {					// iduty > 0の場合、出力
		
			MTU4.TGRB = ( D_CNT_200_MSEC - i_duty);	// MTU4.TGRBの値は、OFF時間
			
			if ( MTU.TSTR.BIT.CST4 == 0 ){	 // 前回カウント停止の場合、カウント開始
				MTU.TSTR.BIT.CST4 = 1;		// MTU4.TCNT　カウント開始
				MTU4.TIORH.BIT.IOA = 1;		//  初期出力はLow,TCNTとTGRAのコンペアマッチでLow (表22.26)
				MTU4.TIORH.BIT.IOB = 2;		//  初期出力はLow,TCNTとTGRBのコンペアマッチでHigh (表22.18)
			}
		}
	
						// 冷却制御用 (MTIOC4C端子)
		MTU4.TGRD = D_CNT_200_MSEC;	// OFFの時間 (常にOFF)				
	 }
	 
	  else if ( heat_cool == 1 ) {			//　冷却制御(正動作)の場合
	 					        // 冷却制御用(MTIOC3A 端子）のPWM出力設定
		if ( i_duty == 0 ) {		        // 出力　0%の場合、初期出力(Low)を出力する処理
			MTU.TSTR.BIT.CST4 = 0;		// MTU4.TCNT　カウント停止
			MTU4.TCNT = 0;		 	// MTU4 タイマカウンタ= 0x0000
			
							// MTIOC4C端子の動作:
			MTU4.TIORL.BIT.IOC = 1;		//  初期出力はLow,TCNTとTGRCのコンペアマッチでLow (表22.27)
			MTU4.TIORL.BIT.IOD = 2;		//  初期出力はLow,TCNTとTGRDのコンペアマッチでHigh (表22.19)
		}
		else if ( i_duty >= D_CNT_200_MSEC ) {	        // 出力 100%以上の場合、初期出力(High)とする処理
			MTU.TSTR.BIT.CST4 = 0;		// MTU4.TCNT　カウント停止
			MTU4.TCNT = 0;		 	// MTU4 タイマカウンタ= 0x0000
			
							// MTIOC4C端子の動作:
			MTU4.TIORL.BIT.IOC = 5;		//  初期出力はHigh,TCNTとTGRCのコンペアマッチでLow (表22.27)
			MTU4.TIORL.BIT.IOD = 6;		//  初期出力はHigh,TCNTとTGRDのコンペアマッチでHigh (表22.19)
			
		}
		else {                                  // iduty > 0の場合、出力
			MTU4.TGRD = ( D_CNT_200_MSEC - i_duty);	// MTU4.TGRDの値は、OFF時間
			
			if ( MTU.TSTR.BIT.CST4 == 0 ){   // 前回カウント停止の場合、カウント開始
				MTU.TSTR.BIT.CST4 = 1;	 // カウント開始
				MTU4.TIORL.BIT.IOC = 1;		//  初期出力はLow,TCNTとTGRCのコンペアマッチでLow (表22.27)
				MTU4.TIORL.BIT.IOD = 2;		//  初期出力はLow,TCNTとTGRDのコンペアマッチでHigh (表22.19)
			}
		}
					         
						// 加熱制御用 (MTIOC4A端子)
		MTU4.TGRB = D_CNT_200_MSEC;	// OFFの時間 (常にOFF)	
	 }
}


//
// PWMモード の設定
//

void PWM_Init_Reg(void)
{

	MTU.TSTR.BIT.CST4 = 0;		// MTU4.TCNT　カウント停止

	MTU.TOER.BIT.OE4A = 1;		// 出力許可 (MTIOC4A)  
	MTU.TOER.BIT.OE4C = 1;		// 出力許可 (MTIOC4C)
		
	
	MTU4.TCR.BIT.TPSC = 5;		// MTU4 PCLK/1024でカウント
	MTU4.TCR.BIT.CKEG = 0;		// MTU4  立ち上がりエッジでカウント
	
	MTU4.TCR.BIT.CCLR = 1;		// TGRA(=TGRC)のコンペアマッチでTCNTクリア
	
	MTU4.TMDR.BIT.MD = 2;		// MTU4 PWM モード1
						
					// MTIOC4A端子の動作:
	MTU4.TIORH.BIT.IOA = 1;		//  初期出力はLow,TCNTとTGRAのコンペアマッチでLow (表22.26)
	MTU4.TIORH.BIT.IOB = 2;		//  初期出力はLow,TCNTとTGRBのコンペアマッチでHigh (表22.18)
	
					// MTIOC4C端子の動作:
	MTU4.TIORL.BIT.IOC = 1;		//  初期出力はLow,TCNTとTGRCのコンペアマッチでLow (表22.27)
	MTU4.TIORL.BIT.IOD = 2;		//  初期出力はLow,TCNTとTGRDのコンペアマッチでHigh (表22.19)
	
	
    	MTU4.TCNT = 0;		 	// MTU4 タイマカウンタ= 0x0000

					// 22.3.5 PWM モード  (a) PWM モード1
					//ペアで使用するTGR レジスタの設定値が同一の場合、コンペアマッチが発生しても出力値は変化しません。
	
					//　周期の設定 
	MTU4.TGRA = D_CNT_200_MSEC;	// 周期 = 200msec
	MTU4.TGRB = D_CNT_200_MSEC;	// OFFの時間
	
	MTU4.TGRC = D_CNT_200_MSEC;	// 周期 = 200msec
	MTU4.TGRD = D_CNT_200_MSEC;	// OFFの時間
		
}


// Cool動作、Heat動作用の出力ポート設定
//
//  P26: MTIOC4C : Cool用 PWM出力  
//  P27: MTIOC4A : Heat用 PWM出力
//
void PWM_Port_Set(void)
{
	
	MPC.PWPR.BIT.B0WI = 0;  	 // マルチファンクションピンコントローラ　プロテクト解除
        MPC.PWPR.BIT.PFSWE = 1;  	// PmnPFS ライトプロテクト解除
    	 
	MPC.P26PFS.BYTE = 0x02;		// P26 = MTIOC4C
	MPC.P27PFS.BYTE = 0x02;		// P27 = MTIOC4A
	
        MPC.PWPR.BYTE = 0x80;      	//  PmnPFS ライトプロテクト 設定	
	
	COOL_PWM_PMR = 1;	     	// P31:周辺モジュールとして使用
	
	HEAT_PWM_PMR = 1;     		// P27:周辺モジュールとして使用
}


