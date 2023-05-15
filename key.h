
// SWの個数
#define KEY_SW_NUM	6	// SWは6個  STOP/RUN(SW0),Auto/Manual(SW1),Para(SW2),▼(Down)(SW3),▲(Up)(SW4), Save(SW5)

// Up キー,Downキーの長押し時の、処理回数
#define LONG_PUSH_CNT   5	// 長押しは、1回押しの処理を5回繰り返す

// スイッチ入力関係の構造体
struct SW_info
{
	uint8_t status;		// 今回の タッチ(Low=0),非タッチ(High=1) 状態 (10msec毎)
	uint8_t pre_status;	// 前回の   :
	uint8_t low_cnt;	// タッチ(ON)の回数
	uint8_t one_push;	// 0:キー入力処理要求なし 1:キー入力処理要求(1度押し)　（キー入力処理、表示処理終了後に0クリア）
        uint8_t long_push;      // 0:キー入力処理要求なし 1:キー入力処理要求(長押し)　 ( Low→High の立上がり検出時に 0クリア)
	
};


#define PARA_MAX_NUMBER  9	// パラメータの個数 9 (0:SV, 1:P, 2:I, 3:D, 4:MR, 5:Hys, 6:H/C, 7:Ch2, 8:CJT)
// 旧仕様 パラメータの個数 12 (0:SV, 1:P, 2:I, 3:D, 4:MR, 5:Hys, 6:H/C, 7:Ch1, 8:Ch2, 9:Ch3, 10:Ch4, 11:CJT)

void touch_pre_status_ini(void);

void touch_xyz_press(void);
void touch_key_status_check(void);
void switch_input_check( uint8_t id ); 

void key_input(void);	

void key_up(void);
void key_down(void);

extern	uint8_t  para_index;

extern volatile struct  SW_info  Key_sw[KEY_SW_NUM];	// スイッチ　5個分の情報格納領域
