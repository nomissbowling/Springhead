#ifndef FW_GRAPHICSADAPTEE_H
#define FW_GRAPHICSADAPTEE_H

#include<sstream>
#include<string>
#include<vector>
#include<Springhead.h>
#include<Framework/FWAppInfo.h>
#include <GL/glut.h>

using namespace std;

namespace Spr{;

class FWApp;
class FWWin;
class FWWinDesc;
typedef void SPR_CDECL GTimerFunc(int id);

class FWGraphicsAdapteeDesc{
public:
	FWGraphicsAdapteeDesc();
};

class FWGraphicsAdaptee :public FWGraphicsAdapteeDesc, public UTRefCount{
protected:
	FWApp*	 fwApp;
public:	
	FWGraphicsAdaptee();
	void SetAdapter(FWApp* a){ fwApp = a; };	// FWApp�̐ݒ�
	FWApp* GetFWApp(){ return fwApp; };			// FWApp�̎擾
	
	///	���������s���B�ŏ��ɂ�����ĂԕK�v������B
	virtual void Init(int argc = 0, char* argv[] = NULL){};
	
	/** �^�C�} */
	/// �^�C�}�[��ݒ肷��
	virtual void Timer(int i){};
	/// �^�C�}�[���쐬����
	virtual void AddTimer(){};
	/// �Ō�ɍ쐬�����^�C�}�[�̔ԍ���Ԃ�
	virtual int GetTimerNo(){return NULL;};
	/// i�Ԗڂ̃^�C�}�[��Timerfunc��ݒ肷��
	virtual void SetTimerFunc(GTimerFunc* f ,int i){};
	/// Timerfunc�ŌĂԂ��Ƃɂ��C���[�v���쐬����
	virtual void Loop(int i,double timeSteo){};
	/// mainloop���ĂсC�^�C�}�[���X�^�[�g����
	virtual void TimerStart(){};

	/** �E�B���h�E */
	///	�E�B���h�E���쐬���A�E�B���h�E ID��Ԃ�
	virtual FWWin* CreateWin(const FWWinDesc& d){return NULL;};
	///	�E�B���h�E��j������
	virtual void DestroyWin(FWWin* w){};
	///	�J�����g�E�B���h�E��ݒ肷��
	virtual void SetCurrentWin(FWWin* w){};
	///	�J�����g�E�B���h�E��Ԃ��B
	virtual FWWin* GetCurrentWin(){return NULL;};
	///	�J�����g�E�B���h�E��Ԃ��B
	virtual int GetWinFromId(){return NULL;};
	/// �J�����g�E�B���h�E�̃m�[�}���v���[�����C�ĕ`��̕K�v�ɉ����ă}�[�N����
	virtual void PostRedisplay(){};
	/// Shift,Ctrl,Alt�̃X�e�[�g��Ԃ�
	virtual int Modifiers(){return NULL;};

};

}

#endif