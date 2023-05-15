#include	<machine.h>
#include	 "iodefine.h"
#include	 "misratypes.h"
#include	"delay.h"
#include 	"dma.h"
#include	"sci.h"
#include	"rspi_9bit.h"
#include	 "ILI9488_9bit_dma.h"
#include	"timer.h"
#include	"lcd_num_disp.h"
#include	"lcd_disp.h"
#include        "touch.h"
#include	"s12ad.h"

#include	"dsad.h"
#include        "pid.h"
#include	"mtu.h"
#include        "thermocouple.h"
#include	"dataflash.h"
#include 	"key.h"

#include	"crc_16.h"
#include       "rom_crc_data.h"
#include	"alarm.h"


extern volatile uint8_t  rcv_data[128];

void clear_module_stop(void);

uint8_t flg_control_interval;    // 1 = ����^�C�~���O 

void main(void)
{
	
	clear_module_stop();	//  ���W���[���X�g�b�v�̉���
	
	alarm_port_ini();	// �A���[��LED�p�|�[�g�ݒ�
	
	ROM_CRC_Err_Check();	// ROM CRC�̊m�F
	
	if ( rom_crc_16_err == 1 ) {    // ROM CRC�ُ�̏ꍇ
	
	   while(1){		        // �������[�v
	        alm_1 = 1;
	    	ALM_1_PODR = 1; 	//  High(ALM1_LED�_��)
		delay_msec(100);   	// 100[msec]�҂�
		ALM_1_PODR = 0; 	//  Low(ALM1_LED����)
		delay_msec(100); 	// 100[msec]�҂�
	   }
	}
	
	
	data_flash_enable();	//  E2�f�[�^�t���b�V���ւ̃A�N�Z�X����
	
	pid_para_flash_read();	 // PID�p�����[�^ (SV, PID��)���f�[�^�t���b�V������ǂݏo��
	
	DMA0_ini();		//  PC�ւ̃V���A���f�[�^��M�p��DMA�����@�����ݒ�
	
	DMAC0.DMSAR = (void *)&SCI1.RDR;	 // �]�����A�h���X SCI1 ��M�f�[�^���W�X�^		
	DMAC0.DMDAR = (void *)&rcv_data[0];	 // �]����A�h���X ��M�o�b�t�@
	DMAC0.DMCRA = 8; 	 	// �]���� (��M�o�C�g���@8byte�Œ�)	
	DMAC0.DMCNT.BIT.DTE = 1;    // DMAC0 (DMAC �`�����l��0) �]������
	
	
	DMA1_ini();           	// PC�ւ̃V���A���f�[�^���M�p��DMA�����@�����ݒ�	
	initSCI_1();		// SCI1(�p�\�R���Ƃ̒ʐM�p)  �����ݒ� 76.8K

	
	LED_comm_port_set();	// ���M����LED�|�[�g�ݒ�
	
	DMA2_ini();           	// DMA �`�����l��0( LCD�ւ̃f�[�^���M�p)�@�����ݒ�
	RSPI_Init_Port();	// RSPI �|�[�g�̏�����  (LCD�R���g���[���p)   
     	RSPI_Init_Reg();        // SPI ���W�X�^�̐ݒ�  

     	RSPI_SPCMD_0();	        // SPI �]���t�H�[�}�b�g��ݒ�, SSLA0�g�p	
	
	ILI9488_Reset();	// LCD �̃��Z�b�g	
	 
	ILI9488_Init();		// LCD�̏�����
	
	delay_msec(10);		// LCD(ILI9488)�����������҂�
	
	IWDT_Refresh();		// �E�I�b�`�h�b�N�^�C�}���t���b�V��
	
	disp_black();		// ��ʁ@��  ( 106 [msec] at SPI Clock=16[MHz] )
	
	IWDT_Refresh();		// �E�I�b�`�h�b�N�^�C�}���t���b�V��
	
	disp_name();		// PV���̕\��
	
	disp_switch();		// �X�C�b�`�̕\��
	
	s12ad_ini();		// A/D�R���o�[�^ S12AD�����ݒ�
	
	touch_port_ini();	// �^�b�`�p�l������p�|�[�g�ݒ�
	
	afe_ini();		// AFE(�A�i���O�t�����g�G���h)�ݒ�
	
	dsad0_ini();		// DASD0�̐ݒ�@(�M�d�Ηp 2�`�����l��)
	dsad1_ini();            // DASD1�̐ݒ� (��ړ_�⏞ RTD 100 ohm)
	
	
	ad_index = 0;		// �e�`�����l���̃f�[�^�i�[�ʒu�̏�����
	ad1_index = 0;
	
	PWM_Init_Reg();		// PWM�p�̐ݒ� ���� 200msec(=�������)

	PWM_Port_Set();		// Heat�p,Cool�p�o�̓|�[�g�̐ݒ�
	
	 
	Timer10msec_Set();      // �^�C�}(10msec)�쐬(CMT0)
     	Timer10msec_Start();    // �^�C�}(10msec)�J�n�@
	
	touch_pre_status_ini();		// �S�Ĕ�^�b�`��ԂƂ���B
	
	IWDT_Refresh();		// �E�I�b�`�h�b�N�^�C�}���t���b�V��
	
	while(1){
           if ( flg_100msec_interval == 1 ) {  // 100msec�o��
	      
	       flg_100msec_interval = 0;	// 100msec�o�߃t���O�̃N���A
	   	   
	        if ( flg_control_interval  == 0 ) { // �񐧌�^�C�~���O(=A/D�ϊ��J�n)	
		   
		
		   
		  dsad0_start();		// DSAD0�J�n
		  dsad1_start();		// DSAD1�J�n
		  
		  dsad0_scan_over = 0;
		  dsad1_scan_over = 0;

		 flg_control_interval = 1;	// ����͐���^�C�~���O
		 
		 
	      }
	      else  {	// ����^�C�~���O
                 	  
		while ( (dsad0_scan_over == 0 )||( dsad1_scan_over == 0) ) {  // 66.6 msec, dsad0 (ch0�`ch3) ��dsad1(ch0)�̃X�L���������m�F�҂�
		}    

		dsad0_stop();		 // DSAD0 ��~ (dsad0��dsad1�̒�~�@1.3 usec)
		dsad1_stop();		 // DSAD1 ��~
		
		Cal_ad_avg();		 // dsad0 �e�`�����l���̕��ϒl�𓾂�
	   	Cal_ad1_avg();		 // dsad1 �e�`�����l���̕��ϒl�𓾂�
		
		tc_temp_cal();		 // ���x�v�Z
		
		if (( tc_temp[0] > 110.0 ) || ( tc_temp[1] > 110.0 )){ //  110���ȏ�Œf���G���[�Ƃ���
			alm_3 = 1;		// �M�d�΁@�f���G���[
		}
		else{
			alm_3 = 0;
		}
		
		pid_pv = tc_temp[0];	  // Ch1�� PV�l�Ƃ���B
		
		control();		  // ���䏈��
		  
		flg_control_interval = 0; // ����͔񐧌�^�C�~���O
		
	     }       // ����^�C�~���O	
	       
	       
	                                        // 100mse���̋��ʏ���
						
	     if ( rcv_over == 1 ) {		// �ʐM���� �R�}���h��M�̏ꍇ
	     	   LED_RX_PODR = 0;		// ��M LED�̏���
  		    
		    comm_cmd();			// ���X�|���X�쐬�A���荞�݂ɂ�鑗�M�J�n
		   
	   	    rcv_over = 0;		// �R�}���h��M�t���O�̃N���A
	       }				
			
	       
	       key_input();			// �L�[���͏���
	       
	                                        // �ȉ��̕\�� 35[msec]���炢������ 
	     
	       disp_symbol_para(para_index);	// ���ږ� SV,P,I,D�@��\��
	       disp_para_value(para_index);	// SP,P,I,D���̃p�����[�^�̒l�\��
	       
	       disp_mv_bar_data();		// MV �l�̕\���i�_�O���t�ƃf�[�^) 6.8 msec
	       
	       disp_float_data(pid_pv, 80, 32, 1, COLOR_WHITE );   // PV �\�� �T�C�Y(48x96) 

	       disp_symbol_stop_run(mode_stop_run); // ��� stop/run �̕\��

	       disp_symbol_auto_manual(mode_auto_manual); // ��� auto/manual �̕\��
	   
	       disp_symbol_heat_cool(heat_cool);  // ���䃂�[�h(heater,cooler,heat-cool)
	       
	       alarm_led();			  // �A���[����������LED�_������
	       
	       IWDT_Refresh();			// �E�I�b�`�h�b�N�^�C�}���t���b�V��
	      
	   }  // 100msec�o��

	}   // whiel(1)
	
}





// ���W���[���X�g�b�v�̉���
//
//   �R���y�A�}�b�`�^�C�}(CMT) ���j�b�g0(CMT0, CMT1) 
//   �}���`�t�@���N�V�����^�C�}�p���X���j�b�g�iMTU0 �` MTU5�j
//   �A�i���O�t�����g�G���h(AFE)
//   24�r�b�g��-�� A/D �R���o�[�^(DSAD0) ���j�b�g0
//   24�r�b�g��-�� A/D �R���o�[�^(DSAD1) ���j�b�g1
//   12 �r�b�gA/D �R���o�[�^ (S12ADE)
//   �V���A���y���t�F�����C���^�t�F�[�X0(RSPI)
//   DMA �R���g���[��(DMACA)
//  �V���A���R�~���j�P�[�V�����C���^�t�F�[�X1(SCI1)(�p�\�R���Ƃ̒ʐM�p)
//  CRC ���Z��iCRC�j
//

void clear_module_stop(void)
{
	SYSTEM.PRCR.WORD = 0xA50F;	// �N���b�N�����A����d�͒ጸ�@�\�֘A���W�X�^�̏������݋���	
	
	MSTP(CMT0) = 0;			// �R���y�A�}�b�`�^�C�}(CMT) ���j�b�g0(CMT0, CMT1) ���W���[���X�g�b�v�̉���
	
	MSTP(MTU) = 0;			// �}���`�t�@���N�V�����^�C�}�p���X���j�b�g ���W���[���X�g�b�v�̉���
	MSTP(AFE) = 0;			// �A�i���O�t�����g�G���h(AFE) ���W���[���X�g�b�v�̉���
	MSTP(DSAD0) = 0;		// 24 �r�b�g��-�� A/D �R���o�[�^(DSAD0) ���j�b�g0 ���W���[���X�g�b�v�̉���
	MSTP(DSAD1) = 0;		//             :                        ���j�b�g1 
	
	MSTP(S12AD) = 0;               //  A/D�R���o�[�^�̃��W���[���X�g�b�v����
	
	MSTP(RSPI0) = 0;		// �V���A���y���t�F�����C���^�t�F�[�X0 ���W���[���X�g�b�v�̉���
	MSTP(DMAC) = 0;                //  DMA ���W���[���X�g�b�v����
	
	MSTP(SCI1) = 0;	        	// SCI1 ���W���[���X�g�b�v�̉���
	MSTP(CRC) = 0;			// CRC ���W���[���X�g�b�v�̉���
	
	SYSTEM.PRCR.WORD = 0xA500;	// �N���b�N�����A����d�͒ጸ�@�\�֘A���W�X�^�������݋֎~
}

