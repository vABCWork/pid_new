#include	 "iodefine.h"
#include	 "misratypes.h"
#include	 "delay.h"
#include         "pid.h"
#include	 "dataflash.h"
#include	 "crc_16.h"
#include 	 "alarm.h"


uint8_t flash_wr_buf[32];	// �f�[�^�t���b�V���������ݗp�o�b�t�@
uint8_t flash_rd_buf[32];       // �f�[�^�t���b�V���ǂݏo���p�o�b�t�@

uint8_t  crc_x8_x5_x4_1;	// CRC-8 (x8 + x5 + X4 + 1)

//
// PID�p�p�����[�^���A�f�[�^�t���b�V�� �u���b�N0�֏����� (���v 26 byte)
// �� 7msec (erase �� 28byte������)
// �����݃f�[�^(���v 28 byte)
//     No.     item     data type
//	0 	SV	float (4byte)
//	1	P	float (4byte)
//	2	I	float (4byte)
//	3	D	float (4byte)
//	4	MR	float (4byte)
//	5	Hys	float (4byte)
//      6    heat/cool  byte  (1byte)
//      7    pid_type   byte  (1byte)
//      8      CRC      word  (2byte)
//
//  �o��: err_fg: = 0 ����
//              : >0  ���s  

uint32_t  pid_para_flash_write(void)
{
	uint16_t crc_cd;
	
	uint32_t i;
	uint32_t blk_index;
	uint32_t wr_num;
	uint32_t err_fg;
 
	memcpy( &flash_wr_buf[0], &pid_sv, 4);	// SV�l ��flash_data_buf[0]��
	
	memcpy( &flash_wr_buf[4], &pid_p, 4);		// P ��flash_data_buf[4]��
	
	memcpy( &flash_wr_buf[8], &pid_i, 4);		// I ��flash_data_buf[8]��
	
	memcpy( &flash_wr_buf[12], &pid_d, 4);	// D ��flash_data_buf[12]��
	
	memcpy( &flash_wr_buf[16], &pid_mr, 4);	// Mr ��flash_data_buf[16]��
	
	memcpy( &flash_wr_buf[20], &pid_hys, 4);	// Hys ��flash_data_buf[20]��
	
	memcpy( &flash_wr_buf[24], &heat_cool, 1);	// heat_cool ��flash_data_buf[24]��
		
	memcpy( &flash_wr_buf[25], &pid_type, 1);	// pid_type ��flash_data_buf[25]��
	
	
	crc_x8_x5_x4_1 = Calc_crc_x8_x5_x4_1(&flash_wr_buf[0],26);   // CRC-8(X8+X5+X4+1)�̌v�Z

	
	flash_wr_buf[26] = crc_x8_x5_x4_1; // CRC-8
	
	
	
	data_flash_enter_pe_mode();	// E2�f�[�^�t���b�V�������[�h���[�h����P/E ���[�h�ɂ���	
	blk_index = 0;			// �������ݑΏۃu���b�N
	
	err_fg = data_flash_erase( blk_index );	// �C���[�Y
	if ( err_fg > 0 ) {			// �G���[�����̏ꍇ
		data_flash_exit_pe_mode();	//  E2�f�[�^�t���b�V����P/E ���[�h���烊�[�h���[�h�ɂ���
		return err_fg;
	}
	
	wr_num = 27;		// �������݃o�C�g��
  
	err_fg = data_flash_write(blk_index, &flash_wr_buf[0], wr_num);  // ������
	if ( err_fg > 0 ) {			// �G���[�����̏ꍇ
		data_flash_exit_pe_mode();	//  E2�f�[�^�t���b�V����P/E ���[�h���烊�[�h���[�h�ɂ���
		return err_fg;
	}
	
        data_flash_exit_pe_mode();		//  E2�f�[�^�t���b�V����P/E ���[�h���烊�[�h���[�h�ɂ���
	
	return 0;
	
}


//
// �p�����[�^�̃f�[�^�t���b�V������̓ǂݏo���ƁACRC�`�F�b�N
//
//     BLOCK_0:
//     No.     item     data type
//	0 	SV	float (4byte)
//	1	P	float (4byte)
//	2	I	float (4byte)
//	3	D	float (4byte)
//	4	MR	float (4byte)
//	5	Hys	float (4byte)
//      6    heat/cool  byte (1byte)
//      7    pid_type   byte (1byte)
//      8      CRC-8    byte (1byte)
//
// ��)E2�f�[�^�t���b�V�����C���[�Y�����ƁA�f�[�^��0xff�ƂȂ�B
//   �@���������_�f�[�^�� 0xffff fffff�̏ꍇ�ANan(Not a number)�ƂȂ�A
//  �@ SV�l���̃p�����[�^�̒l��Nan�ƂȂ��Ă��܂��̂ŁA���̏ꍇ�ɂ́A0�ɏ��������Ă���B
//   

void pid_para_flash_read(void)
{
	uint8_t  crc;
	uint32_t i;
	uint32_t blk_index;
	uint32_t blk_start;
	uint32_t rd_num;
	
	uint32_t u_sv;
	
	
	blk_index = 0;			// �Ǐo���Ώۃu���b�N
	blk_start = 0x400 * ( blk_index ) +  DATA_FLASH_READ_BASE_ADRS;   // �u���b�N�J�n�A�h���X�i�Ǐo���p)
	rd_num = 27;					        // �ǂݏo���o�C�g��
	
	memcpy( &flash_rd_buf[0], blk_start, rd_num);	//  �f�[�^�t���b�V�� block0(0x00100000) ����flash_data_buf[0]�� 26byte���R�s�[

	memcpy( &u_sv, &flash_rd_buf[0], 4);		// flash_data_buf[0]����A����SV�l�֓ǂݏo��
	
	if (  u_sv == 0xffffffff ) {			// SV�l�̕��������f�[�^ 0xffffffff (Nan)�̏ꍇ�A�S�Ẵp�����[�^���A0�ɂ���B
	    pid_sv = 0;
	    pid_p = 0;
	    pid_i = 0;
	    pid_d = 0;
	    pid_mr = 0;
	    pid_hys = 0;
	    heat_cool = 0;
	    pid_type = 0;
	    
	    return;
	}
	
	crc = Calc_crc_x8_x5_x4_1(&flash_rd_buf[0],27);   // CRC-8(X8+X5+X4+1)�̌v�Z
	
	
	if ( crc != 0 ) {   // CRC�̉��Z���� NG�̏ꍇ
	
		alm_2 = 1;	   // E2�f�[�^�t���b�V���@CRC�G���[�L��
	}
	else{
		alm_2 = 0;	 // E2�f�[�^�t���b�V���@CRC�G���[����
	}
				 // CRC�G���[���������Ă��ǂݏo���B	
	memcpy( &pid_sv, &flash_rd_buf[0], 4);		// flash_data_buf[0]����ASV�l��
	
	memcpy( &pid_p, &flash_rd_buf[4], 4);		// flash_data_buf[4]���� P��
	
	memcpy( &pid_i, &flash_rd_buf[8], 4);		// flash_data_buf[8]���� I��
	
	memcpy( &pid_d, &flash_rd_buf[12], 4);		// flash_data_buf[12]���� D��
	
	memcpy( &pid_mr,&flash_rd_buf[16], 4);		// flash_data_buf[16]����Mr��
	
	memcpy( &pid_hys,&flash_rd_buf[20], 4);		// flash_data_buf[20]����Hys��
	
	memcpy( &heat_cool,&flash_rd_buf[24],1 );	// flash_data_buf[24]����heat_cool��
	
	memcpy( &pid_type,&flash_rd_buf[25], 1);	// flash_data_buf[28]����pid_type�� (���g�p)
	
	
}




// CRC-8�̌v�Z 
// CRC-8-Maxim: X8+X5+X4+1 (0x31) �����l=0xff
//
// ���L�T���v���v���O���������p
// STM32 �� AHT20 ���[�`�� (aht20_stm32 demo v1_4)�v (http://www.aosong.com/class-36.html)
// 
//
uint8_t Calc_crc_x8_x5_x4_1(volatile uint8_t *data, uint8_t num)
{
        uint8_t i;
        uint8_t pt;
        uint8_t crc;
	
	crc = 0xff;

	for ( pt = 0; pt < num; pt++ ) {
  
         crc ^= data[pt];
    
	 for ( i = 8 ;i >0 ; --i)  {
    
           if ( crc & 0x80 ) {
               crc = ( crc << 1 ) ^ 0x31;
	   }
           else{
	       crc = ( crc << 1 );
	   }
	 }
       }
 
       return crc;
}





//
// E2�f�[�^�t���b�V���ւ̃A�N�Z�X����
// (E2 �f�[�^�t���b�V���A�N�Z�X�֎~���[�h���烊�[�h���[�h��) 
//
void data_flash_enable(void)
{
	FLASH.DFLCTL.BIT.DFLEN = 1;		// E2�f�[�^�t���b�V���ւ̃A�N�Z�X����

	delay_5usec();	  // E2 �f�[�^�t���b�V��STOP ��������(tDSTOP)�҂��@(5[usec]�ȏ�)
}



//
//  E2�f�[�^�t���b�V���ւ̏�����
//
// ����: block_index:  �u���b�N�C���f�b�N�X (0�`7) 
// �o��: err_fg = 0 :�����ݐ���
//              = 1 :�������ݎ��s
//  		= 2 :�u���b�N�͈͊O�܂��́A�����݃o�C�g����1Kbyte�𒴂��Ă���
//
//
//  E2�f�[�^�t���b�V���̈�: 0x0010 0000����0x0010 1FFF�܂� 8Kbyte
//                          1�u���b�N(1Kbyte)��8�u���b�N�ō\���B
//
// �u���b�Nindex :   �Ǐo���A�h���X,     �@�@ :�@������(�v���O����)�A�C���[�Y�p�A�h���X
//     0         : 0x0010 0000 - 0x0010 03FF  :  0xFE00 0000 - 0xFE00 03FF�@ 
//     1         : 0x0010 0400 - 0x0010 07FF  :  0xFE00 0400 - 0xFE00 07FF�@ 
//     2         : 0x0010 0800 - 0x0010 0BFF  :  0xFE00 0800 - 0xFE00 0BFF�@ 
//     3         : 0x0010 0C00 - 0x0010 0FFF  :  0xFE00 0C00 - 0xFE00 0FFF�@ 
//     4         : 0x0010 1000 - 0x0010 13FF�@:  0xFE00 1000 - 0xFE00 13FF
//     5         : 0x0010 1400 - 0x0010 17FF  :  0xFE00 1400 - 0xFE00 17FF
//     6         : 0x0010 1800 - 0x0010 1BFF  :  0xFE00 1800 - 0xFE00 1BFF
//     7         : 0x0010 1C00 - 0x0010 1FFF  :  0xFE00 1C00 - 0xFE00 1FFF
//
//�@����:
//  �����ݗp�A�h���X�ƓǏo���A�h���X���قȂ�܂��B
//  �f�[�^���������ޑO�ɃC���[�Y���K�v�ł��B
//  �C���[�Y��1�u���b�N(1 Kbyte)�P�ʂł��B
//  �f�[�^�t���b�V���ւ̏������݂�1�o�C�g�P�ʂōs���܂��B
//

uint32_t  data_flash_write(uint32_t block_index, uint8_t *wr_buf, uint32_t wr_num) 
{
	uint32_t 	i;
	uint32_t	block_start;	
	uint32_t	block_end;

	
	if ( ( block_index > 7 ) || ( wr_num > 1024 ))  {  // �u���b�N�ԍ���8�ȏ�܂��͏����݃o�C�g����1Kbyte�𒴂��Ă���ꍇ
		return 2;
	}
	
	block_start = 0x400 * ( block_index ) +  DATA_FLASH_WRITE_BASE_ADRS;   // �u���b�N�J�n�A�h���X �����ݗp)
	block_end = block_start + 0x3ff;				   // �u���b�N�I���A�h���X 
	
	FLASH.FASR.BIT.EXS = 0;				// �t���b�V���̈�̓f�[�^�̈�
	
	FLASH.FSARH = block_start >> 16;		// �t���b�V�������J�n�A�h���X�̐ݒ�
	FLASH.FSARL = block_start & 0xFFFF;
	
	
	for ( i = 0; i < wr_num; i++ ) {		// �������ݏ���
	
		FLASH.FWB0 = wr_buf[i];		// �������݃f�[�^(1byte)�Z�b�g
	
		FLASH.FCR.BYTE = 0x81;			// �������ݎ��s
	
		while( FLASH.FSTATR1.BIT.FRDY == 0 ) {	// ���������҂��@(����������FRDY = 1)
		}
	
		FLASH.FCR.BYTE = 0x00;			// ������~
		while( FLASH.FSTATR1.BIT.FRDY == 1 ) {	// FRDY = 0�̊m�F�҂� (FRDY=0��A���̏������s�\)
		}
	
	
		if (( FLASH.FSTATR0.BIT.PRGERR == 1 ) || ( FLASH.FSTATR0.BIT.ILGLERR == 1 ))  // �v���O�������ɃG���[�����܂��̓C���[�K���ȃR�}���h���s
		{
			FLASH.FRESETR.BIT.FRESET = 1;	// �t���b�V���̃��Z�b�g
    			FLASH.FRESETR.BIT.FRESET = 0;	// ���Z�b�g����
		
			return	0x01;			// �������ݎ��s
		}
	}
	
	return 0;			// �������ݐ���	
	
}



//
// E2�f�[�^�t���b�V�� �u���b�N�P�ʂ̃C���[�Y
// ����: block_index:  �u���b�N�C���f�b�N�X (0�`7) 
//
// �o��: err_fg = 0 :�C���[�Y����
//              = 1 :�C���[�Y���s
//              = 2 :�u���b�N�C���f�b�N�X�͈͊O
//

uint32_t  data_flash_erase(uint32_t block_index )
{
	uint32_t	block_start;	
	uint32_t	block_end;

	if  ( block_index > 7 )  {  		// �u���b�N�C���f�b�N�X��8�ȏ�̏ꍇ
		return 2;
	}
	
	block_start = 0x400 * ( block_index ) +  DATA_FLASH_WRITE_BASE_ADRS;   // �u���b�N�J�n�A�h���X (�����ݗp)
	block_end = block_start + 0x3ff;			           // �u���b�N�I���A�h���X 
	
	FLASH.FASR.BIT.EXS = 0;			// �t���b�V���̈�̓f�[�^�̈�
	
	FLASH.FSARH = block_start >> 16;	// �t���b�V�������J�n�A�h���X�̐ݒ�
	FLASH.FSARL = block_start & 0xFFFF;
	
	FLASH.FEARH = block_end >> 16;		// �t���b�V�������I���A�h���X�̐ݒ�
	FLASH.FEARL = block_end  & 0xFFFF;
	
	FLASH.FCR.BYTE = 0x84;					// �u���b�N�̃C���[�Y�J�n
	
	while( FLASH.FSTATR1.BIT.FRDY == 0 ) {			// ���������҂��@(����������FRDY = 1)
	}
	
	FLASH.FCR.BYTE = 0x00;					// ������~
	while( FLASH.FSTATR1.BIT.FRDY == 1 ) {			// FRDY = 0�̊m�F�҂� (FRDY=0��A���̏������s�\)
	}
	
	
	if (( FLASH.FSTATR0.BIT.ERERR == 1 ) || ( FLASH.FSTATR0.BIT.ILGLERR == 1 ))  // �C���[�Y���ɃG���[�����܂��̓C���[�K���ȃR�}���h���s
	{
		FLASH.FRESETR.BIT.FRESET = 1;			// �t���b�V���̃��Z�b�g
    		FLASH.FRESETR.BIT.FRESET = 0;			// ���Z�b�g����
		
		return	0x01;		// �C���[�Y���s
	}
	
	return 0;			// �C���[�Y����
	
}




//
// E2�f�[�^�t���b�V�� �S�̈�̃C���[�Y 
//
// �o��: err_fg = 0 :�C���[�Y����
//              = 1 :�C���[�Y���s
//

uint32_t  data_flash_erase_all( void )
{
	uint32_t	block_start;	
	uint32_t	block_end;

	block_start = DATA_FLASH_WRITE_BASE_ADRS;    			// �擪�u���b�N�J�n�A�h���X (�����ݗp)
	block_end =   0x400 *7 + DATA_FLASH_WRITE_BASE_ADRS +  0x3ff;   // �ŏI�u���b�N�I���A�h���X 
	
	FLASH.FASR.BIT.EXS = 0;			// �t���b�V���̈�̓f�[�^�̈�
	
	FLASH.FSARH = block_start >> 16;	// �t���b�V�������J�n�A�h���X�̐ݒ�
	FLASH.FSARL = block_start & 0xFFFF;
	
	FLASH.FEARH = block_end >> 16;		// �t���b�V�������I���A�h���X�̐ݒ�
	FLASH.FEARL = block_end  & 0xFFFF;
	
	FLASH.FCR.BYTE = 0x86;					// �S�u���b�N�̃C���[�Y�J�n
	
	while( FLASH.FSTATR1.BIT.FRDY == 0 ) {			// ���������҂��@(����������FRDY = 1)
	}
	
	FLASH.FCR.BYTE = 0x00;					// ������~
	while( FLASH.FSTATR1.BIT.FRDY == 1 ) {			// FRDY = 0�̊m�F�҂� (FRDY=0��A���̏������s�\)
	}
	
	
	if (( FLASH.FSTATR0.BIT.ERERR == 1 ) || ( FLASH.FSTATR0.BIT.ILGLERR == 1 ))  // �C���[�Y���ɃG���[�����܂��̓C���[�K���ȃR�}���h���s
	{
		FLASH.FRESETR.BIT.FRESET = 1;			// �t���b�V���̃��Z�b�g
    		FLASH.FRESETR.BIT.FRESET = 0;			// ���Z�b�g����
		
		return	0x01;		// �C���[�Y���s
	}
	
	return 0;			// �C���[�Y����
	
}



//
// E2�f�[�^�t���b�V�������[�h���[�h����P/E ���[�h�ɂ���
//
void data_flash_enter_pe_mode(void)
{
		
	FLASH.FENTRYR.WORD =0xAA80;	// E2�f�[�^�t���b�V��P/E���[�h�ɐݒ�
	delay_5usec();	  		// E2 �f�[�^�t���b�V��STOP ��������(tDSTOP)�҂��@(5[usec]�ȏ�)
	
					// �������샂�[�h�œ���(SYSTEM.OPCCR.BIT.OPCM = 0)
	FLASH.FPR = 0xA5;		// �v���e�N�g����
	FLASH.FPMCR.BYTE = 0x10;	// ���[�h���[�h����E2 �f�[�^�t���b�V��P/E ���[�h�ɑJ�ڂ�����
        FLASH.FPMCR.BYTE = 0xEF;
	FLASH.FPMCR.BYTE = 0x10;
	
	FLASH.FISR.BIT.PCKA = 0x1F;	// FCLK=32 MHz
	
}


//
// E2�f�[�^�t���b�V����P/E ���[�h���烊�[�h���[�h�ɂ���
//
void data_flash_exit_pe_mode(void)
{
	FLASH.FPR = 0xA5;		// �v���e�N�g����
	FLASH.FPMCR.BYTE = 0x08;	// P/E ���[�h���烊�[�h���[�h�ɑJ�ڂ�����
        FLASH.FPMCR.BYTE = 0xF7;
	FLASH.FPMCR.BYTE = 0x08;
	
	delay_5usec();	  		// E2 �f�[�^�t���b�V��STOP ��������(tDSTOP)�҂��@(5[usec]�ȏ�)
	
	FLASH.FENTRYR.WORD =0xAA00;	// ���[�h���[�h�ɐݒ�
	
	while(0x0000 != FLASH.FENTRYR.WORD) // �ݒ�̊m�F
	{
	}
	
}