
#include "iodefine.h"
#include "misratypes.h"
#include "sci.h"
#include "pid.h"
#include "thermocouple.h"
#include "crc_16.h"
#include "alarm.h"
#include "timer.h"

//
//  SCI1 �V���A���ʐM(��������)����
//
// �p�\�R���Ƃ̒ʐM�@��M�p
volatile uint8_t  rcv_data[128];
volatile uint8_t rxdata;
volatile uint8_t rcv_over;
volatile uint8_t  rcv_cnt;

// �p�\�R���Ƃ̒ʐM ���M�p
volatile uint8_t sd_data[128];
volatile uint8_t  send_cnt;
volatile uint8_t  send_pt;

uint16_t    cal_crc_sd_data( uint16_t num );

//
// SCI1 ���M�I�����荞��(�p�\�R���Ƃ̒ʐM�p)
//  (���M��DMA�Ŏ��{)
//
#pragma interrupt (Excep_SCI1_TEI1(vect=221))
void Excep_SCI1_TEI1(void)
{	 
	SCI1.SCR.BIT.TE = 0;            // ���M�֎~
        SCI1.SCR.BIT.TIE = 0;           // ���M���荞�݋֎~	        
	SCI1.SCR.BIT.TEIE = 0;  	// TEI���荞��(���M�I�����荞��)�֎~

	LED_TX_PODR = 0;	        // ���M LED�̏���
	
	
 }
 





// �R�}���h��M�̑΂���A�R�}���h�����ƃ��X�|���X�쐬����
//
// 0x50 :���j�^�R�}���h(ch1,ch2,ch3,ch4,cjt��ǂݏo��)
//

void comm_cmd(void){
   
	uint8_t  cmd;
	
	uint32_t sd_cnt;

	if ( alm_4 > 0 ) {	// ��M����CRC�s��v�܂��͎�M�d���̒����ُ�̏ꍇ�A�p�\�R�����֕ԑ����Ȃ��B�p�\�R�����̓^�C���A�E�g����B
	
	    return;
	}
	
	
	cmd = rcv_data[0];
	
	sd_cnt = 0;
	
	if ( cmd == 0x03) {        // �p�����[�^�������݃R�}���h(0x03)
	    sd_cnt = write_para_data();	// �p�����[�^�����݂Ɨp���M�f�[�^�쐬
	}
	
	else if ( cmd == 0x04) {	 // ���[�h�ύX�R�}���h(0x04)
	     sd_cnt = set_mode_index();	 // ���[�h�ύX�ƁA���M�f�[�^�쐬
	}
	
	else if ( cmd == 0x05) {	// MV�l�ύX�R�}���h(0x05)
	     sd_cnt = set_mv_data();	// MV�l�̕ύX
	}
	
	else if ( cmd == 0x10) {	// �t���b�V�������݃R�}���h(0x10)
	     sd_cnt = set_flash_write();	// �t���b�V��������
	}
	
	
	else if ( cmd == 0x51 ) {        // ���x�p�����[�^���j�^�R�}���h

	    sd_cnt = resp_temp_para_read();
	}
	
	
	 
	DMAC1.DMSAR = (void *)&sd_data[0];	 // �]�����A�h���X		
	DMAC1.DMDAR = (void *)&SCI1.TDR;	 // �]����A�h���X TXD1 ���M�f�[�^

	DMAC1.DMCRA = sd_cnt; 	 	// �]���� (���M�o�C�g��)	
	    
	DMAC1.DMCNT.BIT.DTE = 1;    // DMAC1 (DMAC �`�����l��1) �]������
	
	    			   // ��ԍŏ��̑��M���荞��(TXI)�𔭐������鏈���B ( RX23E-A ���[�U�[�Y�}�j���A���@�n�[�h�E�F�A�ҁ@28.3.7 �V���A���f�[�^�̑��M�i�������������[�h�j)�@
	SCI1.SCR.BIT.TIE = 1;      // ���M���荞�݋���
	SCI1.SCR.BIT.TE = 1;	   // ���M����
	
	LED_TX_PODR = 1;	   // ���M LED�̓_��
}




//
//  �p�����[�^�������݃R�}���h(0x03)�̏���
//
//  ��M�f�[�^
//  rcv_data[0];�@0x03 (�p�����[�^�������݃R�}���h)
//  rcv_data[1]:  �������݃p�����[�^�̃C���f�b�N�X
//  rcv_data[2]:  �������݃f�[�^ (float) �P���x���������_�f�[�^�̍ŉ��ʃo�C�g �܂��� byte�^
//  rcv_data[3]:  �������݃f�[�^ (float) or 0
//  rcv_data[4]:  �������݃f�[�^ (float) or 0
//  rcv_data[5]:  �������݃f�[�^ (float)�̍ŏ�ʃo�C�g
//  rcv_data[6]: CRC(��ʃo�C�g��)
//  rcv_data[7]: CRC(���ʃo�C�g��)
//
//  para_index:parameter 
//  0:SV, 1:P, 2:I, 3:D, 4:Mr, 5:Hys, 6:heat_cool, 7:pid_type
//
//
// ���M�f�[�^ :
//     sd_data[0] : 0x83 (�p�����[�^�������݃R�}���h�ɑ΂��郌�X�|���X)
//     se_data[1] : status ( 0x00:���� , 0xff:�������݃f�[�^�͈͊O)
//     sd_data[2] : dummy 
//     sd_data[3] : dummy
//     sd_data[4]: CRC(��ʃo�C�g��)
//     sd_data[5]: CRC(���ʃo�C�g��
//
uint32_t	write_para_data(void)
{
	uint8_t id;
	uint8_t status;
	uint8_t rcv_d;
	uint16_t crc_cd;
	uint32_t cnt;
	
	float *pt;
	
	id  = rcv_data[1];    // �p�����[�^�̃C���f�b�N�X
	
	pt = (float *)&rcv_data[2];       // �������݃f�[�^�̃A�h���X
	
	rcv_d = rcv_data[2];
	
	if ( id == 0 ) {	// SV (float�^)
	   if ( ( *pt >= 0.0 ) && ( *pt <= 100.0 ) ) {  // �͈͓��̏ꍇ 
		pid_sv = *pt;
		status = 0x00;
           }
	   else {					// �͈͊O
	   	status = 0xff;
	   }
	}
	
	else if ( id == 1 ) {	// P (float�^)
	  if ( ( *pt >= 0.0 ) && ( *pt <= 100.0 ) ) {  // �͈͓��̏ꍇ 
		pid_p = *pt;
		status = 0x00;
           }
	   else {					// �͈͊O
	   	status = 0xff;
	   }	
	}
	
	else if ( id == 2 ) {	// I (float�^)
	  if ( ( *pt >= 0.0 ) && ( *pt <= 360.0) ) {  // �͈͓��̏ꍇ 
		pid_i = *pt;
		status = 0x00;
	  }
	  else {			// �͈͊O
	   	status = 0xff;
	   }	
	}
	else if ( id == 3 ) {   // D (float�^)
	  if ( ( *pt >= 0.0 ) && ( *pt <= 360.0) ) {  // �͈͓��̏ꍇ 
	 	pid_d = *pt;
		status = 0x00;
	  }
	  else {			// �͈͊O
	   	status = 0xff;
	  }	
	}
	else if ( id == 4 ) {   // Mr (float�^)
	  if ( ( *pt >= 0.0 ) && ( *pt <= 100.0 ) ) {  // �͈͓��̏ꍇ 
		pid_mr = *pt;
		status = 0x00;
	  }
	  else {			// �͈͊O
	   	status = 0xff;
	  }	
	}
	else if ( id == 5 ) {   	// Hys (float�^)
	  if ( ( *pt >= 0.0 ) && ( *pt <= 100.0 ) ) {  // �͈͓��̏ꍇ 
		pid_hys = *pt;
		status = 0x00;
	  }
	  else {			// �͈͊O
	   	status = 0xff;
	  }	
	}
	
	else if ( id == 6 ) {			// heat_cool (byte�^)
	  if ( ( rcv_d >= 0 ) && ( rcv_d <= 1 ) ) {  // �͈͓��̏ꍇ 
		heat_cool = rcv_d;
		status = 0x00;
	  }
	  else {			// �͈͊O
	   	status = 0xff;
	  }	
	}
	
	else if ( id == 7 ) {			// pid_type (byte �^)
	  if ( ( rcv_d >= 0 ) && ( rcv_d <= 2 ) ) {  // �͈͓��̏ꍇ 
		pid_type = rcv_d;
	  	status = 0x00;
	  }
	  else {			// �͈͊O
	   	status = 0xff;
	  }	
	}
	
	
	cnt = 6;			// ���M�o�C�g��
	
	sd_data[0] = 0x83;	 	// �p�����[�^�������݃R�}���h�ɑ΂��郌�X�|���X	
	sd_data[1] = status;		//( 0x00:���� , 0xff:�������݃f�[�^�͈͊O)
	sd_data[2] = 0;			
	sd_data[3] = 0;
	
	
	crc_cd = cal_crc_sd_data( cnt - 2 );   // CRC�̌v�Z
	
	sd_data[4] = crc_cd >> 8;	// CRC��ʃo�C�g
	sd_data[5] = crc_cd;		// CRC���ʃo�C�g
	
	return cnt;
	
}


// 
// 
//  ���䃂�[�h�ύX�R�}���h(0x04)�̏���
//
//  ��M�f�[�^
//  rcv_data[0];�@0x04 (���䃂�[�h�ύX�R�}���h)
//  rcv_data[1]:  mode_stop_run �i���샂�[�h)
//  rcv_data[2]:  mode_auto_manual (���䃂�[�h)
//  rcv_data[3]:  dummy 0
//  rcv_data[4]:  dummy 0   
//  rcv_data[5]:  dummy 0  
//  rcv_data[6]: CRC(��ʃo�C�g��)
//  rcv_data[7]: CRC(���ʃo�C�g��)
//
//
// ���M�f�[�^ :
//     sd_data[0] : 0x84 (���䃂�[�h�ύX�R�}���h�ɑ΂��郌�X�|���X)
//     se_data[1] : mode_stop_run 
//     sd_data[2] : mode_auto_manual
//     sd_data[3] : dummy 
//     sd_data[4]: CRC(��ʃo�C�g��)
//     sd_data[5]: CRC(���ʃo�C�g��
//
uint32_t	set_mode_index(void)
{
	uint16_t crc_cd;
	
	uint32_t cnt;
	
	pre_mode_stop_run = mode_stop_run;	// �ύX�O�̒l��ۑ�
	pre_mode_auto_manual = mode_auto_manual;
	
	mode_stop_run = rcv_data[1];	//  mode_stop_run  ( 0:stop, 1:run )
	mode_auto_manual = rcv_data[2]; //  mode_auto_manual  ( 0:auto, 1:manul) 
	
	cnt = 6;			// ���M�o�C�g��
	
	sd_data[0] = 0x84;	 	// ���䃂�[�h�ύX�R�}���h�ɑ΂��郌�X�|���X	
	sd_data[1] = mode_stop_run;
	sd_data[2] = mode_auto_manual;			
	sd_data[3] = 0;
	
	crc_cd = cal_crc_sd_data( cnt - 2 );   // CRC�̌v�Z
	
	sd_data[4] = crc_cd >> 8;	// CRC��ʃo�C�g
	sd_data[5] = crc_cd;		// CRC���ʃo�C�g
	
	
	return cnt;
	
}


//
//  MV�l�ύX�R�}���h(0x05)�̏���
//
//  ��M�f�[�^
//  rcv_data[0];�@0x05 (MV���ύX�R�}���h)
//  rcv_data[1]:  dummy 0
//  rcv_data[2]:  �������݃f�[�^ (float) �P���x���������_�f�[�^�̍ŉ��ʃo�C�g
//  rcv_data[3]:  �������݃f�[�^ (float) 
//  rcv_data[4]:  �������݃f�[�^ (float) 
//  rcv_data[5]:  �������݃f�[�^ (float)�̍ŏ�ʃo�C�g
//  rcv_data[6]: CRC(��ʃo�C�g��)
//  rcv_data[7]: CRC(���ʃo�C�g��)
//
// ���M�f�[�^ :
//     sd_data[0] : 0x85 (MV���ύX�R�}���h�ɑ΂��郌�X�|���X)
//     se_data[1] :  status ( 0x00:���� , 0xff:�������݃f�[�^�͈͊O)
//     sd_data[2] : dummy 
//     sd_data[3] : dummy 
//     sd_data[4]: CRC(��ʃo�C�g��)
//     sd_data[5]: CRC(���ʃo�C�g��
//
uint32_t	set_mv_data(void)
{
	uint8_t  status;
	uint16_t crc_cd;
	uint32_t cnt;
	
	float *pt;
	
	pt = (float *)&rcv_data[2];       // �������݃f�[�^�̃A�h���X
	
	if ( ( *pt >= 0.0 ) && ( *pt <= 100.0 ) ) {  // �͈͓��̏ꍇ 
		pid_mv = *pt;
		status = 0x00;
	 }
	 else {			// �͈͊O
	   	status = 0xff;
	 }
	 
	 cnt = 6;			// ���M�o�C�g��
	
	 sd_data[0] = 0x85;	 	// ���䃂�[�h�ύX�R�}���h�ɑ΂��郌�X�|���X	
	 sd_data[1] = status;
	 sd_data[2] = 0;			
	 sd_data[3] = 0;
	
	 crc_cd = cal_crc_sd_data( cnt - 2 );   // CRC�̌v�Z
	
	 sd_data[4] = crc_cd >> 8;	// CRC��ʃo�C�g
	 sd_data[5] = crc_cd;		// CRC���ʃo�C�g
	
	 
	 
	 return cnt;
}



//
// �p�����[�^�l�̃t���b�V���������݃R�}���h(0x10)�̏���
//
//  ��M�f�[�^
//  rcv_data[0];�@0x10 (�p�����[�^�l�̃t���b�V���������݃R�}���h)
//  rcv_data[1]:   dummy 0
//  rcv_data[2]:   dummy 0
//  rcv_data[3]:   dummy 0
//  rcv_data[4]:   dummy 0
//  rcv_data[5]:   dummy 0
//  rcv_data[6]: CRC(��ʃo�C�g��)
//  rcv_data[7]: CRC(���ʃo�C�g��)
//
//
// ���M�f�[�^ :
//     sd_data[0] : 0x90 (�p�����[�^�l�̃t���b�V���������݃R�}���h�ɑ΂��郌�X�|���X)
//     se_data[1] : err: (�������݃G���[�t���O 0:�����ݐ���, 0�ȊO:�����ݎ��s)
//     sd_data[2] : dummy 
//     sd_data[3] : dummy 
//     sd_data[4]: CRC(��ʃo�C�g��)
//     sd_data[5]: CRC(���ʃo�C�g��
//

uint32_t	set_flash_write(void)
{
	uint16_t crc_cd;
	
	uint32_t err_fg;
	uint32_t cnt;
	
	err_fg = pid_para_flash_write();		//  �p�����[�^���A�f�[�^�t���b�V���֏�������
	
	cnt = 6;			// ���M�o�C�g��
	
	sd_data[0] = 0x90;	 	// ���䃂�[�h�ύX�R�}���h�ɑ΂��郌�X�|���X	
	sd_data[1] = (uint8_t)err_fg;	// 0: ���� 0�ȊO:���s
	sd_data[2] = 0;			
	sd_data[3] = 0;
	
	 crc_cd = cal_crc_sd_data( cnt - 2 );   // CRC�̌v�Z
	
	 sd_data[4] = crc_cd >> 8;	// CRC��ʃo�C�g
	 sd_data[5] = crc_cd;		// CRC���ʃo�C�g
	
	 return cnt;
	
}






//
// ���x�ƃp�����[�^(SV��)��ǂݏo���R�}���h�̃��X�|���X�쐬
//  ��M�f�[�^
//  rcv_data[0];�@0x51 (���x�ƃp�����[�^�@�ǂݏo���R�}���h)
//  rcv_data[1]: dummy 0
//  rcv_data[2]: dummy 0 
//  rcv_data[3]: dummy 0
//  rcv_data[4]: dummy 0
//  rcv_data[5]: dummy 0
//  rcv_data[6]: CRC(��ʃo�C�g��)
//  rcv_data[7]: CRC(���ʃo�C�g��)
//
//   ���M�f�[�^ :
//     sd_data[0] : 0xd1 (�R�}���h�ɑ΂��郌�X�|���X)
//     se_data[1] : mode_stop_run ( 0:Stop, 1:Run)
//     sd_data[2] : mode_auto_manual (0:Auto, 1:Manual)
//     sd_data[3] : dummy 0
//     sd_data[4] : PV(ch1)��Low�o�C�g  (10�{�����l ��:300�Ȃ��30.0����\��)
//     sd_data[5] : PV(ch1)��High�o�C�g
//     sd_data[6] : ch2 ��Low�o�C�g   (10�{�����l ��:300�Ȃ��30.0����\��)
//     sd_data[7] : ch2 ��High�o�C�g   
//     sd_data[8] :  dummy 0
//     sd_data[9] :  dummy 0   
//     sd_data[10] : dummy 0
//     sd_data[11] : dummy 0 
//     sd_data[12] : cjt ��Low�o�C�g  (10�{�����l ��:150�Ȃ��15.0����\��)
//     sd_data[13] : cjt ��High�o�C�g 
//     sd_data[14] : MV ��Low�o�C�g   (10�{�����l ��:100�Ȃ��10.0%��\��)
//     sd_data[15] : MV ��High�o�C�g
//     sd_data[16] : SV ��Low�o�C�g   (10�{�����l ��:300�Ȃ��30.0����\��)   
//     sd_data[17] : SV ��High�o�C�g
//     sd_data[18] : P  ��Low�o�C�g ( 10�{�����l ��:10�Ȃ��1%��\��)
//     sd_data[19] : P  ��High�o�C�g      
//     sd_data[20] : I  ��Low�o�C�g
//     sd_data[21] : I  ��High�o�C�g
//     sd_data[22] : D  ��Low�o�C�g
//     sd_data[23] : D  ��High�o�C�g
//     sd_data[24] : Mr ��Low�o�C�g  (10�{�����l (��:100�Ȃ��10.0%��\��)
//     sd_data[25] : Mr ��High�o�C�g
//     sd_data[26] : Hys ��Low�o�C�g  (10�{�����l (��:10�Ȃ��1.0����\��)
//     sd_data[27] : Hys ��High�o�C�g    
//     sd_data[28] : H/C (0:Heater(�t����), 1:Coller(������))
//     sd_data[29] : pid_type
//     sd_data[30] : pid_kp(�Q�C��)  ��Low�o�C�g
//     sd_data[31] :  :        	       High�o�C�g
//     sd_data[32] : En(�΍�)��Low�o�C�g (10�{�����l (��:10�Ȃ��1.0����\��)
//     sd_data[33] :  :        High�o�C�g
//     sd_data[34] : P_MV�� Low�o�C�g (10�{�����l (��:100�Ȃ��10.0%��\��)
//     sd_data[35] :   :    High�o�C�g
//     sd_data[36] : I_MV�� Low�o�C�g (10�{�����l (��:100�Ȃ��10.0%��\��)
//     sd_data[37] :   :    High�o�C�g
//     sd_data[38] : D_MV�� Low�o�C�g (10�{�����l (��:100�Ȃ��10.0%��\��)
//     sd_data[39] :   :    High�o�C�g
//     sd_data[40] :  ALM1
//     sd_data[41] :  ALM2
//     sd_data[42] :  ALM3
//     sd_data[43] :  ALM4
//     sd_data[44] :  CRC ��ʃo�C�g
//     sd_data[45] :  CRC ���ʃo�C�g
//
uint32_t	resp_temp_para_read(void)
{
	int16_t   x_ch1, x_ch2, x_ch3, x_ch4, x_cjt;
	int16_t   x_sv, x_out, x_p, x_i, x_d, x_mr, x_hys;
	int16_t   x_kp, x_en, x_p_mv, x_i_mv, x_d_mv;
	
	uint16_t   crc_cd;
	
	uint32_t cnt;
	
	sd_data[0] = 0xd1;	 	// �R�}���h�ɑ΂��郌�X�|���X	
	sd_data[1] = mode_stop_run;
	sd_data[2] = mode_auto_manual;			
	sd_data[3] = 0;
	
        x_ch1 = tc_temp[0] * 10.0;	// ch1��10�{
	sd_data[4] = x_ch1;		// Low�o�C�g��
	sd_data[5] = x_ch1 >> 8;	// High�o�C�g��
	
        x_ch2 = tc_temp[1] * 10.0;	// ch2��10�{
	sd_data[6] = x_ch2;		// Low�o�C�g��
	sd_data[7] = x_ch2 >> 8;	// High�o�C�g��

        x_ch3 = 0;			// dummy 0
	sd_data[8] = x_ch3;		// Low�o�C�g��
	sd_data[9] = x_ch3 >> 8;	// High�o�C�g��
	
        x_ch4 = 0;			// dummy 0
	sd_data[10] = x_ch4;		// Low�o�C�g��
	sd_data[11] = x_ch4 >> 8;	// High�o�C�g��
			
	x_cjt = cj_temp * 10.0;		// ��ړ_���x��10�{
	sd_data[12] = x_cjt;
	sd_data[13] = x_cjt >> 8;
	
	x_out = pid_out * 10.0;		// �o�͂�10�{
	sd_data[14] = x_out;
	sd_data[15] = x_out >> 8;
	
	x_sv = pid_sv * 10.0;		// SV��10�{
	sd_data[16] = x_sv;
	sd_data[17] = x_sv >> 8;
	
	x_p = pid_p * 10.0;		// P�l ��10�{
	sd_data[18] = x_p;
	sd_data[19] = x_p >> 8;
	
	x_i = pid_i;			// I�l
	sd_data[20] = x_i;
	sd_data[21] = x_i >> 8;
	
	x_d = pid_d;			// D�l
	sd_data[22] = x_d;
	sd_data[23] = x_d >> 8;
	
	x_mr = pid_mr * 10.0;		// MR�l��10�{
	sd_data[24] = x_mr;
	sd_data[25] = x_mr >> 8;
	
	x_hys = pid_hys * 10.0;		// Hys�l��10�{
	sd_data[26] = x_hys;
	sd_data[27] = x_hys >> 8;
	
	sd_data[28] = heat_cool;
	sd_data[29] = pid_type;

	x_kp = pid_kp * 1.0;		// Kp = 100/P
	sd_data[30] = x_kp;
	sd_data[31] = x_kp >> 8;
	
	x_en = pid_en * 10.0;		// En(�΍�)
	sd_data[32] = x_en;
	sd_data[33] = x_en >> 8;
	
	x_p_mv = pid_p_mv * 10.0;	// P����ɂ��o�� �� 10�{
	sd_data[34] = x_p_mv;
	sd_data[35] = x_p_mv >> 8;
	
	x_i_mv = pid_i_mv * 10.0;	// I����ɂ��o�� �� 10�{
	sd_data[36] = x_i_mv;
	sd_data[37] = x_i_mv >> 8;
	
	x_d_mv = pid_d_mv * 10.0;	// D����ɂ��o�� �� 10�{
	sd_data[38] = x_d_mv;
	sd_data[39] = x_d_mv >> 8;
	
	sd_data[40] = alm_1;		// ALM 1
	sd_data[41] = alm_2;		// ALM 2
	
	sd_data[42] = alm_3;		// ALM 3
	sd_data[43] = alm_4;		// ALM 4
	
	cnt = 46;
	
	crc_cd = cal_crc_sd_data( cnt - 2 );   // CRC�̌v�Z
	
	sd_data[44] = crc_cd >> 8;	// CRC��ʃo�C�g
	sd_data[45] = crc_cd;		// CRC���ʃo�C�g
	
	
	return cnt;
}

// �@���M�f�[�^��CRC�v�Z
//   sd_data[0]����num�̃f�[�^��CRC���v�Z����B
//
uint16_t    cal_crc_sd_data( uint16_t num )
{
	uint16_t  crc;
	
	uint32_t i;

	Init_CRC();			// CRC���Z��̏�����
	
	for ( i = 0 ; i < num ; i++ ) {	// CRC�v�Z
	
		CRC.CRCDIR = sd_data[i];
	}
	
	crc = CRC.CRCDOR;        // CRC�̉��Z����
	
	return crc;
}



// 
// SCI1 �����ݒ�
//  39.4kbps,   8bit-non parity-1stop
//  PCLKB = 32MHz
//  TXD1= P16,  RXD1 = P15
//
//   BRR�̒l(N):

//    N= (32 x 1000000/(64/2)xB)-1
//�@�@�@�@B: �{�[���[�g bps
//        
// ��1)    B=38.4 kbps�Ƃ���B�@32x38.4K = 1228.8 K	
//     32000 K / 1228.8 K= 26.04	
//     N= 26 - 1 = 25
//
// 38.4Kbps:
//     SCI12.BRR = 25
//     SCI12.SEMR.BIT.BGDM = 0
//
//
// ��2)  B=76.8 kbps�Ƃ���B�@32x76.8K = 2457.6 K	
//     32000 K / 2457.6 K= 13.02	
//     N= 13 - 1 = 12
//
// 76.8Kbps:
//     SCI12.BRR = 12
//     SCI12.SEMR.BIT.BGDM = 0
//
//
// ��2)  B=153.6 kbps�Ƃ���B�@32x153.6K = 4915.2 K	
//     32000 K / 4915.2 K= 6.5	
//     N= 6 - 1 = 5
//
// 76.8Kbps:
//     SCI12.BRR = 5
//     SCI12.SEMR.BIT.BGDM = 0
//

void initSCI_1(void)
{
	
	MPC.PWPR.BIT.B0WI = 0;   // �}���`�t�@���N�V�����s���R���g���[���@�v���e�N�g����
	MPC.PWPR.BIT.PFSWE = 1;  // PmnPFS ���C�g�v���e�N�g����
	
	MPC.P15PFS.BYTE = 0x0A;  // P15 = RXD1
	MPC.P16PFS.BYTE = 0x0A;  // P16 = TXD1
	
	MPC.PWPR.BYTE = 0x80;    //  PmnPFS ���C�g�v���e�N�g �ݒ�

	PORT1.PMR.BIT.B5 = 1;	// P15 ���Ӄ��W���[���Ƃ��Ďg�p
	PORT1.PMR.BIT.B6 = 1;   // P16 ���Ӄ��W���[���Ƃ��Ďg�p	
	
	SCI1.SCR.BYTE = 0;	// �����{�[���[�g�W�F�l���[�^�A����M�֎~
	SCI1.SMR.BYTE = 0;	// PCLKB(=32MHz), ��������,8bit,parity �Ȃ�,1stop
	
	//SCI1.BRR = 25;		// 38.4K 
	
	SCI1.BRR = 12;			// 76.8K  
	SCI1.SEMR.BIT.BGDM = 0;     
	SCI1.SEMR.BIT.ABCS = 0;
	

	
	SCI1.SCR.BIT.TIE = 0;		// TXI���荞�ݗv���� �֎~
	SCI1.SCR.BIT.RIE = 1;		// RXI�����ERI���荞�ݗv���� ����
	SCI1.SCR.BIT.TE = 0;		// �V���A�����M����� �֎~�@�i������ TE=1�ɂ���ƁA��ԍŏ��̑��M���荞�݂��������Ȃ�)
	SCI1.SCR.BIT.RE = 1;		// �V���A����M����� ����
	
	SCI1.SCR.BIT.MPIE = 0;         // (�������������[�h�ŁASMR.MP�r�b�g= 1�̂Ƃ��L��)
	SCI1.SCR.BIT.TEIE = 0;         // TEI���荞�ݗv�����֎~
	SCI1.SCR.BIT.CKE = 0;          // �����{�[���[�g�W�F�l���[�^
	
	
	IEN(SCI1,RXI1) = 1;		// ��M���荞�݋���
	  
	IEN(SCI1,TXI1) = 1;		// ���M���荞�݋���
	
	IPR(SCI1,TEI1) = 12;		// ���M���� ���荞�݃��x�� = 12 �i15���ō����x��)
	IEN(SCI1,TEI1) = 1;		// ���M�������荞�݋���
	
	rcv_cnt = 0;			// ��M�o�C�g���̏�����
	Init_CRC();			// CRC���Z��̏�����
	
	
}




//  ���M���Ǝ�M����LED�@�o�̓|�[�g�ݒ� (�p�\�R���Ƃ̒ʐM�p)
 void LED_comm_port_set(void)	
 {
	 				// ���M�@�\���pLED
	  LED_TX_PDR = 1;		// �o�̓|�[�g�Ɏw��
	  
	 				// ��M�@�\���pLED
	  LED_RX_PDR = 1;		// �o�̓|�[�g�Ɏw��
 }

