#include "Physics.h"
#ifdef USE_HDRSTOP
#pragma hdrstop
#endif
#include <stdlib.h>

namespace Spr{;
void PHRegisterTypeDescs();
void CDRegisterTypeDescs();

struct Sdks{
	typedef std::vector< PHSdkIf* > Cont;
	Cont* cont;

	Sdks(){
#if defined _DEBUG && _MSC_VER			
		// メモリリークチェッカ
		_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif		
		cont = DBG_NEW Cont;
	}
	~Sdks(){
		while(cont->size()){
			PHSdkIf* sdk = cont->back();
			cont->erase(cont->end()-1);
			sdk->DelRef();
			if (sdk->RefCount()==0){
				delete sdk;
			}
		}
		delete cont;
	}
};
static Sdks sdks;
PHSdkIf* SPR_CDECL CreatePHSdk(){
	PHSdkIf* rv = DBG_NEW PHSdk;
	sdks.cont->push_back(rv);
	rv->AddRef();
	return rv;
}

//----------------------------------------------------------------------------
//	PHSdk
IF_OBJECT_IMP(PHSdk, NameManager);

void PHRegisterFactories(){
	bool bFirst = true;
	if (!bFirst) return;
	bFirst=false;
	PHSdkIf::GetIfInfoStatic()->RegisterFactory(new FactoryImp(PHScene, PHSdk));
	PHSceneIf::GetIfInfoStatic()->RegisterFactory(new FactoryImp(PHSolid, PHScene));
}
PHSdk::PHSdk(){
	PHRegisterTypeDescs();
	CDRegisterTypeDescs();
	PHRegisterFactories();
	SetNameManager(NameManager::GetRoot());
}
PHSdk::~PHSdk(){
	for(Sdks::Cont::iterator it = sdks.cont->begin(); it != sdks.cont->end(); ++it){
		if (*it == this){
			sdks.cont->erase(it);
			DelRef();
			break;
		}
	}
}

PHSceneIf* PHSdk::CreateScene(const PHSceneDesc& desc){
	return DCAST(PHSceneIf, CreateObject(PHSceneIf::GetIfInfoStatic(), &desc));
}
PHSceneIf* PHSdk::CreateScene(){
	return CreateScene(PHSceneDesc());
}
int PHSdk::NScene(){
	return scenes.size();
}
PHSceneIf** PHSdk::GetScenes(){
	return (PHSceneIf**)&*scenes.begin();
}

CDShapeIf* PHSdk::CreateShape(const CDShapeDesc& desc){
	CDShape* s = NULL;
	if (desc.type == CDShapeDesc::CONVEXMESH){
		s = DBG_NEW CDConvexMesh((const CDConvexMeshDesc&)desc);
	}else if (desc.type == CDShapeDesc::SPHERE){
		s = DBG_NEW CDSphere((const CDSphereDesc&)desc);
	}else if (desc.type == CDShapeDesc::BOX){
		s = DBG_NEW CDBox((const CDBoxDesc&)desc);
	}
	if (s){
		s->SetNameManager(this);
		shapes.push_back(s);
	}else{
		DSTR << "Error: Unknown shape type " << desc.type << std::endl;
	}
	return s;
}
int PHSdk::NShape(){
	return shapes.size();
}
CDShapeIf* PHSdk::GetShape(int i){
	return shapes[i];
}
ObjectIf* PHSdk::CreateObject(const IfInfo* info, const void* desc){
	ObjectIf* rv = Object::CreateObject(info, desc);
	if (!rv){
		if (info->Inherit(CDShapeIf::GetIfInfoStatic())){
			rv = CreateShape(*(CDShapeDesc*)desc);
		}
	}
	return rv;
}
ObjectIf* PHSdk::GetChildObject(size_t i){		
	if (i<shapes.size()) return shapes[i];
	return scenes[i-shapes.size()];
}

bool PHSdk::AddChildObject(ObjectIf* o){
	PHScene* s = DCAST(PHScene, o);
	if (s){
		if (std::find(scenes.begin(), scenes.end(), s) == scenes.end()){
			scenes.push_back(s);
			return true;
		}
		return false;
	}

	CDShape* h = DCAST(CDShape, o);
	if (h){
		if (std::find(shapes.begin(), shapes.end(), h) == shapes.end()){
			shapes.push_back(h);
			return true;
		}
		return false;
	}

	if (std::find(objects.begin(), objects.end(), DCAST(Object, o)) == objects.end()){
		objects.push_back(DCAST(Object, o));
		return true;
	}
	return false;
}


}
