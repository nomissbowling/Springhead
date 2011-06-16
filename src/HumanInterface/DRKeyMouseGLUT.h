/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#ifndef DRKeyMouseGLUT_H
#define DRKeyMouseGLUT_H

#include <HumanInterface/HIDevice.h>
#include <GL/glut.h>
#include <deque>

#ifndef GLUTCALLBACK
#define GLUTCALLBACK
#endif

namespace Spr {;

/** GLUT版キーボード・マウスの実装

	イベント処理の流れ：
	・staticのコールバックをglutに登録する
	・staticコールバックから実デバイスのインスタンス（シングルトン）のコールバックを呼ぶ
	・実マウスのコールバックから，ポート番号とカレントウィンドウIDが一致する仮想デバイスを探し，
	　そのコールバックを呼ぶ

 */
class DRKeyMouseGLUT: public HIRealDevice{
public:
	SPR_OBJECTDEF(DRKeyMouseGLUT);
	///	仮想デバイス(KeyMouse)
	class DV: public DVKeyMouse{
	public:
		// マウス位置を記憶するリングバッファ
		std::deque<Vec2i>	mousePoints;
	public:

		DV(DRKeyMouseGLUT* dr, int ch):DVKeyMouse(dr, ch){
			mousePoints.resize(64);
		}
		DRKeyMouseGLUT* GetRealDevice(){ return realDevice->Cast(); }
		
		virtual void OnMouse(int button, int state, int x, int y);
		virtual void OnMouseMove(int button, int x, int y, int zdelta);
		virtual void GetMousePosition(int& x, int& y, int& time, int count=0);
	};

	// マウスボタンの状態
	int	buttonState;

public:
	DRKeyMouseGLUT(const DRKeyMouseGLUTDesc& desc = DRKeyMouseGLUTDesc());
	
	///	HIRealDeviceの仮想関数
	virtual bool Init();
	//virtual void Register(HISdkIf* intf);
	virtual HIVirtualDeviceIf*	Rent(const IfInfo* ii, const char* name, int portNo);

	/// DRWinDeviceBaseの仮想関数
	//virtual void RegisterCallback();

	/// GLUTキーコードからSpringhead共通キーコードへの変換
	int	ConvertKeyCode(int key, bool spr_to_glut, bool special);

	///
	void OnMouse		(int button, int state, int x, int y);
	void OnMotion		(int x, int y);
	void OnPassiveMotion(int x, int y);
	void OnKey			(unsigned char ch, int x, int y);
	void OnSpecialKey	(int ch, int x, int y);

	/// ウィンドウIDとデバイスとの対応
	//typedef std::map<int, DRKeyMouseGLUT*> KeyMouseMap;
	static 	DRKeyMouseGLUT* instance;
	/// staticのコールバック関数
	static void GLUTCALLBACK OnMouseStatic			(int button, int state, int x, int y);
	static void GLUTCALLBACK OnMotionStatic			(int x, int y);
	static void GLUTCALLBACK OnPassiveMotionStatic	(int x, int y);
	static void GLUTCALLBACK OnKeyStatic			(unsigned char ch, int x, int y);
	static void GLUTCALLBACK OnSpecialKeyStatic		(int ch, int x, int y);
};

}	//	namespace Spr
#endif
