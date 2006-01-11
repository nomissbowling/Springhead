#include <Graphics/Graphics.h>

#pragma hdrstop

//	メモリリークチェッカ
#define CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>


namespace Spr{;

struct GRSdks{
	typedef std::vector< GRSdkIf* > Cont;
	Cont* cont;

	GRSdks(){
		_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
		cont = DBG_NEW Cont;
	}
	~GRSdks(){
		while(cont->size()){
			GRSdkIf* sdk = cont->back();
			cont->erase(cont->end()-1);
			sdk->DelRef();
			if (sdk->RefCount()==0){
				delete sdk;
			}
		}
		delete cont;
	}
};
static GRSdks sdks;
GRSdkIf* _cdecl CreateGRSdk(){
	GRSdkIf* rv = DBG_NEW GRSdk;
	sdks.cont->push_back(rv);
	rv->AddRef();
	return rv;
}

//----------------------------------------------------------------------------
//	GRSdk
OBJECTIMP(GRSdk, Object);
GRSdk::~GRSdk(){
	for(GRSdks::Cont::iterator it = sdks.cont->begin(); it != sdks.cont->end(); ++it){
		if (*it == this){
			sdks.cont->erase(it);
			DelRef();
			break;
		}
	}
}
/*
PHSceneIf* GRSdk::CreateGraphicsRender(){
	GRRenderIf* rv = DBG_NEW GRRender(this);
	return rv;
}
*/

}
