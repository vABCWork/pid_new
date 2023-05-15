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
// �\�����ږ��̕\��
//   PV,MV,Ch1,Ch2,Ch3,Ch4�̕\��
//
void disp_name(void)
{
	
	disp_s16_font( "PV", 2, 30, 32, COLOR_WHITE); 	//  ���ږ� PV�@�̕\��
	 
	disp_s16_font( "SV", 2, 30, 150, COLOR_WHITE); 	//  ���ږ� SV�@�̕\��
	
	disp_s24_24_font( INDEX_FONT_S24_MV, 30 , 230 , COLOR_WHITE );	// ���ږ� MV�̕\�� (24x24��2���������Ă���)
}





//
// �p�����[�^���ږ� SV,P,I,D �̕\��
// �t�H���g�T�C�Y: 16(W)x32(H), �f�[�^: 16x(32/2)=256 byte
// ���� :�\�����e������ index
//
// ( 0:SV, 1:P, 2:I, 3:D, 4:MR, 5:Hys, 6:H/C , 7:Ch1, 8:Ch2, 9:Ch3, 10:Ch4, 11:CJT )
//
void disp_symbol_para( uint8_t id )
{
	uint32_t st_col;
	uint32_t st_page;
	
	st_col = 30;
	st_page = 150;
	
	
	if ( id == 0) {			 // "SV"�\���̏ꍇ
		disp_s16_font( "SV ", 3, st_col, st_page, COLOR_WHITE);
	}

	else if ( id == 1) {			 // "P" �\���̏ꍇ
		disp_s16_font( "P  ", 3, st_col, st_page, COLOR_WHITE);
	}
	
	else if ( id == 2) {			 // "I" �\���̏ꍇ
		disp_s16_font( "I  ", 3, st_col, st_page, COLOR_WHITE);
	}
	
	else if ( id == 3) {			 // "D" �\���̏ꍇ
		disp_s16_font( "D  ", 3, st_col, st_page, COLOR_WHITE);
	}
	
	else if ( id == 4) {			 // "MR" �\���̏ꍇ
		disp_s16_font( "MR ", 3, st_col, st_page, COLOR_WHITE);
	}
	
	else if ( id == 5) {			 // "Hys" �\���̏ꍇ
		disp_s16_font( "Hys", 3, st_col, st_page, COLOR_WHITE);
	}
	
	else if ( id == 6) {			 // "H/C" �\���̏ꍇ
		disp_s16_font( "H/C", 3, st_col, st_page, COLOR_WHITE);
	}
	
	else if ( id == 7) {			 // "Ch2"�\���̏ꍇ
		disp_s16_font( "ch2", 3, st_col, st_page, COLOR_WHITE);
	}

	else if ( id == 8) {			// "CJT" �\���̏ꍇ
		disp_s16_font( "CJT", 3, st_col, st_page, COLOR_WHITE);
	}
	
	
	
}






//
//  MV�l�̕\��
//  MV�l���A�_�O���t�Ɛ����ŕ\��
//  
//
//  ��: pid_mv = 20.5 �Ȃ�΁A
//�@�@�@�� ��      20%
//
void disp_mv_bar_data(void)
{
	uint32_t	count;
	uint32_t 	mv_t;
	
	
	mv_t = (uint32_t) pid_out;
	
	disp_mv_data( mv_t );			// MV�l�̕\��
	
	
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

	 disp_mv_bar( count );	// �_�O���t�\��
	
	
}



//
//  MV�l��\������B
//  
// ��: pid_mv =100�̏ꍇ�A  100%
//     pid_mv =21.5 �̏ꍇ�A 21%
//     pid_mv = 5.5   :       5%
//
void  disp_mv_data(uint32_t mv_val )
{
	
	uint8_t dig1;    // 1���� 1�̈�
	uint8_t dig2;    // 2���� 10�̈ʁ@
	uint8_t dig3;    // 3���� 100�̈�
	
	uint32_t st_col;
	uint32_t st_page;
	uint32_t end_col;
	uint32_t end_page;

	uint32_t index;
	uint8_t *pt;
	
	uint32_t num;
	
	num = 256;		      //  ���M�f�[�^�� (byte) 256= 16/2 *32 ( 1pixel = 2byte) 
	
	
	// 100�̈ʂ̕\���ʒu
	st_col = 250;
	st_page = 230;
	
	
	end_col = st_col + 15;		// 16 = 16 - 1	
	end_page = st_page + 31;	// 31 = 32 - 1			
	
	lcd_adrs_set(st_col, st_page, end_col, end_page );	 // �������ݔ͈͎w�� 
	spi_cmd_2C_send();	  // Memory Write (2Ch)  �擪�ʒu(�R�}���h2a,2b�Ŏw�肵���ʒu)����f�[�^��������		
					   // 100�̈ʂ̕\��
	dig3 = mv_val / 100;
	
	if ( dig3 == 0 ) {
		index = INDEX_FONT_S16_SPACE;
	}
	else{
	        index = dig3;
	}
	
 	pt = (uint8_t *)&font_16w_32h_seg[index][0];  // �t�H���g�f�[�^�̊i�[�A�h���X
	unpack_font_data ( 64 , pt ,0 );  			// �t�H���g�f�[�^��RGB111�֓W�J  64 = (16/8 *32)  
        rspi_data_send(num, (uint16_t *)&rgb111_data_buf[0]);           //  �f�[�^���M 256 = 16/2 *32 ( 1pixel = 2byte) 
	
	
	
				        // 10�̈ʂ̕\���ʒu
	st_col = end_col + 1;		// �J�n�J�����́A�O��̕\��������end_col + 1
	end_col = st_col + 15;		// 11 = 16 - 1	
	
	
	lcd_adrs_set(st_col, st_page, end_col, end_page );	 // �������ݔ͈͎w�� 
	spi_cmd_2C_send();	  // Memory Write (2Ch)  �擪�ʒu(�R�}���h2a,2b�Ŏw�肵���ʒu)����f�[�^��������	
	

					   // 10�̈ʂ̕\��
	dig2 = ( mv_val - (dig3 * 100) ) / 10;

	if ( ( dig3 == 0 ) && ( dig2 == 0 )) {
		index = INDEX_FONT_S16_SPACE;
	}
	else{
	        index = dig2;
	}
	
	pt = (uint8_t *)&font_16w_32h_seg[index][0];  // �t�H���g�f�[�^�̊i�[�A�h���X
	unpack_font_data ( 64 , pt ,0);  			// �t�H���g�f�[�^��RGB111�֓W�J  64 = (16/8 *32)  
	rspi_data_send(num, (uint16_t *)&rgb111_data_buf[0]);           //  �f�[�^���M 256 = 16/2 *32 ( 1pixel = 2byte) 
	
	
				        // 1�̈ʂ̕\���ʒu
	st_col = end_col + 1;		// �J�n�J�����́A�O��̕\��������end_col + 1
	end_col = st_col + 15;		// 15 = 16 - 1	
	
	lcd_adrs_set(st_col, st_page, end_col, end_page );	 // �������ݔ͈͎w�� 
	spi_cmd_2C_send();	  // Memory Write (2Ch)  �擪�ʒu(�R�}���h2a,2b�Ŏw�肵���ʒu)����f�[�^��������	
	
	dig1 = mv_val - (dig3 * 100) - ( dig2 * 10 );	// 1�̈ʂ̕\��	
	
	index = dig1;
	pt = (uint8_t *)&font_16w_32h_seg[index][0];  // �t�H���g�f�[�^�̊i�[�A�h���X
	unpack_font_data ( 64 , pt ,0);  			// �t�H���g�f�[�^��RGB111�֓W�J  64 = (16/8 *32)  
   
	rspi_data_send(num, (uint16_t *)&rgb111_data_buf[0]);           //  �f�[�^���M 256 = 16/2 *32 ( 1pixel = 2byte) 
	
	
	
				        // %�̕\���ʒu
	st_col = end_col + 1;		// �J�n�J�����́A�O��̕\��������end_col + 1
	end_col = st_col + 15;		// 15 = 16 - 1	
	
	lcd_adrs_set(st_col, st_page, end_col, end_page );	 // �������ݔ͈͎w�� 
        spi_cmd_2C_send();	  // Memory Write (2Ch)  �擪�ʒu(�R�}���h2a,2b�Ŏw�肵���ʒu)����f�[�^��������
	

	index = INDEX_FONT_S16_PERCENT;
	pt = (uint8_t *)&font_16w_32h_seg[index][0];  // �t�H���g�f�[�^�̊i�[�A�h���X
	unpack_font_data ( 64 , pt ,0);  			// �t�H���g�f�[�^��RGB111�֓W�J  64 = (16/8 *32)  
     
	rspi_data_send(num, (uint16_t *)&rgb111_data_buf[0]);           //  �f�[�^���M 256 = 16/2 *32 ( 1pixel = 2byte) 
}



//
//  MV�l���A�_�O���t�̂悤�ɕ\������B
//    ����: cnt , �\���� (0 �` 10 )
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
	
	num = 256;		      //  ���M�f�[�^�� (byte) 256= 16/2 *32 ( 1pixel = 2byte) 
		
	
	ini_col = 80;		// �ŏ��̕\���ʒu
	ini_row = 230;
	
	st_row = ini_row;		// �_�O���t�\���J�n�@(y)
	end_row = st_row + 31;		// 31 = 32 - 1 

							
	index =  INDEX_FONT_S16_RECTANGLE;	// �����`(��)�̕\�� (�_�O���t�p)
	
	for ( i = 0 ;  i < cnt  ; i++ ) {		// cnt����\��
	
	       pt = (uint8_t *)&font_16w_32h_seg[index][0];  // �t�H���g�f�[�^�̊i�[�A�h���X
	       unpack_font_data ( 64 , pt , COLOR_WHITE);  // �t�H���g�f�[�^��RGB111�֓W�J 
	
	  	st_col = i * 16 + ini_col;    //  	
	  	end_col = st_col + 15;   // 15 = 16 - 1	     
	
	 	lcd_adrs_set(st_col, st_row, end_col, end_row);	// �������ݔ͈͎w��(�R�}���h 2a�ƃR�}���h 2b) (�J�n�J���� �J�n�y�[�W, �I���J����, �I���y�[�W)
        	spi_cmd_2C_send();	  // Memory Write (2Ch)  �擪�ʒu(�R�}���h2a,2b�Ŏw�肵���ʒu)����f�[�^��������

		rspi_data_send(num, (uint16_t *)&rgb111_data_buf[0]);           //  �f�[�^���M 256 = 16/2 *32 ( 1pixel = 2byte) 

	 }

	index = INDEX_FONT_S16_SPACE;		// (�_�O���t�@�N���A�p)
	
	for ( j = cnt ;  j < 10  ; j++ ) {		// 10-cnt�����N���A
	    
	        pt = (uint8_t *)&font_16w_32h_seg[index][0];  // �t�H���g�f�[�^�̊i�[�A�h���X
	        unpack_font_data ( 64 , pt , COLOR_WHITE);  // �t�H���g�f�[�^��RGB111�֓W�J 
	  	
		st_col = j * 16 + ini_col;    //  	
	  	end_col = st_col + 15;   // 15 = 16 - 1	     
	
	 	lcd_adrs_set(st_col, st_row, end_col, end_row);	// �������ݔ͈͎w��(�R�}���h 2a�ƃR�}���h 2b) (�J�n�J���� �J�n�y�[�W, �I���J����, �I���y�[�W)
        	spi_cmd_2C_send();	  // Memory Write (2Ch)  �擪�ʒu(�R�}���h2a,2b�Ŏw�肵���ʒu)����f�[�^��������

		rspi_data_send(num, (uint16_t *)&rgb111_data_buf[0]);           //  �f�[�^���M 256 = 16/2 *32 ( 1pixel = 2byte) 

	 }
}





//
//   stop, run �̕\��
//
// ����    :�\�����e
//  id = 0 : stop
//     = 1 :run 
// 
//  �\���������́A�\������ő啶����(���̏ꍇ�@stop)�Ɠ����ɂ���B
//   stop����run�ɕ\����ύX�����ꍇ�Arun��3�����\���ł́Astop�� p ���c���Ă��܂����߁A
//  "run "�Ƃ��āArun�̌�ɃX�y�[�X��1�����B
//
void disp_symbol_stop_run(uint8_t id)
{
	
	uint32_t st_col;
	uint32_t st_row;
	
	st_col = 30;
	st_row = 280;
	
	
	if ( id == 0 ) {		// STOP �\���̏ꍇ
		disp_s48_24_font( INDEX_FONT_S48_24_STOP, st_col, st_row, COLOR_WHITE);
	}
	
	else if ( id == 1)  {		// RUN �\���̏ꍇ 
	       disp_s48_24_font( INDEX_FONT_S48_24_RUN, st_col, st_row, COLOR_WHITE);
	}
	
}



//
//  manual, auto �̕\��
//
// ����    :�\�����e
//  id = 0 : auto
//  �@ = 1 : manual   
// 

void disp_symbol_auto_manual(uint8_t id)
{
	
	uint32_t st_col;
	uint32_t st_row;
	
	st_col = 90;
	st_row = 280;
	
	if ( id == 0 ) {	//  AUTO �\��
	   disp_s48_24_font( INDEX_FONT_S48_24_AUTO, st_col, st_row, COLOR_WHITE);
	}
	
	else if ( id == 1 ) {		// MAN �\���̏ꍇ manual
	   disp_s48_24_font( INDEX_FONT_S48_24_MAN, st_col, st_row, COLOR_WHITE);
	}

}


//
//  heater,cooler, heat and cool �̕\��
//
// ����    :�\�����e
//  id = 0 : heater    
//  �@ = 1 : cooler   
//     = 2 : heat and cool
// 

void disp_symbol_heat_cool(uint32_t id)
{
	
	uint32_t st_col;
	uint32_t st_row;
	
	st_col = 150;
	st_row = 280;
	
	
	if ( id == 0 ) {		// HEAT�\���̏ꍇ
		disp_s48_24_font( INDEX_FONT_S48_24_HEAT, st_col, st_row, COLOR_WHITE);
	}
	
	else if ( id == 1 ) {		// COOL �\���̏ꍇ
		disp_s48_24_font( INDEX_FONT_S48_24_COOL, st_col, st_row, COLOR_WHITE);
	}
	
}



//
//  ������̕\��  (�t�H���g�T�C�Y: 16(W)x32(H) �p)
// 
// ����:  *pt_str       : �\�������񂪊i�[����Ă���擪�A�h���X
//          num         :�@�\��������
//        st_col        : �������ݐ擪 �J���� (x)
//        st_row        : �������ݐ擪 �y�[�W (y)       
//        color         :�����̕\���F(0:��, 1:��, 2:��:, 3:��, 4:��, 5:�V�A��, 6:�}�[���^)
//

void disp_s16_font( uint8_t *pt_str, uint8_t num, uint32_t st_col, uint32_t st_row, uint8_t color )
{
	uint8_t	i;
	uint8_t index;
	
	uint32_t end_col;
	uint32_t end_row;
	uint8_t *pt;

	end_col = st_col + 15;	      // 1�����̏������ݏI���J����
	end_row = st_row + 31;        // �@�@�@�@�@�@:    �@�y�[�W
	
	for ( i = 0 ; i < num ; i++ ) {  // �\�����������̌J��Ԃ�
		
	   index =  s16_font_index ( *pt_str ); // �\�������̃t�H���g�f�[�^�p �C���f�b�N�X�𓾂� 
	   
	   pt = (uint8_t *)&font_16w_32h_seg[index][0];  // �t�H���g�f�[�^�̊i�[�A�h���X
	   
	   unpack_font_data ( 64 , pt , color);  // �t�H���g�f�[�^��RGB111�֓W�J 
	   
	   lcd_adrs_set(st_col, st_row, end_col, end_row );	 // �������ݔ͈͎w�� 
           
	   spi_cmd_2C_send();	  // Memory Write (2Ch)  �擪�ʒu(�R�}���h2a,2b�Ŏw�肵���ʒu)����f�[�^��������				   
						   
           rspi_data_send(256, (uint16_t *)&rgb111_data_buf[0]);           //  �s�N�Z���f�[�^���M
	    
						// ���̕����̃J�����ʒu	  
	   st_col = end_col + 1;		// �O�̕����̍ŏI�J���� + 1
	   end_col = st_col + 15;
	  
 	   pt_str++;				// ���̕����̊i�[�ʒu
	}

}


//
//  �����̃t�H���g�f�[�^�C���f�b�N�X�𒲂ׂ�  (�t�H���g�T�C�Y: 16(W)x32(H) �p)
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







//  �����̕\��  (�t�H���g�T�C�Y: 24(W)x24(H) �p)
// 
// ����:  dt_index        : �\���������index
//        st_col        : �������ݐ擪 �J���� (x)
//        st_row        : �������ݐ擪 �y�[�W (y)
//        color         :�����̕\���F(0:��, 1:��, 2:��:, 3:��, 4:��, 5:�V�A��, 6:�}�[���^)
//  �t�H���g�f�[�^�̃T�C�Y(byte):  72 = 24/8 * 24
//  ���M�f�[�^�� (byte):�@ 288 = 24/2 * 24    (2pixel�� 1byte) 
//
void disp_s24_24_font( uint8_t dt_index,  uint32_t st_col, uint32_t st_row ,uint8_t color)
{
	
	uint32_t end_col;
	uint32_t end_row;
	uint8_t *pt;

	uint32_t num;
	
	num = 288;		      //  ���M�f�[�^�� (byte)
	
	
	end_col = st_col + 23;	      // �����̏������ݏI���J����
	end_row = st_row + 23;        // �@�@�@�@�@�@:    �@�y�[�W
	
	pt = (uint8_t *)&font_24w_24h_seg[dt_index][0];  // �t�H���g�f�[�^�̊i�[�A�h���X
	
	unpack_font_data ( 72 , pt , color);  // �t�H���g�f�[�^��RGB111�֓W�J 
	   
	lcd_adrs_set(st_col, st_row, end_col, end_row );	 // �������ݔ͈͎w�� 
        
	spi_cmd_2C_send();	  // Memory Write (2Ch)  �擪�ʒu(�R�}���h2a,2b�Ŏw�肵���ʒu)����f�[�^��������				   
						   
        rspi_data_send( num, (uint16_t *)&rgb111_data_buf[0]);           //  �s�N�Z���f�[�^���M  
	
}



//  �����̕\��  (�t�H���g�T�C�Y: 48(W)x24(H) �p)
// 
// ����:  dt_index      : �\���������index
//        st_col        : �������ݐ擪 �J���� (x)
//        st_row        : �������ݐ擪 �y�[�W (y)
//        color         :�����̕\���F(0:��, 1:��, 2:��:, 3:��, 4:��, 5:�V�A��, 6:�}�[���^)
//
//  �t�H���g�f�[�^�̃T�C�Y(byte):  144 = 48/8 * 24
//  ���M�f�[�^�� (byte):�@ 576 = 48/2 * 24    (2pixel �� 1byte) 
//

void disp_s48_24_font( uint8_t dt_index,  uint32_t st_col, uint32_t st_row ,uint8_t color)
{
	
	uint32_t end_col;
	uint32_t end_row;
	uint8_t *pt;

	uint32_t num;
	
	num = 576;		      //  ���M�f�[�^�� (byte)
	
	end_col = st_col + 47;	      // �����̏������ݏI���J����
	end_row = st_row + 23;        // �@�@�@�@�@�@:    �@�y�[�W
	
	
	pt = (uint8_t *)&font_48w_24h_seg[dt_index][0];  // �t�H���g�f�[�^�̊i�[�A�h���X
	
	unpack_font_data ( 144 , pt ,color);  // �t�H���g�f�[�^��RGB111�֓W�J 
	   
	lcd_adrs_set(st_col, st_row, end_col, end_row );	 // �������ݔ͈͎w�� 
           
	
	spi_cmd_2C_send();	  // Memory Write (2Ch)  �擪�ʒu(�R�}���h2a,2b�Ŏw�肵���ʒu)����f�[�^��������				   
						   
        rspi_data_send( num, (uint16_t *)&rgb111_data_buf[0]);           //  �s�N�Z���f�[�^���M  
	
}

// �X�C�b�`�̕\��
//
void disp_switch(void)
{
	disp_switch_square_id( 1, 32, 330);	// Auto/Manual
	
	disp_switch_square_id( 0, 32, 410);	// Stop/RUN
	
	disp_switch_square_id( 2, 128, 330);	// Para
	
	disp_switch_square_id( 5, 128, 410);	// Save
	
	disp_switch_square_id( 4, 224, 330);	// ��  ( Up )
	
	disp_switch_square_id( 3, 224, 410);	// ���@(Down)
	
}


//
//  index�Ŏ����ꂽ�X�C�b�`�p�̎l�p�`(64x64 pixel)��`��
//  ����: index : �\������X�C�b�`��Index (0:Stop/RUN, 1:Auto/Manual, 2:Para, 3:���@(Down), 4: ��  ( Up )
//        st_col: �J�n col
//        st_row: �J�n row
//
void disp_switch_square_id( uint8_t index, uint16_t st_col, uint16_t st_row)
{
	uint8_t *pt;
	uint32_t num;
	
	
	num = (64/2) * 64;		// ���M�f�[�^�� (1byte = 2piel�̏��)
	
	
	pt = (uint8_t *)&font_64w_64h_seg[index][0];  // �t�H���g�f�[�^�̊i�[�A�h���X
	     
	unpack_font_data ( 512 , pt, COLOR_CYAN);  // �t�H���g�f�[�^��RGB111�֓W�J �F=�V�A�� 
	
	lcd_adrs_set(st_col, st_row, st_col + 63, st_row + 63);     // �������ݔ͈͎w��(�R�}���h 2a�ƃR�}���h 2b) 
	     
	spi_cmd_2C_send();	  // Memory Write (2Ch)  �擪�ʒu(�R�}���h2a,2b�Ŏw�肵���ʒu)����f�[�^��������				   
						   
        rspi_data_send( num, (uint16_t *)&rgb111_data_buf[0]);           //  �s�N�Z���f�[�^���M
}

