#include "Collision.h"
#pragma hdrstop

namespace Spr{;

//----------------------------------------------------------------------------
//	CDConvex
OBJECTIMPABST(CDConvex, CDShape);

void CDConvex::CalcBBox(Vec3f& bbmin, Vec3f& bbmax){
	Matrix3f rot;
	pose.ori.inv().to_matrix(rot);
	for(int i=0; i<3; ++i){
		bbmin[i] = std::min(bbmin[i], (pose.ori * Support(-rot.col(i)))[i] + pose.pos[i]);
		bbmax[i] = std::max(bbmax[i], (pose.ori * Support( rot.col(i)))[i] + pose.pos[i]);
	}
}

}
