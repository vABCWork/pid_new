#include "iodefine.h"
#include "misratypes.h"

#include "touch.h"


uint16_t   ad_x_val;		// X���f�[�^ (A/D�ϊ��f�[�^)
uint16_t   ad_y_val;            // Y���f�[�^

uint16_t   ad_z1_val;		// �@(�^�b�`������p)
uint16_t   ad_z2_val;


//
//  ��R�����^�b�`�p�l���[�q�֓d����������邽�߂̐���M��
//�@SW0�`SW3�́A74HC125�ւ̏o��
//
void touch_port_ini(void)
{
	SW_0_PDR = 1;	// �o�̓|�[�g�Ɏw��
	
	SW_1_PMR = 0;	// �ėp���o�̓|�[�g
	SW_1_PDR = 1;	// �o�̓|�[�g�Ɏw��
	
	SW_2_PMR = 0;	// �ėp���o�̓|�[�g
	SW_2_PDR = 1;	// �o�̓|�[�g�Ɏw��
	
	SW_3_PDR = 1;	// �o�̓|�[�g�Ɏw��
	
}


//
//   �^�b�`�ʒu�𓾂�
//�@�@74HC125�ւ̐���� SW_0,SW_1,SW_2,SW_3�� Low enable.
//  a) X���莞: 
//     SW_0 = Low, SW_1 = Low , SW_2 = High, SW_3 = High �ɂ�� XP=Vcc ,XN=GND�ƂȂ�BYP(AN004)��A/D�l�� X�ʒu�ƂȂ�B
//  b) Y���莞:
//      SW_2 = Low, SW_3 = Low, SW_0 = High, SW_1 = High �ɂ�� YP=Vcc ,YN=GND�ƂȂ�AXP(AN003)��A/D�l�� Y�ʒu�ƂȂ�B  
//  c) Z1,Z2���莞:
//      SW_0 = Low, SW_3 =  Low, SW_1 = High, SW_2 = High�ɂ��AYP=Vcc, XN=GND�ƂȂ�AXP(AN003)��A/D�l���AZ1�ɂȂ�BYN(AN002)��A/D�l���AZ2�ɂȂ�B
//  
//   A/D�ϊ����� : 1�`���l��������1.4 [��s[ (A/D�ϊ��N���b�NADCLK = 32 MHz���쎞)
//
void touch_position(void)
{
			// X����
	SW_0_PODR = 0;			// X-(GND)�L�� 
	SW_1_PODR = 0;                  // X+(Vcc)�L��
	SW_2_PODR = 1;			// Y- �n�C�C���s�[�_���X  
	SW_3_PODR = 1;			// Y+ �n�C�C���s�[�_���X

	
	S12AD.ADANSA0.BIT.ANSA004 = 1;  // AN004 �ϊ��ΏۂƂ���

	S12AD.ADCSR.BIT.ADST = 1;	// A/D �ϊ��J�n
	while( S12AD.ADCSR.BIT.ADST == 1){  // A/D �ϊ��I���҂� 
	}
	
	ad_x_val = S12AD.ADDR4;		// AN004��A/D�ϊ��l (0�`4095)�@
	S12AD.ADANSA0.BIT.ANSA004 = 0;  // AN004 �ϊ��ΏۂƂ��Ȃ�
	
	
			// Y����
	SW_0_PODR = 1;			// X- �n�C�C���s�[�_���X 
	SW_1_PODR = 1;			// X+ �n�C�C���s�[�_���X 		
	SW_2_PODR = 0;  		// Y-(GND)�L�� 
	SW_3_PODR = 0;  		// Y+(Vcc)�L��
			
	
	S12AD.ADANSA0.BIT.ANSA003 = 1;  // AN003 �ϊ��ΏۂƂ���

	S12AD.ADCSR.BIT.ADST = 1;	// A/D �ϊ��J�n
	while( S12AD.ADCSR.BIT.ADST == 1){  // A/D �ϊ��I���҂� 
	}
	
	ad_y_val = S12AD.ADDR3;		// AN003��A/D�ϊ��l (0�`4095)�@
	S12AD.ADANSA0.BIT.ANSA003 = 0;  // AN003 �ϊ��ΏۂƂ��Ȃ�
	
	
			// Z1,Z2 ����
        SW_1_PODR = 1;			// X+ �n�C�C���s�[�_���X
	SW_2_PODR = 1;			// Y-�@�n�C�C���s�[�_���X
	SW_0_PODR = 0;			// X- (GND)�L��
	SW_3_PODR = 0;			// Y+ (VCC �L��)
	
	S12AD.ADANSA0.BIT.ANSA003 = 1;  // AN003 �ϊ��ΏۂƂ���

	S12AD.ADCSR.BIT.ADST = 1;	// A/D �ϊ��J�n
	while( S12AD.ADCSR.BIT.ADST == 1){  // A/D �ϊ��I���҂� 
	}
	
	ad_z1_val = S12AD.ADDR3;	// AN003��A/D�ϊ��l (0�`4095)�@
	S12AD.ADANSA0.BIT.ANSA003 = 0;  // AN003 �ϊ��ΏۂƂ��Ȃ�
	

	
	S12AD.ADANSA0.BIT.ANSA002 = 1;  // AN002 �ϊ��ΏۂƂ���

	S12AD.ADCSR.BIT.ADST = 1;	// A/D �ϊ��J�n
	while( S12AD.ADCSR.BIT.ADST == 1){  // A/D �ϊ��I���҂� 
	}
	
	ad_z2_val = S12AD.ADDR2;	// AN002��A/D�ϊ��l (0�`4095)�@
	S12AD.ADANSA0.BIT.ANSA002 = 0;  // AN002 �ϊ��ΏۂƂ��Ȃ�
	

					// ����I����@�S�ăn�C�C���s�[�_���X
	SW_0_PODR = 1;			// X- �n�C�C���s�[�_���X
	SW_1_PODR = 1;                  // X+ �n�C�C���s�[�_���X
	SW_2_PODR = 1;			// Y- �n�C�C���s�[�_���X  
	SW_3_PODR = 1;			// Y+ �n�C�C���s�[�_���X
	
}
