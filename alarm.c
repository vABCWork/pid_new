#include "iodefine.h"
#include "misratypes.h"

#include "alarm.h"

uint8_t	alm_1;		// ROM-CRC�G���[
uint8_t alm_2;		// E2 �f�[�^�t���b�V���G���[ (CRC�G���[�܂��͏������ݎ��s)
uint8_t alm_3;		// �M�d�΂̒f���G���[
uint8_t alm_4;          // �p�\�R���Ƃ̒ʐM�G���[  b0: CRC�G���[,b1:�ʐM�d���̒������Z��, b2:���g�p, b3:�擪�o�C�g���R�}���h�ȊO, b4-b7:���g�p

// �A���[��LED�o�͏���
//  alm_2�`alm_3 ��������LED�o�͏���
void alarm_led(void)
{
	if ( alm_2 == 1 ) {  
		ALM_2_PODR = 1; 	//  High(ALM2_LED�_��)
	}
	else {
		ALM_2_PODR = 0; 	//  Low(ALM2_LED����)
	}
	
	if ( alm_3 == 1 ) {  
		ALM_3_PODR = 1; 	//  High(ALM3_LED�_��)
	}
	else {
		ALM_3_PODR = 0; 	//  Low(ALM3_LED����)
	}
	
	if ( alm_4 > 0 ) {  
		ALM_4_PODR = 1; 	//  High(ALM4_LED�_��)
	}
	else {
		ALM_4_PODR = 0; 	//  Low(ALM4_LED����)
	}
}


// �A���[�� LED �|�[�g�̒�`
void alarm_port_ini(void)
{
	ALM_1_PMR = 0;	// �ėp���o�̓|�[�g
	ALM_1_PDR = 1;	// �o�̓|�[�g�Ɏw��
	
	ALM_2_PMR = 0;
	ALM_2_PDR = 1;
	
	ALM_3_PMR = 0;
	ALM_3_PDR = 1;
	
	ALM_4_PMR = 0;
	ALM_4_PDR = 1;
	
	
}