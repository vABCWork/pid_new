#include "iodefine.h"
#include "misratypes.h"

//
//  S12AD�p �A�i���O���� �̐ݒ�
// ���Ȑf�f�����A�f�����o�A�V�X�g����
//
void s12ad_ini(void)
{
					// A/D�R���g���[�����W�X�^(ADCSR)
	S12AD.ADCSR.BIT.ADST = 0;	// A/D �ϊ���~
	
	S12AD.ADCSR.BIT.ADHSC = 0;	// �����ϊ����� 
	S12AD.ADCSR.BIT.ADIE = 0;	// �X�L�����I�����S12ADI0���荞�ݔ����̋֎~
	S12AD.ADCSR.BIT.ADCS = 0;	// �V���O���X�L���� ���[�h
	
	
					// A/D�R���g���[���g�����W�X�^(ADCER)
	S12AD.ADCER.BIT.ACE = 0;	// �����N���A�̋֎~
	
	
	S12AD.ADCER.BIT.DIAGLD = 0;     // ���Ȑf�f�@�d�����[�e�[�V�������[�h
	
	S12AD.ADCER.BIT.DIAGM = 0;	// ���Ȑf�f�����{���Ȃ�
	
	S12AD.ADCER.BIT.ADRFMT = 0;	// �E�l�߃t�H�[�}�b�g
					// ���������͑S�āA�ϊ��ΏۂƂ��Ȃ��B
	S12AD.ADANSA0.BIT.ANSA000 = 0;  // AN000 �ϊ��ΏۂƂ��Ȃ�
	S12AD.ADANSA0.BIT.ANSA001 = 0;  // AN001 �ϊ��ΏۂƂ��Ȃ�
	S12AD.ADANSA0.BIT.ANSA002 = 0;  // AN002 �ϊ��ΏۂƂ��Ȃ�
	S12AD.ADANSA0.BIT.ANSA003 = 0;  // AN003 �ϊ��ΏۂƂ��Ȃ�
	S12AD.ADANSA0.BIT.ANSA004 = 0;  // AN004 �ϊ��ΏۂƂ��Ȃ�
	
	S12AD.ADANSA0.BIT.ANSA005 = 0;  // AN005 �ϊ��ΏۂƂ��Ȃ�
	
	
        S12AD.ADHVREFCNT.BIT.HVSEL = 0;  // ��d��(���d�ʑ�) = AVCC0
	S12AD.ADHVREFCNT.BIT.LVSEL = 0;  // ��d��(��d�ʑ�) = AVSS0   
	
	S12AD.ADDISCR.BIT.ADNDIS = 0x12; // A/D�f�����o�A�V�X�g�@�v���`���[�W �`���[�W���� = 2�X�e�[�g (31.25nsec x 2= 62.5 nsec )
}
