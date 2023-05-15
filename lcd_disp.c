#include "iodefine.h"
#include  "misratypes.h"
#include "lcd_disp.h"
#include "ILI9488_9bit_dma.h"

#include "font_16_32.h"
#include "font_64_64.h"
#include "font_48_24.h"
#include "font_24_24.h"

#include "pid.h"


//
// 表示項目名の表示
//   PV,MV,Ch1,Ch2,Ch3,Ch4の表示
//
void disp_name(void)
{
	
	disp_s16_font( "PV", 2, 30, 32, COLOR_WHITE); 	//  項目名 PV　の表示
	 
	disp_s16_font( "SV", 2, 30, 150, COLOR_WHITE); 	//  項目名 SV　の表示
	
	disp_s24_24_font( INDEX_FONT_S24_MV, 30 , 230 , COLOR_WHITE );	// 項目名 MVの表示 (24x24に2文字入っている)
}





//
// パラメータ項目名 SV,P,I,D の表示
// フォントサイズ: 16(W)x32(H), データ: 16x(32/2)=256 byte
// 入力 :表示内容を示す index
//
// ( 0:SV, 1:P, 2:I, 3:D, 4:MR, 5:Hys, 6:H/C , 7:Ch1, 8:Ch2, 9:Ch3, 10:Ch4, 11:CJT )
//
void disp_symbol_para( uint8_t id )
{
	uint32_t st_col;
	uint32_t st_page;
	
	st_col = 30;
	st_page = 150;
	
	
	if ( id == 0) {			 // "SV"表示の場合
		disp_s16_font( "SV ", 3, st_col, st_page, COLOR_WHITE);
	}

	else if ( id == 1) {			 // "P" 表示の場合
		disp_s16_font( "P  ", 3, st_col, st_page, COLOR_WHITE);
	}
	
	else if ( id == 2) {			 // "I" 表示の場合
		disp_s16_font( "I  ", 3, st_col, st_page, COLOR_WHITE);
	}
	
	else if ( id == 3) {			 // "D" 表示の場合
		disp_s16_font( "D  ", 3, st_col, st_page, COLOR_WHITE);
	}
	
	else if ( id == 4) {			 // "MR" 表示の場合
		disp_s16_font( "MR ", 3, st_col, st_page, COLOR_WHITE);
	}
	
	else if ( id == 5) {			 // "Hys" 表示の場合
		disp_s16_font( "Hys", 3, st_col, st_page, COLOR_WHITE);
	}
	
	else if ( id == 6) {			 // "H/C" 表示の場合
		disp_s16_font( "H/C", 3, st_col, st_page, COLOR_WHITE);
	}
	
	else if ( id == 7) {			 // "Ch2"表示の場合
		disp_s16_font( "ch2", 3, st_col, st_page, COLOR_WHITE);
	}

	else if ( id == 8) {			// "CJT" 表示の場合
		disp_s16_font( "CJT", 3, st_col, st_page, COLOR_WHITE);
	}
	
	
	
}






//
//  MV値の表示
//  MV値を、棒グラフと数字で表示
//  
//
//  例: pid_mv = 20.5 ならば、
//　　　■ ■      20%
//
void disp_mv_bar_data(void)
{
	uint32_t	count;
	uint32_t 	mv_t;
	
	
	mv_t = (uint32_t) pid_out;
	
	disp_mv_data( mv_t );			// MV値の表示
	
	
	if ( mv_t == 0 ){
		count = 0;
	}
	else if (( mv_t > 0  ) && ( mv_t <= 10 )) {
		count = 1;
	}
	else if (( mv_t > 10 ) && (  mv_t <= 20 )){
		count = 2;
	}
	else if (( mv_t > 20 ) && (  mv_t <= 30)){
		count = 3;
	}
	else if ((mv_t > 30 ) && (  mv_t <= 40 )){
		count = 4;
	}
	else if ((mv_t > 40 ) && (  mv_t <= 50 )){
		count = 5;
	}
	else if ((mv_t > 50 ) && (  mv_t <= 60)) {
		count = 6;
	}
	else if ((mv_t > 60 ) && (  mv_t <= 70 )){
		count = 7;
	}
	else if ((mv_t > 70 ) && (  mv_t <= 80 )) {
		count = 8;
	}
	else if ((mv_t > 80 ) && (  mv_t <= 90 )){
		count = 9;
	}
	else if ((mv_t > 90 ) && (  mv_t <= 100 )){
		count = 10;
	}

	 disp_mv_bar( count );	// 棒グラフ表示
	
	
}



//
//  MV値を表示する。
//  
// 例: pid_mv =100の場合、  100%
//     pid_mv =21.5 の場合、 21%
//     pid_mv = 5.5   :       5%
//
void  disp_mv_data(uint32_t mv_val )
{
	
	uint8_t dig1;    // 1桁目 1の位
	uint8_t dig2;    // 2桁目 10の位　
	uint8_t dig3;    // 3桁目 100の位
	
	uint32_t st_col;
	uint32_t st_page;
	uint32_t end_col;
	uint32_t end_page;

	uint32_t index;
	uint8_t *pt;
	
	uint32_t num;
	
	num = 256;		      //  送信データ数 (byte) 256= 16/2 *32 ( 1pixel = 2byte) 
	
	
	// 100の位の表示位置
	st_col = 250;
	st_page = 230;
	
	
	end_col = st_col + 15;		// 16 = 16 - 1	
	end_page = st_page + 31;	// 31 = 32 - 1			
	
	lcd_adrs_set(st_col, st_page, end_col, end_page );	 // 書き込み範囲指定 
	spi_cmd_2C_send();	  // Memory Write (2Ch)  先頭位置(コマンド2a,2bで指定した位置)からデータ書き込み		
					   // 100の位の表示
	dig3 = mv_val / 100;
	
	if ( dig3 == 0 ) {
		index = INDEX_FONT_S16_SPACE;
	}
	else{
	        index = dig3;
	}
	
 	pt = (uint8_t *)&font_16w_32h_seg[index][0];  // フォントデータの格納アドレス
	unpack_font_data ( 64 , pt ,0 );  			// フォントデータをRGB111へ展開  64 = (16/8 *32)  
        rspi_data_send(num, (uint16_t *)&rgb111_data_buf[0]);           //  データ送信 256 = 16/2 *32 ( 1pixel = 2byte) 
	
	
	
				        // 10の位の表示位置
	st_col = end_col + 1;		// 開始カラムは、前回の表示文字のend_col + 1
	end_col = st_col + 15;		// 11 = 16 - 1	
	
	
	lcd_adrs_set(st_col, st_page, end_col, end_page );	 // 書き込み範囲指定 
	spi_cmd_2C_send();	  // Memory Write (2Ch)  先頭位置(コマンド2a,2bで指定した位置)からデータ書き込み	
	

					   // 10の位の表示
	dig2 = ( mv_val - (dig3 * 100) ) / 10;

	if ( ( dig3 == 0 ) && ( dig2 == 0 )) {
		index = INDEX_FONT_S16_SPACE;
	}
	else{
	        index = dig2;
	}
	
	pt = (uint8_t *)&font_16w_32h_seg[index][0];  // フォントデータの格納アドレス
	unpack_font_data ( 64 , pt ,0);  			// フォントデータをRGB111へ展開  64 = (16/8 *32)  
	rspi_data_send(num, (uint16_t *)&rgb111_data_buf[0]);           //  データ送信 256 = 16/2 *32 ( 1pixel = 2byte) 
	
	
				        // 1の位の表示位置
	st_col = end_col + 1;		// 開始カラムは、前回の表示文字のend_col + 1
	end_col = st_col + 15;		// 15 = 16 - 1	
	
	lcd_adrs_set(st_col, st_page, end_col, end_page );	 // 書き込み範囲指定 
	spi_cmd_2C_send();	  // Memory Write (2Ch)  先頭位置(コマンド2a,2bで指定した位置)からデータ書き込み	
	
	dig1 = mv_val - (dig3 * 100) - ( dig2 * 10 );	// 1の位の表示	
	
	index = dig1;
	pt = (uint8_t *)&font_16w_32h_seg[index][0];  // フォントデータの格納アドレス
	unpack_font_data ( 64 , pt ,0);  			// フォントデータをRGB111へ展開  64 = (16/8 *32)  
   
	rspi_data_send(num, (uint16_t *)&rgb111_data_buf[0]);           //  データ送信 256 = 16/2 *32 ( 1pixel = 2byte) 
	
	
	
				        // %の表示位置
	st_col = end_col + 1;		// 開始カラムは、前回の表示文字のend_col + 1
	end_col = st_col + 15;		// 15 = 16 - 1	
	
	lcd_adrs_set(st_col, st_page, end_col, end_page );	 // 書き込み範囲指定 
        spi_cmd_2C_send();	  // Memory Write (2Ch)  先頭位置(コマンド2a,2bで指定した位置)からデータ書き込み
	

	index = INDEX_FONT_S16_PERCENT;
	pt = (uint8_t *)&font_16w_32h_seg[index][0];  // フォントデータの格納アドレス
	unpack_font_data ( 64 , pt ,0);  			// フォントデータをRGB111へ展開  64 = (16/8 *32)  
     
	rspi_data_send(num, (uint16_t *)&rgb111_data_buf[0]);           //  データ送信 256 = 16/2 *32 ( 1pixel = 2byte) 
}



//
//  MV値を、棒グラフのように表示する。
//    入力: cnt , 表示個数 (0 ～ 10 )
//
void  disp_mv_bar(uint32_t cnt)
{
	uint32_t ini_col;
	uint32_t ini_row;
	
	uint32_t st_col;
	uint32_t st_row;
	
	uint32_t end_col;
	uint32_t end_row;

	uint32_t i;
	uint32_t j;

	uint32_t index;

	uint8_t *pt;
	
	uint32_t num;
	
	num = 256;		      //  送信データ数 (byte) 256= 16/2 *32 ( 1pixel = 2byte) 
		
	
	ini_col = 80;		// 最初の表示位置
	ini_row = 230;
	
	st_row = ini_row;		// 棒グラフ表示開始　(y)
	end_row = st_row + 31;		// 31 = 32 - 1 

							
	index =  INDEX_FONT_S16_RECTANGLE;	// 長方形(白)の表示 (棒グラフ用)
	
	for ( i = 0 ;  i < cnt  ; i++ ) {		// cnt分を表示
	
	       pt = (uint8_t *)&font_16w_32h_seg[index][0];  // フォントデータの格納アドレス
	       unpack_font_data ( 64 , pt , COLOR_WHITE);  // フォントデータをRGB111へ展開 
	
	  	st_col = i * 16 + ini_col;    //  	
	  	end_col = st_col + 15;   // 15 = 16 - 1	     
	
	 	lcd_adrs_set(st_col, st_row, end_col, end_row);	// 書き込み範囲指定(コマンド 2aとコマンド 2b) (開始カラム 開始ページ, 終了カラム, 終了ページ)
        	spi_cmd_2C_send();	  // Memory Write (2Ch)  先頭位置(コマンド2a,2bで指定した位置)からデータ書き込み

		rspi_data_send(num, (uint16_t *)&rgb111_data_buf[0]);           //  データ送信 256 = 16/2 *32 ( 1pixel = 2byte) 

	 }

	index = INDEX_FONT_S16_SPACE;		// (棒グラフ　クリア用)
	
	for ( j = cnt ;  j < 10  ; j++ ) {		// 10-cnt分をクリア
	    
	        pt = (uint8_t *)&font_16w_32h_seg[index][0];  // フォントデータの格納アドレス
	        unpack_font_data ( 64 , pt , COLOR_WHITE);  // フォントデータをRGB111へ展開 
	  	
		st_col = j * 16 + ini_col;    //  	
	  	end_col = st_col + 15;   // 15 = 16 - 1	     
	
	 	lcd_adrs_set(st_col, st_row, end_col, end_row);	// 書き込み範囲指定(コマンド 2aとコマンド 2b) (開始カラム 開始ページ, 終了カラム, 終了ページ)
        	spi_cmd_2C_send();	  // Memory Write (2Ch)  先頭位置(コマンド2a,2bで指定した位置)からデータ書き込み

		rspi_data_send(num, (uint16_t *)&rgb111_data_buf[0]);           //  データ送信 256 = 16/2 *32 ( 1pixel = 2byte) 

	 }
}





//
//   stop, run の表示
//
// 入力    :表示内容
//  id = 0 : stop
//     = 1 :run 
// 
//  表示文字数は、表示する最大文字列(この場合　stop)と同じにする。
//   stopからrunに表示を変更した場合、runの3文字表示では、stopの p が残ってしまうため、
//  "run "として、runの後にスペースを1つ入れる。
//
void disp_symbol_stop_run(uint8_t id)
{
	
	uint32_t st_col;
	uint32_t st_row;
	
	st_col = 30;
	st_row = 280;
	
	
	if ( id == 0 ) {		// STOP 表示の場合
		disp_s48_24_font( INDEX_FONT_S48_24_STOP, st_col, st_row, COLOR_WHITE);
	}
	
	else if ( id == 1)  {		// RUN 表示の場合 
	       disp_s48_24_font( INDEX_FONT_S48_24_RUN, st_col, st_row, COLOR_WHITE);
	}
	
}



//
//  manual, auto の表示
//
// 入力    :表示内容
//  id = 0 : auto
//  　 = 1 : manual   
// 

void disp_symbol_auto_manual(uint8_t id)
{
	
	uint32_t st_col;
	uint32_t st_row;
	
	st_col = 90;
	st_row = 280;
	
	if ( id == 0 ) {	//  AUTO 表示
	   disp_s48_24_font( INDEX_FONT_S48_24_AUTO, st_col, st_row, COLOR_WHITE);
	}
	
	else if ( id == 1 ) {		// MAN 表示の場合 manual
	   disp_s48_24_font( INDEX_FONT_S48_24_MAN, st_col, st_row, COLOR_WHITE);
	}

}


//
//  heater,cooler, heat and cool の表示
//
// 入力    :表示内容
//  id = 0 : heater    
//  　 = 1 : cooler   
//     = 2 : heat and cool
// 

void disp_symbol_heat_cool(uint32_t id)
{
	
	uint32_t st_col;
	uint32_t st_row;
	
	st_col = 150;
	st_row = 280;
	
	
	if ( id == 0 ) {		// HEAT表示の場合
		disp_s48_24_font( INDEX_FONT_S48_24_HEAT, st_col, st_row, COLOR_WHITE);
	}
	
	else if ( id == 1 ) {		// COOL 表示の場合
		disp_s48_24_font( INDEX_FONT_S48_24_COOL, st_col, st_row, COLOR_WHITE);
	}
	
}



//
//  文字列の表示  (フォントサイズ: 16(W)x32(H) 用)
// 
// 入力:  *pt_str       : 表示文字列が格納されている先頭アドレス
//          num         :　表示文字数
//        st_col        : 書き込み先頭 カラム (x)
//        st_row        : 書き込み先頭 ページ (y)       
//        color         :文字の表示色(0:白, 1:緑, 2:赤:, 3:青, 4:黄, 5:シアン, 6:マゼンタ)
//

void disp_s16_font( uint8_t *pt_str, uint8_t num, uint32_t st_col, uint32_t st_row, uint8_t color )
{
	uint8_t	i;
	uint8_t index;
	
	uint32_t end_col;
	uint32_t end_row;
	uint8_t *pt;

	end_col = st_col + 15;	      // 1文字の書き込み終了カラム
	end_row = st_row + 31;        // 　　　　　　:    　ページ
	
	for ( i = 0 ; i < num ; i++ ) {  // 表示文字数分の繰り返し
		
	   index =  s16_font_index ( *pt_str ); // 表示文字のフォントデータ用 インデックスを得る 
	   
	   pt = (uint8_t *)&font_16w_32h_seg[index][0];  // フォントデータの格納アドレス
	   
	   unpack_font_data ( 64 , pt , color);  // フォントデータをRGB111へ展開 
	   
	   lcd_adrs_set(st_col, st_row, end_col, end_row );	 // 書き込み範囲指定 
           
	   spi_cmd_2C_send();	  // Memory Write (2Ch)  先頭位置(コマンド2a,2bで指定した位置)からデータ書き込み				   
						   
           rspi_data_send(256, (uint16_t *)&rgb111_data_buf[0]);           //  ピクセルデータ送信
	    
						// 次の文字のカラム位置	  
	   st_col = end_col + 1;		// 前の文字の最終カラム + 1
	   end_col = st_col + 15;
	  
 	   pt_str++;				// 次の文字の格納位置
	}

}


//
//  文字のフォントデータインデックスを調べる  (フォントサイズ: 16(W)x32(H) 用)
//
uint8_t  s16_font_index( uint8_t cd ){

	uint8_t id;
	

        if ( cd == 'P'){
		id = INDEX_FONT_S16_P;
	}
	else if ( cd == 'V'){
		id = INDEX_FONT_S16_V;
	}
	else if ( cd == 'S' ) {
		id = INDEX_FONT_S16_S;
	}
        else if ( cd == 'M' ) {
		id = INDEX_FONT_S16_M;
	}
	else if ( cd == ' ' ) {
		id =  INDEX_FONT_S16_SPACE;
	}
	
	
	else if ( cd == 'I' ){
		id = INDEX_FONT_S16_I;
	}
	else if ( cd == 'D' ){
		id = INDEX_FONT_S16_D;
	}
	
	else if ( cd == 'M' ){
		id = INDEX_FONT_S16_M;
	}
	else if ( cd == 'R' ){
		id = INDEX_FONT_S16_R;
	}
	
	else if ( cd == 'H' ) {
		id = INDEX_FONT_S16_H;
	}
	else if ( cd == 'y' ) {
		id = INDEX_FONT_S16_S_Y;
	}
	else if ( cd == 's' ) {
		id = INDEX_FONT_S16_S_S;
	}
	
	else if ( cd == '/' ) {
		id = INDEX_FONT_S16_SLASH ;
	}
	
	
	else if ( cd == 'C' ){
		id = INDEX_FONT_S16_C;
	}
	
	else if ( cd == 't' ) {
		id =  INDEX_FONT_S16_S_T;
	}
	
	else if ( cd == 'l' ) {
		id =  INDEX_FONT_S16_S_L;
	}
	else if ( cd == 'J' ){
		id = INDEX_FONT_S16_J;
	}
	else if ( cd == 'T' ){
		id = INDEX_FONT_S16_T;
	}
	
	else if ( cd == 'h' ){
		id = INDEX_FONT_S16_S_H;
	}
	
	else if ( cd == 'c' ){
		id = INDEX_FONT_S16_S_C;
	}
	
	else if ( cd == '1' ){
		id = 1;
	}
	else if ( cd == '2' ){
		id = 2;
	}
	else if ( cd == '3' ){
		id = 3;
	}
	else if ( cd == '4' ){
		id = 4;
	}
	
	return	id;
}







//  文字の表示  (フォントサイズ: 24(W)x24(H) 用)
// 
// 入力:  dt_index        : 表示文字列のindex
//        st_col        : 書き込み先頭 カラム (x)
//        st_row        : 書き込み先頭 ページ (y)
//        color         :文字の表示色(0:白, 1:緑, 2:赤:, 3:青, 4:黄, 5:シアン, 6:マゼンタ)
//  フォントデータのサイズ(byte):  72 = 24/8 * 24
//  送信データ数 (byte):　 288 = 24/2 * 24    (2pixelで 1byte) 
//
void disp_s24_24_font( uint8_t dt_index,  uint32_t st_col, uint32_t st_row ,uint8_t color)
{
	
	uint32_t end_col;
	uint32_t end_row;
	uint8_t *pt;

	uint32_t num;
	
	num = 288;		      //  送信データ数 (byte)
	
	
	end_col = st_col + 23;	      // 文字の書き込み終了カラム
	end_row = st_row + 23;        // 　　　　　　:    　ページ
	
	pt = (uint8_t *)&font_24w_24h_seg[dt_index][0];  // フォントデータの格納アドレス
	
	unpack_font_data ( 72 , pt , color);  // フォントデータをRGB111へ展開 
	   
	lcd_adrs_set(st_col, st_row, end_col, end_row );	 // 書き込み範囲指定 
        
	spi_cmd_2C_send();	  // Memory Write (2Ch)  先頭位置(コマンド2a,2bで指定した位置)からデータ書き込み				   
						   
        rspi_data_send( num, (uint16_t *)&rgb111_data_buf[0]);           //  ピクセルデータ送信  
	
}



//  文字の表示  (フォントサイズ: 48(W)x24(H) 用)
// 
// 入力:  dt_index      : 表示文字列のindex
//        st_col        : 書き込み先頭 カラム (x)
//        st_row        : 書き込み先頭 ページ (y)
//        color         :文字の表示色(0:白, 1:緑, 2:赤:, 3:青, 4:黄, 5:シアン, 6:マゼンタ)
//
//  フォントデータのサイズ(byte):  144 = 48/8 * 24
//  送信データ数 (byte):　 576 = 48/2 * 24    (2pixel で 1byte) 
//

void disp_s48_24_font( uint8_t dt_index,  uint32_t st_col, uint32_t st_row ,uint8_t color)
{
	
	uint32_t end_col;
	uint32_t end_row;
	uint8_t *pt;

	uint32_t num;
	
	num = 576;		      //  送信データ数 (byte)
	
	end_col = st_col + 47;	      // 文字の書き込み終了カラム
	end_row = st_row + 23;        // 　　　　　　:    　ページ
	
	
	pt = (uint8_t *)&font_48w_24h_seg[dt_index][0];  // フォントデータの格納アドレス
	
	unpack_font_data ( 144 , pt ,color);  // フォントデータをRGB111へ展開 
	   
	lcd_adrs_set(st_col, st_row, end_col, end_row );	 // 書き込み範囲指定 
           
	
	spi_cmd_2C_send();	  // Memory Write (2Ch)  先頭位置(コマンド2a,2bで指定した位置)からデータ書き込み				   
						   
        rspi_data_send( num, (uint16_t *)&rgb111_data_buf[0]);           //  ピクセルデータ送信  
	
}

// スイッチの表示
//
void disp_switch(void)
{
	disp_switch_square_id( 1, 32, 330);	// Auto/Manual
	
	disp_switch_square_id( 0, 32, 410);	// Stop/RUN
	
	disp_switch_square_id( 2, 128, 330);	// Para
	
	disp_switch_square_id( 5, 128, 410);	// Save
	
	disp_switch_square_id( 4, 224, 330);	// ▲  ( Up )
	
	disp_switch_square_id( 3, 224, 410);	// ▼　(Down)
	
}


//
//  indexで示されたスイッチ用の四角形(64x64 pixel)を描く
//  入力: index : 表示するスイッチのIndex (0:Stop/RUN, 1:Auto/Manual, 2:Para, 3:▼　(Down), 4: ▲  ( Up )
//        st_col: 開始 col
//        st_row: 開始 row
//
void disp_switch_square_id( uint8_t index, uint16_t st_col, uint16_t st_row)
{
	uint8_t *pt;
	uint32_t num;
	
	
	num = (64/2) * 64;		// 送信データ数 (1byte = 2pielの情報)
	
	
	pt = (uint8_t *)&font_64w_64h_seg[index][0];  // フォントデータの格納アドレス
	     
	unpack_font_data ( 512 , pt, COLOR_CYAN);  // フォントデータをRGB111へ展開 色=シアン 
	
	lcd_adrs_set(st_col, st_row, st_col + 63, st_row + 63);     // 書き込み範囲指定(コマンド 2aとコマンド 2b) 
	     
	spi_cmd_2C_send();	  // Memory Write (2Ch)  先頭位置(コマンド2a,2bで指定した位置)からデータ書き込み				   
						   
        rspi_data_send( num, (uint16_t *)&rgb111_data_buf[0]);           //  ピクセルデータ送信
}

