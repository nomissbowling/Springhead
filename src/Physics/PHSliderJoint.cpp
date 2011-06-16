/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#include <Physics/PHSliderJoint.h>
#include <Physics/PHConstraintEngine.h>
#ifdef USE_HDRSTOP
#pragma hdrstop
#endif

using namespace PTM;
using namespace std;
namespace Spr{;

//----------------------------------------------------------------------------
// PHSliderJoint
PHSliderJointDesc::PHSliderJointDesc(){
	bConstraintY		= true;
	bConstraintRollX	= true;
	bConstraintRollZ	= true;
}
PHSliderJoint::PHSliderJoint(const PHSliderJointDesc& desc){
	SetDesc(&desc);
	movableAxes[0] = 2;
}
/*void PHSliderJoint::SetConstrainedIndex(bool *con){
	PHJoint1D::SetConstrainedIndex(con);
	if(!bConstraintY){
		con[1] = false;
	} 
	if(!bConstraintRollX){
		con[3] = false;
	}
	if(!bConstraintRollZ){
		con[5] = false;
	}
}*/
void PHSliderJoint::UpdateJointState(){
	position[0] = Xjrel.r.z;
	velocity[0] = vjrel.v().z;
}

void PHSliderJoint::CompBias(){
	double dtinv = 1.0 / GetScene()->GetTimeStep();

	if (engine->numIterCorrection==0){	//	Correction を速度LCPで行う場合
		db.v()	 = Xjrel.r * dtinv;
		db.v().z = 0.0;
		if(!bConstraintY) db.v().y = 0.0;

		db.w() =  Xjrel.q.RotationHalf() * dtinv;
				//+ Vec3d(0, 0, Xjrel.r.z) % db.v() *0.1;
		
		db *= engine->velCorrectionRate;
	}
	/*if(mode == PHJointDesc::MODE_VELOCITY){
		db.v().z = -targetVelocity;
	}
	else if(spring != 0.0 || damper != 0.0){
		double diff = GetPosition() - targetPosition;
		double tmp	= 1.0 / (damper + spring * GetScene()->GetTimeStep());
		dA.v().z	= tmp / GetScene()->GetTimeStep();
		db.v().z	= spring * (diff) * tmp;
	}*/
}

void PHSliderJoint::CompError(){
	/*B.v() = Xjrel.r;
	B.w() = Xjrel.q.V();
	if(onUpper)
		B.v().z = Xjrel.r.z - upper;
	else if(onLower)
		B.v().z = Xjrel.r.z - lower;
	else B.v().z = 0.0;*/
}

//-----------------------------------------------------------------------------
void PHSliderJointNode::CompJointJacobian(){
	J.col(0).SUBVEC(0,3) = Vec3d(0.0, 0.0, 1.0);
	J.col(0).SUBVEC(3,3).clear();
	PHTreeNode1D::CompJointJacobian();
}
void PHSliderJointNode::CompJointCoriolisAccel(){
	cj.clear();
}
void PHSliderJointNode::CompRelativeVelocity(){
	PHJoint1D* j = GetJoint();
	j->vjrel.v() = Vec3d(0.0, 0.0, j->velocity[0]);
	j->vjrel.w().clear();
}
void PHSliderJointNode::CompRelativePosition(){
	PHJoint1D* j = GetJoint();
	j->Xjrel.q = Matrix3d::Unit();
	j->Xjrel.r = Vec3d(0.0, 0.0, j->position[0]);
}

}

