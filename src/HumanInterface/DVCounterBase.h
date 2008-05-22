/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#ifndef VR_DVCOUNTERBASE_H
#define VR_DVCOUNTERBASE_H

#include <HumanInterface/HIVirtualDevice.h>

namespace Spr {

//----------------------------------------------------------------------------
//	DVCountBase

///	カウンタの1チャンネルを表す．
class SPR_DLL DVCounterBase:public HIVirtualDevice{
public:
	SPR_OBJECTDEF_ABST(DVCounterBase);
	DVCounterBase();
	///	カウンタ値の設定
	virtual void Count(long count)=0;
	///	カウンタ値の読み出し
	virtual long Count()=0;
	///	デバイスの名前
	virtual const char* Name() const=0;
	///	デバイスの種類
	static const char* TypeS() { return "Counter"; }
	///	デバイスの種類
	virtual const char* Type() const{ return TypeS(); }
};

}	//	namespace Spr

#endif
