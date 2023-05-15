#include "iodefine.h"
#include  "misratypes.h"
#include  "mtu.h"
#include  "pid.h"

#define D_CNT_200_MSEC   (25000 - 1)  


//
//  PWM����
//
// �@ MV�o�͂ɑ΂��� PWM��ON/OFF���Ԃ�ݒ�
//�@pid_mv = 0% �Ȃ�΁AOFF���Ԃ́A200msec, ON���Ԃ� 0msec
//         =10%           :        180msec     :    10msec
//           :
//         =80%                     60msec      :   140msec
//
// �J�E���g�Ԋu:  32/1024 MHz �ŃJ�E���g�B�@1�J�E���g�̎��Ԃ́A 1024/32 usec = 32 [usec]
//    ���� 200 msec,  200 000 usec = 32 * A, A = 6250
//    TGRA = 6250 - 1
//
//  
//  ���M���䎞:MTU4_TGRB��OFF���Ԃ�ݒ� 
//  ��p���䎞:MTU4_TGRD��OFF���Ԃ�ݒ� 
//
// ��: pid_mv =�@0% ��0.01%�̏ꍇ�A�@i_duty = 0 �ƂȂ�B
//     i_duty = 0�ɂȂ�ƁA TGRB=TGRA�ƂȂ�A�R���y�A�}�b�`�ŏo�͂��ω����Ȃ��B (22.3.5 PWM���[�h�@(1) PWM���[�h1�Q��)
//    �@(pid_mv = 100% ����@0%�ɂ����ꍇ�ɁA�o�͂��ω����Ȃ����ƂɂȂ�B)
//   ���̂��߁Ai_duty = 0�̏ꍇ�A��x�J�E���^���~���āA�����o�� Low���o�͂���B
//
//     �܂��Apid_mv=100% �ŁATGRB=0�̏ꍇ�A�����o��=Low�̂܂܂ł́A����(200msec)���ɁA1�J�E���g(=32usec)���@Low�ɗ����Ă��܂��B
//�@�@�@100%�ł͏��High�Ƃ��������߁A�����l=high�Ƃ��Ă���B
//
// �}�j���A�����: 22.3.5 PWM ���[�h
//(a) PWM ���[�h1
//   ...
//  �y�A�Ŏg�p����TGR ���W�X�^�̐ݒ�l������̏ꍇ�A�R���y�A�}�b�`���������Ă��o�͒l�͕ω����܂���B
//

void	Set_PWM_duty(float out_mv)
{
	float duty;
	uint16_t i_duty;
	
	duty = ( out_mv  * 0.01 ) * D_CNT_200_MSEC;	// ON���� 
	
	i_duty = duty;
	
	 
	 if ( heat_cool == 0 )  {			//�@���M����(�t����)�̏ꍇ
	                                                // ���M����p (MTIOC4A�[�q)�� PWM�o�͐ݒ�	
	 	if ( i_duty == 0 ) {		        // �o�́@0%�̏ꍇ�A�����o��(Low)���o�͂��鏈��	
			MTU.TSTR.BIT.CST4 = 0;		// MTU4.TCNT�@�J�E���g��~
			MTU4.TCNT = 0;		 	// MTU4 �^�C�}�J�E���^= 0x0000
							// MTIOC4A�[�q�̓���:
			MTU4.TIORH.BIT.IOA = 1;		//  �����o�͂�Low,TCNT��TGRA�̃R���y�A�}�b�`��Low (�\22.26)
			MTU4.TIORH.BIT.IOB = 2;		//  �����o�͂�Low,TCNT��TGRB�̃R���y�A�}�b�`��High (�\22.18)
		}
		else if ( i_duty >=  D_CNT_200_MSEC) {	// �o�� 100%�ȏ�̏ꍇ�A�����o��(High)�Ƃ��鏈��
			MTU.TSTR.BIT.CST4 = 0;		// MTU4.TCNT�@�J�E���g��~
			MTU4.TCNT = 0;		 	// MTU4 �^�C�}�J�E���^= 0x0000
							// MTIOC4A�[�q�̓���:
			MTU4.TIORH.BIT.IOA = 5;		//  �����o�͂�High,TCNT��TGRA�̃R���y�A�}�b�`��Low (�\22.26)
			MTU4.TIORH.BIT.IOB = 6;		//  �����o�͂�High,TCNT��TGRB�̃R���y�A�}�b�`��High (�\22.18)
		
		}
		else {					// iduty > 0�̏ꍇ�A�o��
		
			MTU4.TGRB = ( D_CNT_200_MSEC - i_duty);	// MTU4.TGRB�̒l�́AOFF����
			
			if ( MTU.TSTR.BIT.CST4 == 0 ){	 // �O��J�E���g��~�̏ꍇ�A�J�E���g�J�n
				MTU.TSTR.BIT.CST4 = 1;		// MTU4.TCNT�@�J�E���g�J�n
				MTU4.TIORH.BIT.IOA = 1;		//  �����o�͂�Low,TCNT��TGRA�̃R���y�A�}�b�`��Low (�\22.26)
				MTU4.TIORH.BIT.IOB = 2;		//  �����o�͂�Low,TCNT��TGRB�̃R���y�A�}�b�`��High (�\22.18)
			}
		}
	
						// ��p����p (MTIOC4C�[�q)
		MTU4.TGRD = D_CNT_200_MSEC;	// OFF�̎��� (���OFF)				
	 }
	 
	  else if ( heat_cool == 1 ) {			//�@��p����(������)�̏ꍇ
	 					        // ��p����p(MTIOC3A �[�q�j��PWM�o�͐ݒ�
		if ( i_duty == 0 ) {		        // �o�́@0%�̏ꍇ�A�����o��(Low)���o�͂��鏈��
			MTU.TSTR.BIT.CST4 = 0;		// MTU4.TCNT�@�J�E���g��~
			MTU4.TCNT = 0;		 	// MTU4 �^�C�}�J�E���^= 0x0000
			
							// MTIOC4C�[�q�̓���:
			MTU4.TIORL.BIT.IOC = 1;		//  �����o�͂�Low,TCNT��TGRC�̃R���y�A�}�b�`��Low (�\22.27)
			MTU4.TIORL.BIT.IOD = 2;		//  �����o�͂�Low,TCNT��TGRD�̃R���y�A�}�b�`��High (�\22.19)
		}
		else if ( i_duty >= D_CNT_200_MSEC ) {	        // �o�� 100%�ȏ�̏ꍇ�A�����o��(High)�Ƃ��鏈��
			MTU.TSTR.BIT.CST4 = 0;		// MTU4.TCNT�@�J�E���g��~
			MTU4.TCNT = 0;		 	// MTU4 �^�C�}�J�E���^= 0x0000
			
							// MTIOC4C�[�q�̓���:
			MTU4.TIORL.BIT.IOC = 5;		//  �����o�͂�High,TCNT��TGRC�̃R���y�A�}�b�`��Low (�\22.27)
			MTU4.TIORL.BIT.IOD = 6;		//  �����o�͂�High,TCNT��TGRD�̃R���y�A�}�b�`��High (�\22.19)
			
		}
		else {                                  // iduty > 0�̏ꍇ�A�o��
			MTU4.TGRD = ( D_CNT_200_MSEC - i_duty);	// MTU4.TGRD�̒l�́AOFF����
			
			if ( MTU.TSTR.BIT.CST4 == 0 ){   // �O��J�E���g��~�̏ꍇ�A�J�E���g�J�n
				MTU.TSTR.BIT.CST4 = 1;	 // �J�E���g�J�n
				MTU4.TIORL.BIT.IOC = 1;		//  �����o�͂�Low,TCNT��TGRC�̃R���y�A�}�b�`��Low (�\22.27)
				MTU4.TIORL.BIT.IOD = 2;		//  �����o�͂�Low,TCNT��TGRD�̃R���y�A�}�b�`��High (�\22.19)
			}
		}
					         
						// ���M����p (MTIOC4A�[�q)
		MTU4.TGRB = D_CNT_200_MSEC;	// OFF�̎��� (���OFF)	
	 }
}


//
// PWM���[�h �̐ݒ�
//

void PWM_Init_Reg(void)
{

	MTU.TSTR.BIT.CST4 = 0;		// MTU4.TCNT�@�J�E���g��~

	MTU.TOER.BIT.OE4A = 1;		// �o�͋��� (MTIOC4A)  
	MTU.TOER.BIT.OE4C = 1;		// �o�͋��� (MTIOC4C)
		
	
	MTU4.TCR.BIT.TPSC = 5;		// MTU4 PCLK/1024�ŃJ�E���g
	MTU4.TCR.BIT.CKEG = 0;		// MTU4  �����オ��G�b�W�ŃJ�E���g
	
	MTU4.TCR.BIT.CCLR = 1;		// TGRA(=TGRC)�̃R���y�A�}�b�`��TCNT�N���A
	
	MTU4.TMDR.BIT.MD = 2;		// MTU4 PWM ���[�h1
						
					// MTIOC4A�[�q�̓���:
	MTU4.TIORH.BIT.IOA = 1;		//  �����o�͂�Low,TCNT��TGRA�̃R���y�A�}�b�`��Low (�\22.26)
	MTU4.TIORH.BIT.IOB = 2;		//  �����o�͂�Low,TCNT��TGRB�̃R���y�A�}�b�`��High (�\22.18)
	
					// MTIOC4C�[�q�̓���:
	MTU4.TIORL.BIT.IOC = 1;		//  �����o�͂�Low,TCNT��TGRC�̃R���y�A�}�b�`��Low (�\22.27)
	MTU4.TIORL.BIT.IOD = 2;		//  �����o�͂�Low,TCNT��TGRD�̃R���y�A�}�b�`��High (�\22.19)
	
	
    	MTU4.TCNT = 0;		 	// MTU4 �^�C�}�J�E���^= 0x0000

					// 22.3.5 PWM ���[�h  (a) PWM ���[�h1
					//�y�A�Ŏg�p����TGR ���W�X�^�̐ݒ�l������̏ꍇ�A�R���y�A�}�b�`���������Ă��o�͒l�͕ω����܂���B
	
					//�@�����̐ݒ� 
	MTU4.TGRA = D_CNT_200_MSEC;	// ���� = 200msec
	MTU4.TGRB = D_CNT_200_MSEC;	// OFF�̎���
	
	MTU4.TGRC = D_CNT_200_MSEC;	// ���� = 200msec
	MTU4.TGRD = D_CNT_200_MSEC;	// OFF�̎���
		
}


// Cool����AHeat����p�̏o�̓|�[�g�ݒ�
//
//  P26: MTIOC4C : Cool�p PWM�o��  
//  P27: MTIOC4A : Heat�p PWM�o��
//
void PWM_Port_Set(void)
{
	
	MPC.PWPR.BIT.B0WI = 0;  	 // �}���`�t�@���N�V�����s���R���g���[���@�v���e�N�g����
        MPC.PWPR.BIT.PFSWE = 1;  	// PmnPFS ���C�g�v���e�N�g����
    	 
	MPC.P26PFS.BYTE = 0x02;		// P26 = MTIOC4C
	MPC.P27PFS.BYTE = 0x02;		// P27 = MTIOC4A
	
        MPC.PWPR.BYTE = 0x80;      	//  PmnPFS ���C�g�v���e�N�g �ݒ�	
	
	COOL_PWM_PMR = 1;	     	// P31:���Ӄ��W���[���Ƃ��Ďg�p
	
	HEAT_PWM_PMR = 1;     		// P27:���Ӄ��W���[���Ƃ��Ďg�p
}


