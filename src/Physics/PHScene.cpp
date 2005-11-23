#include "Physics.h"
#pragma hdrstop
#include <sstream>
#include <FLOAT.H>



namespace Spr{;

//----------------------------------------------------------------------------
//	PHScene
OBJECTIMP(PHScene, Scene);

PHScene::PHScene(PHSdkIf* s){
	Init();
	sdk = s;
}
PHScene::PHScene(){
	Init();
}
void PHScene::Init(){
	timeStep = 0.005;
	Scene::Clear();
	PHSolidContainer* sc = new PHSolidContainer;
	engines.Add(sc);
	PHGravityEngine* ge = new PHGravityEngine;
	engines.Add(ge);
}


PHSolidIf* PHScene::CreateSolid(const PHSolidDesc& desc){
	PHSolid* s = new PHSolid(desc);
	s->SetScene(this);
	PHSolidContainer* sc;
	engines.Find(sc);
	assert(sc);
	sc->AddChildObject(s, this);	
	return s;
}
PHSolidIf* PHScene::CreateSolid(){
	PHSolidDesc def;
	return CreateSolid(def);
}

void PHScene::SetGravity(const Vec3d& g){
	//GravityEngineが無ければ作成
	PHGravityEngine* ge;
	engines.Find(ge);

}

void PHScene::Clear(){
	Scene::Clear();
	count = 0;
}

void PHScene::SetTimeStep(double dt){
	timeStep = dt;
}

void PHScene::Step(){
	ClearForce();
	GenerateForce();
	Integrate();
}
void PHScene::ClearForce(){
	engines.ClearForce();
}
#define FP_ERROR_MASK	(_EM_INEXACT|_EM_UNDERFLOW)
void PHScene::GenerateForce(){
//	_controlfp(FP_ERROR_MASK, _MCW_EM);	//	コメントをはずすと例外が起きる．要調査
	engines.GenerateForce();
//	_controlfp(_MCW_EM, _MCW_EM);
}
void PHScene::Integrate(){
//	_controlfp(FP_ERROR_MASK, _MCW_EM);
	engines.Integrate();
//	_controlfp(_MCW_EM, _MCW_EM);
//	time += timeStep;
	count++;
}

CDShapeIf* PHScene::CreateShape(const CDShapeDesc& desc){
	if (desc.type == CDShapeDesc::CONVEXMESH){
		CDShape* s = new CDConvexMesh((const CDConvexMeshDesc&)desc);
		s->SetScene(this);
		shapes.push_back(s);
		return s;
	}else{
		DSTR << "Error: Unknown shape type " << desc.type << std::endl;
		return NULL;
	}
}


}
