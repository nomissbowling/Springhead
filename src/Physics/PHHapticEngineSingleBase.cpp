#include <Physics/PHHapticEngineSingleBase.h>

namespace Spr{;

//----------------------------------------------------------------------------
// PHHapticEngineSingleBase
PHHapticEngineSingleBase::PHHapticEngineSingleBase(){}

void PHHapticEngineSingleBase::Step1(){
	////DSTR << "step" << std::endl;
	engine->StartDetection();
	UpdateInterface();
	UpdateHapticPointer();
	PHHapticRenderInfo hri;
	hri.pointers = GetHapticPointers();
	hri.hsolids = GetHapticSolids();
	hri.sps = GetSolidPairsForHaptic();
	hri.hdt = GetPhysicsTimeStep();
	hri.pdt = GetPhysicsTimeStep();
	hri.loopCount = 1.0;
	hri.bInterpolatePose = false;
	hri.bMultiPoints = false;
	hapticRender.SetRenderMode(PHHapticRenderBase::PENALTY6D);
	hapticRender.SetRenderMode(PHHapticRenderBase::CONSTRAINT);
	hapticRender.HapticRendering(hri);
	//�����_�����O�����͂��V�[���ɔ��f
	for(int i = 0; i < (int)NHapticSolids(); i++){
		PHSolidForHaptic* hsolid = GetHapticSolid(i);
		if(hsolid->bPointer) continue;
		PHSolid* sceneSolid = hsolid->sceneSolid;
		sceneSolid->AddForce(hsolid->force);
		sceneSolid->AddTorque(hsolid->torque);
		hsolid->force.clear();
		hsolid->torque.clear();
	}
}

void PHHapticEngineSingleBase::Step2(){}

void PHHapticEngineSingleBase::UpdateInterface(){
	for(int i = 0; i < NHapticPointers(); i++){
		GetHapticPointer(i)->UpdateInterface((float)GetPhysicsTimeStep());
	}
}

void PHHapticEngineSingleBase::UpdateHapticPointer(){
	for(int i = 0; i < NHapticPointers(); i++){
		if(GetHapticPointer(i)->bDebugControl == true){
			GetHapticPointer(i)->hiSolid = *GetHapticPointer(i);
		}
		GetHapticPointer(i)->UpdateDirect();
	}
}


// �f�o�b�N�p�R�[�h
// ���ŁAPHScene::Step()����������Ă���̂�
// �����ŌĂԂ킯�ɂ͂����Ȃ�
void PHHapticEngineSingleBase::StepSimulation(){
	/// �V�~�����[�V�����̎��s
	engine->GetScene()->Step();
}




}