#include "iodefine.h"
#include  "misratypes.h"
#include  "timer.h"
#include  "key.h"
#include  "pid.h"
#include  "touch.h"
#include  "alarm.h"

uint8_t  para_index;	// 0:SV, 1:P, 2:I, 3:D, 4:MR, 5:Hys, 6:H/C , 7:Ch2, 8:CJT)


volatile struct  SW_info  Key_sw[KEY_SW_NUM];	// スイッチ　6個分の情報格納領域


// X軸、Y軸kの測定値の平均処理用
// 測定値は12bitデータだが、上位 8bitだけ見ている

uint8_t   touch_x_val;		// X軸データ
uint8_t   touch_y_val;          // Y軸データ

uint8_t   touch_z1_val;		// Z方向　(タッチ圧測定用)
uint8_t   touch_z2_val;

uint8_t   tc_val_pt;		// 測定値の格納位置

uint8_t   touch_x[8];		// 8回分の測定値
uint8_t   touch_y[8];


uint16_t   touch_x_average;	// 平均値
uint16_t   touch_y_average;


float	touch_resistance;	// タッチ抵抗

float   fl_touch_z1;		// touch_z1_valの float形式
float   fl_touch_z2;            // touch_z2_valの float形式
float   touch_z1_z2;		// z2/z1

// 
// タッチ状態の初期化 (  電源ON直後に1回実施 )
//  
//   全て非タッチ(SW OFF: High = 1 )とする
//   測定値の格納位置の初期化
//
void touch_pre_status_ini(void)
{
	uint32_t i;
		
	for ( i = 0 ; i < KEY_SW_NUM; i++ ) {		//  全て非タッチ(SW OFF: High = 1 )とする
	
	     Key_sw[i].pre_status = 1;
	
	}
	
 	tc_val_pt = 0;		// 格納位置のクリア	
}


// X軸、Y軸kの測定結果の平均値を得る (デバック用) (キー入力判定処理に平均値は使用していない)
// 12bitデータの上位 8bitを使用 tc_val_pt
void touch_cal_average(void)
{
	uint32_t  i;
	uint32_t  x_avg;
	uint32_t  y_avg;
	
	
	touch_x_val =  ( ad_x_val >> 4 );	// X軸測定データの b11-b4
	touch_y_val =  ( ad_y_val >> 4 );  	// Y軸測定データの b11-b4

	touch_x[tc_val_pt] = touch_x_val;   // 測定位置の格納
	touch_y[tc_val_pt] = touch_y_val;
	
	x_avg = 0;			// 平均値の計算
	y_avg = 0;
	
	for ( i = 0 ; i < 8 ; i++ ) {	// 8回分の総和を得る 
	      x_avg = x_avg + touch_x[i];
	      y_avg = y_avg + touch_y[i];
	 }
	   
	touch_x_average =  x_avg >> 3;   // 割る 8
	touch_y_average =  y_avg >> 3;   // 割る 8
	

	if ( tc_val_pt < 7 ) {		// 
	
	   tc_val_pt = tc_val_pt + 1;	// 格納位置のインクリメント
	}
	else  {				
	  
	   tc_val_pt = 0;		// 格納位置のクリア	
	}
	 
}



//
//   X,Y,Z1,Z2を得るまた、抵抗値を計算する。
//
// タッチパネルのZ方向(フィルムとその下にあるLCDガラスとの上下方向)の抵抗値(Rz)を得る
//  タッチされていない状態では、大きい値になる。
//
//     Rz = Rx_plate * ( X軸測定値 /4096 )* ( ( Z2 / Z1 ) - 1 )
//
//    Rx_plate: タッチスクリーンのXプレート(フィルム側)の抵抗値 約 270[Ω]。

void touch_xyz_press(void)
{
	touch_x_val = ( ad_x_val >> 4 );  // X軸測定データの b11-b4
	touch_y_val = ( ad_y_val >> 4 );  // Y軸測定データの b11-b4
	
	touch_z1_val = ( ad_z1_val >> 4);  // Z1 測定データの b11-b4
	touch_z2_val = ( ad_z2_val >> 4 );  // Z2 測定データの b11-b4
	  
	if ( touch_z1_val > 0 ) {
	  
	 fl_touch_z1 = touch_z1_val;
	 fl_touch_z2 = touch_z2_val;
	 
	 touch_z1_z2 = fl_touch_z2 / fl_touch_z1;
	 
	 touch_resistance = 270.0 * ( touch_x_val / 4096.0 ) * ( touch_z1_z2 - 1.0 );
        }
	else{
	   touch_resistance = 999.9;
	}		
}



// 　
// タッチキーの状態を得る
//  抵抗値が 40Ωを超える場合は、非タッチとする。
//  40Ω以下の場合、タッチされていると判断する。
// 
// X軸とY軸の読み出しデータにより、押されたキーを判断。
//
//
// キーと押された時の読み出しデータ
//  
// 　　　　　　    タッチ無し STOP/RUN(SW0)  Auto/Manual(SW1)  Para(SW2)    ▼(Down)(SW3)   ▲(Up)(SW4)    Save(SW5)     　    
//  touch_x_val:　　 0xff      0x40～0x5f     0x40～0x5f       0x70～0x90   0xa2～0xc0      0xa2～0xc0    0x70～0x90 
//  touch_y_val:     0x76      0x20～0x38     0x40～0x59       0x40～0x59   0x20～0x38      0x40～0x59    0x20～0x38
//
///
//  キー配置
//        Auto/Manual   Para        ▲
//        Stop/RUN   　 Save     　 ▼
//
//
//
//


#define TC_Y_VAL_L_0	0x20	// Y軸下限 (下にあるキー STOP/RUN(SW0), ▼(Down)(SW3), Save(SW5) 用)　,
#define TC_Y_VAL_H_0    0x38    // Y軸上限

#define TC_Y_VAL_L_1	0x40	// Y軸下限 (上にあるキー Auto/Manual(SW1), Para(SW2), ▲(Up)(SW4)  用)
#define TC_Y_VAL_H_1    0x59    // Y軸上限


void touch_key_status_check(void)
{
	uint32_t i;
	
							// キータッチを軽くする場合には、大きな値とする。
	if ( touch_resistance > 40.0 ) {		// 抵抗値が40Ωを超える場合、
	  for ( i = 0 ; i < KEY_SW_NUM; i++ ) {		//  全て非タッチ(SW OFF: High = 1 )とする
	     Key_sw[i].status = 1;
	  }
	  
	  return;  
	}
	
							// 押されたキーの判定
		
	if (( touch_y_val >= TC_Y_VAL_L_0 ) && (  touch_y_val <= TC_Y_VAL_H_0 )){        //   STOP/RUN(SW0), ▼(Down)(SW3), Save(SW5) 用
	
	   if (( touch_x_val >= 0x40 ) && (  touch_x_val <= 0x5f )){   //   STOP/RUN(SW0) タッチ
	  	Key_sw[0].status = 0;				       // 押されたキーをタッチあり(SW ON: Low = 0 )とする
	    }
	   
	    else if (( touch_x_val >= 0x70 ) && (  touch_x_val <= 0x90 )){     //  Save(SW5) タッチ
	  	Key_sw[5].status = 0;						
	    }
	  
	    else if  (( touch_x_val >= 0xa2 ) && (  touch_x_val <= 0xc0 )){   //  ▼(Down)(SW3) タッチ
	         Key_sw[3].status = 0;
	    }
	    
	}
	
	else if (( touch_y_val >= TC_Y_VAL_L_1 ) && (  touch_y_val <= TC_Y_VAL_H_1 )){     //   Auto/Manual(SW1), Para(SW2), ▲(Up)(SW4)  用
	
	    if (( touch_x_val >= 0x40 ) && (  touch_x_val <= 0x5f )){     //   Auto/Manual(SW1) タッチ
	  	Key_sw[1].status = 0;						
	    }
	    
	    else if  (( touch_x_val >= 0x70 ) && (  touch_x_val <= 0x90 )){   //  Para(SW2)   タッチ
	         Key_sw[2].status = 0;
	    }
	    else if  (( touch_x_val >= 0xa2 ) && (  touch_x_val <= 0xc0 )){   //  ▲(Up)(SW4) タッチ
	         Key_sw[4].status = 0;
	    }	
	 	
	}						
		
}

//
// 　スイッチの入力判定 (10msecに1回　実行)
//
// 概要:
//   タッチ状態が4回継続後、非タッチにればキー入力されたとする。
//   長押し有効キーが、250msec以上、タッチ状態が継続すれば、キーが長押しされているとする。
//
//    判定するスイッチ:
//                  0 = SW0 , STOP/RUN
//                  1 = SW1 , Auto/Manual
//                  2 = SW2 , Para
//                  3 = SW3 , ▼(Down)
//                  4 = SW4 , ▲(Up)
//             
//  
void switch_input_check( uint8_t id ) 
{
	
         if ( Key_sw[id].status == 0 ) {     // 今回 タッチ状態
	
	     if ( Key_sw[id].pre_status == 1 ) {  // 前回 非タッチ状態　　(立下がり検出)
	           Key_sw[id].low_cnt =  1;       // Lowカウント = 1 セット
	     
	     }
	     else{				// 前回 タッチ状態
	      	  if ( Key_sw[id].low_cnt > 24 ) {   // 　カウントが25回以上の場合　(250msec以上)
		     
		      if (( id == 3 ) || ( id == 4 )){     // 長押し有効キー SW4,SW5  (▲(Up)と▼(Down))の場合
	                  Key_sw[id].long_push = 1;	    // キー入力処理要求(長押し)のセット
	               }
		      
		   }
		   else{
		           Key_sw[id].low_cnt = Key_sw[id].low_cnt + 1; // Lowカウントのインクリメント  
		   }
	     }
	  }      // 今回タッチ状態

	  else{				// 今回　非タッチ状態
	   
	      if ( Key_sw[id].pre_status == 1 ) {  // 前回　非タッチ状態
	   	
	      }
	      
	      else{				// 前回　タッチ状態 (立上がり検出)
	          if ( Key_sw[id].low_cnt > 3 ) {   // 4回以上　タッチ検出の場合
	             if ( Key_sw[id].long_push == 1 ) {   //   キー入力処理要求(長押し)セット済みの場合
		      
		         Key_sw[id].long_push = 0;	// キー入力処理要求(長押し)クリア
		     }
		     else {
			  
		         Key_sw[id].one_push = 1;	 // キー入力処理要求(1回押し)セット
		     }
		     
		     Key_sw[id].low_cnt = 0;	//  Lowカウントのクリア  
	          }
	      }
	  }	  
	  	
	  Key_sw[id].pre_status = Key_sw[id].status;   // 現在の状態を、一つ前の状態へコピー
	  
}
   


//
// キー入力処理
//   0 = SW0 , STOP/RUN
//   1 = SW1 , Auto/Manual
//   2 = SW2 , Para
//   3 = SW3 , ▼(Down)
//   4 = SW4 , ▲(Up)
//   5 = SW5,  Save
//
void key_input(void)	
{
	uint32_t    i;
	uint32_t err_fg;
	
	if (  Key_sw[0].one_push == 1 ) {	    // Run/Stop key
		pre_mode_stop_run = mode_stop_run;  // 現在のモードを退避
	
		if ( mode_stop_run == 0  ) {	    // 現在、Stopの場合、
		      mode_stop_run = 1;	    // Run にする。
		}
		else if ( mode_stop_run == 1) {	    // 現在、Runの場合、
		      mode_stop_run = 0;	    // Stop にする。
		}
	}
	
	if ( Key_sw[1].one_push == 1 ) {		// Auto/Manual key
		pre_mode_auto_manual = mode_auto_manual; // 現在のモードを退避
	
		if ( mode_auto_manual == 0  ) {	    // 現在、Autoの場合、
		      mode_auto_manual = 1;	    // Manual にする。
		}
		else if ( mode_auto_manual == 1) {  // 現在、Manualの場合、
		      mode_auto_manual = 0;	    // Auto にする。
		}
	}
	
	
	if (  Key_sw[2].one_push == 1 ) {		// Para key
		if ( para_index  < ( PARA_MAX_NUMBER - 1 )) {
			para_index = para_index + 1;
		}
		else {
			para_index = 0;
		}
	}
	
	if ( Key_sw[3].one_push == 1 ) {		// Down
	 
		key_down();
	}
	if ( Key_sw[3].long_push == 1 ) {		// Down (長押し)
	   
	   for ( i = 0 ; i < LONG_PUSH_CNT ; i++ ) {	
		 key_down();
	   }
	}
	
	
	if (  Key_sw[4].one_push == 1 ) {		// Up key
		
		key_up();
	}
	if (  Key_sw[4].long_push == 1 ) {		// Up key (長押し)
	   
	   for ( i = 0 ; i < LONG_PUSH_CNT; i++ ) {
		key_up();
	   }
	}
	
	
	if (  Key_sw[5].one_push == 1 ) {		// Save key
		
	    err_fg = pid_para_flash_write();       //  パラメータを、データフラッシュへ書き込み
	    
	    if ( err_fg > 0 ) {
		    alm_2 = 1;
	    }
	    else{
	    	alm_2 = 0;
	    }
		
	}
	
	
	for ( i = 0 ; i < KEY_SW_NUM ; i++ ) {	// スイッチ 一度押しの情報をクリア
		Key_sw[i].one_push = 0;
	}
	
	
}



//	Up key処理
//  manual モードの場合: pid_mvの値を1増やす。 (max = 99　% )
//  auto モードの場合: 
//     SV(SP)表示時　: pid_svの値を1増やす。　(max =100.0 ℃)
//         P 表示時  : pid_p の値を1増やす。  (max =100.0 %) (0～100℃に対する %)
//         I 表示時  ; pid_i の値を1増やす。  (max = 360.0 秒)
//         D 表示時  : pid_d の値を1増やす。  (max = 360.0 秒)

void key_up(void)
{
		
	if ( mode_auto_manual == 1 ) {	// manual モードの場合
		
		if ( pid_mv < 100.0) {
			pid_mv = pid_mv + 1.0;
		}
	}
	
	
	else{				// auto モードの場合
	
		if ( para_index == 0 ) {	//  SV(SP)表示時
			if ( pid_sv < 100.0) {
				pid_sv = pid_sv + 0.1;
			}
		}
		else if ( para_index == 1 ) {	//  P 表示時
			if ( pid_p < 100.0) {
				pid_p = pid_p + 0.1;
			}
		}
		else if ( para_index == 2 ) {	//  I 表示時
			if ( pid_i < 3600 ) {
				pid_i = pid_i + 1.0;
			}
		}
		else if ( para_index == 3 ) {	//  D 表示時
			if ( pid_d < 3600 ) {
				pid_d = pid_d + 1.0;
			}
		}
		else if ( para_index == 4 ) {	//  MR 表示時
			if ( pid_mr < 100.0 ) {
				pid_mr = pid_mr + 0.1;
			}
		}
		else if ( para_index == 5 ) {	//  Hys 表示時
			if ( pid_hys < 100.0 ) {
				pid_hys = pid_hys + 0.1;
			}
		}
		else if ( para_index == 6 ) {	//  heat_cool 表示時
			if ( heat_cool < 1) {
				 heat_cool = heat_cool + 1;
			}
			
		}
		
		
		
		
	
	}
	
}


//	Down key処理 
//  manual モードの場合: pid_mvの値を1減ら。 (min = 0　% )
//  auto   モードの場合: 
//     SV(SP)表示時　: pid_svの値を1減らす。　(min = 0.0 ℃)
//         P 表示時  : pid_p の値を1減らす。  (min =0 %) (0～100℃に対する %)
//         I 表示時  ; pid_i の値を1減らす。  (min = 0 秒)
//         D 表示時  : pid_d の値を1減らす。  (min = 0 秒)
void key_down(void)
{

	if ( mode_auto_manual == 1 ) {	// manual モードの場合
		if ( pid_mv > 0.0) {
			pid_mv = pid_mv - 1.0;
		}
	
	}
	else{				// auto モードの場合
		if ( para_index == 0 ) {	//  SV(SP)表示時
			if ( pid_sv >= 0.1) {
				pid_sv = pid_sv - 0.1;
			}
		}
		else if ( para_index == 1 ) {	//  P 表示時
			if ( pid_p >= 0.1) {
				pid_p = pid_p - 0.1;
			}
		}
		else if ( para_index == 2 ) {	//  I 表示時
			if ( pid_i >= 1.0 ) {
				pid_i = pid_i - 1.0;
			}
		}
		else if ( para_index == 3 ) {	//  D 表示時
			if ( pid_d >= 1.0 ) {
				pid_d = pid_d - 1.0;
			}
		}
		else if ( para_index == 4 ) {	//  MR 表示時
			if ( pid_mr >= 0.1 ) {
				pid_mr = pid_mr - 0.1;
			}
		}
		else if ( para_index == 5 ) {	//  Hys 表示時
			if ( pid_hys >= 0.1 ) {
				pid_hys = pid_hys - 0.1;
			}
		}
		else if ( para_index == 6 ) {	//  heat_cool 表示時
			if ( heat_cool > 0 ) {
				 heat_cool = heat_cool - 1;
			}
		
		}
	
	}	
	
}
