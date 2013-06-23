/*
 *  Copyright (c) 2003 - 2011, Fumihiro Kato, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#include <SciLab/SprSciLab.h>
#include <Physics/PHFemThermo.h>
#include <Base/Affine.h>
//#include <Framework/FWObject.h>
//#include <Framework/sprFWObject.h>

#include <GL/glew.h>
#include <GL/glut.h>

#include "windows.h"

//#define THCOND 0.574
#define THCOND 0.0574//5.74

using namespace PTM;

namespace Spr{;

#define UseMatAll
//#define DEBUG
//#define DumK



PHFemThermoDesc::PHFemThermoDesc(){
	Init();
}
void PHFemThermoDesc::Init(){
	thConduct = THCOND;
	rho = 970;
	heatTrans = 25;
	specificHeat = 0.196;		//1960
}

///////////////////////////////////////////////////////////////////
//	PHFemThermo

PHFemThermo::PHFemThermo(const PHFemThermoDesc& desc/*, SceneIf* s*/){
	deformed = true;			//�ϐ��̏������A�`�󂪕ς�������ǂ���
	SetDesc(&desc);
	//if (s){ SetScene(s); }
	StepCount =0;				// �X�e�b�v���J�E���^
	StepCount_ =0;				// �X�e�b�v���J�E���^
	//phFloor =  DCAST(FWObjectIf, GetSdk()->GetScene()->FindObject("fwPan"));
	//GetFramePosition();
}

void PHFemThermo::CalcVtxDisFromOrigin(){
	//>	nSurface�̓��Ax,z���W���狗�������߂�sqrt(2��a)�A�����FemVertex�Ɋi�[����
	//> ���S�~�n�̌v�Z�ɗ��p����@distance from origin
	
	PHFemMeshNew* mesh = GetPHFemMesh(); 

	/// ����t���O�̏�����
	for(unsigned i=0; i<mesh->nSurfaceFace; i++){
		faceVars[i].mayIHheated = false;
	}
	/// ������
	for(unsigned i =0;i<mesh->vertices.size();i++){
		vertexVars[i].disFromOrigin =0.0;
	}

	/// debug
	//DSTR << "faces.size(): " << faces.size() << std::endl;

	//> �\��face�̓��A���_����eface�̐ߓ_�̃��[�J��(x,z)���W�n�ł̕��ʏ�̋����̌v�Z���Aface�̑S�ߓ_��y���W�����̂��̂ɑ΂��āAIH���M�̉\���������t���O��ݒ�
	for(unsigned i=0;i<mesh->nSurfaceFace;i++){
		//> �\�ʂ�face�̑S�ߓ_��y���W�����Ȃ�΁A����face��IH���M��face�ʂƔ��肵�A�t���O��^����
		if(mesh->vertices[mesh->faces[i].vertexIDs[0]].pos.y < 0.0 && mesh->vertices[mesh->faces[i].vertexIDs[1]].pos.y < 0.0 && mesh->vertices[mesh->faces[i].vertexIDs[2]].pos.y < 0.0){
			faceVars[i].mayIHheated = true;
			//	(x,z)���ʂɂ�����mayIHheated��face�S�ߓ_�̌��_����̋������v�Z����
			for(unsigned j=0; j<3; j++){
				vertexVars[mesh->faces[i].vertexIDs[j]].disFromOrigin = sqrt(mesh->vertices[mesh->faces[i].vertexIDs[j]].pos.x * mesh->vertices[mesh->faces[i].vertexIDs[j]].pos.x + mesh->vertices[mesh->faces[i].vertexIDs[j]].pos.z * mesh->vertices[mesh->faces[i].vertexIDs[j]].pos.z);
			}
		}
	}

	//	debug		//>	�������Ή����ɂ̓R�����g�A�E�g����
	//>	���W�l���m�F����
	for(unsigned i=0; i < mesh->nSurfaceFace; i++){
		if(faceVars[i].mayIHheated){
			/// 3�̒��_�̑g�݁��ӂ�x,z�������ŁAy���W�������قȂ�_�̑g�݂��Ȃ����Ƃ��m�F����
			for(unsigned j=0;j<3;j++){
				if(mesh->vertices[mesh->faces[i].vertexIDs[j]].pos.x == mesh->vertices[mesh->faces[i].vertexIDs[(j+1)%3]].pos.x
					&& mesh->vertices[mesh->faces[i].vertexIDs[j]].pos.z == mesh->vertices[mesh->faces[i].vertexIDs[(j+1)%3]].pos.z
					 && mesh->vertices[mesh->faces[i].vertexIDs[j]].pos.y != mesh->vertices[mesh->faces[i].vertexIDs[(j+1)%3]].pos.y){
					DSTR <<i << "th: " << mesh->vertices[mesh->faces[i].vertexIDs[j]].pos << " : " << mesh->vertices[mesh->faces[i].vertexIDs[(j+1)%3]].pos << " : " << mesh->vertices[mesh->faces[i].vertexIDs[(j+2)%3]].pos<<std::endl;
					DSTR << "CalcVtxDisFromOrigin() error" << std::endl;
					assert(0);
				}
			}
		}
	}
}

void PHFemThermo::CalcVtxDisFromVertex(Vec2d originVertexIH){
	//>	nSurface�̓��Ax,z���W���狗�������߂�sqrt(2��a)�A�����FemVertex�Ɋi�[����
	//> ���S�~�n�̌v�Z�ɗ��p����@distance from origin
	
	PHFemMeshNew* mesh = GetPHFemMesh(); // shibata
	/// ����t���O�̏�����
	for(unsigned i=0; i<mesh->nSurfaceFace; i++){
		faceVars[i].mayIHheated = false;
	}
	/// ������
	for(unsigned i =0;i<mesh->vertices.size();i++){
		vertexVars[i].disFromOrigin =0.0;
	}

	/// debug
	//DSTR << "faces.size(): " << faces.size() << std::endl;

	//> �\��face�̓��A���_����eface�̐ߓ_�̃��[�J��(x,z)���W�n�ł̕��ʏ�̋����̌v�Z���Aface�̑S�ߓ_��y���W�����̂��̂ɑ΂��āAIH���M�̉\���������t���O��ݒ�
	for(unsigned i=0;i<mesh->nSurfaceFace;i++){
		//> �\�ʂ�face�̑S�ߓ_��y���W�����Ȃ�΁A����face��IH���M��face�ʂƔ��肵�A�t���O��^����
		if(mesh->vertices[mesh->faces[i].vertexIDs[0]].pos.y < 0.0 && mesh->vertices[mesh->faces[i].vertexIDs[1]].pos.y < 0.0 && mesh->vertices[mesh->faces[i].vertexIDs[2]].pos.y < 0.0){
			faceVars[i].mayIHheated = true;
			//	(x,z)���ʂɂ�����mayIHheated��face�S�ߓ_�̌��_����̋������v�Z����
			for(unsigned j=0; j<3; j++){
				double dx = mesh->vertices[mesh->faces[i].vertexIDs[j]].pos.x - originVertexIH.x;
				double dz = mesh->vertices[mesh->faces[i].vertexIDs[j]].pos.z - originVertexIH.y;	//	�\�L��y�����A����z���W�������Ă���
				vertexVars[mesh->faces[i].vertexIDs[j]].disFromOrigin = sqrt( dx * dx + dz * dz);
			}
		}
	}
}

void PHFemThermo::SetThermalBoundaryCondition(){
	
}

void PHFemThermo::CreateMatKAll(){

}

void PHFemThermo::CreateMatCAll(){

}

void PHFemThermo::ScilabTest(){
	if (!ScilabStart()) std::cout << "Error : ScilabStart \n";

	//	�s��̓ǂݏ���
	Matrix2f A;
	A.Ex() = Vec2f(1,2);
	A.Ey() = Vec2f(3,4);
	std::cout << "A:  " << A ;
	std::cout << "A00:" << A[0][0] << std::endl;
	std::cout << "A01:" << A[0][1] << std::endl;
	std::cout << "A10:" << A[1][0] << std::endl;
	std::cout << "A11:" << A[1][1] << std::endl;
	
	ScilabSetMatrix("A", A);
	ScilabJob("b=[4;5]");
	std::cout << "A=";
	ScilabJob("disp(A);");
	std::cout << "b=";
	ScilabJob("disp(b);");
	std::cout << "x=A\\b" << std::endl;
	ScilabJob("A,b,x=A\\b;");
	
	ScilabGetMatrix(A, "A");
	SCMatrix b = ScilabMatrix("b");
	SCMatrix x = ScilabMatrix("x");
	std::cout << "x:" << x << std::endl;

	ScilabJob("y = A;");
	SCMatrix y = ScilabMatrix("y");
	std::cout << "y=" << y;
	y = 2*A;
	std::cout << "y = 2*A is done by C++ code" << std::endl;
	std::cout << "y=";
	ScilabJob("disp(y);");
	std::cout << A;
	std::cout << y;
	ScilabJob("clear;");

	//	�O���t�`��
	ScilabJob("t = 0:0.01:2*3.141592653;");
	ScilabJob("x = sin(t);");
	ScilabJob("y = cos(t);");
	ScilabJob("plot2d(x, y);");
	for(int i=0; i<100000; ++i){
		ScilabJob("");
	}
//	ScilabEnd();
}

void PHFemThermo::UsingFixedTempBoundaryCondition(unsigned id,double temp){
	//���x�Œ苫�E����
	SetVertexTemp(id,temp);
	//for(unsigned i =0;i < vertices.size()/3; i++){
	//	SetVertexTemp(i,temp);
	//}
}

void PHFemThermo::UsingHeatTransferBoundaryCondition(unsigned id,double temp,double heatTransRatio){
	
	PHFemMeshNew* mesh = GetPHFemMesh(); // shiba

	//�M�`�B���E����
	//�ߓ_�̎��͗��̉��x�̐ݒ�(K,C,F�Ȃǂ̍s��x�N�g���̍쐬��Ɏ��s�K�v����)
//	if(vertices[id].Tc != temp){					//�X�V����ߓ_��Tc���ω������������ATc��F�x�N�g�����X�V����
		SetLocalFluidTemp(id,temp);
		vertexVars[id].heatTransRatio = heatTransRatio;
		//�M�`�B���E�������g����悤�ɁA����B				///	�{�����ăx�N�g��������Ă���̂ŁA���̃R�[�h�ł́A�]�v�ɑ����Ă��܂��Ă��āA�������s������Ȃ��B
		//for(unsigned i =0;i < vertices[id].tets.size();i++){
		//	CreateVecfLocal(tets[vertices[id].tets[i]]);		//	Tc���܂ރx�N�g�����X�V����
		//}

		///	�M�`�B�����܂ލ�(K2,f3)�̂ݍČv�Z
		InitCreateVecf_();				///	�ύX����K�v�̂��鍀�̂݁A���ꕨ��������
		InitCreateMatk_();
		for(unsigned i =0; i < mesh->edges.size();i++){
			edgeCoeffs[i].k = 0.0;
		}
		for(unsigned i=0; i< mesh->tets.size();i++){ // shiba this��mesh
			//DSTR << "this->tets.size(): " << this->tets.size() <<std::endl;			//12
			CreateVecFAll(i);				///	VecF�̍č쐬
			CreateMatkLocal(i);				///	MatK2�̍č쐬 ��if(deformed==true){matk1�𐶐�}		matK1��matk1�̕ϐ��ɓ���Ă����āAmatk2�����A����āA���Z
			//DSTR <<"tets["<< i << "]: " << std::endl;
			//DSTR << "this->tets[i].matk1: " <<std::endl;
			//DSTR << this->tets[i].matk1  <<std::endl;
			//DSTR << "this->tets[i].matk2: " <<std::endl;
			//DSTR << this->tets[i].matk2  <<std::endl;
			//
			//DSTR << "dMatCAll: " << dMatCAll << std::endl;
			int hogehogehoge =0;
		}
//	}
		///	�ߓ_�̑�����ʂ�alphaUpdated��true�ɂ���
		for(unsigned i=0;i<mesh->vertices[id].faceIDs.size();i++){
			faceVars[mesh->vertices[id].faceIDs[i]].alphaUpdated = true;
			alphaUpdated = true;
		}
}

void PHFemThermo::UsingHeatTransferBoundaryCondition(unsigned id,double temp){
	//�M�`�B���E����
	//�ߓ_�̎��͗��̉��x�̐ݒ�(K,C,F�Ȃǂ̍s��x�N�g���̍쐬��Ɏ��s�K�v����)
//	if(vertices[id].Tc != temp){					//�X�V����ߓ_��Tc���ω������������ATc��F�x�N�g�����X�V����
		SetLocalFluidTemp(id,temp);
		//�M�`�B���E�������g����悤�ɁA����B
		//for(unsigned i =0;i < vertices[id].tets.size();i++){
		//	CreateVecfLocal(tets[vertices[id].tets[i]]);		//	Tc���܂ރx�N�g�����X�V����
		//}
		InitCreateVecf_();
		for(unsigned i=0; i < GetPHFemMesh()->tets.size();i++){// shiba this��GetPHFemMesh()
			CreateVecFAll(i);				///	VeecF�̍č쐬
													///	MatK2�̍č쐬��matK1��matk1�̕ϐ��ɓ���Ă����āAmatk2�����A����āA���Z
		}
//	}
}

void PHFemThermo::SetRhoSpheat(double r,double Spheat){
	//> ���x�A��M of ���b�V���̃O���[�o���ϐ�(=���b�V���ŗL�̒l)���X�V
	rho = r;
	specificHeat = Spheat;
}

std::vector<Vec2d> PHFemThermo::CalcIntersectionPoint2(unsigned id0,unsigned id1,double r,double R){

	PHFemMeshNew* mesh = GetPHFemMesh(); // shiba

	//	2�_��ʂ钼����1��	2�̒萔�����߂�
	double constA = 0.0;
	double constB = 0.0;
	///	r�ƌ�_
	double constX1 = 0.0;
	double constX1_ = 0.0;
	double constY1 = 0.0;
	///	R�ƌ�_
	double constX2 = 0.0;
	double constX2_ = 0.0;
	double constY2 = 0.0;

	//> �����̑�֏���	�֐��������Ƃ��ɁA�ϊ����遫
	unsigned vtxId0 = id0;
	unsigned vtxId1 = id1;
	/// ���_�ɋ߂����ɕ��ёւ�
	if(vertexVars[vtxId0].disFromOrigin > vertexVars[vtxId1].disFromOrigin){
		unsigned tempId = vtxId0;
		vtxId0 = vtxId1;
		vtxId1 = tempId;
	}
	/// vtxId0 < vtxId1 ���ۏ؂���Ă���

	//	2�_�Ō���邱�Ƃ��O��
	//> 2�_��disFromOrigin��r,R�Ɣ�r���Ăǂ���ƌ���邩�𔻕ʂ���B
	//> �~�Ƃ̌�_�����߂�
	// x-z���ʂōl���Ă���
	/// constA,B:vtxId0.vtxId1��ʂ钼���̌X���ƐؕЁ@/// aconsta,constb�͐����\��Ȃ�
	DSTR << "id0: " << id0 << ", id1: " << id1 <<std::endl;
	constA = ( mesh->vertices[vtxId0].pos.z - mesh->vertices[vtxId1].pos.z) / ( mesh->vertices[vtxId0].pos.x - mesh->vertices[vtxId1].pos.x);
	DSTR << "vertices[vtxId0].pos.z: " << mesh->vertices[vtxId0].pos.z <<std::endl;
	DSTR << "vertices[vtxId0].pos.z: " << mesh->vertices[vtxId1].pos.z <<std::endl;
	DSTR << "dz: vertices[vtxId0].pos.z - vertices[vtxId1].pos.z : " << mesh->vertices[vtxId0].pos.z - mesh->vertices[vtxId1].pos.z << std::endl;

	DSTR << "vertices[vtxId0].pos.x: " << mesh->vertices[vtxId0].pos.x << std::endl;
	DSTR << "vertices[vtxId1].pos.x: " << mesh->vertices[vtxId1].pos.x << std::endl;
	DSTR << "dx: vertices[vtxId0].pos.x - vertices[vtxId1].pos.x: " << mesh->vertices[vtxId0].pos.x - mesh->vertices[vtxId1].pos.x << std::endl;

	DSTR << "constA = dz / dx: " << constA << std::endl;
	if(mesh->vertices[vtxId0].pos.z == mesh->vertices[vtxId1].pos.z && mesh->vertices[vtxId0].pos.x == mesh->vertices[vtxId1].pos.x){
		DSTR << "vertices[vtxId0].pos.y: " << mesh->vertices[vtxId0].pos.y << ", vertices[vtxId1].pos.y: " << mesh->vertices[vtxId1].pos.y << std::endl;
		if(mesh->vertices[vtxId0].pos.y == mesh->vertices[vtxId1].pos.y)
			DSTR << "id[" << id0 <<"], id[" << id1 << "] �͓������_ !" << std::endl;
	}
	
	constB = mesh->vertices[vtxId0].pos.z - constA * mesh->vertices[vtxId0].pos.x;
	DSTR << "constB = vertices[vtxId0].pos.z - constA * vertices[vtxId0].pos.x : " << mesh->vertices[vtxId0].pos.z - constA * mesh->vertices[vtxId0].pos.x << std::endl;

	//DSTR << "constA: " << constA << std::endl;
	//DSTR << "constB: " << constB << std::endl;
	//DSTR << std::endl;

	///	��_�̍��W���v�Z
	if(vertexVars[vtxId0].disFromOrigin < r){		/// ���ar�̉~�ƌ����Ƃ�
		//CalcYfromXatcross(vtxId0,vtxId1,r);	//�֐������Ȃ�
		//> �ȉ��A�֐���,vtxId0,1,r:�����AconstY��Ԃ�
		constX1 = (- constA * constB + sqrt(r * r *(constA * constA + 1.0) - constB * constB));
		constX1_ = (- constA * constB - sqrt(r * r *(constA * constA + 1.0) - constB * constB));
		// �ǂ��炩�����_�̊Ԃɂ���@�召���킩��Ȃ��̂ŁAor�ŁA�_1��x���W�A2��x���W�ƁA���̓���ւ��ƁA���
		//> �����̗��[�̓_�̊Ԃɂ���Ƃ�
		if( (mesh->vertices[vtxId0].pos.x <= constX1 && constX1 <= mesh->vertices[vtxId1].pos.x) || (mesh->vertices[vtxId1].pos.x <= constX1 && constX1 <= mesh->vertices[vtxId0].pos.x) ){
			constY1 = sqrt(r * r - constX1 * constX1 );
		}else{
			constY1 = sqrt(r * r - constX1_ * constX1_ );
			constX1 = constX1_;		///		�_��x���W��constX_�������������킩�����B
		}
	}else if(vertexVars[vtxId0].disFromOrigin < R){		/// ���aR�̉~�ƌ����Ƃ�
		constX1 = (- constA * constB + sqrt(R * R *(constA * constA + 1.0) - constB * constB));
		constX1_ = (- constA * constB - sqrt(R * R *(constA * constA + 1.0) - constB * constB));
		// �ǂ��炩�����_�̊Ԃɂ���@�召���킩��Ȃ��̂ŁAor�ŁA�_1��x���W�A2��x���W�ƁA���̓���ւ��ƁA���
		//> �����̗��[�̓_�̊Ԃɂ���Ƃ�
		if( (mesh->vertices[vtxId0].pos.x <= constX1 && constX1 <= mesh->vertices[vtxId1].pos.x) || (mesh->vertices[vtxId1].pos.x <= constX1 && constX1 <= mesh->vertices[vtxId0].pos.x) ){
			constY1 = sqrt(R * R - constX1 * constX1 );
		}else{
			constY1 = sqrt(R * R - constX1_ * constX1_ );
			constX1 = constX1_;		///		�_��x���W��constX_�������������킩�����B
		}
		
	}
	//> �ǂ���Ƃ������Ƃ�
	else if(vertexVars[vtxId0].disFromOrigin < r && R < vertexVars[vtxId1].disFromOrigin){
		//> �萔��2�~����
		constX1 = (- constA * constB + sqrt(r * r *(constA * constA + 1.0) - constB * constB));
		constX1_ = (- constA * constB - sqrt(r * r *(constA * constA + 1.0) - constB * constB));
		// �ǂ��炩�����_�̊Ԃɂ���@�召���킩��Ȃ��̂ŁAor�ŁA�_1��x���W�A2��x���W�ƁA���̓���ւ��ƁA���
		//> �����̗��[�̓_�̊Ԃɂ���Ƃ�
		if( (mesh->vertices[vtxId0].pos.x <= constX1 && constX1 <= mesh->vertices[vtxId1].pos.x) || (mesh->vertices[vtxId1].pos.x <= constX1 && constX1 <= mesh->vertices[vtxId0].pos.x) ){
			constY1 = sqrt(r * r - constX1 * constX1 );
		}else{
			constY1 = sqrt(r * r - constX1_ * constX1_ );
			constX1 = constX1_;		///		�_��x���W��constX_�������������킩�����B
		}
		constX2 = (- constA * constB + sqrt(R * R *(constA * constA + 1.0) - constB * constB));
		constX2_ = (- constA * constB - sqrt(R * R *(constA * constA + 1.0) - constB * constB));
		//> �����̗��[�̓_�̊Ԃɂ���Ƃ�
		if( (mesh->vertices[vtxId0].pos.x <= constX2 && constX2 <= mesh->vertices[vtxId1].pos.x) || (mesh->vertices[vtxId1].pos.x <= constX2 && constX2 <= mesh->vertices[vtxId0].pos.x) ){
			constY2 = sqrt(R * R - constX2 * constX2 );
		}else{
			constY2 = sqrt(R * R - constX2_ * constX2_ );
			constX2 = constX2_;		///		�_��x���W��constX_�������������킩�����B
		}
	}
	std::vector<Vec2d> intersection;
	intersection.push_back(Vec2d(constX1,constY1));
	if(constX2 && constY2){
		intersection.push_back(Vec2d(constX2,constY2));
	}
		//Vec4d interSection;
		//interSection[0] = constX1;
		//interSection[1] = constY1;
		//interSection[2] = constX2;
		//interSection[3] = constY2;
		DSTR << __FILE__  << "(" <<  __LINE__ << "):"<< "intersection Vtx: " << intersection[0] << std::endl;
		//":" <<  __TIME__ << 
		//return interSection;
		return intersection;
}

Vec2d PHFemThermo::CalcIntersectionPoint(unsigned id0,unsigned id1,double r,double R){

	PHFemMeshNew* mesh = GetPHFemMesh(); // shiba

	double constA = 0.0;
	double constB = 0.0;
	double constX = 0.0;
	double constX_ = 0.0;
	double constY = 0.0;

	//> �����̑�֏���	�֐��������Ƃ��ɁA�ϊ����遫
	unsigned vtxId0 = id0;
	unsigned vtxId1 = id1;
	/// ���_�ɋ߂����ɕ��ёւ�
	if(vertexVars[vtxId0].disFromOrigin > vertexVars[vtxId1].disFromOrigin){
		unsigned tempId = vtxId0;
		vtxId0 = vtxId1;
		vtxId1 = tempId;
	}
	/// vtxId0 < vtxId1 ���ۏ؂���Ă���

	//> 2�_��disFromOrigin��r,R�Ɣ�r���Ăǂ���ƌ���邩�𔻕ʂ���B
	if( (r <= vertexVars[vtxId0].disFromOrigin && vertexVars[vtxId0].disFromOrigin <= R) ^ (r <= vertexVars[vtxId1].disFromOrigin && vertexVars[vtxId1].disFromOrigin <= R)){
		//> �~�Ƃ̌�_�����߂�
		// x-z���ʂōl���Ă���
		/// constA,B:vtxId0.vtxId1��ʂ钼���̌X���ƐؕЁ@/// aconsta,constb�͐����\��Ȃ�
		constA = ( mesh->vertices[vtxId0].pos.z - mesh->vertices[vtxId1].pos.z) / ( mesh->vertices[vtxId0].pos.x - mesh->vertices[vtxId1].pos.x);
		constB = mesh->vertices[vtxId0].pos.z - constA * mesh->vertices[vtxId0].pos.x;

		///	��_�̍��W���v�Z
		if(vertexVars[vtxId0].disFromOrigin < r){		/// ���ar�̉~�ƌ����Ƃ�
			//CalcYfromXatcross(vtxId0,vtxId1,r);	//�֐������Ȃ�
			//> �ȉ��A�֐���,vtxId0,1,r:�����AconstY��Ԃ�
			constX = (- constA * constB + sqrt(r * r *(constA * constA + 1.0) - constB * constB));
			constX_ = (- constA * constB - sqrt(r * r *(constA * constA + 1.0) - constB * constB));
			// �ǂ��炩�����_�̊Ԃɂ���@�召���킩��Ȃ��̂ŁAor�ŁA�_1��x���W�A2��x���W�ƁA���̓���ւ��ƁA���
			//> �����̗��[�̓_�̊Ԃɂ���Ƃ�
			if( (mesh->vertices[vtxId0].pos.x <= constX && constX <= mesh->vertices[vtxId1].pos.x) || (mesh->vertices[vtxId1].pos.x <= constX && constX <= mesh->vertices[vtxId0].pos.x) ){
				constY = sqrt(r * r - constX * constX );
			}else{
				constY = sqrt(r * r - constX_ * constX_ );
				constX = constX_;		///		�_��x���W��constX_�������������킩�����B
			}
		}else if(r < vertexVars[vtxId0].disFromOrigin && vertexVars[vtxId0].disFromOrigin < R){		/// ���aR�̉~�ƌ����Ƃ�
			constX = (- constA * constB + sqrt(r * R *(constA * constA + 1.0) - constB * constB));
			constX_ = (- constA * constB - sqrt(r * R *(constA * constA + 1.0) - constB * constB));
			// �ǂ��炩�����_�̊Ԃɂ���@�召���킩��Ȃ��̂ŁAor�ŁA�_1��x���W�A2��x���W�ƁA���̓���ւ��ƁA���
			//> �����̗��[�̓_�̊Ԃɂ���Ƃ�
			if( (mesh->vertices[vtxId0].pos.x <= constX && constX <= mesh->vertices[vtxId1].pos.x) || (mesh->vertices[vtxId1].pos.x <= constX && constX <= mesh->vertices[vtxId0].pos.x) ){
				constY = sqrt(R * R - constX * constX );
			}else{
				constY = sqrt(R * R - constX_ * constX_ );
				constX = constX_;		///		�_��x���W��constX_�������������킩�����B
			}
		}
		//> �ǂ���Ƃ������Ƃ�
		else if(vertexVars[vtxId0].disFromOrigin < r && R < vertexVars[vtxId1].disFromOrigin){
			//> �萔��2�~����
		}
	}
		Vec2d interSection;
		interSection[0] = constX;
		interSection[1] = constY;
		DSTR << __FILE__  << "(" <<  __LINE__ << "):"<< "interSection: " << interSection << std::endl;
		//":" <<  __TIME__ << 
		return interSection;
}

void PHFemThermo::ArrangeFacevtxdisAscendingOrder(int faceID){

	PHFemMeshNew* mesh = GetPHFemMesh();

	///	3�_�����_�ɋ߂����ɕ��ׂ�		//>	�N�C�b�N�\�[�g�ɂ����������H
	int vtxmin[3];		///	�ʂ��̒��_�ԍ�������
	vtxmin[0] = mesh->faces[faceID].vertexIDs[0];
	vtxmin[1] = 0;
	vtxmin[2] = 0;
	if(vertexVars[mesh->faces[faceID].vertexIDs[1]].disFromOrigin < vertexVars[mesh->faces[faceID].vertexIDs[0]].disFromOrigin ){
		vtxmin[0] = mesh->faces[faceID].vertexIDs[1];
		vtxmin[1] = mesh->faces[faceID].vertexIDs[0];
	}else{
		vtxmin[1] = mesh->faces[faceID].vertexIDs[1];
	}
	if(vertexVars[mesh->faces[faceID].vertexIDs[2]].disFromOrigin < vertexVars[vtxmin[0]].disFromOrigin){
		vtxmin[2] = vtxmin[1];
		vtxmin[1] = vtxmin[0];
		vtxmin[0] = mesh->faces[faceID].vertexIDs[2];
	}else if(vertexVars[vtxmin[0]].disFromOrigin < vertexVars[mesh->faces[faceID].vertexIDs[2]].disFromOrigin && vertexVars[mesh->faces[faceID].vertexIDs[2]].disFromOrigin < vertexVars[vtxmin[1]].disFromOrigin){
		vtxmin[2] = vtxmin[1];
		vtxmin[1] = mesh->faces[faceID].vertexIDs[2];
	}else if(vertexVars[vtxmin[1]].disFromOrigin < vertexVars[mesh->faces[faceID].vertexIDs[2]].disFromOrigin ){
		vtxmin[2] = mesh->faces[faceID].vertexIDs[2];
	}
	//>	���������ɂȂ��Ă��Ȃ��Ƃ��́Aassert(0)
	if( !(vertexVars[vtxmin[0]].disFromOrigin < vertexVars[vtxmin[1]].disFromOrigin && vertexVars[vtxmin[1]].disFromOrigin < vertexVars[vtxmin[2]].disFromOrigin )
		){	assert(0);}
	/// debug
	//DSTR << "�������� ";
	//for(unsigned j=0; j <3; j++){
	//	DSTR << vertices[vtxmin[j]].disFromOrigin;
	//	if(j<2){ DSTR << ", ";}
	//}
	//DSTR << std::endl;
	/// face���̔z���face���ł̌��_����߂����Ԃ��i�[
	for(unsigned i=0;i<3;i++){
		faceVars[faceID].ascendVtx[i] = vtxmin[i];
	}
	//DSTR << "vtxmin[0~2]:  " << vtxmin[0] <<" ," << vtxmin[1] << " ,"  << vtxmin[2] << std::endl;	
	//vtxmin[0~2]:  11 ,324 ,281 ,vtxmin[0~2]:  353 ,11 ,324 ���̏��Ɍ��_�ɋ߂�

}
Vec2d PHFemThermo::CalcIntersectionOfCircleAndLine(unsigned id0,unsigned id1,double radius){
	//	x-z���ʂł̔��ar�̉~�Ɛ����̌�_�̍��W�����߂�
	//	�֐����Ă΂������F����邱�Ƃ������ȂƂ��A�����~�ʂ̔��a�ƁA�~�ʂ̓��E�O���̒��_�����󂯌v�Z

	PHFemMeshNew* mesh = GetPHFemMesh();

	// ...2�_��ʂ钼���̕������̌X���ƐؕЂ����߂�
	double constA = 0.0;	//	�X��
	double constB = 0.0;	//	�ؕ�
	double constX1 = 0.0;	//	��_��x ���W�P
	double constX1_ = 0.0;	//		  x ���W���Q
	double constZ1 = 0.0;	//		  Y ���W
	double constZ1_ = 0.0;	//		  Y ���W���Q
	double constx = 0.0;	//	1���֐��ɂȂ�Ȃ��ꍇ�̒萔
	double constz = 0.0;	//�@		����

	//	...���_�ԍ������_�ɋ߂�����������( id0 < id1 )�ɕ��ёւ�
	if( vertexVars[ id1 ].disFromOrigin	<	vertexVars[ id0 ].disFromOrigin ){
		unsigned farfromOriginId = id0;
		id0 = id1;
		id1 = farfromOriginId;
	}	// id0 < id1 ��ۏ�

	// 1���֐��ɂȂ�Ȃ��ꍇ
	// .. x == const
	if(mesh->vertices[id0].pos.x == mesh->vertices[id1].pos.x && mesh->vertices[id0].pos.z != mesh->vertices[id1].pos.z ){
		//	x == const �̎�

	}
	// .. z == const

	// ...�X���ƐؕЂ����߂�
	//
	constA = ( mesh->vertices[id0].pos.z - mesh->vertices[id1].pos.z) / ( mesh->vertices[id0].pos.x - mesh->vertices[id1].pos.x);
	constB = mesh->vertices[id0].pos.z - constA * mesh->vertices[id0].pos.x;
	
	
	// ..���̏����́A�ȍ~�̏����ŃG���[���o��ꍇ��m�点�邽�߂́A�f�o�b�O�p
	if(!constA && !constB){
		//if( id0 != id1)
		//	vertices[id0].pos.z != 0 && vertices[id1].pos.z!= 0 && vertices[id0].pos.x
		// z == 0�Ƃ��������̎��ł���B�Ƃ������O�Ƃ��������̎��̎������낤�B
		//	���̏�������̈Ӗ��́A�����̗��[�̓_��x,z���W��(0,0)��A�����̎����P���֐��ɂȂ�Ȃ��ƋL���������Ƃ��ł���Ƃ��ɁA�p����B
		DSTR << "�����̎��̐ؕЂƌX��������0" << std::endl;
		DSTR << "id0: " << id0 << ", id1: " << id1 << "radius: " << radius << std::endl;
		DSTR << "vertices[id0].pos.x: " << mesh->vertices[id0].pos.x << ", vertices[id1].pos.x: " << mesh->vertices[id1].pos.x << std::endl;
		DSTR << "vertices[id0].pos.z: " << mesh->vertices[id0].pos.z << ", vertices[id1].pos.z: " << mesh->vertices[id1].pos.z << std::endl;
		assert(0);
	}
	//DSTR << "constA: " << constA << ",  " << "constB: " << constB <<std::endl; 

	//debug
	//DSTR << "id0: " << id0 << ", id1: " << id1 <<std::endl;
	//DSTR << "vertices[id0].pos.z: " << vertices[id0].pos.z <<std::endl;
	//DSTR << "vertices[id0].pos.z: " << vertices[id1].pos.z <<std::endl;
	//DSTR << "dz: vertices[id0].pos.z - vertices[id1].pos.z : " << vertices[id0].pos.z - vertices[id1].pos.z << std::endl;
	//DSTR << "vertices[id0].pos.x: " << vertices[id0].pos.x << std::endl;
	//DSTR << "vertices[id1].pos.x: " << vertices[id1].pos.x << std::endl;
	//DSTR << "dx: vertices[id0].pos.x - vertices[id1].pos.x: " << vertices[id0].pos.x - vertices[id1].pos.x << std::endl;

	// for Check Debug
	//DSTR << "constA = dz / dx: " << constA << std::endl;
	//if(vertices[id0].pos.z == vertices[id1].pos.z && vertices[id0].pos.x == vertices[id1].pos.x){
	//	DSTR << "vertices[id0].pos.y: " << vertices[id0].pos.y << ", vertices[id1].pos.y: " << vertices[id1].pos.y << std::endl;
	//	if(vertices[id0].pos.y == vertices[id1].pos.y)
	//		DSTR << "id[" << id0 <<"], id[" << id1 << "] �͓������_ !" << std::endl;
	//}
	//DSTR << "constB = vertices[id0].pos.z - constA * vertices[id0].pos.x : " << vertices[id0].pos.z - constA * vertices[id0].pos.x << std::endl;
	//DSTR << "constA: " << constA << std::endl;
	//DSTR << "constB: " << constB << std::endl;
	//DSTR << std::endl;

	///	.��_�̍��W���v�Z
	// .�P���֐��̏ꍇ�Ax,z���ɕ��s�Ȓ����̏ꍇ������
	if(vertexVars[id0].disFromOrigin <= radius && radius <= vertexVars[id1].disFromOrigin ){		/// ���ar�̉~�ƌ����ׂ�
		double radius2 = radius * radius;
		constX1  = (- constA * constB + sqrt(radius2 * (constA * constA + 1.0) - constB * constB )  )  /  (constA * constA + 1);
		constX1_ = (- constA * constB - sqrt(radius2 * (constA * constA + 1.0) - constB * constB )  )  /  (constA * constA + 1);
		// ��_��x���W�������̗��[�_��x���W�Ԃɂ���Ƃ�
		if( (mesh->vertices[id0].pos.x <= constX1 && constX1 <= mesh->vertices[id1].pos.x) || (mesh->vertices[id1].pos.x <= constX1 && constX1 <= mesh->vertices[id0].pos.x) ){
			constZ1 = sqrt(radius2 - constX1  * constX1  );
			constZ1_ = - sqrt(radius2 - constX1  * constX1  );
			//	��_��z���W�����[�_��z���W�Ԃɂ���Ƃ�
			if( (mesh->vertices[id0].pos.z <= constZ1_ && constZ1_ <= mesh->vertices[id1].pos.z) || (mesh->vertices[id1].pos.z <= constZ1_ && constZ1_ <= mesh->vertices[id0].pos.z) ){
				constZ1 = constZ1_;
			}
		}else{
			constX1 = constX1_;		///		�_��x���W��constX_�������������킩�����B
			constZ1  =   sqrt(radius2 - constX1  * constX1  );
			constZ1_ = - sqrt(radius2 - constX1  * constX1  );
			//	��_��z���W�����[�_��z���W�Ԃɂ���Ƃ�
			if( (mesh->vertices[id0].pos.z <= constZ1_ && constZ1_ <= mesh->vertices[id1].pos.z) || (mesh->vertices[id1].pos.z <= constZ1_ && constZ1_ <= mesh->vertices[id0].pos.z) ){
				constZ1 = constZ1_;
			}
		}
	}else{
		DSTR << "CalcVtxCircleAndLine()�֐��̂��̒��_�g�݂Ɖ~�ʂ͌����܂���" << std::endl;
		constX1 = 0.0;
		constZ1 = 0.0;
		DSTR << "(id0, vertices[id0].disFromOrigin): (" << id0 << ", " << vertexVars[id0].disFromOrigin << "), (id1, vertices[id1].disFromOrigin): (" << id1 << ", " << vertexVars[id1].disFromOrigin << "), radius: " << radius << std::endl;  
		assert(0);
	}
	Vec2d intersection = Vec2d(constX1,constZ1);
	//":" <<  __TIME__ << 
	return intersection;
}		//	CalcVtxCircleAndLine() :difinition

void PHFemThermo::ShowIntersectionVtxDSTR(unsigned faceID,unsigned faceVtxNum,double radius){
	
	PHFemMeshNew* mesh = GetPHFemMesh();
	
	unsigned i = faceID;
	unsigned j = faceVtxNum;
	DSTR << "ascendVtx[" << j << "]: " << faceVars[i].ascendVtx[j] << ", " << "[ " << (j+1)%3 << "]: " << faceVars[i].ascendVtx[(j+1)%3] << "; ";
	DSTR << " (vertices[" << faceVars[i].ascendVtx[j] << "].pos.x, .z) = ( " <<  mesh->vertices[faceVars[i].ascendVtx[j]].pos.x << ", "<<  mesh->vertices[faceVars[i].ascendVtx[j]].pos.z  << "), " ;
	DSTR << " (vertices[" << faceVars[i].ascendVtx[(j+1)%3] << "].pos.x, .z) : ( " <<  mesh->vertices[faceVars[i].ascendVtx[(j+1)%3]].pos.x << ", "<<  mesh->vertices[faceVars[i].ascendVtx[(j+1)%3]].pos.z << "), " <<std::endl;
	DSTR <<"face[i].[(" << j << "], [" << (j+1)%3 << "]�F�e�X�̌��_����̋���" << vertexVars[faceVars[i].ascendVtx[j]].disFromOrigin << ", " << vertexVars[faceVars[i].ascendVtx[(j+1)%3]].disFromOrigin << ", "; 
	DSTR << " radius: " << radius <<" ��2�_�ō\�����������Ƃ̌�_�͉��L"<< std::endl;
	DSTR << __FILE__  << "(" <<  __LINE__ << "):"<< "Intersection Vtx (x,z)= " << CalcIntersectionOfCircleAndLine( faceVars[i].ascendVtx[ j ] , faceVars[i].ascendVtx[ (j+1)%3 ] , radius) << std::endl;
	DSTR << std::endl;
}

void PHFemThermo::CalcIHarea(double radius,double Radius,double dqdtAll){

	PHFemMeshNew* mesh = GetPHFemMesh();

	//	face�\���̃����oiharea���v�Z
	//�A���b�V���S�̂�iharea�̍��v���v�Z
	//	...dqdtRatio(�M������)�𑍖ʐςɑ΂���iharea�ʐϔ䂩���΂̎��ɁAface���󂯎��ׂ�dqdt���v�Z���ĕԂ�

	//	�`��֐��̌v�Z�́Aiharea ��p���āACalcVecf2surface()���ōs��,face.shapefunk�ōs��		���N���X�����悤���H
		//	.... �`��֐����i�[����		// �����ŕ�����`��֐��́A���_�Ԃ̋������番������`��ԌW���@���Ȃ킿�A�����@�O�����`�����P�ŗǂ��āA�Ō�ɁA�s��ɓ����O�ɁA�����ȊO������΂����̂��ȁH

	// radius value check
	if(Radius <= radius){
		DSTR << "inner radius size is larger than outer Radius " << std::endl;
		DSTR << "check and set another value" << std::endl;
		assert(0);
	}
	///	�����a�ƊO���a�̊Ԃ̐ߓ_�ɔM�������E������ݒ�
	//> �l�ʑ̖ʂ̎O�p�`�Ɖ~�̈�̏d�������̌`��E�ʐς����߂铖���蔻����v�Z����B
	//>	�؂���`��ɉ������`��֐������߁A�M�����x�N�g���̐����ɑ�����A�v�Z����

	//> 1.�~�̈�Əd�Ȃ�face�O�p�`�̌`����Z�o����B�̈�Ɋ܂܂�钸�_�Aface�O�p�`�̕ӂƂ̌�_�����߂�vecteor�Ɋi�[����
	//>	2.vector�ɂ́A��0,1,2�̏��ɗ̈���̒��_���_�������Ă��邪�A��������ɎO�p�`�������s���B�O�p�`�������ł�����A�e�O�p�`�����߂�B�O�p�`�̑��a���A����face�̉��M�̈�Ƃ���B
	//>	3.vector�̓_�ɂ�����`��֐������߂āA�[���̐ρi�d�Ȃ��Ă���ʐρ~�`��֐��̒l�j���g���āA�l�ʑ̓��̊e�_�ɂ�����`��֐��̖ʐϕ������߂�B���߂��l�́A�M�����x�N�g���̐����Ƃ��ėv�f�����s��̐����ɑ������B
	//>	4.���X�e�b�v�A�����M�����̒l���x�N�g�������ɉ�����
	
	/// debug
	//unsigned numIHheated0 = 0; 
	//for(unsigned i=0; i < nSurfaceFace;i++){
	//	if(faces[i].mayIHheated){	
	//		DSTR << i << " ; "  << std::endl;
	//		numIHheated0 +=1;
	//	}
	//}
	//DSTR << "numIHheated0 / nSurfaceFace: " << numIHheated0 << " / " << nSurfaceFace << std::endl;	////	761 / 980	���ĂقƂ�ǂ���Ȃ����I�����ʂɂȂ�Ȃ��Ƃ��������͂������E�E�E�@���[�J��y���W�l���}�C�i�X�̂��̂�I��ł���̂�

	//	debug	mayIHheated�̊m�x���グ��O�̐���m�肽��
	unsigned numIHheated0 = 0; 
	for(unsigned i=0; i < mesh->nSurfaceFace;i++){
		if(faceVars[i].mayIHheated){	
			//DSTR << i << " ; "  << std::endl;
			numIHheated0 +=1;
		}
	}
	DSTR << "numIHheated0 / nSurfaceFace: " << numIHheated0 << " / " << mesh->nSurfaceFace << std::endl;

	//	face���_�̂ǂꂩ1���A�~�̈�ɓ����Ă���face�����Atrue�ɁA����ȊO�́Afalse��
	//> raius,Radius�ɂ���mayIHheated�̊m�x���グ�Ă���A�~�̈�Əd�Ȃ��Ă���`������߂�
	for(unsigned i=0;i < mesh->nSurfaceFace; i++){
		if(faceVars[i].mayIHheated){			// face�̐ߓ_��y���W�����̏ꍇ��IH���M�̑Ώېߓ_ �~�͈͓̔��ɓ����Ă���Ƃ͌���Ȃ�
			for(unsigned j=0;j<3;j++){
				/// �~�̈����face���_���܂܂��
				if(radius <= vertexVars[mesh->faces[i].vertexIDs[j]].disFromOrigin && vertexVars[mesh->faces[i].vertexIDs[j]].disFromOrigin <= Radius){
					faceVars[i].mayIHheated = true;
					break;		//>	����������A�����true�̂܂܂ŗǂ��B�œ�����for�𔲂���
				}
				else{
					faceVars[i].mayIHheated = false;
				}
				//> �i�~�̈�ɂ͊܂܂ꂸ�j�~�̈�������ƊO����face�̕ӂ̒��_������	vertices[j%3] �� vertices[(j+1)%3]�@�ō��ӂ�����Ƃ�
				if(vertexVars[mesh->faces[i].vertexIDs[j]].disFromOrigin < radius && Radius < vertexVars[mesh->faces[i].vertexIDs[(j+1)%3]].disFromOrigin 
					|| vertexVars[mesh->faces[i].vertexIDs[(j+1)%3]].disFromOrigin < radius && Radius < vertexVars[mesh->faces[i].vertexIDs[j]].disFromOrigin){
						faceVars[i].mayIHheated = true;
						break;		//>	����
				}else{
					faceVars[i].mayIHheated = false;
				}
				//>	�~�̈����face�ӂ̂ǂ��炩�̒��_���܂܂��Ƃ�(r<P1<R<P2,P1<r<P2<R,(��Pa1,P2�����ւ�������))
				if(radius <= vertexVars[mesh->faces[i].vertexIDs[j]].disFromOrigin && vertexVars[mesh->faces[i].vertexIDs[j]].disFromOrigin < Radius && Radius < vertexVars[mesh->faces[i].vertexIDs[(j+1)%3]].disFromOrigin
					|| radius <= vertexVars[mesh->faces[i].vertexIDs[(j+1)%3]].disFromOrigin && vertexVars[mesh->faces[i].vertexIDs[(j+1)%3]].disFromOrigin < Radius && Radius < vertexVars[mesh->faces[i].vertexIDs[j]].disFromOrigin
					|| vertexVars[mesh->faces[i].vertexIDs[j]].disFromOrigin <= radius && radius < vertexVars[mesh->faces[i].vertexIDs[(j+1)%3]].disFromOrigin && vertexVars[mesh->faces[i].vertexIDs[(j+1)%3]].disFromOrigin < Radius
					|| vertexVars[mesh->faces[i].vertexIDs[(j+1)%3]].disFromOrigin <= radius && radius < vertexVars[mesh->faces[i].vertexIDs[j]].disFromOrigin && vertexVars[mesh->faces[i].vertexIDs[j]].disFromOrigin < Radius){
						faceVars[i].mayIHheated = true;
						break;		//>	����
				}else{
					faceVars[i].mayIHheated = false;
				}
			}		//	for
		}		//	if
	}		//	for
	//> debug
	//>	mayIHheated�̃t���O�������Ă���face�ɂ��̖ʐς̌`��֐���^���Ă݂�B	�d�Ȃ�ʐς�������ƌv�Z�ƁA�����ł������������Ă���΁A���M�ʂɓ���Ă��܂��v�Z�A����
	//> CalcIHdqdt3 or 4

	/// debug
	unsigned numIHheated = 0; 
	for(unsigned i=0; i < mesh->nSurfaceFace;i++){
		if(faceVars[i].mayIHheated){	
			//DSTR << i << " ; "  << std::endl;
			numIHheated +=1;
		}
	}
	DSTR << "numIHheated / nSurfaceFace: " << numIHheated << " / " << mesh->nSurfaceFace << std::endl;		///:	�\��face�̓��A���M�ߓ_���܂�face�̐��A�S��:264/980�@����Ȃ��񂩂� 
	
	//	.. ��_�����߁Afaces�\���̂�vector�ɗ̈���̒��_���_���i�[
	for(unsigned i=0;i < mesh->nSurfaceFace; i++){
		if(faceVars[i].mayIHheated){		//	may �� �u�m���v�ɕω��ς݂̃t���O
			//	area:face�ʐς��v�Z����ĂȂ���΁A�v�Z
			if(faceVars[i].area==0) faceVars[i].area = CalcTriangleArea(mesh->faces[i].vertexIDs[0],mesh->faces[i].vertexIDs[1],mesh->faces[i].vertexIDs[2]);
			////	face���̒��_��disFromOrigin�̒l�Ń\�[�g
			//unsigned nearestvtxnum		=	0;				///	���_�Ɉ�ԋ߂����_��face���_�ԍ�(0~2)
			//for(unsigned j=0;j<3;j++){
			//	double hikaku = DBL_MAX;
			//	if(hikaku > vertices[faces[i].vertices[j]].disFromOrigin){	hikaku = vertices[faces[i].vertices[j]].disFromOrigin;	nearestvtxnum = j;}
			//}
			
			// ... 3�_�����_�ɋ߂����ɕ��ׂ�		vtxOrder:�߂����Ɋi�[,�Ԗڂ��߂����ԁA�i�[�l�����_�ԍ� ==	faces[i].ascendVtx[3]
			ArrangeFacevtxdisAscendingOrder(i);		///	ArrangeVtxdisAscendingOrder(int faceID,int vtx0,int vtx1,int vtx2)
			//DSTR <<  "�����������m�F: " << vertices[vtxOrder[0]].disFromOrigin << ", "<< vertices[vtxOrder[1]].disFromOrigin << ", "<< vertices[vtxOrder[2]].disFromOrigin << std::endl;
			
			// ... face���̊e���_�������Ă���̈�𔻒� 0 | 1 | 2	///	faces[i].ascendVtx[0~2]�ɊY�����钸�_���@�~�̈�̑O��̂ǂ��ɑ��݂��Ă��邩
			// ... vtxdiv[0~2]�ɋ߂����ɕ��񂾒��_�̗̈�ID(0~2)������U�� �� faces[i].ascendVtx[j] �̏��ƑΉ�
			unsigned vtxdiv[3];		//	���_����߂���:0~2�ɕ��בւ���ꂽ���_ID�ɑΉ�����̈���O�̋敪���@�z��
			//DSTR << "faces[i].ascendVtx[0~2]:" ;
			for(unsigned j=0;j<3;j++){
				if( vertexVars[faceVars[i].ascendVtx[j]].disFromOrigin < radius){			vtxdiv[j] = 0;
				/// �~�ʏ���܂݁A�~�ʏ���~�̈���ƒ�`����
				}else if(radius <= vertexVars[faceVars[i].ascendVtx[j]].disFromOrigin && vertexVars[faceVars[i].ascendVtx[j]].disFromOrigin <= Radius ){	vtxdiv[j] = 1;
				}else if(Radius < vertexVars[faceVars[i].ascendVtx[j]].disFromOrigin){		vtxdiv[j] = 2;	}
				//DSTR << faces[i].ascendVtx[j] ;
				//if(j <  2) DSTR << ", " ;
			}
			//DSTR << std::endl;

			//> debug
			//DSTR << "���_�̗̈�ԍ�: " ;
			//for(unsigned j =0;j<3;j++){
			//	DSTR << vtxexistarea[j];
			//	if(j<2) DSTR << ", ";
			//}
			//DSTR << std::endl;
			//int vtxexistareadebug =0;
			//... 2012.2.14�����܂�...

			//...	�z��̐����̒l�̕ω������āA�n�_�A��_�A�ӑ΂ƂȂ�_������vector�Ɋi�[���Ă���
			
			//	vector�ɓ����ۂ̒���!!! %%%%%%%%%%%%%%%%%%
			// %%%	vtxOrder[ ���_����߂����_�̏�(0,1,2) ]:���̒��_ID�̌��_�ɋ߂����ɕ��בւ���ID���i�[		(��:vtxOrder[0] = (ID)278, [1] = (ID)35, [2] = (ID)76 etc)
			// %%%	vtxdiv[ ���_����߂����_�̏�(0,1,2) ]:���̒��_���~�̈�̓����i1�j���O��(0,2)����\��

			//>	face�̕ӂ��Ƃɏꍇ����
			///	 j ��(�ׂ�) (j+1)%3 �Ƃő΂𐬂��ӂɂ���
			for(unsigned j=0;j<3;j++){
				double f[3]={0.0, 0.0, 0.0};	// ���_0,1,2,3���猩���`��֐� 
				//debug
				//DSTR <<"j: " << j << ", faces[i].ascendVtx[j]: " << faces[i].ascendVtx[j] << ", faces[i].ascendVtx[(j+1)%3]: " << faces[i].ascendVtx[(j+1)%3] << std::endl;
				//DSTR << "vertices[faces[i].ascendVtx[j]].pos: (" << vertices[faces[i].ascendVtx[j]].pos.x  << ", "<< vertices[faces[i].ascendVtx[j]].pos.z << ") " << std::endl;
				//DSTR << "vertices[faces[i].ascendVtx[(j+1)%3]].pos: (" << vertices[faces[i].ascendVtx[(j+1)%3]].pos.x  << ", "<< vertices[faces[i].ascendVtx[(j+1)%3]].pos.z << ") " << std::endl; 
				//DSTR << std::endl;

				//	0�̗̈�ɂ����:
				if(vtxdiv[j] == 0 && vtxdiv[(j+1)%3] == 0){
					//	������̓_�����̈��vector�ɂ͓���Ȃ�
				}
				//	�����a�Ƃ���������(��:0���O:1�ƊO:1����:0):
				else if(vtxdiv[j] == 0 && vtxdiv[(j+1)%3] - vtxdiv[j] > 0 || vtxdiv[j] == 1 && vtxdiv[(j+1)%3] - vtxdiv[j] < 0){
					//	(�n�_�i���œ���Ă���ꍇ�ɂ͕s�v�j�j�Ɠ����a�Ƃ��A�΂̓_�ɓ����
					if(vtxdiv[(j+1)%3] - vtxdiv[j] > 0){	//�����O
						//	�����a�Ƃ̌�_�����߂āA���W������
						//%%%	�������\�����钸�_�Ɣ��a�A��_�̃`�F�b�N�֐���DSTR�\��	%%%//		//faceID,face���ߓ_�ԍ��A���a��p���āA�ׂ荇���ߓ_�ō������Ɖ~�ʂ̌�_�����߂ĕ\��
						ShowIntersectionVtxDSTR(i,j,radius);
						//	..�����a�Ƃ̌�_��x,z���W������
						faceVars[i].ihvtx.push_back( CalcIntersectionOfCircleAndLine( faceVars[i].ascendVtx[ j ] , faceVars[i].ascendVtx[ (j+1)%3 ] , radius) );
						//	..�g�Γ_�̍��W��intersection�ɓ����
						faceVars[i].ihvtx.push_back( Vec2d( mesh->vertices[ faceVars[i].ascendVtx[ (j+1)%3 ] ].pos.x, mesh->vertices[faceVars[i].ascendVtx[ (j+1)%3 ] ].pos.z) );
						//	....���̓_�ʒu�ł̌`��֐��𓱏o
					}else if(vtxdiv[(j+1)%3] - vtxdiv[j] < 0){		//�O����
						//	�����a�Ƃ̌�_�����߁A��_�̍��W������
						faceVars[i].ihvtx.push_back( CalcIntersectionOfCircleAndLine( faceVars[i].ascendVtx[ j ] , faceVars[i].ascendVtx[ (j+1)%3 ] , radius) );
						//..�g�Γ_�́A�����a�̓����F�~�̈�O�Ȃ̂ŁA�g�Γ_��vector�ɂ͓���Ȃ�
					}
				}
				//	�~�̈�(:1)���ɂ����
				else if(vtxdiv[j] == 1 && vtxdiv[(j+1)%3] == 1){
					//	(�n�_�����Ă���̂Ȃ�A)�ӑΓ_��vector�ɓ����
					//	�n�_�͓��ꂸ�Ƃ��A�Ō�ɓ���͂�
					////intersection.push_back(Vec2d(vertices[vtxdiv[(j+1)%3]].pos.x,vertices[vtxdiv[(j+1)%3]].pos.z));
					faceVars[i].ihvtx.push_back(Vec2d(mesh->vertices[faceVars[i].ascendVtx[(j+1)%3]].pos.x,	mesh->vertices[faceVars[i].ascendVtx[(j+1)%3]].pos.z));
				}
				//	�O���a�ƌ�����(�����O�A�O����)
				else if(vtxdiv[j] == 1 && vtxdiv[(j+1)%3] == 2 || vtxdiv[j] == 2 && vtxdiv[(j+1)%3] == 1){
					// ���������O���������A�����Ŕ��肷�邱�ƂŁA���	or	�̂ǂ��炩�𔻒肵�Avector�ɓ���鏇�Ԃ�ς���
					//	�O���a�Ƃ̌�_�����߂�
					//	�����O 2 - 1 = 1 > 0
					if(vtxdiv[(j+1)%3] - vtxdiv[j] > 0){
						//	��_���i�[
						faceVars[i].ihvtx.push_back( CalcIntersectionOfCircleAndLine( faceVars[i].ascendVtx[ j ] , faceVars[i].ascendVtx[ (j+1)%3 ] , Radius) );
						////	�g�Γ_��X,Z���W���i�[���i�[������_���ł���I
						//faces[i].ihvtx.push_back(Vec2d(vertices[faces[i].ascendVtx[(j+1)%3]].pos.x,	vertices[faces[i].ascendVtx[(j+1)%3]].pos.z));
						////	?��	intersection.push_back(Vec2d(vertices[vtxOrder[(j+1)%3]].pos.x,vertices[vtxOrder[(j+1)%3]].pos.z));
						
					}
					//	�O���� 1 - 2 = -1 < 0
					else if(vtxdiv[(j+1)%3] - vtxdiv[j] < 0){
						// �O���a�Ƃ̌�_�̍��W���i�[����
						faceVars[i].ihvtx.push_back( CalcIntersectionOfCircleAndLine( faceVars[i].ascendVtx[ j ] , faceVars[i].ascendVtx[ (j+1)%3 ] , Radius) );
						// �g�Γ_���i�[����
						faceVars[i].ihvtx.push_back( Vec2d( mesh->vertices[ faceVars[i].ascendVtx[ (j+1)%3 ] ].pos.x, mesh->vertices[faceVars[i].ascendVtx[ (j+1)%3 ] ].pos.z) );
					}
					else if(vtxdiv[(j+1)%3] - vtxdiv[j] == 0) assert(0);
					//	(�n�_��vector�ɓ����Ă��邱�Ƃ��m�F����)��_��vector�ɓ����
					faceVars[i].ihvtx.push_back( CalcIntersectionOfCircleAndLine( faceVars[i].ascendVtx[ j ] , faceVars[i].ascendVtx[ (j+1)%3 ] , Radius) );
				}
				//	�����a�ƊO���a�Ƃ̂ǂ���Ƃ�������(�����O�A�O����)
				else if(vtxdiv[j] == 0 && vtxdiv[(j+1)%3] == 2 || vtxdiv[j] == 2 && vtxdiv[(j+1)%3] == 0){
					// ���������O���������A�����Ŕ��肷�邱�ƂŁA���	or	�̂ǂ��炩�𔻒肵�Avector�ɓ���鏇�Ԃ�ς���
					//%%%	�ǂ���Ƃ���������
					// �����a�~�ʂ�茴�_��聨�O�F0���Q: 2 - 0 > 0
					if(vtxdiv[(j+1)%3] - vtxdiv[j] > 0){
						//�����a�ƌ����A��_���i�[
						faceVars[i].ihvtx.push_back( CalcIntersectionOfCircleAndLine( faceVars[i].ascendVtx[ j ] , faceVars[i].ascendVtx[ (j+1)%3 ] , radius) );
						//�O�n�ƌ����A��_���i�[
						faceVars[i].ihvtx.push_back( CalcIntersectionOfCircleAndLine( faceVars[i].ascendVtx[ j ] , faceVars[i].ascendVtx[ (j+1)%3 ] , Radius) );
					}else{	// �O�������a�~�ʂ�茴�_���F2���O
						//�O���a�ƌ����A��_���i�[
						faceVars[i].ihvtx.push_back( CalcIntersectionOfCircleAndLine( faceVars[i].ascendVtx[ j ] , faceVars[i].ascendVtx[ (j+1)%3 ] , Radius) );
						//���n�ƌ����A��_���i�[
						faceVars[i].ihvtx.push_back( CalcIntersectionOfCircleAndLine( faceVars[i].ascendVtx[ j ] , faceVars[i].ascendVtx[ (j+1)%3 ] , radius) );
					}
				}
				//	�����a�ƊO���a�ƌ�����(�O�����������)
				//else if(){
				//}
					//	if���̒��ŁA�������v���X���}�C�i�X�Ŕ���ł�����
				
				//	�O���a�̊O���ɂ����
				else if(vtxdiv[j] == 2 && vtxdiv[(j+1)%3] == 2){
					//	������̓_�����̈��vector�ɂ͓���Ȃ�
				}
				//	��������O���ɍs���ӂ͏�ŋL�q�ł��邪�A�O����������Ɍ������ӂ�����ŋL�q�ł���̂��H
			}	//	for(unsigned j=0;j<3;j++){

			//	..vector���O�p�`��������
			//	..���������O�p�`�̖ʐς��e�X���߂�
			// ..IH�ʐςɒǉ�

			//faces[i].iharea = 

			//	�f�o�b�O���ځFface��ihvtx(vector)�ɁAmayihheated�Ńt���O�̗�����face���͐ߓ_���S�Ċ܂܂�Ă��邩�B


		}	//	if(mayIHheated)
	}	//	for(faces[i]


	//>	Step	1.�̎���
	//>	radius:�����a�ARadius:�O���a,�@dqdtAll:�P�ʎ��Ԃ�����ɓ����Ă���S�M��

	//%%	�菇 
	//> isHeated(->nearIHHeater)��faces�̓��A3�̐ߓ_�S���ɂ��āA���蔼�a�̒��ɓ����Ă�����̂����邩�ǂ������`�F�b�N����B
	//>	�����Ă�����̂��A��������A�ʐόv�Z�����������A�����ŁA���̐ߓ_�������Ă��邩�ɂ���ďꍇ�������s���B
	//>	�R��:face�̖ʐς�faceS�ɑ������	1~2��:�ʐς��ߎ��v�Z���邽�߂ɁA3�ߓ_�̓��A�ǂ̐ߓ_�Ɛߓ_�ō\�����ꂽ�ӂ������ar�܂��́A�O���aR�ƌ�����Ă��邩����
	//>	������Ă���ꍇ�ɂ́A���̌�_�����߂�B���߂���_�Ɠ����a�ȏ�O���a�ȓ��̗̈�ɂ���face���̐ߓ_��p���āA�O�p�`�����A���̖ʐς����߂�B

//> �ȉ��R�s�y���̃\�[�X�A������ɏ���
	//>	1.1�̈�Əd������ʐς�face���ɋ��߂�
	double faceS = 0.0;
	for(unsigned i=0;i < mesh->nSurfaceFace; i++){
		if(faceVars[i].mayIHheated){			// face�̐ߓ_��y���W�����̏ꍇ��IH���M�̑Ώېߓ_ �~�͈͓̔��ɓ����Ă���Ƃ͌���Ȃ�
			unsigned nObinnerVtx = 0;

			unsigned innervtx[4] = {0,0,0,0};
			//>	���_������vector or �z��		//>	�~�̈�Əd�Ȃ�`����v�Z���邽�߂ɁA�d�Ȃ�̈���ɂ���face���_�A�ӂƂ̌�_��face�̕�0~2�̏���vector�Ɋi�[���Ă����B�i�[���ɏd���������悤�ɂ���
			std::vector<Vec2d> intersectVtx;
			//Vec2d innertVtx[12];		//>	vector����z��ɕύX���遨�ÓI�������m��,���ǁAvector�̕����R�[�h�͊y?�������̊J����Y�ꂸ��
			unsigned judge[2] = {0,0};		///	judge[0],[1]�̏��Ɍ��_�ɋ߂��_�̔��茋��
			if(faceVars[i].area==0) faceVars[i].area = CalcTriangleArea(mesh->faces[i].vertexIDs[0],mesh->faces[i].vertexIDs[1],mesh->faces[i].vertexIDs[2]);		// �ʐόv�Z���ς�łȂ���Όv�Z����
			
			//> �̈���ɓ����Ă���_�������莟��Aface�̒��_���܂߁Aface.vector�ɓ����Bface�̒��_������ꍇ�ɂ́A�d�����Ȃ����𒲂ׂ�B

			/// face�̊e�ӂ̗��[�̓_�ɂ��ď������� ////	j��0���珇�ɏ����Ă��邾���ŁAface�̕Ӕԍ��ł͂Ȃ�
			for(unsigned j=0; j < 3; j++){
				///	face���̓_�̑g(��)�ɂ��āA���̕ӂ̑g��
				/// vtx[(0+j)%3]�ɂ���,���_�ɋ߂����ɕ��ёւ�	//> == :���������̎��A�������Ȃ��B
				unsigned vtx0 = mesh->faces[i].vertexIDs[(j+0)%3];
				unsigned vtx1 = mesh->faces[i].vertexIDs[(j+1)%3];
				if(vertexVars[vtx0].disFromOrigin > vertexVars[vtx1].disFromOrigin){
					vtx0 = mesh->faces[i].vertexIDs[(j+1)%3];
					vtx1 = mesh->faces[i].vertexIDs[(j+0)%3];
				}
				///		   �_      �_	
				///  �E	  �E )  �E   )	 �E
				///			/       /	 	
				///  ��     ��   �@��	 ��	
				/// ���_�@�����a�@�O���a face���_	
				/// judge[0]:vtx0, judge[1]:vtx1���̈���ɂ��邩�ǂ��� (O(=Origin) < vtx0.disFromOrigin < vtx1.disFromOrigin )
				///	vtx0�� r<, < R, else �̈悲�Ƃ�judge[]

				///	3�_�S�����̈���ɂ���Ƃ��A����vector�Ɋi�[���邾���ł́A�d�����钸�_���o�Ă��Ă��܂��B
				///	�̂ŁA3�_���S���̈���ɂ���Ƃ��́Apush_back�̑O�ɁA���O����K�v������B
				///	�������W�̓_��vector�ɓ����Ă��Ȃ����m�F���Ă���i�[����

				//> �֐�������
				if(vertexVars[vtx0].disFromOrigin < radius){judge[0] =0;}
				else if(Radius < vertexVars[vtx0].disFromOrigin){judge[0] =2;}
				else{
					///	faces�O�p�`���_�̊i�[
					judge[0] = 1;
					//> �̈���̒��_���i�[
					Vec2d tempcoord = Vec2d(mesh->vertices[vtx0].pos.x,mesh->vertices[vtx0].pos.z);		
					//>	�������W�l�̓_���Ȃ���΁A�i�[�Bfind���Ă����֐����������悤�ȋC������.	map ? list?
					unsigned tempjudge =0;
					for(unsigned k =0; k < intersectVtx.size(); k++){
						if(intersectVtx[k] == tempcoord){
							tempjudge += 1;
						}
					}
					if(tempjudge == 0){
						//faces[i].innerIH.push_back(tempcoord);
						faceVars[i].ihvtx.push_back(tempcoord);
//						DSTR << "tempcoord: " << tempcoord <<std::endl;
					}
					for(unsigned k=0;k < intersectVtx.size();k++){
						DSTR << k << "th: " << intersectVtx[k] << std::endl;
					}
					//DSTR << "faces[i].innerIH: " << faces[i].innerIH.end() << std::endl;
				}
				
				///	vtx1�� r<, < R, else �̈悲�Ƃ�judge[]
				if(vertexVars[vtx1].disFromOrigin < radius){judge[1] =0;}
				else if(Radius < vertexVars[vtx1].disFromOrigin){judge[1] =2;}
				else{
					///	faces�O�p�`���_�̊i�[
					judge[1] = 1;
					//> �̈���̒��_���i�[
					Vec2d tempcoord = Vec2d(mesh->vertices[vtx1].pos.x,mesh->vertices[vtx1].pos.z);
					//>	�������W�l�̓_���Ȃ���΁A�i�[�Bfind���Ă����֐����������悤�ȋC������.	map ? list?
					unsigned tempjudge =0;
					for(unsigned k =0; k < intersectVtx.size(); k++){
						if(intersectVtx[k] == tempcoord){
							tempjudge += 1;
						}
					}
					if(tempjudge == 0){
						faceVars[i].ihvtx.push_back(tempcoord);
//						DSTR << "tempcoord: " << tempcoord <<std::endl;
					}
					for(unsigned k=0;k < intersectVtx.size();k++){
						DSTR << k << "th: " << intersectVtx[k] << std::endl;
					}
					//DSTR << "faces[i].innerIH: " << faces[i].innerIH.end() << std::endl;					//DSTR << "faces[i].innerIH: " << faces[i].innerIH[i].end() << std::endl;
				}

				//> ��_�̊i�[
				/// face�̕ӂ̗��[�̒��_��judge�̒l���قȂ�	��	�~�Ƃ̌�_�����߂�
				/// �����a(r),�O���a(R)�Ƃ̌�_�����߂�	[0]!=[1]:��_�����A�ǂ��炩���Ar,R�Ƃ̌�_������
				if(judge[0] == 0 && judge [1]==1 )
				{
					//> radius�Ƃ̌�_�����߂�
					//> ���߂���_��������
					Vec2d vtxXZ = Vec2d(0.0,0.0);		//> ��_�̍��W����
					//> ��_��vector�Ɋi�[
					faceVars[i].ihvtx.push_back(vtxXZ);
					//faces[i].innerIH.push_back(vtxXZ);
					//intersectVtx.push_back(vtxXZ);
				}
				else if(judge[0] == 1 && judge [1]==2 )
				{
					//> R�Ƃ̌�_�����߂�
					//> ���߂���_��vector�Ɋi�[����
				}
				else if(judge[0] == 1 && judge [1]==2 )
				{
					//> r,R�Ƃ̌�_�����߂�
					//> ���߂���_��vector�Ɋi�[����
				}
			}
		}
	}
}

void PHFemThermo::CalcIHdqdt3(double r,double R,double dqdtAll,unsigned num){
	///	�����a�ƊO���a�̊Ԃ̐ߓ_�ɔM�������E������ݒ�

	PHFemMeshNew* mesh = GetPHFemMesh(); // shiba

	//> ���M����l�ʑ̖ʂ̖ʐς̑��a�����߂�
	double faceS = 0.0;
	for(unsigned i=0;i < mesh->nSurfaceFace; i++){
		if(faceVars[i].mayIHheated){			// face�̐ߓ_��y���W�����̏ꍇ��IH���M�̑Ώېߓ_
			unsigned nObinnerVtx = 0;
			if(faceVars[i].area==0) faceVars[i].area = CalcTriangleArea(mesh->faces[i].vertexIDs[0],mesh->faces[i].vertexIDs[1],mesh->faces[i].vertexIDs[2]);
			for(unsigned j=0;j<3;j++){
				if( r <= vertexVars[mesh->faces[i].vertexIDs[j]].disFromOrigin && vertexVars[mesh->faces[i].vertexIDs[j]].disFromOrigin <= R){
					nObinnerVtx += 1;
				}
			}
			if( nObinnerVtx == 1)			faceVars[i].fluxarea = 1.5/3.0 * faceVars[i].area;//faces[i].fluxarea = 1.0/3.0 * faces[i].area;
			else if(nObinnerVtx == 2)		faceVars[i].fluxarea = 2.8/3.0 * faceVars[i].area;//faces[i].fluxarea = 2.0/3.0 * faces[i].area;
			else if(nObinnerVtx == 3)		faceVars[i].fluxarea = faceVars[i].area;
			else if(nObinnerVtx == 0)		faceVars[i].fluxarea = 0;

			if(faceVars[i].fluxarea >= 0){	
				faceS += faceVars[i].fluxarea;
			}else{		assert(0);	}		//	faces[i].fluxarea��0�����̐����������Ă���̂ɉ��Z���悤�Ƃ��Ă���
			//DSTR << "faces[" << i << "].fluxarea: " << faces[i].fluxarea << std::endl;
		}
	}

	//for(unsigned i=0;i < nSurfaceFace; i++){
	//	DSTR << "faces[" << i << "].fluxarea: " << faces[i].fluxarea << std::endl;
	//}

	if(faceS > 0){
		//> dqdt ��P�ʖʐς�����ɒ���([1/m^2])
		double dqdtds = dqdtAll / faceS;
//		DSTR << "dqdtds:  " << dqdtds << std::endl;
		//>	�ȉ��A�M������faces�Ɋi�[����
		//>	�M�����̖ʐόv�Z��fluxarea��p���čs��
		for(unsigned i=0;i < mesh->nSurfaceFace; i++){
			if(faceVars[i].mayIHheated){
				faceVars[i].heatflux[num] = dqdtds * faceVars[i].fluxarea;		//	�M�����̗ʂ�heatflux�̖ʐς���v�Z
//				DSTR << "faces[" << i <<"].heatflux: " << faces[i].heatflux <<std::endl;
			}
		}
	}
	
	//�@�ȏ�A�l�͓����Ă���悤��

	int katoon =0;
	//���������āACreateMatk2t���R�s�[�����֐��ŁAVecf2?�������

	//>	�M�ʂ́Adqdtds��p����

	//> r <= <= R�̒��S������ˏ�ɉ��M

	//	�ߓ_��dqdt�̒l���X�V����

	//�@�ȉ��́A�x�N�g�������֐��̎d��
	//	�ߓ_�̑�����\�ʂ̖ʂŁA�v�Z����
	//  vertices[].heatFluxValue����Ɍv�Z��i�߂�
	//	�K�E�X�U�C�f���v�Z�ł���悤�ɏ����ȂǁA��������

}

void PHFemThermo::CalcIHdqdt2(double r,double R,double dqdtAll,unsigned num){
	///	�����a�ƊO���a�̊Ԃ̐ߓ_�ɔM�������E������ݒ�
	//> �~�ŋ�؂���l�ʑ̖ʂ̗̈���O�p�`�ŋߎ�����

	//> ���M����l�ʑ̖ʂ̖ʐς̑��a�����߂�

	//%%	�菇 
	//> isHeated(->nearIHHeater)��faces�̓��A3�̐ߓ_�S���ɂ��āA���蔼�a�̒��ɓ����Ă�����̂����邩�ǂ������`�F�b�N����B
	//>	�����Ă�����̂��A��������A�ʐόv�Z�����������A�����ŁA���̐ߓ_�������Ă��邩�ɂ���ďꍇ�������s���B
	//>	�R��:face�̖ʐς�faceS�ɑ������	1~2��:�ʐς��ߎ��v�Z���邽�߂ɁA3�ߓ_�̓��A�ǂ̐ߓ_�Ɛߓ_�ō\�����ꂽ�ӂ������ar�܂��́A�O���aR�ƌ�����Ă��邩����
	//>	������Ă���ꍇ�ɂ́A���̌�_�����߂�B���߂���_�Ɠ����a�ȏ�O���a�ȓ��̗̈�ɂ���face���̐ߓ_��p���āA�O�p�`�����A���̖ʐς����߂�B

	PHFemMeshNew* mesh = GetPHFemMesh(); // shiba

	double faceS = 0.0;
	for(unsigned i=0;i < mesh->nSurfaceFace; i++){
		if(faceVars[i].mayIHheated){			// face�̐ߓ_��y���W�����̏ꍇ��IH���M�̑Ώېߓ_ �~�͈͓̔��ɓ����Ă���Ƃ͌���Ȃ�
			unsigned nObinnerVtx = 0;
			unsigned inner[3] = {0,0,0};
			unsigned judge[2] = {0,0};		///	judge[0],[1]�̏��Ɍ��_�ɋ߂��_�̔��茋��
			if(faceVars[i].area==0) faceVars[i].area = CalcTriangleArea(mesh->faces[i].vertexIDs[0],mesh->faces[i].vertexIDs[1],mesh->faces[i].vertexIDs[2]);		// �ʐόv�Z���ς�łȂ���Όv�Z����
				//> �u��������0,1�����ւ��邾��(=�|�����[�t�B�Y��)�Ŏc��̎��������
				// ���L�������֐�������?�B�_��ID������΁A��������������֐�
				//> 0-1�Ō�_������Ă���Ƃ� true false �͘_���Z�̔r���I�_���aXOR��true���A��_���v�Z���� 00=0,01=10=1,11=0; �Е��̓_���͈͓��ŁA����������͈͊O�̏ꍇ�Atrue�ɂȂ�
			
				//> �~�͈͓̔��ɓ����Ă�����AmayIHheated��true�ɁA���邢�́Afalse�ɂ���(�ꉞ)

				/// faces[i]�̒��_�����A�̈���ɓ����Ă��邩��������
				for(unsigned j=0;j<3;j++){
					if( r <= vertexVars[mesh->faces[i].vertexIDs[j]].disFromOrigin && vertexVars[mesh->faces[i].vertexIDs[j]].disFromOrigin <= R){
						nObinnerVtx += 1;
						inner[j] = 1;
					}
				}

				//> nObinnerVtx�̒l�ŏ�������
			
				///	
				if(nObinnerVtx == 1){
					Vec2d vtxXY[3];			/// face�̕ӂƔ��ar,R�Ƃ̌�_��x,z���W�@�ő�R�_
					for(unsigned j=0;j<3;j++){		///	face���\������3���_�ɂ���
						unsigned k = 0;
						unsigned m = 0;
						k = (j+1)%3;		///	j=0;k=1, j=1;k=2, j=2;k=0 
						m = (j+2)%3;
						//vtxXY[j] = CalcIntersectionPoint(faces[i].vertices[j],faces[i].vertices[k],r,R);
						//DSTR << "vtxXY[" << j << "]: " << vtxXY[j] << std::endl; 
						if(inner[j] ==1){			/// faces[i]��j�Ԗڂ̐ߓ_���~�̈���ɓ����Ă���
						/// j�Ԗڂ̒��_�ƃG�b�W���\������_(���̂Q�_)���g���āA���ar,R�ƌ����_���Z�o����						
						//> j 0,1,2
							if(j == 0){	/// ���_j�ƕӂ���钸�_���g���āA�ӂƌ�����_�����߂�
								//k = (j+1)%3;		///	j=0;k=1, j=1;k=2, j=2;k=0 
							vtxXY[j] = CalcIntersectionPoint(mesh->faces[i].vertexIDs[j],mesh->faces[i].vertexIDs[k],r,R);
							vtxXY[k] = CalcIntersectionPoint(mesh->faces[i].vertexIDs[j],mesh->faces[i].vertexIDs[m],r,R);
							}
							else if(j == 1){
							vtxXY[j] = CalcIntersectionPoint(mesh->faces[i].vertexIDs[j],mesh->faces[0].vertexIDs[1],r,R);
							vtxXY[j] = CalcIntersectionPoint(mesh->faces[i].vertexIDs[j],mesh->faces[2].vertexIDs[2],r,R);
							}
							else if(j == 2){
							vtxXY[j] = CalcIntersectionPoint(mesh->faces[i].vertexIDs[j],mesh->faces[0].vertexIDs[1],r,R);
							vtxXY[j] = CalcIntersectionPoint(mesh->faces[i].vertexIDs[j],mesh->faces[1].vertexIDs[2],r,R);
							}
							else{
								assert(0);
								DSTR << "Error in PHFemThermo::CalcIHdqdt2(hogehoge) �̈攻��Ƀ~�X" << std::endl;
							}
						 }
					}
				}
			}
		}

	if(faceS > 0){
		//> dqdt ��P�ʖʐς�����ɒ���([1/m^2])
		double dqdtds = dqdtAll / faceS;
//		DSTR << "dqdtds:  " << dqdtds << std::endl;
		//>	�ȉ��A�M������faces�Ɋi�[����
		//>	�M�����̖ʐόv�Z��fluxarea��p���čs��
		for(unsigned i=0;i < mesh->nSurfaceFace; i++){
			if(faceVars[i].mayIHheated){
				faceVars[i].heatflux[num] = dqdtds * faceVars[i].fluxarea;		//	�M�����̗ʂ�heatflux�̖ʐς���v�Z
//				DSTR << "faces[" << i <<"].heatflux: " << faces[i].heatflux <<std::endl;
			}
		}
	}
	//�@�ȏ�A�l�͓����Ă���悤��
	int katoon =0;
	//���������āACreateMatk2t���R�s�[�����֐��ŁAVecf2?�������

	//>	�M�ʂ́Adqdtds��p����

	//> r <= <= R�̒��S������ˏ�ɉ��M

	//	�ߓ_��dqdt�̒l���X�V����

	//�@�ȉ��́A�x�N�g�������֐��̎d��
	//	�ߓ_�̑�����\�ʂ̖ʂŁA�v�Z����
	//  vertices[].heatFluxValue����Ɍv�Z��i�߂�
	//	�K�E�X�U�C�f���v�Z�ł���悤�ɏ����ȂǁA��������
}

Vec2d PHFemThermo::GetIHbandDrawVtx(){	return IHLineVtxX;	}

void PHFemThermo::SetIHbandDrawVtx(double xS, double xE){
	IHLineVtxX = Vec2d(xS,xE);
}

void PHFemThermo::CalcIHdqdtband_(double xS,double xE,double dqdtAll,unsigned num){
	///	x���W�FxS~xE�̊Ԃ̐ߓ_�ɔM�������E������ݒ�
	// xS,��E�̊Ԃɂ����ꂩ��_������A
	// 
	// mayIHheated�͎g��Ȃ�	:���̏������ŁAface�̑S�ߓ_��y���W�����̂��̂ɂ��Ă̂݁AfacesS�ɉ��Z

	PHFemMeshNew* mesh = GetPHFemMesh();

	//> ���M����l�ʑ̖ʂ̖ʐς̑��a�����߂�
	double faceS = 0.0;			// initialize
	for(unsigned i=0;i < mesh->nSurfaceFace; i++){
		//if(faces[i].mayIHheated){			// face�̐ߓ_��y���W�����̏ꍇ��IH���M�̑Ώېߓ_
			unsigned nObinnerVtx = 0;
			if(faceVars[i].area==0) faceVars[i].area = CalcTriangleArea(mesh->faces[i].vertexIDs[0],mesh->faces[i].vertexIDs[1],mesh->faces[i].vertexIDs[2]);
			//..�S�ߓ_��y<0�Ȃ�Afluxarea�ɉ��Z
			if(mesh->vertices[mesh->faces[i].vertexIDs[0]].pos.y < 0 && mesh->vertices[mesh->faces[i].vertexIDs[1]].pos.y < 0 && mesh->vertices[mesh->faces[i].vertexIDs[2]].pos.y < 0){
				for(unsigned j=0;j<3;j++){
					// ||.. :x, |.|.:o , .|.|:o ,   .||.:o  , ..||:x 
					//. �ǂꂩ�P�_��xS~xE�̊Ԃɂ���A���́A�ׂ荇��2�_���o���h�̊O���ɂ���AIHarea�ɎZ��
					if( xS <= mesh->vertices[mesh->faces[i].vertexIDs[j]].pos.x && mesh->vertices[mesh->faces[i].vertexIDs[j]].pos.x <= xE){
						faceVars[i].fluxarea = faceVars[i].area;
						break;
					}
					//.���_�΂��o���h�����ނƂ�
					if(mesh->vertices[mesh->faces[i].vertexIDs[j]].pos.x <= xS && xE <= mesh->vertices[mesh->faces[i].vertexIDs[(j+1)%3]].pos.x 
						|| mesh->vertices[mesh->faces[i].vertexIDs[(j+1)%3]].pos.x <= xS && xE <= mesh->vertices[mesh->faces[i].vertexIDs[j]].pos.x ){
							faceVars[i].fluxarea = faceVars[i].area;
						break;
					}
				}
			}
			if(faceVars[i].fluxarea >= 0){	
				faceS += faceVars[i].fluxarea;
			}else{		assert(0);	}		//	faces[i].fluxarea��0�����̐����������Ă���̂ɉ��Z���悤�Ƃ��Ă���
			//DSTR << "faces[" << i << "].fluxarea: " << faces[i].fluxarea << std::endl;
		//}
	}

	//..face�ʐςɉ������M������S�̖̂ʐϊ�����苁�߂�
	if(faceS > 0){
		//> dqdt ��P�ʖʐς�����ɒ���([1/m^2])
		//double dqdt_ds = dqdtAll / faceS;
//		DSTR << "dqdtds:  " << dqdtds << std::endl;
		//>	�ȉ��A�M������faces�Ɋi�[����
		//>	�M�����̖ʐόv�Z��fluxarea��p���čs��
		for(unsigned i=0;i < mesh->nSurfaceFace; i++){
			if(faceVars[i].fluxarea){
				faceVars[i].heatflux[num] = faceVars[i].fluxarea / faceS * dqdtAll;		//	�M�����̗ʂ�heatflux�̖ʐς���v�Z
//				DSTR << "faces[" << i <<"].heatflux: " << faces[i].heatflux <<std::endl;			
				// debug
				//for(unsigned j = 0; j < 3;++j){
 				//	vertices[faces[i].vertices[j]].temp = 100.0;
				//}
			}
		}
	}

	//	�f�o�b�O�E�\���p
	SetIHbandDrawVtx(xS,xE);
}

void PHFemThermo::CalcIHdqdtband(double xS,double xE,double dqdtAll,unsigned num){
	///	x���W�FxS~xE�̊Ԃ̐ߓ_�ɔM�������E������ݒ�

	PHFemMeshNew* mesh = GetPHFemMesh();

	//> ���M����l�ʑ̖ʂ̖ʐς̑��a�����߂�
	double faceS = 0.0;
	for(unsigned i=0;i < mesh->nSurfaceFace; i++){
		if(faceVars[i].mayIHheated){			// face�̐ߓ_��y���W�����̏ꍇ��IH���M�̑Ώېߓ_
			unsigned nObinnerVtx = 0;
			if(faceVars[i].area==0) faceVars[i].area = CalcTriangleArea(mesh->faces[i].vertexIDs[0],mesh->faces[i].vertexIDs[1],mesh->faces[i].vertexIDs[2]);
			for(unsigned j=0;j<3;j++){
				if( xS <= mesh->vertices[mesh->faces[i].vertexIDs[j]].pos.x && mesh->vertices[mesh->faces[i].vertexIDs[j]].pos.x <= xE){
					nObinnerVtx += 1;
				}
			}
			if( nObinnerVtx == 1)			faceVars[i].fluxarea = faceVars[i].area;//faces[i].fluxarea = 1.0/3.0 * faces[i].area;
			else if(nObinnerVtx == 2)		faceVars[i].fluxarea = faceVars[i].area;//faces[i].fluxarea = 2.0/3.0 * faces[i].area;
			else if(nObinnerVtx == 3)		faceVars[i].fluxarea = faceVars[i].area;
			else if(nObinnerVtx == 0)		faceVars[i].fluxarea = 0.0;

			if(faceVars[i].fluxarea >= 0){	
				faceS += faceVars[i].fluxarea;
			}else{		assert(0);	}		//	faces[i].fluxarea��0�����̐����������Ă���̂ɉ��Z���悤�Ƃ��Ă���
			//DSTR << "faces[" << i << "].fluxarea: " << faces[i].fluxarea << std::endl;
		}
	}

	//for(unsigned i=0;i < nSurfaceFace; i++){
	//	DSTR << "faces[" << i << "].fluxarea: " << faces[i].fluxarea << std::endl;
	//}



	if(faceS > 0){
		//> dqdt ��P�ʖʐς�����ɒ���([1/m^2])
		double dqdtds = dqdtAll / faceS;
//		DSTR << "dqdtds:  " << dqdtds << std::endl;
		//>	�ȉ��A�M������faces�Ɋi�[����
		//>	�M�����̖ʐόv�Z��fluxarea��p���čs��
		for(unsigned i=0;i < mesh->nSurfaceFace; i++){
			if(faceVars[i].mayIHheated){
				faceVars[i].heatflux[num] = dqdtds * faceVars[i].fluxarea;		//	�M�����̗ʂ�heatflux�̖ʐς���v�Z
//				DSTR << "faces[" << i <<"].heatflux: " << faces[i].heatflux <<std::endl;
			}
		}
	}
	
	//�@�ȏ�A�l�͓����Ă���悤��

	int katoon =0;

	}

void PHFemThermo::CalcIHdqdt_atleast(double r,double R,double dqdtAll,unsigned mode){
	///	�����a�ƊO���a�̊Ԃ̐ߓ_�ɔM�������E������ݒ�
	//	�k��V���{�F��F�O�R�����g
	//	�����ł��̈�ɂ������Ă���΁AIH���M�Ɋ܂߂�

	PHFemMeshNew* mesh = GetPHFemMesh();

	//> ���M����l�ʑ̖ʂ̖ʐς̑��a�����߂�
	double faceS = 0.0;
	for(unsigned i=0;i < mesh->nSurfaceFace; i++){
		if(faceVars[i].mayIHheated){			// face�̐ߓ_��y���W�����̏ꍇ��IH���M�̑Ώېߓ_
			unsigned nObinnerVtx = 0;
			if(faceVars[i].area==0) faceVars[i].area = CalcTriangleArea(mesh->faces[i].vertexIDs[0],mesh->faces[i].vertexIDs[1],mesh->faces[i].vertexIDs[2]);
			for(unsigned j=0;j<3;j++){
				if( r <= vertexVars[mesh->faces[i].vertexIDs[j]].disFromOrigin && vertexVars[mesh->faces[i].vertexIDs[j]].disFromOrigin <= R){
					//nObinnerVtx += 1;
					faceVars[i].fluxarea = faceVars[i].area;
					break;
				}
			}
			//if( nObinnerVtx == 1)			faces[i].fluxarea = faces[i].area;//faces[i].fluxarea = 1.0/3.0 * faces[i].area;
			//else if(nObinnerVtx == 2)		faces[i].fluxarea = faces[i].area;//faces[i].fluxarea = 2.0/3.0 * faces[i].area;
			//else if(nObinnerVtx == 3)		faces[i].fluxarea = faces[i].area;
			//else if(nObinnerVtx == 0)		faces[i].fluxarea = 0.0;

			//if(faces[i].fluxarea >= 0){	
			faceS += faceVars[i].fluxarea;
			//}else{		assert(0);	}		//	faces[i].fluxarea��0�����̐����������Ă���̂ɉ��Z���悤�Ƃ��Ă���
			//DSTR << "faces[" << i << "].fluxarea: " << faces[i].fluxarea << std::endl;
		}
	}

	//for(unsigned i=0;i < nSurfaceFace; i++){
	//	DSTR << "faces[" << i << "].fluxarea: " << faces[i].fluxarea << std::endl;
	//}

	if(faceS > 0){
		//> dqdt ��P�ʖʐς�����ɒ���([1/m^2])
		double dqdtds = dqdtAll / faceS;
//		DSTR << "dqdtds:  " << dqdtds << std::endl;
		//>	�ȉ��A�M������faces�Ɋi�[����
		//>	�M�����̖ʐόv�Z��fluxarea��p���čs��
		for(unsigned i=0;i < mesh->nSurfaceFace; i++){
			if(faceVars[i].mayIHheated){
				faceVars[i].heatflux[mode] = dqdtds * faceVars[i].fluxarea;		//	�M�����̗ʂ�heatflux�̖ʐς���v�Z
				//debug
				//if(faces[i].fluxarea > 0.0){
				//	int kattonnnn =0;
				//}
//				DSTR << "faces[" << i <<"].heatflux: " << faces[i].heatflux <<std::endl;
			}
		}
	}
	
	//�@�ȏ�A�l�͓����Ă���悤��

	int katoon =0;
	//���������āACreateMatk2t���R�s�[�����֐��ŁAVecf2?�������

	//>	�M�ʂ́Adqdtds��p����

	//> r <= <= R�̒��S������ˏ�ɉ��M

	//	�ߓ_��dqdt�̒l���X�V����

	//�@�ȉ��́A�x�N�g�������֐��̎d��
	//	�ߓ_�̑�����\�ʂ̖ʂŁA�v�Z����
	//  vertices[].heatFluxValue����Ɍv�Z��i�߂�
	//	�K�E�X�U�C�f���v�Z�ł���悤�ɏ����ȂǁA��������

}

void PHFemThermo::CalcIHdqdt(double r,double R,double dqdtAll,unsigned num){
	///	�����a�ƊO���a�̊Ԃ̐ߓ_�ɔM�������E������ݒ�

	PHFemMeshNew* mesh = GetPHFemMesh();

	//> ���M����l�ʑ̖ʂ̖ʐς̑��a�����߂�
	double faceS = 0.0;
	for(unsigned i=0;i < mesh->nSurfaceFace; i++){
		if(faceVars[i].mayIHheated){			// face�̐ߓ_��y���W�����̏ꍇ��IH���M�̑Ώېߓ_
			unsigned nObinnerVtx = 0;
			if(faceVars[i].area==0) faceVars[i].area = CalcTriangleArea(mesh->faces[i].vertexIDs[0],mesh->faces[i].vertexIDs[1],mesh->faces[i].vertexIDs[2]);
			for(unsigned j=0;j<3;j++){
				if( r <= vertexVars[mesh->faces[i].vertexIDs[j]].disFromOrigin && vertexVars[mesh->faces[i].vertexIDs[j]].disFromOrigin <= R){
					nObinnerVtx += 1;
				}
			}
			if( nObinnerVtx == 1)			faceVars[i].fluxarea = 1.5/3.0 * faceVars[i].area;//faces[i].fluxarea = 1.0/3.0 * faces[i].area;
			else if(nObinnerVtx == 2)		faceVars[i].fluxarea = 2.8/3.0 * faceVars[i].area;//faces[i].fluxarea = 2.0/3.0 * faces[i].area;
			else if(nObinnerVtx == 3)		faceVars[i].fluxarea = faceVars[i].area;
			else if(nObinnerVtx == 0)		faceVars[i].fluxarea = 0;

			if(faceVars[i].fluxarea >= 0){	
				faceS += faceVars[i].fluxarea;
			}else{		assert(0);	}		//	faces[i].fluxarea��0�����̐����������Ă���̂ɉ��Z���悤�Ƃ��Ă���
			//DSTR << "faces[" << i << "].fluxarea: " << faces[i].fluxarea << std::endl;
		}
	}

	//for(unsigned i=0;i < nSurfaceFace; i++){
	//	DSTR << "faces[" << i << "].fluxarea: " << faces[i].fluxarea << std::endl;
	//}

	if(faceS > 0){
		//> dqdt ��P�ʖʐς�����ɒ���([1/m^2])
		double dqdtds = dqdtAll / faceS;
//		DSTR << "dqdtds:  " << dqdtds << std::endl;
		//>	�ȉ��A�M������faces�Ɋi�[����
		//>	�M�����̖ʐόv�Z��fluxarea��p���čs��
		for(unsigned i=0;i < mesh->nSurfaceFace; i++){
			if(faceVars[i].mayIHheated){
				faceVars[i].heatflux[num] = dqdtds * faceVars[i].fluxarea;		//	�M�����̗ʂ�heatflux�̖ʐς���v�Z
//				DSTR << "faces[" << i <<"].heatflux: " << faces[i].heatflux <<std::endl;
			}
		}
	}
	
	//�@�ȏ�A�l�͓����Ă���悤��

	int katoon =0;
	//���������āACreateMatk2t���R�s�[�����֐��ŁAVecf2?�������

	//>	�M�ʂ́Adqdtds��p����

	//> r <= <= R�̒��S������ˏ�ɉ��M

	//	�ߓ_��dqdt�̒l���X�V����

	//�@�ȉ��́A�x�N�g�������֐��̎d��
	//	�ߓ_�̑�����\�ʂ̖ʂŁA�v�Z����
	//  vertices[].heatFluxValue����Ɍv�Z��i�߂�
	//	�K�E�X�U�C�f���v�Z�ł���悤�ɏ����ȂǁA��������

}
/// face���ɍ���Ă��܂��̂��ǂ��̂��Avertices���Ƃɂ��̂������̂��B�ǂ�����������������Ȃ��̂ŁA�ЂƂ܂��Avertices���ɍ���Ă��܂���

void PHFemThermo::SetVertexHeatFlux(int id,double heatFlux){
	vertexVars[id].heatFluxValue = heatFlux;
}

void PHFemThermo::SetVtxHeatFluxAll(double heatFlux){
	for(unsigned i=0; i < GetPHFemMesh()->vertices.size() ;i++){
		SetVertexHeatFlux(i,heatFlux);
	}
}

void PHFemThermo::CalcHeatTransUsingGaussSeidel(unsigned NofCyc,double dt){
	//dt = 0.0000000000001 * dt;		//�f�o�b�O�p�ɁAdt�����̂��������������Ă��A�ߓ_0���}�C�i�X�ɂȂ�̂��A���ׂ�
	
	PHFemMeshNew* mesh = GetPHFemMesh();

	//dt��PHFemEngine.cpp�Ŏ擾���铮�͊w�V�~�����[�V�����̃X�e�b�v����
	bool DoCalc =true;											//���񂾂��萔�x�N�g��b�̌v�Z���s��bool		//NofCyc��0�̎��ɂ���΂����̂���
	//std::ofstream ofs("log.txt");
	for(unsigned i=0; i < NofCyc; i++){							//�K�E�X�U�C�f���̌v�Z���[�v
		if(DoCalc){		
			if(deformed || alphaUpdated){												//D_ii�̍쐬�@�`���M�`�B�����X�V���ꂽ�ۂ�1�x�����s���΂悢
				for(unsigned j =0; j < mesh->vertices.size() ; j++){
					//for(unsigned k =0;k < vertices.size(); k++){
					//	DSTR << "dMatCAll "<< k << " : " << dMatCAll[0][k] << std::endl;
					//}
					_dMatAll.resize(1,mesh->vertices.size());
					_dMatAll[0][j] = 1.0/ ( 1.0/2.0 * dMatKAll[0][j] + 1.0/dt * dMatCAll[0][j] );		//1 / D__ii	�����߂�
					//1.0/dt = 500 d
					//DSTR << "dMatKAll : "  << dMatKAll << std::endl;
					//DSTR << "dMatCAll : "  << dMatCAll << std::endl;
					//DSTR << "1.0/dt : " << 1.0/dt <<std::endl;
					//DSTR <<  1.0/dt *dMatCAll[0][j] << std::endl;		//0.001�̃I�[�_�[
					//DSTR << 1.0/2.0 * dMatKAll[0][j] << std::endl;		//0.0003�O��̃I�[�_�[
					//�l�������Ă��邩���`�F�b�N
					//DSTR << "_dMatAll[0][" << j << "] : " << _dMatAll[0][j]  << std::endl;
					int debughogeshi =0;
				}
				deformed = false;
				alphaUpdated =false;
				//	for DEBUG
				//DSTR << "_dMatAll : " << std::endl;
				//for(unsigned j =0; j < vertices.size() ;j++){
				//	DSTR << j << " : " << _dMatAll[0][j] << std::endl;
				//}
				//int hogeshi=0;
			}
			//	 1      1        1  
			//	--- ( - - [K] + ---[C] ){T(t)} + {F} 
			//	D_jj    2       ��t
			//

			for(unsigned j =0; j < mesh->vertices.size() ; j++){		//���񃋁[�v����	�W���x�N�g��bVecAll�̐������v�Z
				bVecAll[j][0] = 0.0;							//bVecAll[j][0]�̏�����
				bVecAll_IH[j][0] = 0.0;
				//�ߓ_������edges���Ɂ@�Ίp����(j,j)�Ɣ�Ίp����(j,?)���Ɍv�Z
				//�Ίp�����́Avertices[j].k or .c �ɓ����Ă���l���A��Ίp������edges[hoge].vertices[0] or vertices[1] .k or .c�ɓ����Ă���l��p����
				//�@)��Ίp�����ɂ���
				for(unsigned k =0;k < mesh->vertices[j].edgeIDs.size() ; k++){
					unsigned edgeId = mesh->vertices[j].edgeIDs[k];
					//���t�@�N�^�����O	�ȉ��̏�������ɂ���j>edges[edgeId].vertices[0] �Ƃ����łȂ����Ƃŕ������ق����R�ꂪ�o��S�z�͂Ȃ��H
					if( j != mesh->edges[edgeId].vertexIDs[0]){					//�ߓ_�ԍ�j��edges.vertices[0]���قȂ�ߓ_�ԍ��̎�:��Ίp����
						unsigned vtxid0 = mesh->edges[edgeId].vertexIDs[0];
						bVecAll[j][0] += (-1.0/2.0 * edgeCoeffs[edgeId].k + 1.0/dt * edgeCoeffs[edgeId].c ) * TVecAll[vtxid0];
					}
					else if( j != mesh->edges[edgeId].vertexIDs[1] ){			//�ߓ_�ԍ�j��edges.vertices[1]���قȂ�ߓ_�ԍ��̎�:��Ίp����
						unsigned vtxid1 = mesh->edges[edgeId].vertexIDs[1];
						bVecAll[j][0] += (-1.0/2.0 * edgeCoeffs[edgeId].k + 1.0/dt * edgeCoeffs[edgeId].c ) * TVecAll[vtxid1];

					}
					else{
						//��L�̂ǂ���ł��Ȃ��ꍇ�A�G���[
						DSTR << "edges.vertex has 3 vertexies or any other problem" <<std::endl;
					}
					//	for Debug
					//DSTR << "edges[" << edgeId << "].vertices[0] : " << edges[edgeId].vertices[0] << std::endl;
					//DSTR << "edges[" << edgeId << "].vertices[1] : " << edges[edgeId].vertices[1] << std::endl;
					//int hogeshi =0;
				}
				//�A)�Ίp�����ɂ���
				bVecAll[j][0] += (-1.0/2.0 * dMatKAll[0][j] + 1.0/dt * dMatCAll[0][j] ) * TVecAll[j];
				//ofs << "bVecAll[" << j <<"][0] : " << bVecAll[j][0] << std::endl;			// DSTR
				//{F}�����Z
				bVecAll[j][0] += vecFAllSum[j];		//F�����Z
				//DSTR << " vecFAllSum[" << j << "] : "  << vecFAllSum[j] << std::endl;
				//DSTR << std::endl;
				//D_ii�Ŋ��� �˂��̏ꏊ�́A�����ŗǂ��́H�ǂ��܂Ŋ|���Z����́H
				bVecAll[j][0] = bVecAll[j][0] * _dMatAll[0][j];
				//ofs << "bVecAll[" << j <<"][0] * _dMatAll : " << bVecAll[j][0] << std::endl;
				//	DSTR <<  "bVecAll[" << j <<"][0] * _dMatAll : " << bVecAll[j][0] << std::endl;
				//ofs << "TVecAll[" << j <<"] : " << TVecAll[j] << std::endl;
				//	DSTR << "TVecAll[" << j <<"] : " << TVecAll[j] << std::endl;
			}
			DoCalc = false;			//����̃��[�v�����ŗ��p
			//�l�������Ă��邩�A���킻�������`�F�b�N
			//DSTR << "bVecAll[j][0] : " << std::endl;
			//for(unsigned j =0;j <vertices.size() ; j++){
			//	DSTR << j << " : "<< bVecAll[j][0] << std::endl;
			//}
			int debughogeshi =0;
		}		//if(DoCalc){...}

#ifdef DEBUG
		//	�O�̂��߁A�v�Z�O�̏������x��0�ɂ��Ă���B
		if(i == 0){
				for(unsigned j=0;j <mesh->vertices.size() ;j++){
					TVecAll[j] = 0.0;
				}
		}
#endif
		//	 1      
		//	--- [F]{T(t+dt)}
		//	D_jj 		
		//[F] = 1/2 [K] +1/dt [C] ����Ίp����������(-1)������������
		//�G�b�W�ɓ����Ă��鐬����-1��������̂ł͂Ȃ��A�Ō��-1��������B
		//
		for(unsigned j =0; j < mesh->vertices.size() ; j++){
			//T(t+dt) = �̎�
			//	�܂�tempkj�����
			double tempkj = 0.0;			//�K�E�X�U�C�f���̓r���v�Z�ŏo�Ă���F�̐����v�Z�Ɏg�p����ꎞ�ϐ�
			for(unsigned k =0;k < mesh->vertices[j].edgeIDs.size() ; k++){
				unsigned edgeId = mesh->vertices[j].edgeIDs[k]; 
				if( j != mesh->edges[edgeId].vertexIDs[0]){					//�ߓ_�ԍ�j��edges.vertices[0]���قȂ�ߓ_�ԍ��̎�:��Ίp����		//OK
					unsigned vtxid0 = mesh->edges[edgeId].vertexIDs[0];
					//DSTR << "TVecAll["<< vtxid0<<"] : " << TVecAll[vtxid0] <<std::endl;
					//TVecAll[j] +=_dMatAll[j][0] * -(1.0/2.0 * edges[edgeId].k + 1.0/dt * edges[edgeId].c ) * TVecAll[vtxid0] + bVecAll[j][0]; 
					//DSTR << "j : " << j << ", vtxid0 : " << vtxid0 <<", edges[edgeId].vertices[0] : " << edges[edgeId].vertices[0] <<  std::endl;
					tempkj += (1.0/2.0 * edgeCoeffs[edgeId].k + 1.0/dt * edgeCoeffs[edgeId].c ) * TVecAll[vtxid0];
				}
				else if( j != mesh->edges[edgeId].vertexIDs[1] ){			//�ߓ_�ԍ�j��edges.vertices[1]���قȂ�ߓ_�ԍ��̎�:��Ίp����
					unsigned vtxid1 = mesh->edges[edgeId].vertexIDs[1];
					//DSTR << "TVecAll["<< vtxid1<<"] : " << TVecAll[vtxid1] <<std::endl;
					tempkj += (1.0/2.0 * edgeCoeffs[edgeId].k + 1.0/dt * edgeCoeffs[edgeId].c ) * TVecAll[vtxid1];
				}
				else{
					//��L�̂ǂ���ł��Ȃ��ꍇ�A�G���[
					DSTR << "edges.vertex has 3 vertexies or any other problem" <<std::endl;
				}
				//	for Debug
				//DSTR << "TVecAll:"
				//DSTR << "edges[" << edgeId << "].vertices[0] : " << edges[edgeId].vertices[0] << std::endl;
				//DSTR << "edges[" << edgeId << "].vertices[1] : " << edges[edgeId].vertices[1] << std::endl;
				//int hogeshi =0;
			}
			//	TVecAll�̌v�Z
			TVecAll[j] =	_dMatAll[0][j] * ( -1.0 * tempkj) + bVecAll[j][0];			//	-b = D^(-1) [ (-1/2 * K + 1/dt * C ){T(t+dt)} + {F} ]�Ȃ̂ŁAbVecAll�͂����̉��Z�ł悢
			//TVecAll[j] =	_dMatAll[0][j] * ( -1.0 * tempkj) + bVecAll[j][0];   // -b = D^(-1) [ (-1/2 * K + 1/dt * C ){T(t+dt)} + {F} ]�Ȃ̂ŁAbVecAll�͂����̉��Z�ł悢
//			TVecAll[j] =	_dMatAll[0][j] * ( -1.0 * tempkj) + bVecAll[j][0];			//���̌v�Z�����ƁA�܂Ƃ������Ȓl���o�邪�E�E�E���_�I�ɂ͂ǂ��Ȃ̂��A������Ȃ��B�B�B
			////	for DEBUG
			//int hofgeshi =0;
			//if(TVecAll[j] != 0.0){
			//	DSTR << "!=0 TVecAll["<< j<<"] : " << TVecAll[j] <<std::endl;
			//}
			//DSTR << i << "��ڂ̌v�Z�A" << j <<"�s�ڂ�tempkj: " << tempkj << std::endl;
			//tempkj =0.0;

			//ofs << j << std::endl;
			//ofs << "tempkj: "<< tempkj << std::endl;
			//ofs << "DMatAll[0][j] * ( -1.0 * tempkj) :" <<_dMatAll[0][j] * ( -1.0 * tempkj) << std::endl;
			//ofs << "bVecAll[j][0] :  " <<  bVecAll[j][0] << std::endl;
			//ofs << "  TVecAll[j] : " << TVecAll[j] << std::endl;
			//ofs << std::endl;
		}
		////	for Debug
		//for(unsigned j=0;j < vertices.size();j++){
		//	//DSTR << "tempk" << j << " : " << tempkj << std::endl;
		//	int hogeshi__ =0;
		//	//TVecAll[j]�̌v�Z���ʂ�������
		//	//�萔�x�N�g��b����Ōv�Z�A���s��bVecAll�����Z����΂悢�B
		//	DSTR << i << "��ڂ̌v�Z�� " << "TVecAll[" << j << "] : " << TVecAll[j] << std::endl;
		//}
		//for(unsigned j=0;j < vertices.size();j++){
		//	//DSTR << "tempk" << j << " : " << tempkj << std::endl;
		//	int hogeshi__ =0;
		//	//TVecAll[j]�̌v�Z���ʂ�������
		//	//�萔�x�N�g��b����Ōv�Z�A���s��bVecAll�����Z����΂悢�B
		//	DSTR << i << "��ڂ̌v�Z�� " << "bVecAll[" << j << "][0] : " << bVecAll[j][0] << std::endl;
		//}

		//DSTR << i <<  "th Cyc" << std::endl; 
		//DSTR << i << "��ڂ̌v�Z�ATVecAll : " <<std::endl;
		//DSTR << TVecAll << std::endl;
		//ofs << i <<  "th Cyc" << std::endl;
		//ofs << i << "��ڂ̌v�Z�ATVecAll : " <<std::endl;
		//ofs << TVecAll << std::endl;
		//ofs << "bVecAll: " <<std::endl;
		//ofs << bVecAll << std::endl;
		//ofs << "_dMatAll: " <<std::endl; 
		//ofs << _dMatAll <<std::endl;
		int piyopiyoyo =0;
		double tempTemp=0.0;
		for(unsigned j=0;j <mesh->vertices.size() ; j++){
			tempTemp += TVecAll[j];
		}
		//	DSTR
		//ofs << i <<"��ڂ̌v�Z���́@�S�ߓ_�̉��x�̘a : " << tempTemp << std::endl;
		//ofs << std::endl;
	}
//	deformed = true;
}

#define FEMLOG(x)
//#define FEMLOG(x) x
void PHFemThermo::CalcHeatTransUsingGaussSeidel(unsigned NofCyc,double dt,double eps){

	PHFemMeshNew* mesh = GetPHFemMesh();

	//dt = 0.0000000000001 * dt;		//�f�o�b�O�p�ɁAdt�����̂��������������Ă��A�ߓ_0���}�C�i�X�ɂȂ�̂��A���ׂ�
	double _eps = 1-eps;			// 1-eps�̌v�Z�ɗ��p
	//dt��PHFemEngine.cpp�Ŏ擾���铮�͊w�V�~�����[�V�����̃X�e�b�v����
	bool DoCalc =true;											//���񂾂��萔�x�N�g��b�̌v�Z���s��bool		//NofCyc��0�̎��ɂ���΂����̂���
	//FEMLOG( std::ofstream ofs("log.txt") ) ;
	for(unsigned i=0; i < NofCyc; i++){							//�K�E�X�U�C�f���̌v�Z���[�v
		if(DoCalc){												
			if(deformed || alphaUpdated ){												//D_ii�̍쐬�@�`�󂪍X�V���ꂽ�ۂ�1�x�����s���΂悢
				for(unsigned j =0; j < mesh->vertices.size() ; j++){
					//for(unsigned k =0;k < vertices.size(); k++){
					//	DSTR << "dMatCAll "<< k << " : " << dMatCAll[0][k] << std::endl;
					//}
					_dMatAll.resize(1,mesh->vertices.size());
					_dMatAll[0][j] = 1.0/ ( eps * dMatKAll[0][j] + 1.0/dt * dMatCAll[0][j] );		//1 / D__ii	�����߂�
					//1.0/dt = 500 d
					//DSTR << "dMatKAll : "  << dMatKAll << std::endl;
					//DSTR << "dMatCAll : "  << dMatCAll << std::endl;
					//DSTR << "1.0/dt : " << 1.0/dt <<std::endl;
					//DSTR <<  1.0/dt *dMatCAll[0][j] << std::endl;		//0.001�̃I�[�_�[
					//DSTR << 1.0/2.0 * dMatKAll[0][j] << std::endl;		//0.0003�O��̃I�[�_�[
					//�l�������Ă��邩���`�F�b�N
					//DSTR << "_dMatAll[0][" << j << "] : " << _dMatAll[0][j]  << std::endl;
					int debughogeshi =0;
				}
				deformed = false;
				//	for DEBUG
				//DSTR << "_dMatAll : " << std::endl;
				//for(unsigned j =0; j < vertices.size() ;j++){
				//	DSTR << j << " : " << _dMatAll[0][j] << std::endl;
				//}
				//int hogeshi=0;
			}
			//	 1      1        1  
			//	--- ( - - [K] + ---[C] ){T(t)} + {F} 
			//	D_jj    2       ��t
			//

			for(unsigned j =0; j < mesh->vertices.size() ; j++){		//���񃋁[�v����	�W���x�N�g��bVecAll�̐������v�Z
				bVecAll[j][0] = 0.0;							//bVecAll[j][0]�̏�����
				//�ߓ_������edges���Ɂ@�Ίp����(j,j)�Ɣ�Ίp����(j,?)���Ɍv�Z
				//�Ίp�����́Avertices[j].k or .c �ɓ����Ă���l���A��Ίp������edges[hoge].vertices[0] or vertices[1] .k or .c�ɓ����Ă���l��p����
				//�@)��Ίp�����ɂ���
				for(unsigned k =0;k < mesh->vertices[j].edgeIDs.size() ; k++){
					unsigned edgeId = mesh->vertices[j].edgeIDs[k];
					//���t�@�N�^�����O	�ȉ��̏�������ɂ���j>edges[edgeId].vertices[0] �Ƃ����łȂ����Ƃŕ������ق����R�ꂪ�o��S�z�͂Ȃ��H
					if( j != mesh->edges[edgeId].vertexIDs[0]){					//�ߓ_�ԍ�j��edges.vertices[0]���قȂ�ߓ_�ԍ��̎�:��Ίp����
						unsigned vtxid0 = mesh->edges[edgeId].vertexIDs[0];
						bVecAll[j][0] += (-_eps * edgeCoeffs[edgeId].k + 1.0/dt * edgeCoeffs[edgeId].c ) * TVecAll[vtxid0];
					}
					else if( j != mesh->edges[edgeId].vertexIDs[1] ){			//�ߓ_�ԍ�j��edges.vertices[1]���قȂ�ߓ_�ԍ��̎�:��Ίp����
						unsigned vtxid1 = mesh->edges[edgeId].vertexIDs[1];
						bVecAll[j][0] += (-_eps * edgeCoeffs[edgeId].k + 1.0/dt * edgeCoeffs[edgeId].c ) * TVecAll[vtxid1];

					}
					else{
						//��L�̂ǂ���ł��Ȃ��ꍇ�A�G���[
						DSTR << "edges.vertex has 3 vertexies or any other problem" <<std::endl;
					}
					//	for Debug
					//DSTR << "edges[" << edgeId << "].vertices[0] : " << edges[edgeId].vertices[0] << std::endl;
					//DSTR << "edges[" << edgeId << "].vertices[1] : " << edges[edgeId].vertices[1] << std::endl;
					//int hogeshi =0;
				}
				//�A)�Ίp�����ɂ���
				bVecAll[j][0] += (-_eps * dMatKAll[0][j] + 1.0/dt * dMatCAll[0][j] ) * TVecAll[j];
				//FEMLOG(ofs << "bVecAll[" << j <<"][0] : " << bVecAll[j][0] << std::endl;)			// DSTR
				//FEMLOG(ofs << "dMatKAll[0][" << j <<"] : " << dMatKAll[0][j] << std::endl;)			// DSTR
				//FEMLOG(ofs << "dMatCAll[0][" << j <<"] : " << dMatCAll[0][j] << std::endl;)			// DSTR
				//  {F}�����Z
				bVecAll[j][0] += vecFAllSum[j];		//F�����Z
				//DSTR << " vecFAllSum[" << j << "] : "  << vecFAllSum[j] << std::endl;
				//DSTR << std::endl;
				//D_ii�Ŋ��� �˂��̏ꏊ�́A�����ŗǂ��́H�ǂ��܂Ŋ|���Z����́H
				bVecAll[j][0] = bVecAll[j][0] * _dMatAll[0][j];
				//FEMLOG(ofs << "bVecAll[" << j <<"][0] * _dMatAll : " << bVecAll[j][0] << std::endl);
				//FEMLOG(ofs << "TVecAll[" << j <<"] : " << TVecAll[j] << std::endl);
			}
			DoCalc = false;			//����̃��[�v�����ŗ��p
			//�l�������Ă��邩�A���킻�������`�F�b�N
			//DSTR << "bVecAll[j][0] : " << std::endl;
			//for(unsigned j =0;j <vertices.size() ; j++){
			//	DSTR << j << " : "<< bVecAll[j][0] << std::endl;
			//}
			int debughogeshi =0;
		}		//if(DoCalc){...}

#ifdef DEBUG
		//	�O�̂��߁A�v�Z�O�̏������x��0�ɂ��Ă���B
		if(i == 0){
				for(unsigned j=0;j <mesh->vertices.size() ;j++){
					TVecAll[j] = 0.0;
				}
		}
#endif
		//	 1      
		//	--- [F]{T(t+dt)}
		//	D_jj 		
		//[F] = eps(ilon) [K] +1/dt [C] ����Ίp����������(-1)������������
		//�G�b�W�ɓ����Ă��鐬����-1��������̂ł͂Ȃ��A�Ō��-1��������B
		//
		for(unsigned j =0; j < mesh->vertices.size() ; j++){
			//T(t+dt) = �̎�
			//	�܂�tempkj�����
			double tempkj = 0.0;			//�K�E�X�U�C�f���̓r���v�Z�ŏo�Ă���F�̐����v�Z�Ɏg�p����ꎞ�ϐ�
			for(unsigned k =0;k < mesh->vertices[j].edgeIDs.size() ; k++){
				unsigned edgeId = mesh->vertices[j].edgeIDs[k]; 
				if( j != mesh->edges[edgeId].vertexIDs[0]){					//�ߓ_�ԍ�j��edges.vertices[0]���قȂ�ߓ_�ԍ��̎�:��Ίp����		//OK
					unsigned vtxid0 = mesh->edges[edgeId].vertexIDs[0];
					//DSTR << "TVecAll["<< vtxid0<<"] : " << TVecAll[vtxid0] <<std::endl;
					//TVecAll[j] +=_dMatAll[j][0] * -(1.0/2.0 * edges[edgeId].k + 1.0/dt * edges[edgeId].c ) * TVecAll[vtxid0] + bVecAll[j][0]; 
					//DSTR << "j : " << j << ", vtxid0 : " << vtxid0 <<", edges[edgeId].vertices[0] : " << edges[edgeId].vertices[0] <<  std::endl;
					tempkj += (eps * edgeCoeffs[edgeId].k + 1.0/dt * edgeCoeffs[edgeId].c ) * TVecAll[vtxid0];
				}
				else if( j != mesh->edges[edgeId].vertexIDs[1] ){			//�ߓ_�ԍ�j��edges.vertices[1]���قȂ�ߓ_�ԍ��̎�:��Ίp����
					unsigned vtxid1 = mesh->edges[edgeId].vertexIDs[1];
					//DSTR << "TVecAll["<< vtxid1<<"] : " << TVecAll[vtxid1] <<std::endl;
					tempkj += (eps * edgeCoeffs[edgeId].k + 1.0/dt * edgeCoeffs[edgeId].c ) * TVecAll[vtxid1];
				}
				else{
					//��L�̂ǂ���ł��Ȃ��ꍇ�A�G���[
					DSTR << "edges.vertex has 3 vertexies or any other problem" <<std::endl;
				}
				//	for Debug
				//DSTR << "TVecAll:"
				//DSTR << "edges[" << edgeId << "].vertices[0] : " << edges[edgeId].vertices[0] << std::endl;
				//DSTR << "edges[" << edgeId << "].vertices[1] : " << edges[edgeId].vertices[1] << std::endl;
				//int hogeshi =0;
			}
			//	TVecAll�̌v�Z
			TVecAll[j] =	_dMatAll[0][j] * ( -1.0 * tempkj) + bVecAll[j][0];			//	-b = D^(-1) [ (-1/2 * K + 1/dt * C ){T(t+dt)} + {F} ]�Ȃ̂ŁAbVecAll�͂����̉��Z�ł悢
			//TVecAll[j] =	_dMatAll[0][j] * ( -1.0 * tempkj) + bVecAll[j][0];   // -b = D^(-1) [ (-1/2 * K + 1/dt * C ){T(t+dt)} + {F} ]�Ȃ̂ŁAbVecAll�͂����̉��Z�ł悢
//			TVecAll[j] =	_dMatAll[0][j] * ( -1.0 * tempkj) + bVecAll[j][0];			//���̌v�Z�����ƁA�܂Ƃ������Ȓl���o�邪�E�E�E���_�I�ɂ͂ǂ��Ȃ̂��A������Ȃ��B�B�B
			////	for DEBUG
			//int hofgeshi =0;
			//if(TVecAll[j] != 0.0){
			//	DSTR << "!=0 TVecAll["<< j<<"] : " << TVecAll[j] <<std::endl;
			//}
			//DSTR << i << "��ڂ̌v�Z�A" << j <<"�s�ڂ�tempkj: " << tempkj << std::endl;
			//tempkj =0.0;

			//FEMLOG(ofs << j << std::endl);
			//FEMLOG(ofs << "tempkj: "<< tempkj << std::endl);
			//FEMLOG(ofs << "DMatAll[0][j] * ( -1.0 * tempkj) :" <<_dMatAll[0][j] * ( -1.0 * tempkj) << std::endl;)
			//FEMLOG(ofs << "bVecAll[j][0] :  " <<  bVecAll[j][0] << std::endl;)
			//FEMLOG(ofs << "  TVecAll[j] : " << TVecAll[j] << std::endl;)
			//FEMLOG(ofs << std::endl;)
		}
		////	for Debug
		//for(unsigned j=0;j < vertices.size();j++){
		//	//DSTR << "tempk" << j << " : " << tempkj << std::endl;
		//	int hogeshi__ =0;
		//	//TVecAll[j]�̌v�Z���ʂ�������
		//	//�萔�x�N�g��b����Ōv�Z�A���s��bVecAll�����Z����΂悢�B
		//	DSTR << i << "��ڂ̌v�Z�� " << "TVecAll[" << j << "] : " << TVecAll[j] << std::endl;
		//}
		//for(unsigned j=0;j < vertices.size();j++){
		//	//DSTR << "tempk" << j << " : " << tempkj << std::endl;
		//	int hogeshi__ =0;
		//	//TVecAll[j]�̌v�Z���ʂ�������
		//	//�萔�x�N�g��b����Ōv�Z�A���s��bVecAll�����Z����΂悢�B
		//	DSTR << i << "��ڂ̌v�Z�� " << "bVecAll[" << j << "][0] : " << bVecAll[j][0] << std::endl;
		//}

		//DSTR << i <<  "th Cyc" << std::endl; 
		//DSTR << i << "��ڂ̌v�Z�ATVecAll : " <<std::endl;
		//DSTR << TVecAll << std::endl;

		//FEMLOG(ofs << i <<  "th Cyc" << std::endl;)
		//FEMLOG(ofs << i << "��ڂ̌v�Z�ATVecAll : " <<std::endl;)
		//FEMLOG(ofs << TVecAll << std::endl;)
		//FEMLOG(ofs << "bVecAll: " <<std::endl;)
		//FEMLOG(ofs << bVecAll << std::endl;)
		//FEMLOG(ofs << "_dMatAll: " <<std::endl;) 
		//FEMLOG(ofs << _dMatAll <<std::endl;)
		int piyopiyoyo =0;
		double tempTemp=0.0;
		for(unsigned j=0;j <mesh->vertices.size() ; j++){
			tempTemp += TVecAll[j];
		}
		//	DSTR
		//FEMLOG(ofs << i <<"��ڂ̌v�Z���́@�S�ߓ_�̉��x�̘a : " << tempTemp << std::endl;)
		//FEMLOG(ofs << std::endl;)
	}
//	deformed = true;
}

void PHFemThermo::UpdateVertexTempAll(unsigned size){
	for(unsigned i=0;i < size;i++){
		vertexVars[i].temp = TVecAll[i];
	}
}
void PHFemThermo::UpdateVertexTemp(unsigned vtxid){
		vertexVars[vtxid].temp = TVecAll[vtxid];
}

void PHFemThermo::TexChange(unsigned id,double tz){

}

void PHFemThermo::HeatTransFromPanToFoodShell(){
	//if(pan){
	//	Affinef afPan = pan->GetGRFrame()->GetWorldTransform();
	//	Affinef afMesh = tmesh.obj->GetGRFrame()->GetWorldTransform();
	//	Affinef afMeshToPan = afPan.inv() * afMesh;	
	//}

	//shape pair solid pair
	

	//	2���̂̐ڐG�ʂ���A���M����ߓ_�����߂�B
	//	Shape pair ��Solid pare�ӂ�ɋL�q������

	//	�ŊO�k�̐ߓ_�𐢊E���W�ɕϊ����A���̃t���C�p�����́A�S����ɂ������W�n�ɕϊ��������W�l���A������W�ȉ��Ȃ�
	//	�ŊO�k�̐ߓ_�ɔM�`�B����
	//PHSolidIf* phs ;
//	Affinef afPan = phs
	//Affinef afPan = pan->GetGRFrame()->GetWorldTransform();
	//Affinef afMesh = tmesh.obj->GetGRFrame()->GetWorldTransform();
	//Affinef afMeshToPan = afPan.inv() * afMesh;	
//	for(unsigned i=-0; i < surfaceVertices.size();i++){
//		if(vertices[surfaceVertices[i]].pos){};
//		//vertices[surfaceVertices[i]].Tc;
//	}

	//	�ڐG�ʂ���̋�������苗���ȓ��Ȃ�

	//	�M�`�B���E�����ŁA�M�`�B

	//	�M�`�B���E�����ŔM�`�B��A�t���C�p���̔M�͋z�M�Ƃ������A���邪�A����͗L���v�f�@���ǂ̂悤�Ɍ������Ă��΂����̂��H
	
	//	�ߓ_���͂�Tc�ƔM�`�B�����ɂ���āA�M���`���̂ŁA�����ŁA�t���C�p������M��`�������ߓ_��Tc�ƔM�`�B����ݒ肷��

	//	�M�`�B���́A����Ƃ̊֌W�ɂ���ĈقȂ�̂ŁA�ߓ_�����Ɛڂ��Ă��邩�ɂ���ĕύX����


}

void PHFemThermo::DrawEdge(unsigned id0, unsigned id1){
	//Vec3d pos0 = vertices[id0].pos;
	//Vec3d pos1 = vertices[id1].pos;
	//
	//Vec3d wpos0 = 
	//	GetWorldTransform() * pos0; //* ���[�J�����W�� ���E���W�ւ̕ϊ����đ��
	//Vec3d wpos1 = GetWorldTransform() * pos1; //* ���[�J�����W�� ���E���W�ւ̕ϊ����đ��
	//glColor3d(1.0,0.0,0.0);
	//glBegin(GL_LINES);
	//	glVertex3f(wpos0[0],wpos0[1],wpos0[2]);
	//	glVertex3f(wpos0[0] + wpos1[0], wpos0[1] + wpos1[1], wpos0[2] + wpos1[2]);
	//glEnd();
	//glFlush();
}




void PHFemThermo::Step(double dt){

	PHFemMeshNew* mesh = GetPHFemMesh();

	//// cps�\���p
	//static bool bOneSecond = false;
	//{
	//	static DWORD lastTick = GetTickCount();
	//	static int cpsCount = 0;
	//	int ellapsed = GetTickCount() - lastTick;
	//	++cpsCount;
	//	bOneSecond = false;
	//	if (ellapsed > 1000) {
	//		std::cout << "cps : " << cpsCount << std::endl;
	//		cpslog << StepCount<< "," << cpsCount << "," ;	
	//		lastTick = GetTickCount();
	//		cpsCount = 0;
	//		bOneSecond = true;
	//	}
	//	if(cpsCount){	cpstime	= 1 / cpsCount;		}
	//}
	//static DWORD stepStart = GetTickCount();
	////�r������
	//if (bOneSecond) {
	//	std::cout << "1: " << GetTickCount() - stepStart << std::endl;
	//	cpslog << GetTickCount() - stepStart << ",";
	//} 
	//stepStart = GetTickCount();
	//�r������
	//if (bOneSecond) { std::cout << "1: " << GetTickCount() - stepStart << std::endl; }
	// %%% CPS�\��

	//std::ofstream templog("templog.txt");

	//std::ofstream ofs_("log_.txt");
//	ScilabTest();									//	Scilab���g���e�X�g
	//���E������ݒ�:���x�̐ݒ�
//	UsingFixedTempBoundaryCondition(0,200.0);

	//	�����p�̃R�[�h
	//	a) ���ԗ��U�����ɑ�`�������p�A�O�i�E��ލ����ŐU��������M����
	//if(StepCount == 0){
	//	unsigned texid_ =7;
	//	UsingFixedTempBoundaryCondition(texid_,200.0);
	//}

	//	b) �f�M�ߒ��̎���
	//	�M�`�B����0�ɂ���B���x�Œ苫�E�����ŉ��M�B

	//	UsingFixedTempBoundaryCondition(3,50.0);
	//for(unsigned i=0 ;i<1;i++){
	//	UsingFixedTempBoundaryCondition(i,200.0);
	//}

	//%%%%		�M�`�B���E����		%%%%//
	//	�H�ރ��b�V���̕\�ʂ̐ߓ_�ɁA���̗͂��̉��x��^����
	//	���̗͂��̉��x�́A�t���C�p���̕\�ʉ��x��A�H�ނ�UsingFixedTempBoundaryCondition(0,200.0);�t�̓��̉��x�̕��z����A���̏ꏊ�ł̎��͗��̉��x�𔻕ʂ���B
	//	�ʒu���W���画�ʂ���R�[�h�������ɋL�q
	//UsingHeatTransferBoundaryCondition(unsigned id,double temp);
	//�G�l���M�[�ۑ������A���͗��̉��x�̒ቺ��A���̂ւ̋����M�ʂ͐��������ׂ�

	//for(unsigned i =0; i < 2; i++){
	//	UsingHeatTransferBoundaryCondition(surfaceVertices[i],200.0);
	//}
//	UsingHeatTransferBoundaryCondition(7,200.0);
//	UsingHeatTransferBoundaryCondition(0,200.0);
	//for(unsigned i =0; i < surfaceVertices.size(); i++){
	//	UsingHeatTransferBoundaryCondition(surfaceVertices[i],150.0);
	//}
	//DSTR << "vecFAllSum : " <<std::endl;
	//DSTR << vecFAllSum << std::endl;

	//	test�@shapepair������Ă���
	//GetScene()->
	
	//dt = dt *0.01;		�덷1�x���x�ɂȂ�
	//dt = dt;				�����������́A�v�Z�덷�H�i�}�C�i�X�ɂȂ��Ă���ߓ_���x������Ȃ�ɑ傫���Ȃ�B�j
	
	///>	�K�E�X�U�C�f���@�̐ݒ�
	//	CalcHeatTransUsingGaussSeidel(20,dt);			//�K�E�X�U�C�f���@�ŔM�`���v�Z�������@�N�����N�j�R���\���@�݂̂��g�������ꍇ

//	dNdt = 10.0 * dt;

#if 0
	// �����O�ɂ��Ȃ炸�s��
	UpdateVecFAll_frypan(WEEK);				// �����ɉ��M������^����B(OFF/WEEK/MIDDLE/HIGH)
#endif
	//�K�E�X�U�C�f���@�ŉ���
	CalcHeatTransUsingGaussSeidel(1,dt,1.0);			//�K�E�X�U�C�f���@�ŔM�`���v�Z������ ��O�����́A�O�i�E�N�����N�j�R���\���E��ސϕ��̂����ꂩ�𐔒l�őI��

	//���x��\�����Ă݂�
	//DSTR << "vertices[3].temp : " << vertices[3].temp << std::endl;

	//���x�̃x�N�g������ߓ_�։��x�̔��f
	UpdateVertexTempAll((unsigned)mesh->vertices.size());
	//for(unsigned i =0;i<vertices.size();i++){
	//	DSTR << "vertices[" << i << "].temp : " << vertices[i].temp << std::endl;
	//}
	int templogkatoon =0;

	//for(unsigned i =0;i<vertices.size();i++){
	//	if(vertices[i].temp !=0){
	//		ofs_ << "vertices[" << i << "].temp : " << vertices[i].temp << std::endl;
	//	}
	//}
	int hogehoge=0;

	//	�ߓ_�̉��x�̐��ڗ����̕ۑ�
	//if(StepCount ==0){
	//	templog <<"�X�e�b�v��"<<",";
	//	templog <<"�M�V�~�����[�V��������"<<",";
	//	for(unsigned i=0; i<vertices.size();i++){
	//		templog << "�ߓ_" << i << "�̉��x" << ",";
	//	}
	//	templog <<"," << std::endl;
	//	}
	//templog << StepCount << ", ";
	//templog << StepCount * dt << ", ";
	//for(unsigned i=0; i<vertices.size();i++){
	//	templog << vertices[i].temp << ",";
	//}
	//templog <<"," << std::endl;

	//templog;
	//�ߓ_���x����ʂɕ\�������3�����e�N�X�`�����g�����e�N�X�`���؂�ւ��ɒl��n���ˊ֐���

	//DSTR << "TVecAll : " <<std::endl;
	//DSTR << TVecAll << std::endl;
	//DSTR << "bVecAll : " <<std::endl;
	//DSTR << bVecAll << std::endl;


	//	for	DEBUG	�ߓ_3�ƃG�b�W�΂����ߓ_��\��
	//for(unsigned i =0; i < vertices[3].edges.size();i++){
	//	DSTR << "edges[vertices[3].edges[" << i << "]].vertices[0] : " << edges[vertices[3].edges[i]].vertices[0] << std::endl;
	//	DSTR << "edges[vertices[3].edges[" << i << "]].vertices[1] : " << edges[vertices[3].edges[i]].vertices[1] << std::endl;
	//}
	//for(unsigned i =0; i < vertices[3].edges.size();i++){
	//	DSTR << "vertices[3].edges[" << i << "] : " << vertices[3].edges[i] << std::endl;
	//}
	int hogeshi = 0;
	//	for DEBUG
	//�i�`�󂪕ς������A�}�g���N�X��x�N�g������蒼���j
	//���x�ω��E�ŐV�̎��Ԃ�{T}�c�x�N�g���ɋL�ڂ���Ă���ߓ_���x����ɉ��w�ω��V�~�����[�V�������s��
		//SetChemicalSimulation();
		//���w�ω��V�~�����[�V�����ɕK�v�ȉ��x�Ȃǂ̃p�����[�^��n��
	//���x�ω��≻�w�V�~�����[�V�����̌��ʂ̓O���t�B�N�X�\�����s��
	StepCount += 1;
	if(StepCount >= 1000*1000*1000){
		StepCount = 0;
		StepCount_ += 1;
	}
	int temphogeshi =0;

}

void PHFemThermo::CreateMatrix(){
}

void PHFemThermo::InitTcAll(double temp){
	for(unsigned i =0; i <GetPHFemMesh()->vertices.size();i++){
		vertexVars[i].Tc = temp;
	}
}

void PHFemThermo::InitCreateMatC(){
	/// MatC�ɂ���	//	�g�p����s��̏�����
	//dMatCAll�F�Ίp�s��̐����̓������s��̃T�C�Y���`:�z��Ƃ��ė��p	��:vertices.size(),����:1
	dMatCAll.resize(1,GetPHFemMesh()->vertices.size()); //(h,w)
	dMatCAll.clear();								///	�l�̏�����
	//matc�̏������́Amatc�����֐��ł���Ă���̂ŁA�ȗ�
}

void PHFemThermo::InitCreateMatk_(){
	///	MatK�ɂ���
	//matk�̏�����
	for(unsigned i =0; i < 4 ; i++){
		for(unsigned j =0; j < 4 ; j++){
			matk[i][j] = 0.0;
		}
	}
	dMatKAll.clear();		///	������
#ifdef UseMatAll
	//matKAll.resize(vertices.size(),vertices.size());	///	matKAll�̃T�C�Y�̑��
	matKAll.clear();									///	matKAll�̏�����
	//DSTR << "matKAll: " << matKAll <<std::endl;
#endif UseMatAll

#ifdef DumK
	//matKAll�̏�����
	matKAll.resize(GetPHFemMesh()->vertices.size(),GetPHFemMesh()->vertices.size());
	matkAll.clear();					///	�������A���̏������R�[�h�͕s�v
#endif


}
void PHFemThermo::InitCreateMatk(){

	PHFemMeshNew* mesh = GetPHFemMesh();

	///	MatK�ɂ���
	//matk�̏�����
	for(unsigned i =0; i < 4 ; i++){
		for(unsigned j =0; j < 4 ; j++){
			matk[i][j] = 0.0;
		}
	}
	//for(unsigned i=0;i<3;i++){
	//tets.matk[i]
	//}
	//DMatAll�F�Ίp�s��̐����̓������s��̃T�C�Y���`:�z��Ƃ��ė��p	��:vertices.size(),����:1
	dMatKAll.resize(1,mesh->vertices.size()); //(h,w)
	dMatKAll.clear();		///	������
	////�l�̏�����
	//for(unsigned i=0;i<vertices.size();i++){
	//	dMatKAll[0][i] = 0.0;
	//}
#ifdef UseMatAll
	
	matKAll.resize(mesh->vertices.size(),mesh->vertices.size());	///	matKAll�̃T�C�Y�̑��
	matKAll.clear();									///	matKAll�̏�����
	//for(unsigned i=0;i<vertices.size();i++){
	//	for(unsigned j=0;j<vertices.size();j++){
	//		matKAll[i][j] = 0.0;
	//	}
	//}
#endif UseMatAll

#ifdef DumK
	//matKAll�̏�����
	matKAll.resize(mesh->vertices.size(),mesh->vertices.size());
	matkAll.clear();					///	�������A���̏������R�[�h�͕s�v
	//for(unsigned i=0;i<vertices.size();i++){
	//	for(unsigned j=0;j<vertices.size();j++){
	//		matKAll[i][j] = 0.0;
	//	}
	//}
#endif
}

void PHFemThermo::InitCreateVecf_(){
	///	Vecf�ɂ���
	//Vecf�̏�����
	for(unsigned i =0; i < 4 ; i++){
			vecf[i] = 0.0;
	}
	vecFAllSum.clear();						///	������
}

void PHFemThermo::InitVecFAlls(){

	PHFemMeshNew* mesh = GetPHFemMesh();

	for(unsigned i =0; i < 4 ; i++){ vecf[i] = 0.0;}	/// Vecf�̏�����
	vecFAllSum.resize(mesh->vertices.size());					///	�S�̍����x�N�g��F�̃T�C�Y���K��
	vecFAllSum.clear();									///		�V			�̏�����

	for(unsigned i=0;i < HIGH +1 ; i++){				/// IH���M���[�h�̊e�x�N�g����������
		vecFAll_f2IH[i].resize(mesh->vertices.size(),1);
		vecFAll_f2IH[i].clear();
	}
	// tets.vecf[4]�̏�����
	for(unsigned i=0;i<mesh->tets.size();i++){
		for(unsigned j=0; j <4;j++){
			mesh->tets[i].vecf[j].clear();
		}
	}
	// vecFAll�̏�����
	//initialize
	for(unsigned i =0;i<4;i++){
		vecFAll[i].resize(mesh->vertices.size());
		vecFAll[i].clear();
	}
}

void PHFemThermo::UpdateVecF_frypan(){

	PHFemMeshNew* mesh = GetPHFemMesh();

	// mode�͕K�v���H
	
	//	Initialize
	InitVecFAlls();		// ���̒��̏������̂��ׂĂ��K�v���H

//. 1)�ŏ���2��({F2},{F3})�́AF2,F3�̂ǂ��炩�����X�V����Ηǂ��ꍇ�ɗp����
#if 0
	// {F2}
	for(unsigned tetsid = 0; tetsid < mesh->tets.size();tetsid++){
		unsigned id = tetsid;
		CreateVecf2surface(id);			// tets[id].vecf[1];�Ɍ��ʊi�[
		for(unsigned j =0;j < 4; j++){
			int vtxid0 = mesh->tets[id].vertexIDs[j];
			//vecFAll[1][vtxid0] += vecf[j];
			vecFAll[1][vtxid0] += mesh->tets[id].vecf[1][j];
		}
	}	
#endif

#if 0
	// {F3}
	for(unsigned tetsid = 0; tetsid < mesh->tets.size();tetsid++){
		unsigned id = tetsid;
		CreateVecf3(id);		// tets[id].vecf[2];�Ɍ��ʊi�[
		//vecf = tets[id].vecf[2];
		for(unsigned j =0;j < 4; j++){
			int vtxid0 = mesh->tets[id].vertexIDs[j];
			//vecFAll[1][vtxid0] += vecf[j];
			vecFAll[2][vtxid0] += mesh->tets[id].vecf[2][j];
		}
	}	
#endif

//. 2) {F2,F3}�̗������X�V���ėǂ��ꍇ
#if 1
	for(unsigned tetsid = 0; tetsid < mesh->tets.size();tetsid++){
		unsigned id = tetsid;
		CreateVecf2surface(id);
		CreateVecf3(id);		// tets[id].vecf[2];�Ɍ��ʊi�[
		//vecf = tets[id].vecf[2];
		for(unsigned j =0;j < 4; j++){
			int vtxid0 = mesh->tets[id].vertexIDs[j];
			//vecFAll[1][vtxid0] += vecf[j];
			vecFAll[1][vtxid0] += mesh->tets[id].vecf[1][j];
			vecFAll[2][vtxid0] += mesh->tets[id].vecf[2][j];
		}
	}
#endif

int debugParam =0;

	vecFAllSum.clear();	// �OStep�ł̔M���o�͂�����
	//��{F[i]}_{i=1}^{4}
	vecFAllSum = vecFAll[1] + vecFAll[2];

//%%%% ���̊֐��͂����܂łłƂ肠�������� 2012.10.09

	//	�����\��
#if 0
//depend on mode, I don't need to use mode state.Because mode state cause different calc result of heatflus.
// I just use the result of IHdqdt Function.
	//vecFAll[1],[2]�ɑ��
	for(unsigned id = 0; id < mesh->vertices.size();id++){
		if(mode == OFF){ 
			// F2�͉��Z���Ȃ�
			vecFAllSum[id] = vecFAllSum[id]; //F3
		}
		else if(mode == WEEK){
			vecFAllSum[id] =  vecFAll_f2IH[mode][id][0] + vecFAll_f3[id][0];//F2+F3		//mode=0 -> F2��WEEK�̋���
		}
		else if(mode == MIDDLE){
			vecFAllSum[id] = vecFAll_f2IH[mode][id][0];//F2+F3		//mode=1 -> F2��middle�̋���
		}
		else if(mode == HIGH){
			vecFAllSum[id] = vecFAll_f2IH[mode][id][0];//F2+F3		//mode=2 -> F2��high�̋���
		}
	}

#endif

#if 0
	for(unsigned i =0; i< 4;i++){
		vecFAllSum += vecFAll[i];				//�S�̍����s��̘a�����
	}
	// F2,F3�����Z����
#endif
}


void PHFemThermo::UpdateVecF(){

	PHFemMeshNew* mesh = GetPHFemMesh();

	// mode�͕K�v���H
	
	//	Initialize
	//InitVecFAlls();		// ���̒��̏������̂��ׂĂ��K�v���H

//. 1)�ŏ���2��({F2},{F3})�́AF2,F3�̂ǂ��炩�����X�V����Ηǂ��ꍇ�ɗp����
#if 0
	// {F2}
	for(unsigned tetsid = 0; tetsid < mesh->tets.size();tetsid++){
		unsigned id = tetsid;
		CreateVecf2surface(id);			// tets[id].vecf[1];�Ɍ��ʊi�[
		for(unsigned j =0;j < 4; j++){
			int vtxid0 = mesh->tets[id].vertexIDs[j];
			//vecFAll[1][vtxid0] += vecf[j];
			vecFAll[1][vtxid0] += mesh->tets[id].vecf[1][j];
		}
	}	
#endif

#if 0
	// {F3}
	for(unsigned tetsid = 0; tetsid < mesh->tets.size();tetsid++){
		unsigned id = tetsid;
		CreateVecf3(id);		// tets[id].vecf[2];�Ɍ��ʊi�[
		//vecf = tets[id].vecf[2];
		for(unsigned j =0;j < 4; j++){
			int vtxid0 = mesh->tets[id].vertexIDs[j];
			//vecFAll[1][vtxid0] += vecf[j];
			vecFAll[2][vtxid0] += mesh->tets[id].vecf[2][j];
		}
	}	
#endif

//. 2) {F2,F3}�̗������X�V���ėǂ��ꍇ
#if 0
	for(unsigned tetsid = 0; tetsid < mesh->tets.size();tetsid++){
		unsigned id = tetsid;
		CreateVecf2surface(id);
		CreateVecf3(id);		// tets[id].vecf[2];�Ɍ��ʊi�[
		//vecf = tets[id].vecf[2];
		for(unsigned j =0;j < 4; j++){
			int vtxid0 = mesh->tets[id].vertexIDs[j];
			//vecFAll[1][vtxid0] += vecf[j];
			vecFAll[1][vtxid0] += mesh->tets[id].vecf[1][j];
			vecFAll[2][vtxid0] += mesh->tets[id].vecf[2][j];
		}
	}
#endif

int debugParam =0;

	vecFAllSum.clear();	// �OStep�ł̔M���o�͂�����
	//��{F[i]}_{i=1}^{4}
//	vecFAllSum = vecFAll[1] + vecFAll[2];

//%%%% ���̊֐��͂����܂łłƂ肠�������� 2012.12.03

#if 0
	for(unsigned i =0; i< 4;i++){
		vecFAllSum += vecFAll[i];				//�S�̍����s��̘a�����
	}
	// F2,F3�����Z����
#endif
}

void PHFemThermo::UpdateIHheat(unsigned heatingMODE){

	//�M�`�����A���x�A��M�A�M�`�B���@�̃p�����[�^�[��ݒ�E���
		//PHFemThermo�̃����o�ϐ��̒l���� CADThermo���A0.574;//�ʂ˂��̒l//�M�`����[W/(���EK)]�@Cp = 1.96 * (Ndt);//�ʂ˂��̔�M[kJ/(kg�EK) 1.96kJ/(kg K),�i�ʂ˂��̖��x�j�H�i���M�̉Ȋwp64���970kg/m^3
		//�M�`�B���̒P�ʌn�@W/(m^2 K)�˂����SI�P�ʌn�Ȃ̂��H�@25�͘_��(MEAT COOKING SIMULATION BY FINITE ELEMENTS)�̃I�[�u�����M���̎����l
		//SetInitThermoConductionParam(0.574,970,1.96,25);
	//SetInitThermoConductionParam(0.574,970,0.1960,25 * 0.01);		//> thConduct:�M�`���� ,roh:���x,	specificHeat:��M J/ (K�Ekg):1960 ,�@heatTrans:�M�`�B�� W/(m^2�EK)
		//�����A�ϐ��l�͌ォ��v�Z�̓r���ŕύX�ł���悤��SetParam()�֐�������Ă������ق����������ȁH

	//.		�M�����̐ݒ�
	//..	������
	//SetVtxHeatFluxAll(0.0);

	//1.�t���C�p���ʒu������Ă���
		//ih���M�~���S����̓��S�~����M�̈���v�Z���Aihdqdt�ɓ��Ă͂߂郁�b�V�����𐶐�
		//�@if(�t���C�p������������)	�����Ă��Ȃ���΁Avecf���A1step�O�̒l���g����悤�ɂ��Ă��������B

	//2...	face�ʂł̔M�����ʂ��v�Z�i�t���C�p���ʒu���̓|�C���^�������ɑ���F����t���C�p���̈ʒu���ω�����̂ŁA�t���C�p���ʒu�̕ω��̓x�ɐ�������j
	if(heatingMODE == OFF){
		CalcIHdqdt_atleast(0.0,0.0,0.0, OFF);		//	IH���M�s��̌W��0�ƂȂ邽�߁A�v�Z����Ȃ�
	}
	else if(heatingMODE == WEEK){	
		CalcIHdqdt_atleast(0.13,0.14,231.9 * 5e1, WEEK);		//
	}
	else if(heatingMODE == MIDDLE){
		CalcIHdqdt_atleast(0.11,0.14,231.9 * 0.005 * 1e4, MIDDLE);		//
	}
	else if(heatingMODE == HIGH){
		CalcIHdqdt_atleast(0.11,0.14,231.9 * 0.005 * 1e5, HIGH);		//
	}

	//3.�e�ʂł̔M�����ʂ���S�̍����x�N�g�������B{F}�ɑ��

#if 1			// switch1
	UpdateVecF_frypan();
#endif
	//%%	IH���M�̃��[�h�ؑ�
	//	���C����ɉ��M
	//	CalcIHdqdtband_(0.09,0.10,231.9 * 5e3);		//*0.5*1e4	�l��ς��Ď���	//*1e3�@//*1e4 //5e3
	//	�~��ɉ��M

	//	���̌�ŁA�M�����x�N�g�����v�Z����֐����Ăяo��
	///	�M�`�B�����e�ߓ_�Ɋi�[
	//SetHeatTransRatioToAllVertex();
#if 0			//!switch1
	InitVecFAlls();
	for(unsigned i=0; i < GetPHFemMesh()->tets.size(); i++){ //this��GetPHFemMesh() shiba
		CreateVecFAll(i);
	}
#endif

#if 0
	CreateVecF2surfaceAll();		//	CreateVecFAll(i);�̑���
	CreateVecF3surfaceAll();		//	CreateVecFAll(i);�̑���
#endif
}

void PHFemThermo::SetParamAndReCreateMatrix(double thConduct0,double roh0,double specificHeat0){

	PHFemMeshNew* mesh = GetPHFemMesh();

	for(unsigned i =0; i < mesh->edges.size();i++){
		edgeCoeffs[i].c = 0.0;
		edgeCoeffs[i].k = 0.0;
	}

	///	faces
	for(unsigned i=0;i<mesh->faces.size();i++){
		faceVars[i].alphaUpdated = true;
		faceVars[i].area = 0.0;
		faceVars[i].heatTransRatio = 0.0;
		faceVars[i].deformed = true;				//������Ԃ́A�ό`��Ƃ���
		faceVars[i].fluxarea =0.0;
		for(unsigned j =0; j < HIGH +1 ; j++){			// ���M���[�h�̐������A�x�N�g���𐶐�
			faceVars[i].heatflux[j] = 0.0;
		}
	}

	//�s��̐������Ȃǂ�������
	bVecAll.resize(mesh->vertices.size(),1);

	//�ߓ_���x�̏����ݒ�(�s������O�ɍs��)
	SetVerticesTempAll(0.0);			///	�������x�̐ݒ�

	//���͗��̉��x�̏�����(temp�x�ɂ���)
	InitTcAll(0.0);

	//dmnN �����̉��x�̏c�i��j�x�N�g��
	CreateTempMatrix();

	//�M�`�����A���x�A��M�A�M�`�B���@�̃p�����[�^�[��ݒ�E���
		//PHFemThermo�̃����o�ϐ��̒l���� CADThermo���A0.574;//�ʂ˂��̒l//�M�`����[W/(���EK)]�@Cp = 1.96 * (Ndt);//�ʂ˂��̔�M[kJ/(kg�EK) 1.96kJ/(kg K),�i�ʂ˂��̖��x�j�H�i���M�̉Ȋwp64���970kg/m^3
		//�M�`�B���̒P�ʌn�@W/(m^2 K)�˂����SI�P�ʌn�Ȃ̂��H�@25�͘_��(MEAT COOKING SIMULATION BY FINITE ELEMENTS)�̃I�[�u�����M���̎����l
	//. �M�`�B���� SetInitThermoConductionParam(0.574,970,0.1960,25 * 0.001 );		//> thConduct:�M�`���� ,roh:���x,	specificHeat:��M J/ (K�Ekg):1960 ,�@heatTrans:�M�`�B�� W/(m^2�EK)
	//. �M�`�B���Ȃ�
	SetInitThermoConductionParam(thConduct0,roh0,specificHeat0,0);		// �M�`�B��=0;�ɂ��Ă���w
	
	//> �M�����̏�����
	SetVtxHeatFluxAll(0.0);

	//>	�M���˗��̐ݒ�
	SetThermalEmissivityToVerticesAll(0.0);				///	�b��l0.0�ŏ�����	�F�M���˂͂��Ȃ���

	//> IH���M����face��������x(�\��face && �����)�i��A�֌W��������face�ߓ_�̌��_����̋������v�Z���Aface[].mayIHheated�𔻒�
	CalcVtxDisFromOrigin();
	//CalcVtxDisFromVertex(0.0,-1.2);
	//>	IH����̒P�ʎ��ԓ�����̉��M�M��	//�P�ʎ��ԓ�����̑����M�M��	231.9; //>	J/sec
	
	//	���̌�ŁA�M�����x�N�g�����v�Z����֐����Ăяo��
	InitCreateMatC();					///	CreateMatC�̏�����
	InitVecFAlls();					///	VecFAll�ނ̏�����
	InitCreateMatk();					///	CreateMatK�̏�����

	///	�M�`�B�����e�ߓ_�Ɋi�[
	SetHeatTransRatioToAllVertex();
	for(unsigned i=0; i < mesh->tets.size(); i++){ //this��mesh shiba
		//�e�s�������āA�K�E�X�U�C�f���Ōv�Z���邽�߂̌W���̊�{�����BTimestep�̓����Ă��鍀�́A���̃\�[�X(SetDesc())�ł́A�����ł��Ȃ����Ƃ���������(NULL���Ԃ��Ă���)
		CreateMatkLocal(i);				///	Matk1 Matk2(�X�V���K�v�ȏꍇ������)�����	//ifdef�X�C�b�`�őS�̍����s���(�\���p����)�����\
		CreatedMatCAll(i);
		CreateVecFAll(i);
	}
	
	// �J�E���g�̏�����
	//Ndt =0;

	//������������̏�����
	InitMoist();

	SetVerticesTempAll(0.0);

}

void PHFemThermo::AfterSetDesc() {	

	PHFemMeshNew* mesh = GetPHFemMesh();
	if(!mesh)
		return;

	//%%%	��������		%%%//

	//�e�탁���o�ϐ��̏������˃R���X�g���N�^�łł����ق���������������Ȃ��B
	///	Edges
	for(unsigned i =0; i < mesh->edges.size();i++){
		edgeCoeffs[i].c = 0.0;	
		edgeCoeffs[i].k = 0.0;
	}

	///	faces
	for(unsigned i=0;i<mesh->faces.size();i++){
		faceVars[i].alphaUpdated = true;
		faceVars[i].area = 0.0;
		faceVars[i].heatTransRatio = 0.0;
		faceVars[i].deformed = true;				//������Ԃ́A�ό`��Ƃ���
		faceVars[i].fluxarea =0.0;
//		faces[i].heatflux =0.0;
		//faces[i].heatflux.clear();				// ������
		//faces[i].heatflux[hum]�̗̈�m�ہF�z��Ƃ��āA���Avetor�Ƃ��Ă�push_back���A�ǂ��炩���s���B�z��Ȃ炱���ɋL�q�B
		for(unsigned j =0; j < HIGH +1 ; j++){			// ���M���[�h�̐������A�x�N�g���𐶐�
			faceVars[i].heatflux[j] = 0.0;
		}
	}

	///	vertex

	///	tets

	//�s��̐������Ȃǂ�������
	bVecAll.resize(mesh->vertices.size(),1);

	//�s������
		//�s�����邽�߂ɕK�v�Ȑߓ_��l�ʑ̂̏��́APHFemThermo�̍\���̂ɓ����Ă���B
			//PHFemThermo�̃I�u�W�F�N�g�����ۂɁA�f�B�X�N���v�^�ɒl��ݒ肵�č��
		
	//�ߓ_���x�̏����ݒ�(�s������O�ɍs��)
	SetVerticesTempAll(0.0);			///	�������x�̐ݒ�

	//���͗��̉��x�̏�����(temp�x�ɂ���)
	InitTcAll(0.0);

	//�ߓ_�̏������x��ݒ肷���{T}�c�x�N�g���ɑ��
		//{T}�c�x�N�g�����쐬����B�ȍ~��K,C,F�s��E�x�N�g���̐ߓ_�ԍ��́A���̏c�x�N�g���̐ߓ_�̕��я��ɍ��킹��?
		
	//dmnN �����̉��x�̏c�i��j�x�N�g��
	CreateTempMatrix();

	//�M�`�����A���x�A��M�A�M�`�B���@�̃p�����[�^�[��ݒ�E���
		//PHFemThermo�̃����o�ϐ��̒l���� CADThermo���A0.574;//�ʂ˂��̒l//�M�`����[W/(���EK)]�@Cp = 1.96 * (Ndt);//�ʂ˂��̔�M[kJ/(kg�EK) 1.96kJ/(kg K),�i�ʂ˂��̖��x�j�H�i���M�̉Ȋwp64���970kg/m^3
		//�M�`�B���̒P�ʌn�@W/(m^2 K)�˂����SI�P�ʌn�Ȃ̂��H�@25�͘_��(MEAT COOKING SIMULATION BY FINITE ELEMENTS)�̃I�[�u�����M���̎����l
		//SetInitThermoConductionParam(0.574,970,1.96,25);
	//. �M�`�B���� SetInitThermoConductionParam(0.574,970,0.1960,25 * 0.001 );		//> thConduct:�M�`���� ,roh:���x,	specificHeat:��M J/ (K�Ekg):1960 ,�@heatTrans:�M�`�B�� W/(m^2�EK)
	//. �M�`�B���Ȃ�
//	SetInitThermoConductionParam(0.574,970,0.1960,0 );		// �M�`�B��=0;�ɂ��Ă���w
	SetInitThermoConductionParam(THCOND,970,0.1960,0 );		// �M�`�B��=0;�ɂ��Ă���w
	


	//�f�M�ߒ�
	//SetInitThermoConductionParam(0.574,970,0.1960,0.0);		//> thConduct:�M�`���� ,roh:���x,	specificHeat:��M J/ (K�Ekg):1960 ,�@heatTrans:�M�`�B�� W/(m^2�EK)
	//�����A�ϐ��l�͌ォ��v�Z�̓r���ŕύX�ł���悤��SetParam()�֐�������Ă������ق����������ȁH

	//> �M�����̏�����
	SetVtxHeatFluxAll(0.0);

	//>	�M���˗��̐ݒ�
	SetThermalEmissivityToVerticesAll(0.0);				///	�b��l0.0�ŏ�����	�F�M���˂͂��Ȃ���

	//>	�s��̍쐬�@�s��̍쐬�ɕK�v�ȕϐ��͂��̍s�ȑO�ɐݒ肪�K�v
		//�v�Z�ɗp����}�g���N�X�A�x�N�g�����쐬�i���b�V�����Ƃ̗v�f�����s��/�x�N�g���ˑS�̍����s��/�x�N�g���j
		//{T}�c�x�N�g���̐ߓ_�̕��я��ɕ��Ԃ悤�ɁA�W���s������Z����B�W���s��ɂ́A�ʐς�̐ρA�M�`�B���Ȃǂ̃p�����[�^�̐ς����Ă��܂������̂�����B


	//> IH���M����face��������x(�\��face && �����)�i��A�֌W��������face�ߓ_�̌��_����̋������v�Z���Aface[].mayIHheated�𔻒�
	//CalcVtxDisFromOrigin();
	CalcVtxDisFromVertex(Vec2d(0.0, -0.005));
	
	//>	IH����̒P�ʎ��ԓ�����̉��M�M��
	//�P�ʎ��ԓ�����̑����M�M��	231.9; //>	J/sec
	
	//�~���M�FIH
	//CalcIHdqdt(0.04,0.095,231.9 * 0.005 * 1e6);		/// �P�� m,m,J/sec		//> 0.002:dt�̕�;Step�ŗp����dt�{��������...	// 0.05,0.11�͓K���l
	//CalcIHdqdt_atleast(0.06,0.095,231.9 * 0.005 * 1e5);		///	�����ł��~�̈�ɂ������Ă�����A����face�̖ʐϑS����IH���M��������
	
	//	�d�v
	//20120811
	//CalcIHdqdt_atleast(0.11,0.14,231.9 * 0.005 * 1e5);		//main�̒��Ɏ����AphPan�ɂ������s��������
	
	
	//..debug 
	//�o���h����M
//	CalcIHdqdtband_(-0.02,0.20,231.9 * 0.005 * 1e6);

	
	//%%	IH���M�̃��[�h�ؑ�
	//	���C����ɉ��M
	//	CalcIHdqdtband_(0.09,0.10,231.9 * 5e3);		//*0.5*1e4	�l��ς��Ď���	//*1e3�@//*1e4 //5e3
	//	�~��ɉ��M
	
	//CalcIHarea(0.04,0.095,231.9 * 0.005 * 1e6);




	//	���̌�ŁA�M�����x�N�g�����v�Z����֐����Ăяo��

	InitCreateMatC();					///	CreateMatC�̏�����
	InitVecFAlls();					///	VecFAll�ނ̏�����
	InitCreateMatk();					///	CreateMatK�̏�����
	//..	CreateLocalMatrixAndSet();			//> �ȏ�̏������A���̊֐��ɏW��

	///	�M�`�B�����e�ߓ_�Ɋi�[
	SetHeatTransRatioToAllVertex();
	for(unsigned i=0; i < mesh->tets.size(); i++){ //this��mesh shiba
		//�e�s�������āA�K�E�X�U�C�f���Ōv�Z���邽�߂̌W���̊�{�����BTimestep�̓����Ă��鍀�́A���̃\�[�X(SetDesc())�ł́A�����ł��Ȃ����Ƃ���������(NULL���Ԃ��Ă���)
		CreateMatkLocal(i);				///	Matk1 Matk2(�X�V���K�v�ȏꍇ������)�����	//ifdef�X�C�b�`�őS�̍����s���(�\���p����)�����\
		//CreateMatKall();		//CreateMatkLocal();�Ɏ��������̂ŁA���������B
		CreatedMatCAll(i);
		CreateVecFAll(i);
	}

	int hogeshidebug =0;
	//	�ߓ_���x���ڂ̏����o��
	templog.open("templog.csv");

	//	CPS�̌o���ω��������o��
	//cpslog.open("cpslog.csv");

	// �J�E���g�̏�����
	Ndt =0;

	//������������̏�����
	InitMoist();

}

void PHFemThermo::Setup(){
	PHFemMeshNew* mesh = GetPHFemMesh();
	
	//%%%	��������		%%%//

	//�e�탁���o�ϐ��̏������˃R���X�g���N�^�łł����ق���������������Ȃ��B
	///	Edges
	for(unsigned i =0; i < mesh->edges.size();i++){
		edgeCoeffs[i].c = 0.0;	
		edgeCoeffs[i].k = 0.0;
	}

	///	faces
	for(unsigned i=0;i<mesh->faces.size();i++){
		faceVars[i].alphaUpdated = true;
		faceVars[i].area = 0.0;
		faceVars[i].heatTransRatio = 0.0;
		faceVars[i].deformed = true;				//������Ԃ́A�ό`��Ƃ���
		faceVars[i].fluxarea =0.0;
//		faces[i].heatflux =0.0;
		//faces[i].heatflux.clear();				// ������
		//faces[i].heatflux[hum]�̗̈�m�ہF�z��Ƃ��āA���Avetor�Ƃ��Ă�push_back���A�ǂ��炩���s���B�z��Ȃ炱���ɋL�q�B
		for(unsigned j =0; j < HIGH +1 ; j++){			// ���M���[�h�̐������A�x�N�g���𐶐�
			faceVars[i].heatflux[j] = 0.0;
		}
	}

	///	vertex

	///	tets

	//�s��̐������Ȃǂ�������
	bVecAll.resize(mesh->vertices.size(),1);

	//�s������
		//�s�����邽�߂ɕK�v�Ȑߓ_��l�ʑ̂̏��́APHFemThermo�̍\���̂ɓ����Ă���B
			//PHFemThermo�̃I�u�W�F�N�g�����ۂɁA�f�B�X�N���v�^�ɒl��ݒ肵�č��
		
	//�ߓ_���x�̏����ݒ�(�s������O�ɍs��)
	SetVerticesTempAll(0.0);			///	�������x�̐ݒ�

	//���͗��̉��x�̏�����(temp�x�ɂ���)
	InitTcAll(0.0);

	//�ߓ_�̏������x��ݒ肷���{T}�c�x�N�g���ɑ��
		//{T}�c�x�N�g�����쐬����B�ȍ~��K,C,F�s��E�x�N�g���̐ߓ_�ԍ��́A���̏c�x�N�g���̐ߓ_�̕��я��ɍ��킹��?
		
	//dmnN �����̉��x�̏c�i��j�x�N�g��
	CreateTempMatrix();

	//�M�`�����A���x�A��M�A�M�`�B���@�̃p�����[�^�[��ݒ�E���
		//PHFemThermo�̃����o�ϐ��̒l���� CADThermo���A0.574;//�ʂ˂��̒l//�M�`����[W/(���EK)]�@Cp = 1.96 * (Ndt);//�ʂ˂��̔�M[kJ/(kg�EK) 1.96kJ/(kg K),�i�ʂ˂��̖��x�j�H�i���M�̉Ȋwp64���970kg/m^3
		//�M�`�B���̒P�ʌn�@W/(m^2 K)�˂����SI�P�ʌn�Ȃ̂��H�@25�͘_��(MEAT COOKING SIMULATION BY FINITE ELEMENTS)�̃I�[�u�����M���̎����l
		//SetInitThermoConductionParam(0.574,970,1.96,25);
	//. �M�`�B���� SetInitThermoConductionParam(0.574,970,0.1960,25 * 0.001 );		//> thConduct:�M�`���� ,roh:���x,	specificHeat:��M J/ (K�Ekg):1960 ,�@heatTrans:�M�`�B�� W/(m^2�EK)
	//. �M�`�B���Ȃ�
//	SetInitThermoConductionParam(0.574,970,0.1960,0 );		// �M�`�B��=0;�ɂ��Ă���w
	SetInitThermoConductionParam(THCOND,970,0.1960,0 );		// �M�`�B��=0;�ɂ��Ă���w
	


	//�f�M�ߒ�
	//SetInitThermoConductionParam(0.574,970,0.1960,0.0);		//> thConduct:�M�`���� ,roh:���x,	specificHeat:��M J/ (K�Ekg):1960 ,�@heatTrans:�M�`�B�� W/(m^2�EK)
	//�����A�ϐ��l�͌ォ��v�Z�̓r���ŕύX�ł���悤��SetParam()�֐�������Ă������ق����������ȁH

	//> �M�����̏�����
	SetVtxHeatFluxAll(0.0);

	//>	�M���˗��̐ݒ�
	SetThermalEmissivityToVerticesAll(0.0);				///	�b��l0.0�ŏ�����	�F�M���˂͂��Ȃ���

	//>	�s��̍쐬�@�s��̍쐬�ɕK�v�ȕϐ��͂��̍s�ȑO�ɐݒ肪�K�v
		//�v�Z�ɗp����}�g���N�X�A�x�N�g�����쐬�i���b�V�����Ƃ̗v�f�����s��/�x�N�g���ˑS�̍����s��/�x�N�g���j
		//{T}�c�x�N�g���̐ߓ_�̕��я��ɕ��Ԃ悤�ɁA�W���s������Z����B�W���s��ɂ́A�ʐς�̐ρA�M�`�B���Ȃǂ̃p�����[�^�̐ς����Ă��܂������̂�����B


	//> IH���M����face��������x(�\��face && �����)�i��A�֌W��������face�ߓ_�̌��_����̋������v�Z���Aface[].mayIHheated�𔻒�
	//CalcVtxDisFromOrigin();
	CalcVtxDisFromVertex(Vec2d(0.0, -0.005));
	
	//>	IH����̒P�ʎ��ԓ�����̉��M�M��
	//�P�ʎ��ԓ�����̑����M�M��	231.9; //>	J/sec
	
	//�~���M�FIH
	//CalcIHdqdt(0.04,0.095,231.9 * 0.005 * 1e6);		/// �P�� m,m,J/sec		//> 0.002:dt�̕�;Step�ŗp����dt�{��������...	// 0.05,0.11�͓K���l
	//CalcIHdqdt_atleast(0.06,0.095,231.9 * 0.005 * 1e5);		///	�����ł��~�̈�ɂ������Ă�����A����face�̖ʐϑS����IH���M��������
	
	//	�d�v
	//20120811
	//CalcIHdqdt_atleast(0.11,0.14,231.9 * 0.005 * 1e5);		//main�̒��Ɏ����AphPan�ɂ������s��������
	
	
	//..debug 
	//�o���h����M
//	CalcIHdqdtband_(-0.02,0.20,231.9 * 0.005 * 1e6);

	
	//%%	IH���M�̃��[�h�ؑ�
	//	���C����ɉ��M
	//	CalcIHdqdtband_(0.09,0.10,231.9 * 5e3);		//*0.5*1e4	�l��ς��Ď���	//*1e3�@//*1e4 //5e3
	//	�~��ɉ��M
	
	//CalcIHarea(0.04,0.095,231.9 * 0.005 * 1e6);




	//	���̌�ŁA�M�����x�N�g�����v�Z����֐����Ăяo��

	InitCreateMatC();					///	CreateMatC�̏�����
	InitVecFAlls();					///	VecFAll�ނ̏�����
	InitCreateMatk();					///	CreateMatK�̏�����
	//..	CreateLocalMatrixAndSet();			//> �ȏ�̏������A���̊֐��ɏW��

	///	�M�`�B�����e�ߓ_�Ɋi�[
	SetHeatTransRatioToAllVertex();
	for(unsigned i=0; i < mesh->tets.size(); i++){ //this��mesh shiba
		//�e�s�������āA�K�E�X�U�C�f���Ōv�Z���邽�߂̌W���̊�{�����BTimestep�̓����Ă��鍀�́A���̃\�[�X(SetDesc())�ł́A�����ł��Ȃ����Ƃ���������(NULL���Ԃ��Ă���)
		CreateMatkLocal(i);				///	Matk1 Matk2(�X�V���K�v�ȏꍇ������)�����	//ifdef�X�C�b�`�őS�̍����s���(�\���p����)�����\
		//CreateMatKall();		//CreateMatkLocal();�Ɏ��������̂ŁA���������B
		CreatedMatCAll(i);
		CreateVecFAll(i);
	}

	int hogeshidebug =0;
	//	�ߓ_���x���ڂ̏����o��
	templog.open("templog.csv");

	//	CPS�̌o���ω��������o��
	//cpslog.open("cpslog.csv");

	// �J�E���g�̏�����
	Ndt =0;

	//������������̏�����
	InitMoist();
}

//void PHFemThermo::CreateLocalMatrixAndSet(){
//	//K,C,F�̍s������֐����Ăяo���āA��点��
//	for(unsigned i = 0; i< tets.size() ; i++){
//		//tets�������ɂ����ƁA���̍s��E�x�N�g��������Ă���āA�ł����s��A�x�N�g������ɌW����ݒ肵�����
//		//��������΁A�e�v�f�����s���for�����񂳂Ȃ��Ă��悭�Ȃ�
//		//CreateMatkLocal(tets);
//		//CreateMatcLocal(tets);
//		//CreateVecfLocal(tets);
//
//		//tets�����č�点��
////		SetkcfParam(tets);
//
//	}
//
//}
void PHFemThermo::SetkcfParam(FemTet tets){
}

void PHFemThermo::CreateMatc(unsigned id){
	//�Ō�ɓ����s���������
	for(unsigned i =0; i < 4 ;i++){
		for(unsigned j =0; j < 4 ;j++){
			matc[i][j] = 0.0;
		}
	}
	//matc ��21�łł����s�������
	matc = Create44Mat21();
	//	for debug
		//DSTR << "matc " << matc << " �� ";
	matc = rho * specificHeat * CalcTetrahedraVolume(GetPHFemMesh()->tets[id]) / 20.0 * matc;
	//	debug	//�W���̐ς��Ƃ�
		//DSTR << matc << std::endl;
		//int hogemat =0 ;
}

void PHFemThermo::CreatedMatCAll(unsigned id){

	PHFemMeshNew* mesh = GetPHFemMesh();

	//���ׂĂ̗v�f�ɂ��ČW���s������
		//c
	CreateMatc(id);
	int mathoge=0;
	//	(�K�E�X�U�C�f�����g�����v�Z��)�v�f���ɍ�����s��̐������A�G�b�W�ɌW�����i�[����
	//	or	(�K�E�X�U�C�f�����g��Ȃ��v�Z��)�v�f���Ƃ̌v�Z���I��邽�тɁA�v�f�����s��̐����������G�b�W��_�ɍ��ϐ��Ɋi�[���Ă���	#ifedef�Ń��[�h����āA�ǂ�����ł���悤�ɂ��Ă����Ă��ǂ�����w
	for(unsigned j=1; j < 4; j++){
		int vtxid0 = mesh->tets[id].vertexIDs[j];
		//	���O�p�s�񕔕��ɂ��Ă̂ݎ��s
		//	j==1:k=0, j==2:k=0,1, j==3:k=0,1,2
		for(unsigned k = 0; k < j; k++){
			int vtxid1 = mesh->tets[id].vertexIDs[k];
				for(unsigned l =0; l < mesh->vertices[vtxid0].edgeIDs.size(); l++){
					for(unsigned m =0; m < mesh->vertices[vtxid1].edgeIDs.size(); m++){
						if(mesh->vertices[vtxid0].edgeIDs[l] == mesh->vertices[vtxid1].edgeIDs[m]){
							edgeCoeffs[mesh->vertices[vtxid0].edgeIDs[l]].c += matc[j][k];		//�������̂������͂������甼���ɂ���B��O�p�����O�p������������ɂ́A�ǂ�����for�����ɂ���Ηǂ��̂��H
							//DSTR << edges[vertices[vtxid0].edges[l]].k << std::endl;
						}
					}
				}
		}
	}
	//�Ίp������Ίp�����̑S�̍����s�񂩂甲���o����1�~n�̍s��ɑ������
	//j=0~4�܂ő��(��̃��[�v�ł́Aj�͑Ίp�����͈̔͂����Ȃ��̂ŁA�l������Ȃ��������o�Ă��܂�)
	for(unsigned j =0;j<4;j++){
		dMatCAll[0][mesh->tets[id].vertexIDs[j]] += matc[j][j];
	}

	////	for debug
	//DSTR << "dMatCAll : " << std::endl;
	//for(unsigned j =0;j < vertices.size();j++){
	//	DSTR << j << "th : " << dMatCAll[0][j] << std::endl;
	//}
	// �l�M�ɂ��Ĕ�0�����ɂȂ����B

	//	���ׂ�
	//dMatKAll�̐����̂����A0�ƂȂ�v�f����������A�G���[�\��������R�[�h������
	// try catch���ɂ���
	//for(unsigned j = 0; j < vertices.size() ; j++){
	//	if(dMatCAll[0][j] ==0.0){
	//		DSTR << "dMatCAll[0][" << j << "] element is blank" << std::endl;
	//	}
	//}
	int piyodebug =0;
}

void PHFemThermo::CreateVecFAll(unsigned id){

	//	����
	//	f3���g�p����ꍇ:���͗��̉��xTc��0�̐ߓ_�̗v�f��0�ɂȂ邽�߁A���x�̐ݒ肪�K�v
	
	//���ׂĂ̗v�f�ɂ��ČW���s������
	//f1�����
	//>	�M�������E����	vecf2�����			
	//CreateVecf2(id);				//>	tets[id].vecf[1] ��������,���		�M�����͑������ςŋ��߂�
	CreateVecf2surface(id);			
	//>	�M�`�B���E����	f3�����
	CreateVecf3(id);			// surface�����ׂ������	//>	tets[id].vecf[2] ��������,���		�M�`�B���͑������ρA���͗��̉��x�͐ߓ_�̌`��֐��H���Ƃɋ��߂�
	//CreateVecf3_(id);			//>	tets[id].vecf[2] ��������,���		�M�`�B���A���͗��̉��x�𑊉����ςŋ��߂�
	//f4�����
	//f1:vecf[0],f2:vecf[1],f3:vecf[2],f4:vecf[3]�����Z����
	vecf = GetPHFemMesh()->tets[id].vecf[1] + GetPHFemMesh()->tets[id].vecf[2];		//>	+ tets[id].vecf[0] +  tets[id].vecf[3] �̗\��
	//	(�K�E�X�U�C�f�����g�����v�Z��)�v�f���ɍ�����s��̐������A�G�b�W�ɌW�����i�[����
	//	or	(�K�E�X�U�C�f�����g��Ȃ��v�Z��)�v�f���Ƃ̌v�Z���I��邽�тɁA�v�f�����s��̐����������G�b�W��_�ɍ��ϐ��Ɋi�[���Ă���	#ifedef�Ń��[�h����āA�ǂ�����ł���悤�ɂ��Ă����Ă��ǂ�����w

	//�v�f�̐ߓ_�ԍ��̏ꏊ�ɁA���̐ߓ_��f�̒l������
	//j:�v�f�̒��̉��Ԗڂ�
	for(unsigned j =0;j < 4; j++){
		int vtxid0 = GetPHFemMesh()->tets[id].vertexIDs[j];
		vecFAllSum[vtxid0] += vecf[j];
		//vecFAll_f2IH[num][vtxid0][0] += vecf[j];
	}
	//	for debug
	//vecFAllSum�ɒl���������̂��ǂ����𒲂ׂ� 2011.09.21�S���ɒl�������Ă��邱�Ƃ��m�F����
	//DSTR << "vecFAllSum : " << std::endl;
	//for(unsigned j =0; j < vertices.size() ; j++){
	//	DSTR << j << " ele is :  " << vecFAllSum[j] << std::endl;
	//}

	////	���ׂ�
	////vecFAllSum�̐����̂����A0�ƂȂ�v�f����������A�G���[�\��������R�[�h������
	//// try catch���ɂ���
	//for(unsigned j = 0; j < vertices.size() ; j++){
	//	if(vecFAllSum[j] ==0.0){
	//		DSTR << "vecFAllSum[" << j << "] element is blank" << std::endl;
	//	}
	//}

}

void PHFemThermo::CreateMatkLocal(unsigned id){

	PHFemMeshNew* mesh = GetPHFemMesh();

	//���ׂĂ̎l�ʑ̗v�f�ɂ��ČW���s������

	//	k1�����	k1k�ł��Ak1b�ł��ǂ���ł��\��Ȃ�	�ǂ�炪���������ׂ�
	///	�ό`������������������
	if(deformed){	CreateMatk1k(id);}			//  K��ꍀ�̍s��쐬	//k���_�������ɁA���M���āA�`��֐��𓱏o	
//	if(deformed){	CreateMatk1b(id);}			//	���Ђ̗��_�������ɁA������p���Č`��֐��𓱏o
	//DSTR << "tets[id].matk1: " << tets[id].matk1 << std::endl;

	//�M�`�B���E�����ɕK�v�ȁAk2����邩�ۂ�
	CreateMatk2t(id);					///	�M�`�B���E����
	//CreateMatk2(id,tets[id]);			///	���̊֐��͎g�p���Ȃ�
	//DSTR << "tets[id].matk2: " << tets[id].matk2 << std::endl;
	int hogehogehoge=0;

	//k1,k2,k3�����Z����(�g���Ă��鐔�l����)
	matk = tetVars[id].matk[0] + tetVars[id].matk[1];	
	//DSTR << "matk: " << matk << std::endl;

	//	(�K�E�X�U�C�f�����g�����v�Z��)�v�f���ɍ�����s��̐������A�G�b�W�ɌW�����i�[����
	//	or	(�K�E�X�U�C�f�����g��Ȃ��v�Z��)�v�f���Ƃ̌v�Z���I��邽�тɁA�v�f�����s��̐����������G�b�W��_�ɍ��ϐ��Ɋi�[���Ă���	#ifedef�Ń��[�h����āA�ǂ�����ł���悤�ɂ��Ă����Ă��ǂ�����w
	//	Edges �̂��̗v�f�Ōv�Z����K�s��̐�����k�ɌW���Ƃ��Ċi�[����
		
	//matk�̑Ίp�����ȊO�ŁA���O�p�̕����̒l���Aedge��k�ɑ������
	//
	//DSTR << i <<"th tetrahedra element'edges[vertices[vtxid0].edges[l]].k (All edges.k' value): " << std::endl;
	for(unsigned j=1; j < 4; j++){
		int vtxid0 = mesh->tets[id].vertexIDs[j];
		//	���O�p�s�񕔕��ɂ��Ă̂ݎ��s
		//	j==1:k=0, j==2:k=0,1, j==3:k=0,1,2
		for(unsigned k = 0; k < j; k++){
			int vtxid1 = mesh->tets[id].vertexIDs[k];
			for(unsigned l =0; l < mesh->vertices[vtxid0].edgeIDs.size(); l++){
				for(unsigned m =0; m < mesh->vertices[vtxid1].edgeIDs.size(); m++){
					if(mesh->vertices[vtxid0].edgeIDs[l] == mesh->vertices[vtxid1].edgeIDs[m]){
						edgeCoeffs[mesh->vertices[vtxid0].edgeIDs[l]].k += matk[j][k];		//�������̂������͂������甼���ɂ���B��O�p�����O�p������������ɂ́A�ǂ�����for�����ɂ���Ηǂ��̂��H
						//DSTR << edges[vertices[vtxid0].edges[l]].k << std::endl;
#ifdef DumK
						mesh->edges[mesh->vertices[vtxid0].edgeIDs[l]].k = 0.0;
#endif DumK
					}
				}
			}
		}
	}

#ifdef UseMatAll
	//SciLab�Ŏg�����߂ɁA�S�̍����s������
	//matk������
	for(unsigned j=0; j<4 ; j++){
		for(unsigned k=0; k<4 ;k++){
			matKAll[mesh->tets[id].vertexIDs[j]][mesh->tets[id].vertexIDs[k]] += matk[j][k];
		}
	}

	////edges�ɓ������W��������
	//for(unsigned j=1; j < 4; j++){
	//	int vtxid0 = tets[i].vertices[j];
	//	//	���O�p�s�񕔕��ɂ��Ă̂ݎ��s
	//	//	j==1:k=0, j==2:k=0,1, j==3:k=0,1,2
	//	for(unsigned k = 0; k < j; k++){
	//		int vtxid1 = tets[i].vertices[k];
	//			for(unsigned l =0; l < vertices[vtxid0].edges.size(); l++){
	//				for(unsigned m =0; m < vertices[vtxid1].edges.size(); m++){
	//					if(vertices[vtxid0].edges[l] == vertices[vtxid1].edges[m]){
	//						edges[vertices[vtxid0].edges[l]].k += matk[j][k];		//�������̂������͂������甼���ɂ���B��O�p�����O�p������������ɂ́A�ǂ�����for�����ɂ���Ηǂ��̂��H
	//						//DSTR << edges[vertices[vtxid0].edges[l]].k << std::endl;
	//					}
	//				}
	//			}
	//	}
	//}

#endif UseMatAll

#ifdef DumK
	//SciLab�Ŏg�����߂ɁA�S�̍����s������
	//matk������
	for(unsigned j=0; j<4 ; j++){
		for(unsigned k=0; k<4 ;k++){
			matKAll[tets.vertices[j]][tets.vertices[k]] = 0.0;;
		}
	}
#endif

	//�Ίp������Ίp�����̑S�̍����s�񂩂甲���o����1�~n�̍s��ɑ������
	//j=0~4�܂ő��(��̃��[�v�ł́Aj�͑Ίp�����͈̔͂����Ȃ��̂ŁA�l������Ȃ��������o�Ă��܂�)
	for(unsigned j =0;j<4;j++){
		dMatKAll[0][mesh->tets[id].vertexIDs[j]] += matk[j][j];
		//DSTR << "matk[" << j << "][" << j << "] : " << matk[j][j] << std::endl;
		//DSTR << "dMatKAll[0][" << tets[i].vertices[j] << "] : " << dMatKAll[0][tets[i].vertices[j]] << std::endl;
		int hoge4 =0;
	}
	//DSTR << std::endl;	//���s

	//std::ofstream matKAll("matKAll.csv");
	//for(){
	//	matKAll
	//	}


#ifdef DumK
	for(unsigned j=0;j<4;j++){
		dMatKAll[0][tets.vertices[j]] = 0.0;
		int hogeshi =0;
	} 
#endif DumK

	
	//	for debug
	//�v�f25��0~3�Ԗڂ̐ߓ_�������\������
	//if(i == 25){
	//		for(unsigned n=0;n < 4;n++){
	//			DSTR << n << " : " << tets[25].vertices[n] << std::endl;	//�v�f25��0�Ԗڂ̐ߓ_��63�ł���B
	//		}
	//}
	//�ߓ_�ԍ���63�̓_���ǂ̗v�f�ɓ����Ă���̂��𒲂ׂ��25,57������
	//for(unsigned j=0;j < vertices[63].tets.size();j++){
	//	DSTR << vertices[63].tets[j] <<std::endl;
	//}
		
	//	���ׂ�
	//dMatKAll�̐����̂����A0�ƂȂ�v�f����������A�G���[���H�@�����Ă��Ȃ������������Ă��A���Ȃ��C������
	// try catch���ɂ���
//	for(unsigned j = 0; j < vertices.size() ; j++){
//		if(dMatKAll[0][j] ==0.0){
//			DSTR << "Creating dMatKAll error!? : dMatKAll[0][" << j << "] == 0.0 " << std::endl;
////			DSTR << "If " << j <<" 's blank eroors didn't banished until display simulation scene, I recommened Source Code Check!" <<std::endl;  
//		}
//	}

	//DSTR << "matKAll : " << matKAll <<std::endl;
	//for(unsigned i=0;i<vertices.size();i++){
	//	for(unsigned j=0;j <  vertices.size();j++){
	//		if(matKAll[i][j] !=0){
	//			DSTR << "matKAll[" << i <<"][" << j << "]: " << matKAll[i][j] <<std::endl;
	//		}
	//	}
	//}
	//DSTR << "dMatKAll : " <<dMatKAll << std::endl;
#ifdef UseMatAll
	for(unsigned j =0;j<mesh->vertices.size();j++){
		if(matKAll[j][j] != dMatKAll[0][j]){
			DSTR << j <<" �����̗v�f�͂��������I�������K�v�ł���B " <<std::endl;
		}
	}
#endif UseMatAll
	int hoge5 =0;

}

void PHFemThermo::CreateMatk1b(unsigned id){

	PHFemMeshNew* mesh = GetPHFemMesh();

	//yagawa1983����Ƀm�[�g�Ɏ��W�J�����v�Z��
	unsigned i=0;
	unsigned j=0;
	unsigned k=0;
	unsigned l=0;

	//a_1~a_4, ... , c_4	���쐬
	//�W��(����)�~�s�񎮂̓��ꕨ
	double a[4];
	double b[4];
	double c[4];

	//�v�f���\������ߓ_�̍��W�̓��ꕨ
	double x[4];
	double y[4];
	double z[4];
	//�v�f����4�ߓ_��x,y,z���W���i�[
	for(unsigned m=0; m < 4;m++){
		x[m] = mesh->vertices[mesh->tets[id].vertexIDs[m]].pos.x;
		y[m] = mesh->vertices[mesh->tets[id].vertexIDs[m]].pos.y;
		z[m] = mesh->vertices[mesh->tets[id].vertexIDs[m]].pos.z;
	}

	//mata~matc
	//a[i]
	for(unsigned i =0;i<4;i++){
		double fugou =0.0;				// (-1)^i �̕����̒�`

		//	fugou �̕�������
		if(i == 0 || i == 2){		//0,2�̎��A(-1)^1,3 = -1
			fugou = -1.0;
		}
		else{					//1,3�̎��A(-1)^0,2 = 1
			fugou = 1.0;
		}
		
		//i,j,k,l�̊֌W�Z�b�g�˔z��̗v�f�ɂ��Ă����������Bi[4],if(i[0]=0){i[1](=j)=1, i[2](=k)=2, i[3](=l)=3}	if(i[0]=1){i[1](=j)=2, i[2](=k)=3, i[3](=l)=0}
		if(i==0){		j=1;	k=2;	l=3;	}
		else if(i==1){	j=2;	k=3;	l=0;	}
		else if(i==2){	j=3;	k=0;	l=1;	}
		else if(i==3){	j=0;	k=1;	l=2;	}

		// a_i�̍쐬
		for(unsigned m =0;m<3;m++){						//	1�̐����ւ̑���͂���
			mata[m][0] = 1.0;
		}
		mata[0][1] = y[j];
		mata[1][1] = y[k];
		mata[2][1] = y[l];

		mata[0][2] = z[j];
		mata[1][2] = z[k];
		mata[2][2] = z[l];

		a[i] = fugou * mata.det();

		//DSTR << "mata : " <<std::endl;
		//DSTR << mata << std::endl;

		//DSTR << "mata.det() : " <<std::endl;
		//DSTR << mata.det() << std::endl;

		//DSTR << "a[" << i << "] : " <<std::endl;
		//DSTR << a[i] << std::endl;


		// b_i�̍쐬
		matb[0][0]=x[j];
		matb[1][0]=x[k];
		matb[2][0]=x[l];

		for(unsigned m =0;m<3;m++){						//	1�̐����ւ̑���͂���
			matb[m][1] = 1.0;
		}

		matb[0][2]=z[j];
		matb[1][2]=z[k];
		matb[2][2]=z[l];

		b[i] = fugou * matb.det();

		//DSTR << "matb : " <<std::endl;
		//DSTR << matb << std::endl;
		//DSTR << "matb.det() : " <<std::endl;
		//DSTR << matb.det() << std::endl;

		//DSTR << "b[" << i << "] : " <<std::endl;
		//DSTR << b[i] << std::endl;

		// c_i�̍쐬
		matcc[0][0]=x[j];
		matcc[1][0]=x[k];
		matcc[2][0]=x[l];

		matcc[0][1]=y[j];
		matcc[1][1]=y[k];
		matcc[2][1]=y[l];

		for(unsigned m =0;m<3;m++){						//	1�̐����ւ̑���͂���
			matcc[m][2] = 1.0;
		}

		//DSTR << "matcc : " <<std::endl;
		//DSTR << matcc << std::endl;
		//DSTR << "matcc.det() : " <<std::endl;
		//DSTR << matcc.det() << std::endl;

		c[i] = fugou * matcc.det();
		
		//	for debug�@�v�f���Ƃ�a_i~c_i�̎Z�o
		//DSTR << "a["<< i << "] : " << a[i] << std::endl;
		//DSTR << "b["<< i << "] : " << b[i] << std::endl;
		//DSTR << "c["<< i << "] : " << c[i] << std::endl;
		//DSTR << std::endl;
		int debughogeshi =0;
	}
	
	//	matk1�̐�����a_i ~ c_i�̑���������	���ꂢ�����邪�A���̃R�[�h�ŗǂ��I	�Ίp��������Ίp�������A�S���A���̃R�[�h
	//	���P��		���O�p�ƑΊp���������A�v�Z���A��O�p�͉��O�p�����ł��悢�B
	for(unsigned i =0;i<4;i++){
		for(unsigned j =0;j<4;j++){
			tetVars[id].matk[0][i][j] = a[i] * a[j] +b[i] * b[j] + c[i] * c[j];
		}
	}

	////	��L���R�X�g�̏��Ȃ��R�[�h?
	//matk[0][0] = a[0] * a[0] +b[0] * b[0] + c[0] * c[0];
	//matk[1][1] = a[1] * a[1] +b[1] * b[1] + c[1] * c[1];
	//matk[2][2] = a[2] * a[2] +b[2] * b[2] + c[2] * c[2];
	//matk[3][3] = a[3] * a[3] +b[3] * b[3] + c[3] * c[3];

	//matk[0][1] = a[0] * a[1] + b[0] * b[1] + c[0] * c[1];
	//matk[1][0] = matk[0][1];

	//matk[0][2] = a[0] * a[2] + b[0] * b[2] + c[0] * c[2];
	//matk[2][0] = matk[0][2];

	//matk[0][3] = a[0] * a[3] + b[0] * b[3] + c[0] * c[3];
	//matk[3][0] = matk[0][3];

	//	for DEBUG
	//DSTR << "matk1 : " << std::endl;
	//DSTR << matk1 << std::endl;
	//int debughogeshi2 =0;
	
	//�W���̐�
	tetVars[id].matk[0]= thConduct / (36 *  CalcTetrahedraVolume(mesh->tets[id])) * tetVars[id].matk[0];		//���_���Ԉ���Ă����̂ŁA�C��

	//	for DEBUG
	//DSTR << "�W���ό�� matk1 : " << std::endl;
	//DSTR << matk1 << std::endl;
	int debughogeshi3 =0;

	DSTR << "Inner Function matk1b _ tets["<< id << "].matk[0] "<< tetVars[id].matk[0] << std::endl;  
	//a~c�̑�������K1�ɑ��
	//matk1(4x4)�ɑ��

}

void PHFemThermo::CreateMatk1k(unsigned id){

	PHFemMeshNew* mesh = GetPHFemMesh();

	//���̌v�Z���Ăяo���Ƃ��ɁA�e�l�ʑ̂��ƂɌv�Z���邽�߁A�l�ʑ̂�0�Ԃ��珇�ɂ��̌v�Z���s��
	//�l�ʑ̂��\������4�ߓ_��ߓ_�̔z��(Tets�ɂ́A�ߓ_�̔z�񂪍���Ă���)�ɓ����Ă��鏇�Ԃ��g���āA�ʂ̌v�Z���s������A�s��̌v�Z���s�����肷��B
	//���̂��߁A���̊֐��̈����ɁA�l�ʑ̗v�f�̔ԍ������

	//�Ō�ɓ����s���������
	tetVars[id].matk[0].clear();
	//for(unsigned i =0; i < 4 ;i++){
	//	for(unsigned j =0; j < 4 ;j++){
	//		//tets[id].matk1[i][j] = 0.0;
	//		tets[id].matk[0][i][j] = 0.0;
	//	}
	//}

	//	A�s��@=	a11 a12 a13
	//				a21 a22 a23
	//				a31 a32 a33
	//�𐶐�
	PTM::TMatrixRow<4,4,double> matk1A;
	FemVertex p[4];
	for(unsigned i = 0; i< 4 ; i++){
		p[i]= mesh->vertices[mesh->tets[id].vertexIDs[i]];
	}
	
	matk1A[0][0] = (p[2].pos.y - p[0].pos.y) * (p[3].pos.z - p[0].pos.z) - (p[2].pos.z - p[0].pos.z) * (p[3].pos.y - p[0].pos.y);
	matk1A[0][1] = (p[1].pos.z - p[0].pos.z) * (p[3].pos.y - p[0].pos.y) - (p[1].pos.y - p[0].pos.y) * (p[3].pos.z - p[0].pos.z);
	matk1A[0][2] = (p[1].pos.y - p[0].pos.y) * (p[2].pos.z - p[0].pos.z) - (p[1].pos.z - p[0].pos.z) * (p[2].pos.y - p[0].pos.y);

	matk1A[1][0] = (p[2].pos.z - p[0].pos.z) * (p[3].pos.x - p[0].pos.x) - (p[2].pos.x - p[0].pos.x) * (p[3].pos.z - p[0].pos.z);
	matk1A[1][1] = (p[1].pos.x - p[0].pos.x) * (p[3].pos.z - p[0].pos.z) - (p[1].pos.z - p[0].pos.z) * (p[3].pos.x - p[0].pos.x);
	matk1A[1][2] = (p[1].pos.z - p[0].pos.z) * (p[2].pos.x - p[0].pos.x) - (p[1].pos.x - p[0].pos.x) * (p[2].pos.z - p[0].pos.z);

	matk1A[2][0] = (p[2].pos.x - p[0].pos.x) * (p[3].pos.y - p[0].pos.y) - (p[2].pos.y - p[0].pos.y) * (p[3].pos.x - p[0].pos.x);
	matk1A[2][1] = (p[1].pos.y - p[0].pos.y) * (p[3].pos.x - p[0].pos.x) - (p[1].pos.x - p[0].pos.x) * (p[3].pos.y - p[0].pos.y);
	matk1A[2][2] = (p[1].pos.x - p[0].pos.x) * (p[2].pos.y - p[0].pos.y) - (p[1].pos.y - p[0].pos.y) * (p[2].pos.x - p[0].pos.x);

	//	for	debug
	//DSTR << matk1A[0][0] << std::endl;
	//DSTR << "matk1A : " << matk1A << std::endl; 
	//int hogeshi =0;

	//a11 ~ a33 ���s��ɓ���āA[N~T] [N] ���v�Z������
	
	PTM::TMatrixRow<1,4,double> Nx;
	PTM::TMatrixRow<1,4,double> Ny;
	PTM::TMatrixRow<1,4,double> Nz;

	Nx[0][0] = -matk1A[0][0] - matk1A[0][1] -matk1A[0][2];
	Nx[0][1] = matk1A[0][0];
	Nx[0][2] = matk1A[0][1];
	Nx[0][3] = matk1A[0][2];

	Ny[0][0] = -matk1A[1][0] - matk1A[1][1] -matk1A[1][2];
	Ny[0][1] = matk1A[1][0];
	Ny[0][2] = matk1A[1][1];
	Ny[0][3] = matk1A[1][2];

	Nz[0][0] = -matk1A[2][0] - matk1A[2][1] -matk1A[2][2];
	Nz[0][1] = matk1A[2][0];
	Nz[0][2] = matk1A[2][1];
	Nz[0][3] = matk1A[2][2];

	//	Km �̎Z�o
	//tets[id].matk1 = Nx.trans() * Nx + Ny.trans() * Ny + Nz.trans() * Nz;
	tetVars[id].matk[0] = Nx.trans() * Nx + Ny.trans() * Ny + Nz.trans() * Nz;

	////	for debug
	//DSTR << "Nx : " << Nx << std::endl;
	//DSTR << "Nx^T : " << Nx.trans() << std::endl;
	//DSTR << "Nx^T * Nx : " << Nx.trans() * Nx << std::endl;
	//DSTR << "Ny^T * Ny : " << Ny.trans() * Ny << std::endl;
	//DSTR << "Nz^T * Nz : " << Nz.trans() * Nz << std::endl;
	//DSTR << "matk1 : " << matk1 << std::endl;
	//int hogehoge =0;

	//	for	DEBUG
	//DSTR << "matk1 : " << matk1 << std::endl;

	//K1
//	matk1 = thConduct / (36 * CalcTetrahedraVolume(tets) ) * matk1;
	
	//tets[id].matk1 = thConduct / (36 * CalcTetrahedraVolume(tets[id]) ) * tets[id].matk1;
	tetVars[id].matk[0] = thConduct / (36 * CalcTetrahedraVolume(mesh->tets[id]) ) * tetVars[id].matk[0];
	//DSTR << "Inner Function _tets[id].matk1 : " << tets[id].matk1 << std::endl;

}

void PHFemThermo::CreateVecf2surface(unsigned id,unsigned num){

	PHFemMeshNew* mesh = GetPHFemMesh();

	// ������
	mesh->tets[id].vecf[1].clear();
	//l=0�̎�f21,1�̎�:f22, 2�̎�:f23, 3�̎�:f24	�𐶐�
	///	..j�Ԗڂ̍s��̐�����0�̃x�N�g�����쐬
	for(unsigned l= 0 ; l < 4; l++){
		vecf2array[l] = Create41Vec1();
		vecf2array[l][l] = 0.0;			//	l�s��0��
	}
	for(unsigned l= 0 ; l < 4; l++){
		///	�l�ʑ̂̊e��(l = 0 �` 3) �ɂ��ă��b�V���\�ʂ��ǂ������`�F�b�N����B�\�ʂȂ�A�s��������vecf2array�ɓ����
		//faces[tets.faces[i]].sorted;		/// 1,24,58�݂����Ȑߓ_�ԍ��������Ă���
		///	..�s��^�̓��ꕨ��p��
		//faces[tets.faces[l]].vertices;
		if(mesh->tets[id].faceIDs[l] < (int)mesh->nSurfaceFace && faceVars[mesh->tets[id].faceIDs[l]].fluxarea > 0 ){			///	�O�k�̖� ���� �M�`�B�����X�V���ꂽ�� matk2���X�V����K�v������
			///	�l�ʑ̂̎O�p�`�̖ʐς��v�Z		///	���̊֐��̊O�Ŗʐϕ��̖ʐόv�Z����������B�ړ�����
			if(faceVars[mesh->tets[id].faceIDs[l]].area ==0 || faceVars[mesh->tets[id].faceIDs[l]].deformed ){		///	�ʐς��v�Z����Ă��Ȃ����i�͂��߁j or deformed(�ό`�������E�������)��true�̎�		///	�����̒ǉ�	�ʐς�0�� ||(OR) �����X�V���ꂽ��
				faceVars[mesh->tets[id].faceIDs[l]].area = CalcTriangleArea(mesh->faces[mesh->tets[id].faceIDs[l]].vertexIDs[0], mesh->faces[mesh->tets[id].faceIDs[l]].vertexIDs[1], mesh->faces[mesh->tets[id].faceIDs[l]].vertexIDs[2]);
				faceVars[mesh->tets[id].faceIDs[l]].deformed = false;
			}
			///	�v�Z���ʂ��s��ɑ��
			///	area�̌v�Z�Ɏg���Ă��Ȃ��_�������Ă���s���������x�N�g���̐ς��Ƃ�
			///	�ϕ��v�Z�����{����l����
			unsigned vtx = mesh->tets[id].vertexIDs[0] + mesh->tets[id].vertexIDs[1] + mesh->tets[id].vertexIDs[2] + mesh->tets[id].vertexIDs[3];			
			///	area�v�Z�Ɏg���Ă��Ȃ��ߓ_ID�FID
			unsigned ID = vtx -( mesh->faces[mesh->tets[id].faceIDs[l]].vertexIDs[0] + mesh->faces[mesh->tets[id].faceIDs[l]].vertexIDs[1] + mesh->faces[mesh->tets[id].faceIDs[l]].vertexIDs[2] );
			for(unsigned j=0;j<4;j++){
				if(mesh->tets[id].vertexIDs[j] == ID){					///	�`��֐����P�A�i���Ȃ킿�j����face�ɑΖʂ��钸�_�@�ƈ�v������@���̎���face�Ŗʐϕ�����
					///	�O�k�ɂȂ����b�V���ʂ̖ʐς�0�ŏ��������Ă���
					///	�ȉ���[]�͏�܂ł�[l]�ƈقȂ�B
					///	ID�����Ԗڂ��ɂ���āA�`��֐��̌W�����قȂ�̂ŁA
					mesh->tets[id].vecf[1] += faceVars[mesh->tets[id].faceIDs[l]].heatflux[num] * (1.0/3.0) * faceVars[mesh->tets[id].faceIDs[l]].area * vecf2array[j];
					//DSTR << "tets[id].matk2��faces[tets[id].faces[l]].heatTransRatio * (1.0/12.0) * faces[tets[id].faces[l]].area * matk2array[" << j << "]"<< "�����Z: " <<faces[tets[id].faces[l]].heatTransRatio * (1.0/12.0) * faces[tets[id].faces[l]].area * matk2array[j] << std::endl;
					//DSTR << "tets[id].matk2 +=  " << tets[id].matk2 << std::endl;
				}
				//else{
				//	///	ID�ƈ�v���Ȃ��ꍇ�ɂ́Amatk2array[j]�ɂ͑S����0������
				//	///	�Ƃ������Ƃ��낾���A
				//	//matk2array[j] =0.0 * matk2array[j];
				//	//DSTR << "matk2array[" << j << "]: " << matk2array[j] << std::endl;
				//}
			}
		}
		///	SurfaceFace����Ȃ�������Amatk2array�ɂ�0������
		//else{
		//	//matk2array[l];
		//}
	}


}

void PHFemThermo::CreateVecf2surface(unsigned id){

	PHFemMeshNew* mesh = GetPHFemMesh();

	// ������
	mesh->tets[id].vecf[1].clear();
	//l=0�̎�f21,1�̎�:f22, 2�̎�:f23, 3�̎�:f24	�𐶐�
	///	..j�Ԗڂ̍s��̐�����0�̃x�N�g�����쐬
	for(unsigned l= 0 ; l < 4; l++){
		vecf2array[l] = Create41Vec1();
		vecf2array[l][l] = 0.0;			//	l�s��0��
	}
	for(unsigned l= 0 ; l < 4; l++){
		///	�l�ʑ̂̊e��(l = 0 �` 3) �ɂ��ă��b�V���\�ʂ��ǂ������`�F�b�N����B�\�ʂȂ�A�s��������vecf2array�ɓ����
		//faces[tets.faces[i]].sorted;		/// 1,24,58�݂����Ȑߓ_�ԍ��������Ă���
		///	..�s��^�̓��ꕨ��p��
		//faces[tets.faces[l]].vertices;
		if(mesh->tets[id].faceIDs[l] < (int)mesh->nSurfaceFace && faceVars[mesh->tets[id].faceIDs[l]].fluxarea > 0 ){			///	�O�k�̖� ���� �M�`�B�����X�V���ꂽ�� matk2���X�V����K�v������
			///	�l�ʑ̂̎O�p�`�̖ʐς��v�Z		///	���̊֐��̊O�Ŗʐϕ��̖ʐόv�Z����������B�ړ�����
			if(faceVars[mesh->tets[id].faceIDs[l]].area ==0 || faceVars[mesh->tets[id].faceIDs[l]].deformed ){		///	�ʐς��v�Z����Ă��Ȃ����i�͂��߁j or deformed(�ό`�������E�������)��true�̎�		///	�����̒ǉ�	�ʐς�0�� ||(OR) �����X�V���ꂽ��
				faceVars[mesh->tets[id].faceIDs[l]].area = CalcTriangleArea(mesh->faces[mesh->tets[id].faceIDs[l]].vertexIDs[0], mesh->faces[mesh->tets[id].faceIDs[l]].vertexIDs[1], mesh->faces[mesh->tets[id].faceIDs[l]].vertexIDs[2]);
				faceVars[mesh->tets[id].faceIDs[l]].deformed = false;
			}
			///	�v�Z���ʂ��s��ɑ��
			///	area�̌v�Z�Ɏg���Ă��Ȃ��_�������Ă���s���������x�N�g���̐ς��Ƃ�
			///	�ϕ��v�Z�����{����l����
			unsigned vtx = mesh->tets[id].vertexIDs[0] + mesh->tets[id].vertexIDs[1] + mesh->tets[id].vertexIDs[2] + mesh->tets[id].vertexIDs[3];			
			///	area�v�Z�Ɏg���Ă��Ȃ��ߓ_ID�FID
			unsigned ID = vtx -( mesh->faces[mesh->tets[id].faceIDs[l]].vertexIDs[0] + mesh->faces[mesh->tets[id].faceIDs[l]].vertexIDs[1] + mesh->faces[mesh->tets[id].faceIDs[l]].vertexIDs[2] );
			for(unsigned j=0;j<4;j++){
				if(mesh->tets[id].vertexIDs[j] == ID){					///	�`��֐����P�A�i���Ȃ킿�j����face�ɑΖʂ��钸�_�@�ƈ�v������@���̎���face�Ŗʐϕ�����
					///	�O�k�ɂȂ����b�V���ʂ̖ʐς�0�ŏ��������Ă���
					///	�ȉ���[]�͏�܂ł�[l]�ƈقȂ�B
					///	ID�����Ԗڂ��ɂ���āA�`��֐��̌W�����قȂ�̂ŁA
					mesh->tets[id].vecf[1] += faceVars[mesh->tets[id].faceIDs[l]].heatflux[1] * (1.0/3.0) * faceVars[mesh->tets[id].faceIDs[l]].area * vecf2array[j];
				}
			}
		}
		///	SurfaceFace����Ȃ�������Amatk2array�ɂ�0������
		//else{
		//	//matk2array[l];
		//}
	}


}
#if 0
void PHFemThermo::CreateVecF3surfaceAll(){
	//	������
	//	��΁A���΁A���΂ɂ��ď�����(�x�N�g���̍s���ݒ�A������)
	vecFAll_f3.resize(GetPHFemMesh()->vertices.size(),1);			//�\�ʂ����łȂ��A�S�ߓ_�ɂ��Čv�Z���Ȃ��ƁA�x�N�g���~�s��̌v�Z���s�����̂��߁B
	vecFAll_f3.clear();
	
	//�l�ʑ̗v�f���Ƃɍs������A�ǂ����ō�������
	//id�����āA�ċA�I�ɍ���Ă���
	for(unsigned id =0; id < GetPHFemMesh()->tets.size();id++){ 
		//�s������
		CreateVecf3(id);//;		// f3surface�ł͂Ȃ�����ǁA�����̂��H	//CreateVecf2surface(id,num);	//	���̊֐����A�����Ɏw�肵���x�N�g���ɓ������悤�ɂ���?
		for(unsigned j =0;j < 4; j++){
			int vtxid0 = GetPHFemMesh()->tets[id].vertexIDs[j];
			//vecFAllSum[vtxid0] = vecf[j];			//�S�̍����x�N�g�����쐬�F�K�E�X�U�C�f���v�Z���ł���Ă��鏈���E������s���B�����܂ł�Vecf2�ł��B
			vecFAll_f3[vtxid0][0] += GetPHFemMesh()->tets[id].vecf[2][j];		//	+= ����Ȃ��Ă������̂��H���l�ɁA���̃\�[�X�ł� += �̕K�v������̂ł́H
		}
	}
	//�������ɁA�K�E�X�U�C�f���v�Z�ŁAVecFAll�ɃZ�b�g����֐������B
	//vecFAllSum�ɉ��Z�Ƃ�����ƁA�ǂ�ǂ񑝂��Ă��܂����A�t�ɁA���́A�ω����Ȃ��v�f{F_3}�ȂǁA�S�̃x�N�g��������āA�ۑ����Ă����K�v
	//�K�E�X�U�C�f���̌v�Z�̒��ŁA����܂ł̌v�Z��F�x�N�g�����g���̂ɑウ�āA�}�C�X�e�b�v�ŁAVecF��F1,F2������K�v������B
}

void PHFemThermo::CreateVecF2surfaceAll(){
	//	������
	//	��΁A���΁A���΂ɂ��ď�����(�x�N�g���̍s���ݒ�A������) initVecFAlls()�Ŏ��s
	//for(unsigned i =0; i < 4 ;i++){
	//	vecFAll_f2IH[i].resize(vertices.size(),1);			//�\�ʂ����łȂ��A�S�ߓ_�ɂ��Čv�Z���Ȃ��ƁA�x�N�g���~�s��̌v�Z���s�����̂��߁B
	//	vecFAll_f2IH[i].clear();
	//}
	
	//�l�ʑ̗v�f���Ƃɍs������A�ǂ����ō�������
	//id�����āA�ċA�I�ɍ���Ă���
	for(unsigned id =0; id < GetPHFemMesh()->tets.size();id++){ 
		//�s������
		for(unsigned num =0; num <4 ; num++){	//�S�Η�(OFF/WEEK/MIDDLE/HIGH)�ɂ���
			CreateVecf2surface(id,num);			//	���̊֐����A�����Ɏw�肵���x�N�g���ɓ������悤�ɂ���?
			//num���ɁA���ꕨ�ɓ����B
			for(unsigned j =0;j < 4; j++){
				int vtxid0 = GetPHFemMesh()->tets[id].vertexIDs[j];
				//vecFAllSum[vtxid0][0] = vecf[j];			//�S�̍����x�N�g�����쐬�F�K�E�X�U�C�f���v�Z���ł���Ă��鏈���E������s���B�����܂ł�Vecf2�ł��B
				//vecFAll_f2IHw[vtxid0][0] = vecf[j];
				vecFAll_f2IH[num][vtxid0][0] += GetPHFemMesh()->tets[id].vecf[1][j];		//f2��[num(�Η�)]	+= ����Ȃ��Ă������̂��H���l�ɁA���̃\�[�X�ł� += �̕K�v������̂ł́H
			}

		}


	
		////�v�f�̐ߓ_�ԍ��̏ꏊ�ɁA���̐ߓ_��f�̒l������
		////j:�v�f�̒��̉��Ԗڂ�
		//for(unsigned j =0;j < 4; j++){
		//	int vtxid0 = tets[id].vertices[j]
		//	vecFAll_f2IHw[vtxid0][0] = vecf[j];tets[id].vecf[1]//��j�v�f
		//}

		//
		////���ꕨ�ɓ����
		////j:�v�f�̒��̉��Ԗڂ�
		//for(unsigned j =0;j < 4; j++){
		//	int vtxid0 = tets[id].vertices[j];
		//	vecFAllSum[vtxid0] = vecf[j];			//�S�̍����x�N�g�����쐬�F�K�E�X�U�C�f���v�Z���ł���Ă��鏈���E������s���B�����܂ł�Vecf2�ł��B
		//	vecFAll_f2IHw[vtxid0][0] = vecf[j];
		//	
		//}
	

	//�������ɁA�K�E�X�U�C�f���v�Z�ŁAvecFAllSum�ɃZ�b�g����֐������B
	//vecFAllSum�ɉ��Z�Ƃ�����ƁA�ǂ�ǂ񑝂��Ă��܂����A�t�ɁA���́A�ω����Ȃ��v�f{F_3}�ȂǁA�S�̃x�N�g��������āA�ۑ����Ă����K�v
	//�K�E�X�U�C�f���̌v�Z�̒��ŁA����܂ł̌v�Z��F�x�N�g�����g���̂ɑウ�āA�}�C�X�e�b�v�ŁAVecF��F1,F2������K�v������B

	}

	//�ȉ��ACreateVecfLocal����R�s�y�@2012.9.25
	//���ׂĂ̗v�f�ɂ��ČW���s������
	//f1�����
	//>	�M�������E����	vecf2�����			


	



}
#endif
void PHFemThermo::CreateVecf2(unsigned id){

	PHFemMeshNew* mesh = GetPHFemMesh();

	//	������
	for(unsigned i =0; i < 4 ;i++){
		//�Ō�ɓ����s���������
		mesh->tets[id].vecf[1][i] =0.0;				//>	f3 = vecf[1] 
	}	
	//l=0�̎�f31,1:f32, 2:f33, 3:f34	�𐶐�
	for(unsigned l= 0 ; l < 4; l++){
		//matk2array[l] = matk2temp;
		vecf2array[l] = Create41Vec1();
		//	l�s��0��
		vecf2array[l][l] = 0.0;

		//array[n][m][l]	= narray[n],m�sl��
		//	f_3	(vecf3array[0], vecf3array[1],..)
		// =	| 0 | + | 1 |+...
		//		| 1 |   | 0 |
		//		| 1 |   | 1 |
		//		| 1 |   | 1 |

		//	for debug
		//DSTR << "vecf3array[" << l << "] : " << std::endl;
		//DSTR << vecf3array[l] << std::endl;

		//�W���̐ς��Ƃ�
		//���̐ߓ_�ō\�������l�ʑ̖̂ʐς̐ς��Ƃ�
		//�l�ʑ̂̐ߓ_1,2,3(0�ȊO)�ō��O�p�`�̖ʐ�
		//l==0�Ԗڂ̎��A 123	��������
		//l==1			0 23
		//l==2			01 3
		//l==3			012
		//��CalcTriangleArea�ɓ���邱�Ƃ��ł���悤�ɃA���S���Y�����l����B

		//>	CreateMatk2t�̂悤�ȃA���S���Y���ɕύX�\��
		//k21
		if(l==0){
			//>	�O�p�`�ʂ��\������3���_�̔M�����̑�������
			double tempHF = (vertexVars[mesh->tets[id].vertexIDs[1]].heatFluxValue + vertexVars[mesh->tets[id].vertexIDs[2]].heatFluxValue + vertexVars[mesh->tets[id].vertexIDs[3]].heatFluxValue ) / 3.0;		//HTR:HeatTransRatio
			vecf2array[l] = tempHF * (1.0/3.0) * CalcTriangleArea( mesh->tets[id].vertexIDs[1],mesh->tets[id].vertexIDs[2],mesh->tets[id].vertexIDs[3] ) * vecf2array[l];
			//DSTR << "vecf2array[" << l << "] : " << vecf2array[l] << std::endl;
			//Vec�̐ߓ_���ɂ��̐ߓ_�ł̎��͗��̉��xTc�Ƃ̐ς��s��
			
			////>	�s�v�H
			//for(unsigned m=0; m<4; m++){
			//	vecf2array[l][m] = vertices[tets[id].vertices[m]].Tc * vecf2array[l][m];
			//}
		}
		//	k22
		else if(l==1){
			double tempHF = (vertexVars[mesh->tets[id].vertexIDs[0]].heatFluxValue + vertexVars[mesh->tets[id].vertexIDs[2]].heatFluxValue + vertexVars[mesh->tets[id].vertexIDs[3]].heatFluxValue ) / 3.0;		//HTR:HeatTransRatio
			vecf2array[l] = tempHF * (1.0/3.0) * CalcTriangleArea( mesh->tets[id].vertexIDs[0],mesh->tets[id].vertexIDs[2],mesh->tets[id].vertexIDs[3] ) * vecf2array[l];
			//vecf3array[l] = heatTrans * (1.0/3.0) * CalcTriangleArea( tets[id].vertices[0],tets[id].vertices[2],tets[id].vertices[3] ) * vecf3array[l];
			//DSTR << "vecf3array[" << l << "] : " << vecf3array[l] << std::endl;
			//Vec�̐ߓ_���ɂ��̐ߓ_�ł̎��͗��̉��xTc�Ƃ̐ς��s��
			//for(unsigned m=0; m<4; m++){
			//	vecf2array[l][m] = vertices[tets[id].vertices[m]].Tc * vecf2array[l][m];
			//}
		}
		//	k23
		else if(l==2){
			double tempHF = (vertexVars[mesh->tets[id].vertexIDs[0]].heatFluxValue + vertexVars[mesh->tets[id].vertexIDs[1]].heatFluxValue + vertexVars[mesh->tets[id].vertexIDs[3]].heatFluxValue ) / 3.0;		//HTR:HeatTransRatio
			vecf2array[l] = tempHF * (1.0/3.0) * CalcTriangleArea( mesh->tets[id].vertexIDs[0],mesh->tets[id].vertexIDs[1],mesh->tets[id].vertexIDs[3] ) * vecf2array[l];
			//vecf3array[l] = heatTrans * (1.0/3.0) * CalcTriangleArea( tets[id].vertices[0],tets[id].vertices[1],tets[id].vertices[3] ) * vecf3array[l];
			//DSTR << "vecf3array[" << l << "] : " << vecf3array[l] << std::endl;
			//Vec�̐ߓ_���ɂ��̐ߓ_�ł̎��͗��̉��xTc�Ƃ̐ς��s��
			//for(unsigned m=0; m<4; m++){
			//	vecf2array[l][m] = vertices[tets[id].vertices[m]].Tc * vecf2array[l][m];
			//}
		}
		//	k24
		else if(l==3){
			double tempHF = (vertexVars[mesh->tets[id].vertexIDs[0]].heatFluxValue + vertexVars[mesh->tets[id].vertexIDs[1]].heatFluxValue + vertexVars[mesh->tets[id].vertexIDs[2]].heatFluxValue ) / 3.0;		//HTR:HeatTransRatio
			vecf2array[l] = tempHF * (1.0/3.0) * CalcTriangleArea( mesh->tets[id].vertexIDs[0],mesh->tets[id].vertexIDs[1],mesh->tets[id].vertexIDs[2] ) * vecf2array[l];
			//vecf3array[l] = heatTrans * (1.0/3.0) * CalcTriangleArea( tets[id].vertices[0],tets[id].vertices[1],tets[id].vertices[2] ) * vecf3array[l];
			//DSTR << "vecf3array[" << l << "] : " << vecf3array[l] << std::endl;
			//Vec�̐ߓ_���ɂ��̐ߓ_�ł̎��͗��̉��xTc�Ƃ̐ς��s��
			//for(unsigned m=0; m<4; m++){
			//	vecf2array[l][m] = vertices[tets[id].vertices[m]].Tc * vecf2array[l][m];
			//}
		}
		//for debug
		//DSTR << "vecf3array[" << l << "]�̊����ł́�" << std::endl;
		//DSTR << vecf3array[l] << std::endl;
		//if(dMatCAll == NULL){
		//	//DSTR <<"i : "<< i << ", l : " << l << std::endl;
		//	DSTR << "dMatCAll == NULL" <<std::endl;
		//	DSTR <<"l : " << l << std::endl;
		//}
	}

	//f3 = f31 + f32 + f33 + f34
	for(unsigned i=0; i < 4; i++){
		//vecf3 += vecf3array[i];
		mesh->tets[id].vecf[1] += vecf2array[i];
		//	for debug
		//DSTR << "vecf3 �� vecf3array = f3" << i+1 <<"�܂ŉ��Z�����s��" << std::endl;
		//DSTR << vecf3 << std::endl;
	}
	
	//	f1,f2,f3,f4	���v�Z����ۂɁA[0][0]��������[3][0]�����܂ł̔�0�����ɂ��āA���Tc�������Ă��܂�


	//for debug
	//DSTR << "�ߓ_�i";
	//for(unsigned i =0; i < 4; i++){
	//	DSTR << tets[id].vertices[i] << "," ;
	//}
	//DSTR << ")�ō\�������l�ʑ̂�" << std::endl;
	//DSTR << "vecf3 : " << std::endl;
	//DSTR << vecf3 << std::endl;
	//int hogeshishi =0;
}

void PHFemThermo::CreateVecf3_(unsigned id){

	PHFemMeshNew* mesh = GetPHFemMesh();

	//	������
	for(unsigned i =0; i < 4 ;i++){
		//vecf3[i] = 0.0;		//�Ō�ɓ����s���������
		mesh->tets[id].vecf[2][i] =0.0;
	}	
	//l=0�̎�f31,1:f32, 2:f33, 3:f34	�𐶐�
	for(unsigned l= 0 ; l < 4; l++){
		//matk2array[l] = matk2temp;
		vecf3array[l] = Create41Vec1();
		//	l�s��0��
		//for(int i=0;i<4;i++){
		//	vecf3array[l][l][i] = 0.0;
		//}
		vecf3array[l][l] = 0.0;
		//array[n][m][l]	= narray[n],m�sl��
		//	f_3	(vecf3array[0], vecf3array[1],..)
		// =	| 0 | + | 1 |+...
		//		| 1 |   | 0 |
		//		| 1 |   | 1 |
		//		| 1 |   | 1 |
		//	for debug
		//DSTR << "vecf3array[" << l << "] : " << std::endl;
		//DSTR << vecf3array[l] << std::endl;

		//�W���̐ς��Ƃ�
		//���̐ߓ_�ō\�������l�ʑ̖̂ʐς̐ς��Ƃ�
		//�l�ʑ̂̐ߓ_1,2,3(0�ȊO)�ō��O�p�`�̖ʐ�
		//l==0�Ԗڂ̎��A 123	��������
		//l==1			0 23
		//l==2			01 3
		//l==3			012
		//��CalcTriangleArea�ɓ���邱�Ƃ��ł���悤�ɃA���S���Y�����l����B
		//k21
		if(l==0){
			//�O�p�`�ʂ��\������3���_�̔M�`�B���̑�������
			double tempHTR = (vertexVars[mesh->tets[id].vertexIDs[1]].heatTransRatio + vertexVars[mesh->tets[id].vertexIDs[2]].heatTransRatio + vertexVars[mesh->tets[id].vertexIDs[3]].heatTransRatio ) / 3.0;		//HTR:HeatTransRatio
			double avgTc = (vertexVars[mesh->tets[id].vertexIDs[1]].Tc + vertexVars[mesh->tets[id].vertexIDs[2]].Tc + vertexVars[mesh->tets[id].vertexIDs[3]].Tc ) / 3.0;
			vecf3array[l] = tempHTR * avgTc * (1.0/3.0) * CalcTriangleArea( mesh->tets[id].vertexIDs[1],mesh->tets[id].vertexIDs[2],mesh->tets[id].vertexIDs[3] ) * vecf3array[l];
			//DSTR << "vecf3array[" << l << "] : " << vecf3array[l] << std::endl;
			
			//>	���͖{���H
			//Vec�̐ߓ_���ɂ��̐ߓ_�ł̎��͗��̉��xTc�Ƃ̐ς��s��
			//for(unsigned m=0; m<4; m++){
			//	vecf3array[l][m] = vertices[tets[id].vertices[m]].Tc * vecf3array[l][m];
			//}
		}
		//	k22
		else if(l==1){
			double tempHTR = (vertexVars[mesh->tets[id].vertexIDs[0]].heatTransRatio + vertexVars[mesh->tets[id].vertexIDs[2]].heatTransRatio + vertexVars[mesh->tets[id].vertexIDs[3]].heatTransRatio ) / 3.0;		//HTR:HeatTransRatio
			double avgTc = (vertexVars[mesh->tets[id].vertexIDs[0]].Tc + vertexVars[mesh->tets[id].vertexIDs[2]].Tc + vertexVars[mesh->tets[id].vertexIDs[3]].Tc ) / 3.0;		//HTR:HeatTransRatio
			vecf3array[l] = tempHTR * avgTc * (1.0/3.0) * CalcTriangleArea( mesh->tets[id].vertexIDs[0],mesh->tets[id].vertexIDs[2],mesh->tets[id].vertexIDs[3] ) * vecf3array[l];
			//DSTR << "vecf3array[" << l << "] : " << vecf3array[l] << std::endl;
			//Vec�̐ߓ_���ɂ��̐ߓ_�ł̎��͗��̉��xTc�Ƃ̐ς��s��
			//for(unsigned m=0; m<4; m++){
			//	vecf3array[l][m] = vertices[tets[id].vertices[m]].Tc * vecf3array[l][m];
			//}
		}
		//	k23
		else if(l==2){
			double tempHTR = (vertexVars[mesh->tets[id].vertexIDs[0]].heatTransRatio + vertexVars[mesh->tets[id].vertexIDs[1]].heatTransRatio + vertexVars[mesh->tets[id].vertexIDs[3]].heatTransRatio ) / 3.0;		//HTR:HeatTransRatio
			double avgTc = (vertexVars[mesh->tets[id].vertexIDs[0]].Tc + vertexVars[mesh->tets[id].vertexIDs[1]].Tc + vertexVars[mesh->tets[id].vertexIDs[3]].Tc ) / 3.0;		//HTR:HeatTransRatio
			vecf3array[l] = tempHTR * avgTc * (1.0/3.0) * CalcTriangleArea( mesh->tets[id].vertexIDs[0],mesh->tets[id].vertexIDs[1],mesh->tets[id].vertexIDs[3] ) * vecf3array[l];
			//vecf3array[l] = heatTrans * (1.0/3.0) * CalcTriangleArea( tets[id].vertices[0],tets[id].vertices[1],tets[id].vertices[3] ) * vecf3array[l];
			//DSTR << "vecf3array[" << l << "] : " << vecf3array[l] << std::endl;
			//Vec�̐ߓ_���ɂ��̐ߓ_�ł̎��͗��̉��xTc�Ƃ̐ς��s��
			//for(unsigned m=0; m<4; m++){
			//	vecf3array[l][m] = vertices[tets[id].vertices[m]].Tc * vecf3array[l][m];
			//}
		}
		//	k24
		else if(l==3){
			double tempHTR = (vertexVars[mesh->tets[id].vertexIDs[0]].heatTransRatio + vertexVars[mesh->tets[id].vertexIDs[1]].heatTransRatio + vertexVars[mesh->tets[id].vertexIDs[2]].heatTransRatio ) / 3.0;		//HTR:HeatTransRatio
			double avgTc = (vertexVars[mesh->tets[id].vertexIDs[0]].Tc + vertexVars[mesh->tets[id].vertexIDs[1]].Tc + vertexVars[mesh->tets[id].vertexIDs[2]].Tc ) / 3.0;		//HTR:HeatTransRatio
			vecf3array[l] = tempHTR * avgTc * (1.0/3.0) * CalcTriangleArea( mesh->tets[id].vertexIDs[0],mesh->tets[id].vertexIDs[1],mesh->tets[id].vertexIDs[2] ) * vecf3array[l];
			//DSTR << "vecf3array[" << l << "] : " << vecf3array[l] << std::endl;
			//Vec�̐ߓ_���ɂ��̐ߓ_�ł̎��͗��̉��xTc�Ƃ̐ς��s��
			//for(unsigned m=0; m<4; m++){
			//	vecf3array[l][m] = vertices[tets[id].vertices[m]].Tc * vecf3array[l][m];
			//}
		}
		//for debug
		//DSTR << "vecf3array[" << l << "]�̊����ł́�" << std::endl;
		//DSTR << vecf3array[l] << std::endl;
		//if(dMatCAll == NULL){
		//	//DSTR <<"i : "<< i << ", l : " << l << std::endl;
		//	DSTR << "dMatCAll == NULL" <<std::endl;
		//	DSTR <<"l : " << l << std::endl;
		//}
	}

	//f3 = f31 + f32 + f33 + f34
	for(unsigned i=0; i < 4; i++){
		//vecf3 += vecf3array[i];
		mesh->tets[id].vecf[2] += vecf3array[i];
		//	for debug
		//DSTR << "vecf3 �� vecf3array = f3" << i+1 <<"�܂ŉ��Z�����s��" << std::endl;
		//DSTR << vecf3 << std::endl;
	}
	
	//	f1,f2,f3,f4	���v�Z����ۂɁA[0][0]��������[3][0]�����܂ł̔�0�����ɂ��āA���Tc�������Ă��܂�


	//for debug
	//DSTR << "�ߓ_�i";
	//for(unsigned i =0; i < 4; i++){
	//	DSTR << tets[id].vertices[i] << "," ;
	//}
	//DSTR << ")�ō\�������l�ʑ̂�" << std::endl;
	//DSTR << "vecf3 : " << std::endl;
	//DSTR << vecf3 << std::endl;
	//int hogeshishi =0;
}

void PHFemThermo::CreateVecf3(unsigned id){

	PHFemMeshNew* mesh = GetPHFemMesh();

	//	������
	for(unsigned i =0; i < 4 ;i++){
		//vecf3[i] = 0.0;		//�Ō�ɓ����s���������
		mesh->tets[id].vecf[2][i] =0.0;
	}	
	//l=0�̎�f31,1:f32, 2:f33, 3:f34	�𐶐�
	for(unsigned l= 0 ; l < 4; l++){
		//matk2array[l] = matk2temp;
		vecf3array[l] = Create41Vec1();
		//	l�s��0��
		//for(int i=0;i<4;i++){
		//	vecf3array[l][l][i] = 0.0;
		//}
		vecf3array[l][l] = 0.0;
		//array[n][m][l]	= narray[n],m�sl��
		//	f_3	(vecf3array[0], vecf3array[1],..)
		// =	| 0 | + | 1 |+...
		//		| 1 |   | 0 |
		//		| 1 |   | 1 |
		//		| 1 |   | 1 |
		//	for debug
		//DSTR << "vecf3array[" << l << "] : " << std::endl;
		//DSTR << vecf3array[l] << std::endl;

		//�W���̐ς��Ƃ�
		//���̐ߓ_�ō\�������l�ʑ̖̂ʐς̐ς��Ƃ�
		//�l�ʑ̂̐ߓ_1,2,3(0�ȊO)�ō��O�p�`�̖ʐ�
		//l==0�Ԗڂ̎��A 123	��������
		//l==1			0 23
		//l==2			01 3
		//l==3			012
		//��CalcTriangleArea�ɓ���邱�Ƃ��ł���悤�ɃA���S���Y�����l����B
		//k21
		if(l==0){
			//�O�p�`�ʂ��\������3���_�̔M�`�B���̑�������
			double tempHTR = (vertexVars[mesh->tets[id].vertexIDs[1]].heatTransRatio + vertexVars[mesh->tets[id].vertexIDs[2]].heatTransRatio + vertexVars[mesh->tets[id].vertexIDs[3]].heatTransRatio ) / 3.0;		//HTR:HeatTransRatio
			vecf3array[l] = tempHTR * (1.0/3.0) * CalcTriangleArea( mesh->tets[id].vertexIDs[1],mesh->tets[id].vertexIDs[2],mesh->tets[id].vertexIDs[3] ) * vecf3array[l];
			//vecf3array[l] = heatTrans * (1.0/3.0) * CalcTriangleArea( tets.vertices[1],tets.vertices[2],tets.vertices[3] ) * vecf3array[l];
			//DSTR << "vecf3array[" << l << "] : " << vecf3array[l] << std::endl;
			//Vec�̐ߓ_���ɂ��̐ߓ_�ł̎��͗��̉��xTc�Ƃ̐ς��s��
			for(unsigned m=0; m<4; m++){
				vecf3array[l][m] = vertexVars[mesh->tets[id].vertexIDs[m]].Tc * vecf3array[l][m];
			}
		}
		//	k22
		else if(l==1){
			double tempHTR = (vertexVars[mesh->tets[id].vertexIDs[0]].heatTransRatio + vertexVars[mesh->tets[id].vertexIDs[2]].heatTransRatio + vertexVars[mesh->tets[id].vertexIDs[3]].heatTransRatio ) / 3.0;		//HTR:HeatTransRatio
			vecf3array[l] = tempHTR * (1.0/3.0) * CalcTriangleArea( mesh->tets[id].vertexIDs[0],mesh->tets[id].vertexIDs[2],mesh->tets[id].vertexIDs[3] ) * vecf3array[l];
			//vecf3array[l] = heatTrans * (1.0/3.0) * CalcTriangleArea( tets[id].vertices[0],tets[id].vertices[2],tets[id].vertices[3] ) * vecf3array[l];
			//DSTR << "vecf3array[" << l << "] : " << vecf3array[l] << std::endl;
			//Vec�̐ߓ_���ɂ��̐ߓ_�ł̎��͗��̉��xTc�Ƃ̐ς��s��
			for(unsigned m=0; m<4; m++){
				vecf3array[l][m] = vertexVars[mesh->tets[id].vertexIDs[m]].Tc * vecf3array[l][m];
			}
		}
		//	k23
		else if(l==2){
			double tempHTR = (vertexVars[mesh->tets[id].vertexIDs[0]].heatTransRatio + vertexVars[mesh->tets[id].vertexIDs[1]].heatTransRatio + vertexVars[mesh->tets[id].vertexIDs[3]].heatTransRatio ) / 3.0;		//HTR:HeatTransRatio
			vecf3array[l] = tempHTR * (1.0/3.0) * CalcTriangleArea( mesh->tets[id].vertexIDs[0],mesh->tets[id].vertexIDs[1],mesh->tets[id].vertexIDs[3] ) * vecf3array[l];
			//vecf3array[l] = heatTrans * (1.0/3.0) * CalcTriangleArea( tets[id].vertices[0],tets[id].vertices[1],tets[id].vertices[3] ) * vecf3array[l];
			//DSTR << "vecf3array[" << l << "] : " << vecf3array[l] << std::endl;
			//Vec�̐ߓ_���ɂ��̐ߓ_�ł̎��͗��̉��xTc�Ƃ̐ς��s��
			for(unsigned m=0; m<4; m++){
				vecf3array[l][m] = vertexVars[mesh->tets[id].vertexIDs[m]].Tc * vecf3array[l][m];
			}
		}
		//	k24
		else if(l==3){
			double tempHTR = (vertexVars[mesh->tets[id].vertexIDs[0]].heatTransRatio + vertexVars[mesh->tets[id].vertexIDs[1]].heatTransRatio + vertexVars[mesh->tets[id].vertexIDs[2]].heatTransRatio ) / 3.0;		//HTR:HeatTransRatio
			vecf3array[l] = tempHTR * (1.0/3.0) * CalcTriangleArea( mesh->tets[id].vertexIDs[0],mesh->tets[id].vertexIDs[1],mesh->tets[id].vertexIDs[2] ) * vecf3array[l];
			//vecf3array[l] = heatTrans * (1.0/3.0) * CalcTriangleArea( tets[id].vertices[0],tets[id].vertices[1],tets[id].vertices[2] ) * vecf3array[l];
			//DSTR << "vecf3array[" << l << "] : " << vecf3array[l] << std::endl;
			//Vec�̐ߓ_���ɂ��̐ߓ_�ł̎��͗��̉��xTc�Ƃ̐ς��s��
			for(unsigned m=0; m<4; m++){
				vecf3array[l][m] = vertexVars[mesh->tets[id].vertexIDs[m]].Tc * vecf3array[l][m];
			}
		}
		//for debug
		//DSTR << "vecf3array[" << l << "]�̊����ł́�" << std::endl;
		//DSTR << vecf3array[l] << std::endl;
		//if(dMatCAll == NULL){
		//	//DSTR <<"i : "<< i << ", l : " << l << std::endl;
		//	DSTR << "dMatCAll == NULL" <<std::endl;
		//	DSTR <<"l : " << l << std::endl;
		//}
	}

	//f3 = f31 + f32 + f33 + f34
	for(unsigned i=0; i < 4; i++){
		//vecf3 += vecf3array[i];
		mesh->tets[id].vecf[2] += vecf3array[i];
		//	for debug
		//DSTR << "vecf3 �� vecf3array = f3" << i+1 <<"�܂ŉ��Z�����s��" << std::endl;
		//DSTR << vecf3 << std::endl;
	}
	
	//	f1,f2,f3,f4	���v�Z����ۂɁA[0][0]��������[3][0]�����܂ł̔�0�����ɂ��āA���Tc�������Ă��܂�


	//for debug
	//DSTR << "�ߓ_�i";
	//for(unsigned i =0; i < 4; i++){
	//	DSTR << tets[id].vertices[i] << "," ;
	//}
	//DSTR << ")�ō\�������l�ʑ̂�" << std::endl;
	//DSTR << "vecf3 : " << std::endl;
	//DSTR << vecf3 << std::endl;
	//int hogeshishi =0;
}

double PHFemThermo::CalcTetrahedraVolume(FemTet tets){

	PHFemMeshNew* mesh = GetPHFemMesh();

	PTM::TMatrixRow<4,4,double> tempMat44;
	for(unsigned i =0; i < 4; i++){
		for(unsigned j =0; j < 4; j++){
			if(i == 0){
				tempMat44[i][j] = 1.0;
			}
			else if(i == 1){
				tempMat44[i][j] = mesh->vertices[tets.vertexIDs[j]].pos.x;
			}
			else if(i == 2){
				tempMat44[i][j] = mesh->vertices[tets.vertexIDs[j]].pos.y;
			}
			else if(i == 3){
				tempMat44[i][j] = mesh->vertices[tets.vertexIDs[j]].pos.z;
			}
			
		}
	}
	//	for debug
	//DSTR << tempMat44 << std::endl;
	//for(unsigned i =0; i < 4 ;i++){
	//	DSTR << vertices[tets.vertices[i]].pos.x << " , " << vertices[tets.vertices[i]].pos.y << " , " << vertices[tets.vertices[i]].pos.z << std::endl; 
	//}
	//DSTR << tempMat44.det() << std::endl;
	//int hogever = 0;
	return tempMat44.det() / 6.0;
}

double PHFemThermo::CalcTriangleArea(int id0, int id1, int id2){

	PHFemMeshNew* mesh = GetPHFemMesh();

	double area=0.0;								///	�v���P	faces[id].area�ɒl������ 

	//�s�񎮂̐�����p���Ėʐς����߂�
	//triarea =
	//|  1     1     1   |
	//|x2-x1 y2-y1 z2-z1 |
	//|x3-x1 y3-y1 z3-z1 |
	//|
	PTM::TMatrixRow<3,3,double> triarea;		//�O�p�`�̖ʐρ@= tri + area
	for(unsigned i =0 ; i < 3 ; i++){
		triarea[0][i] = 1.0;
	}
	for(unsigned i =0 ; i < 3 ; i++){
		//					x2(�ߓ_2��x(pos��i����)��)	-	x1(�V)
		// i==0�̎�	vertices[id1].pos[i]	=>	 pos[0] == pos.x
		triarea[1][i] = mesh->vertices[id1].pos[i] - mesh->vertices[id0].pos[i];
	}
	for(unsigned i =0 ; i < 3 ; i++){
		//					x3(�ߓ_3��x(pos��i����)��)	-	x1(�V)
		triarea[2][i] = mesh->vertices[id2].pos[i] - mesh->vertices[id0].pos[i];
	}
	double m1,m2,m3 = 0.0;
	m1 = triarea[1][1] * triarea[2][2] - triarea[1][2] * triarea[2][1];
	m2 = triarea[2][0] * triarea[1][2] - triarea[1][0] * triarea[2][2];
	m3 = triarea[1][0] * triarea[2][1] - triarea[2][0] * triarea[1][1];

	area = sqrt(m1 * m1 + m2 * m2 + m3 * m3) / 2.0;

	//	for debug
	//DSTR << "�O�p�`�̖ʐς� : " << area << std::endl; 

	//0�Ԗڂ̐ߓ_��40,1�Ԗڂ̐ߓ_��134,2�Ԗڂ̐ߓ_��79 �̍��W�Ōv�Z���Ă݂�
	//�O�p�`�����߂�s�� : 2.75949e-005 * 1 = 2.75949 �~ 10-5(byGoogle�v�Z�@) [m^2] = 2.75949 �~ 10-1 [cm^2]�Ȃ̂ŁA�l�M�̃��b�V���̃X�P�[���Ȃ��̂����Ă���͂�

	return area;
}

PTM::TMatrixCol<4,1,double> PHFemThermo::Create41Vec1(){
	PTM::TMatrixCol<4,1,double> Mat1temp;
	for(int i =0; i <4 ; i++){
				Mat1temp[i][0] = 1.0;
	}
	return Mat1temp;
}

PTM::TMatrixRow<4,4,double> PHFemThermo::Create44Mat21(){
	//|2 1 1 1 |
	//|1 2 1 1 |
	//|1 1 2 1 |
	//|1 1 1 2 |	�����
	PTM::TMatrixRow<4,4,double> MatTemp;
	for(int i =0; i <4 ; i++){
		for(int j=0; j < 4 ; j++){
			if(i==j){
				MatTemp[i][j] = 2.0;
			}else{
				MatTemp[i][j] = 1.0;
			}
		}
	}
	return MatTemp;
}
void PHFemThermo::CreateMatk3t(unsigned id){

	PHFemMeshNew* mesh = GetPHFemMesh();

	//l=0�̎�k21,1�̎�:k22, 2�̎�:k23, 3�̎�:k24	�𐶐�
	for(unsigned l= 0 ; l < 4; l++){
		//matk2array[l] = matk2temp;
		matk3array[l] = Create44Mat21();
		//	1�si���0��
		for(int i=0;i<4;i++){
			matk3array[l][l][i] = 0.0;
		}
		//	i�s1���0��
		for(int i=0;i<4;i++){
			matk3array[l][i][l] = 0.0;
		}
	}

	///	������
	tetVars[id].matk[2].clear();
	
	for(unsigned l= 0 ; l < 4; l++){
		///	�l�ʑ̂̊e��(l = 0 �` 3) �ɂ��ă��b�V���\�ʂ��ǂ������`�F�b�N����B�\�ʂȂ�A�s��������matk2array�ɓ����
		//faces[tets.faces[i]].sorted;		/// 1,24,58�݂����Ȑߓ_�ԍ��������Ă���
		///	�s��^�̓��ꕨ��p��

		//faces[tets.faces[l]].vertices;
		if(mesh->tets[id].faceIDs[l] < (int)mesh->nSurfaceFace && faceVars[mesh->tets[id].faceIDs[l]].alphaUpdated ){			///	�O�k�̖� ���� �M�`�B�����X�V���ꂽ�� matk2���X�V����K�v������
			//�Ō�ɓ����s���������
			for(unsigned i =0; i < 4 ;i++){
				for(unsigned j =0; j < 4 ;j++){
					tetVars[id].matk[2][i][j] = 0.0;
				}
			}
			///	�l�ʑ̂̎O�p�`�̖ʐς��v�Z		///	���̊֐��̊O�Ŗʐϕ��̖ʐόv�Z����������B�ړ�����
			if(faceVars[mesh->tets[id].faceIDs[l]].area ==0 || faceVars[mesh->tets[id].faceIDs[l]].deformed ){		///	�ʐς��v�Z����Ă��Ȃ����i�͂��߁j or deformed(�ό`�������E�������)��true�̎�		///	�����̒ǉ�	�ʐς�0�� ||(OR) �����X�V���ꂽ��
				faceVars[mesh->tets[id].faceIDs[l]].area = CalcTriangleArea(mesh->faces[mesh->tets[id].faceIDs[l]].vertexIDs[0], mesh->faces[mesh->tets[id].faceIDs[l]].vertexIDs[1], mesh->faces[mesh->tets[id].faceIDs[l]].vertexIDs[2]);
				faceVars[mesh->tets[id].faceIDs[l]].deformed = false;
			}
			///	�v�Z���ʂ��s��ɑ��
			///	area�̌v�Z�Ɏg���Ă��Ȃ��_�������Ă���s�Ɨ���������s��̐ς��Ƃ�
			///	�ϕ��v�Z�����{����l����
			unsigned vtx = mesh->tets[id].vertexIDs[0] + mesh->tets[id].vertexIDs[1] + mesh->tets[id].vertexIDs[2] + mesh->tets[id].vertexIDs[3];
			//DSTR << "vtx: " << vtx <<std::endl;
				///	area�v�Z�Ɏg���Ă��Ȃ��ߓ_ID�FID
			unsigned ID = vtx -( mesh->faces[mesh->tets[id].faceIDs[l]].vertexIDs[0] + mesh->faces[mesh->tets[id].faceIDs[l]].vertexIDs[1] + mesh->faces[mesh->tets[id].faceIDs[l]].vertexIDs[2] );
			//DSTR << "���b�V���\�ʂ̖ʂ͎���3���_����Ȃ�B" << std::endl;
			//DSTR << "faces[tets.faces[l]].vertices[0]: " << faces[tets.faces[l]].vertices[0] <<std::endl;
			//DSTR << "faces[tets.faces[l]].vertices[1]: " << faces[tets.faces[l]].vertices[1] <<std::endl;
			//DSTR << "faces[tets.faces[l]].vertices[2]: " << faces[tets.faces[l]].vertices[2] <<std::endl;
			//DSTR << "ID: " << ID <<"�̂Ƃ��̐ߓ_�ƑΖʂ���ʂŖʐϕ����v�Z����"<<std::endl;
			for(unsigned j=0;j<4;j++){
				if(mesh->tets[id].vertexIDs[j] == ID){					///	�`��֐����P�A�i���Ȃ킿�j����face�ɑΖʂ��钸�_�@�ƈ�v������@���̎���face�Ŗʐϕ�����
					///	j�Ԗڂ̍s��̐�����0�ɂ���matk2array�Ōv�Z����
					///	�O�k�ɂȂ����b�V���ʂ̖ʐς�0�ŏ��������Ă���
					faceVars[mesh->tets[id].faceIDs[l]].thermalEmissivity = (vertexVars[mesh->faces[mesh->tets[id].faceIDs[l]].vertexIDs[0]].thermalEmissivity + vertexVars[mesh->faces[mesh->tets[id].faceIDs[l]].vertexIDs[1]].thermalEmissivity 
						+ vertexVars[mesh->faces[mesh->tets[id].faceIDs[l]].vertexIDs[2]].thermalEmissivity ) / 3.0;		///	���Yface�̔M�`�B�����\���ߓ_�ł̒l�̑������ς��Ƃ�
					///	�M�t�˗����v�Z�A�M�t�˗����A�ߓ_�̔M�t�˗�����v�Z�H

					///	�ȉ���[]�͏�܂ł�[l]�ƈقȂ�B
					///	ID�����Ԗڂ��ɂ���āA�`��֐��̌W�����قȂ�̂ŁA
					tetVars[id].matk[2] += faceVars[mesh->tets[id].faceIDs[l]].thermalEmissivity * (1.0/12.0) * faceVars[mesh->tets[id].faceIDs[l]].area * matk2array[j];
					//DSTR << "tets[id].matk2��faces[tets[id].faces[l]].heatTransRatio * (1.0/12.0) * faces[tets[id].faces[l]].area * matk2array[" << j << "]"<< "�����Z: " <<faces[tets[id].faces[l]].heatTransRatio * (1.0/12.0) * faces[tets[id].faces[l]].area * matk2array[j] << std::endl;
					//DSTR << "tets[id].matk2 +=  " << tets[id].matk2 << std::endl;
				}
			}
		}
	}
}
void PHFemThermo::CreateMatk2t(unsigned id){

	PHFemMeshNew* mesh = GetPHFemMesh();

	//l=0�̎�k21,1�̎�:k22, 2�̎�:k23, 3�̎�:k24	�𐶐�
	for(unsigned l= 0 ; l < 4; l++){
		//matk2array[l] = matk2temp;
		matk2array[l] = Create44Mat21();
		//	1�si���0��
		for(int i=0;i<4;i++){
			matk2array[l][l][i] = 0.0;
		}
		//	i�s1���0��
		for(int i=0;i<4;i++){
			matk2array[l][i][l] = 0.0;
		}
	}

	///	������
	tetVars[id].matk[1].clear();
	//for(unsigned i =0; i < 4 ;i++){
	//	for(unsigned j =0; j < 4 ;j++){
	//		//tets[id].matk2[i][j] = 0.0;
	//		tets[id].matk[1][i][j] = 0.0;
	//	}
	//}

	//	Check
	//DSTR << "matk2array:" << std::endl;
	//for(unsigned i=0;i<4;i++){
	//	DSTR <<i <<": " << matk2array[i] << std::endl;
	//}
	//DSTR << "++i" <<std::endl;
	//for(unsigned i=0;i<4;++i){
	//	DSTR <<i <<": " << matk2array[i] << std::endl;
	//}


	for(unsigned l= 0 ; l < 4; l++){
		///	�l�ʑ̂̊e��(l = 0 �` 3) �ɂ��ă��b�V���\�ʂ��ǂ������`�F�b�N����B�\�ʂȂ�A�s��������matk2array�ɓ����
		//faces[tets.faces[i]].sorted;		/// 1,24,58�݂����Ȑߓ_�ԍ��������Ă���
		///	�s��^�̓��ꕨ��p��

		//faces[tets.faces[l]].vertices;
		if(mesh->tets[id].faceIDs[l] < (int)mesh->nSurfaceFace && faceVars[mesh->tets[id].faceIDs[l]].alphaUpdated ){			///	�O�k�̖� ���� �M�`�B�����X�V���ꂽ�� matk2���X�V����K�v������
			//�Ō�ɓ����s���������
			for(unsigned i =0; i < 4 ;i++){
				for(unsigned j =0; j < 4 ;j++){
					//matk2[i][j] = 0.0;
					//tets[id].matk2[i][j] = 0.0;
					tetVars[id].matk[1][i][j] = 0.0;
				}
			}
			///	�l�ʑ̂̎O�p�`�̖ʐς��v�Z		///	���̊֐��̊O�Ŗʐϕ��̖ʐόv�Z����������B�ړ�����
			if(faceVars[mesh->tets[id].faceIDs[l]].area ==0 || faceVars[mesh->tets[id].faceIDs[l]].deformed ){		///	�ʐς��v�Z����Ă��Ȃ����i�͂��߁j or deformed(�ό`�������E�������)��true�̎�		///	�����̒ǉ�	�ʐς�0�� ||(OR) �����X�V���ꂽ��
				faceVars[mesh->tets[id].faceIDs[l]].area = CalcTriangleArea(mesh->faces[mesh->tets[id].faceIDs[l]].vertexIDs[0], mesh->faces[mesh->tets[id].faceIDs[l]].vertexIDs[1], mesh->faces[mesh->tets[id].faceIDs[l]].vertexIDs[2]);
				faceVars[mesh->tets[id].faceIDs[l]].deformed = false;
			}
			///	�v�Z���ʂ��s��ɑ��
			///	area�̌v�Z�Ɏg���Ă��Ȃ��_�������Ă���s�Ɨ���������s��̐ς��Ƃ�
			///	�ϕ��v�Z�����{����l����
			unsigned vtx = mesh->tets[id].vertexIDs[0] + mesh->tets[id].vertexIDs[1] + mesh->tets[id].vertexIDs[2] + mesh->tets[id].vertexIDs[3];
			//DSTR << "vtx: " << vtx <<std::endl;
			
			///	area�v�Z�Ɏg���Ă��Ȃ��ߓ_ID�FID
			unsigned ID = vtx -( mesh->faces[mesh->tets[id].faceIDs[l]].vertexIDs[0] + mesh->faces[mesh->tets[id].faceIDs[l]].vertexIDs[1] + mesh->faces[mesh->tets[id].faceIDs[l]].vertexIDs[2] );
			//DSTR << "���b�V���\�ʂ̖ʂ͎���3���_����Ȃ�B" << std::endl;
			//DSTR << "faces[tets.faces[l]].vertices[0]: " << faces[tets.faces[l]].vertices[0] <<std::endl;
			//DSTR << "faces[tets.faces[l]].vertices[1]: " << faces[tets.faces[l]].vertices[1] <<std::endl;
			//DSTR << "faces[tets.faces[l]].vertices[2]: " << faces[tets.faces[l]].vertices[2] <<std::endl;
			//DSTR << "ID: " << ID <<"�̂Ƃ��̐ߓ_�ƑΖʂ���ʂŖʐϕ����v�Z����"<<std::endl;
			for(unsigned j=0;j<4;j++){
				if(mesh->tets[id].vertexIDs[j] == ID){					///	�`��֐����P�A�i���Ȃ킿�j����face�ɑΖʂ��钸�_�@�ƈ�v������@���̎���face�Ŗʐϕ�����
					///	j�Ԗڂ̍s��̐�����0�ɂ���matk2array�Ōv�Z����
					///	�O�k�ɂȂ����b�V���ʂ̖ʐς�0�ŏ��������Ă���
					faceVars[mesh->tets[id].faceIDs[l]].heatTransRatio = (vertexVars[mesh->faces[mesh->tets[id].faceIDs[l]].vertexIDs[0]].heatTransRatio + vertexVars[mesh->faces[mesh->tets[id].faceIDs[l]].vertexIDs[1]].heatTransRatio 
						+ vertexVars[mesh->faces[mesh->tets[id].faceIDs[l]].vertexIDs[2]].heatTransRatio ) / 3.0;		///	���Yface�̔M�`�B�����\���ߓ_�ł̒l�̑������ς��Ƃ�
					///	�ȉ���[]�͏�܂ł�[l]�ƈقȂ�B
					///	ID�����Ԗڂ��ɂ���āA�`��֐��̌W�����قȂ�̂ŁA
					tetVars[id].matk[1] += faceVars[mesh->tets[id].faceIDs[l]].heatTransRatio * (1.0/12.0) * faceVars[mesh->tets[id].faceIDs[l]].area * matk2array[j];		//����tets[id].matk2 +=
					//DSTR << "tets[id].matk2��faces[tets[id].faces[l]].heatTransRatio * (1.0/12.0) * faces[tets[id].faces[l]].area * matk2array[" << j << "]"<< "�����Z: " <<faces[tets[id].faces[l]].heatTransRatio * (1.0/12.0) * faces[tets[id].faces[l]].area * matk2array[j] << std::endl;
					//DSTR << "tets[id].matk2 +=  " << tets[id].matk2 << std::endl;
				}
				//else{
				//	///	ID�ƈ�v���Ȃ��ꍇ�ɂ́Amatk2array[j]�ɂ͑S����0������
				//	///	�Ƃ������Ƃ��낾���A
				//	//matk2array[j] =0.0 * matk2array[j];
				//	//DSTR << "matk2array[" << j << "]: " << matk2array[j] << std::endl;
				//}
			}
		}
		///	SurfaceFace����Ȃ�������Amatk2array�ɂ�0������
		//else{
		//	//matk2array[l];
		//}
	}

	//DSTR << "matk2array:" << std::endl;
	//for(unsigned i=0;i<4;i++){
	//	DSTR <<i <<": " << matk2array[i] << std::endl;
	//}

	//k2 = k21 + k22 + k23 + k24
	//for(unsigned i=0; i < 4; i++){
	//	matk2 += matk2array[i];
	//	//	for debug
	//	//DSTR << "matk2 �� matk2array = k2" << i+1 <<"�܂ŉ��Z�����s��" << std::endl;
	//	//DSTR << matk2 << std::endl;
	//}
	
	//for debug
	//DSTR << "�ߓ_�i";
	//for(unsigned i =0; i < 4; i++){
	//	DSTR << tets[id].vertices[i] << "," ;
	//}
	//DSTR << ")�ō\�������l�ʑ̂�" << std::endl;
	//DSTR << "matk2 : " << std::endl;
	//DSTR << matk2 << std::endl;
	//int hogeshishi =0;
	
	//DSTR << "Inner Function _ matk2t tets[id].matk2: " <<tets[id].matk2 << std::endl;
}

void PHFemThermo::SetInitThermoConductionParam(double thConduct0,double roh0,double specificHeat0,double heatTrans0){
	thConduct = thConduct0;
	rho = roh0;
	specificHeat = specificHeat0;
	heatTrans = heatTrans0;
}
void PHFemThermo::SetThermalEmissivityToVtx(unsigned id,double thermalEmissivity){
	vertexVars[id].thermalEmissivity = thermalEmissivity;
}
void PHFemThermo::SetThermalEmissivityToVerticesAll(double thermalEmissivity){
	for(unsigned i =0; i < GetPHFemMesh()->vertices.size(); i++){
		vertexVars[i].thermalEmissivity = thermalEmissivity;
	}
}
void PHFemThermo::SetHeatTransRatioToAllVertex(){
	for(unsigned i =0; i < GetPHFemMesh()->vertices.size() ; i++){
		vertexVars[i].heatTransRatio = heatTrans;
	}
}

void PHFemThermo::SetTempAllToTVecAll(unsigned size){
	for(unsigned i =0; i < size;i++){
		TVecAll[i] = vertexVars[i].temp;
	}
}

void PHFemThermo::CreateTempMatrix(){
	unsigned dmnN = (unsigned)GetPHFemMesh()->vertices.size();
	TVecAll.resize(dmnN);
	SetTempAllToTVecAll(dmnN);
	//for(std::vector<unsigned int>::size_type i=0; i < dmnN ; i++){
	//	TVecAll[i] = vertices[i].temp;
	//}

	//for Debug
	//for(unsigned int i =0; i < dmnN; i++){
	//	DSTR << i <<" : " << TVecAll[i] << std::endl;
	//}
	//for debug
	//for(std::vector<unsigned int>::size_type i =0; i < vertices.size(); i++){
	//	DSTR << i << " : " << &vertices[i] << std::endl;
	//}

}

void PHFemThermo::SetTempToTVecAll(unsigned vtxid){
	if(0 <= vtxid && vtxid < TVecAll.size()){
		TVecAll[vtxid] = vertexVars[vtxid].temp;
	}
}

void PHFemThermo::UpdateheatTransRatio(unsigned id,double heatTransRatio){
	//if(vertices[id].heatTransRatio != heatTransRatio){	//�قȂ��Ă�����
	//	vertices[id].heatTransRatio = heatTransRatio;	
	//	///	�����܂ލs��̍X�V	K2,f3
	//	///	f3
	//	for(unsigned i =0; i < vertices[1].tets.size(); i++){
	//		CreateVecf3(tets[vertices[id].tets[i]]);
	//	}
	//	///	K3

	//}
	///	�����Ȃ牽�����Ȃ�
}

void PHFemThermo::SetLocalFluidTemp(unsigned i,double temp){
	vertexVars[i].Tc = temp;			//�ߓ_�̎��͗��̉��x�̐ݒ�
}

void PHFemThermo::SetVertexTemp(unsigned i,double temp){
	vertexVars[i].temp = temp;
	SetTempToTVecAll(i);
}

void PHFemThermo::SetVerticesTempAll(double temp){
	for(std::vector<unsigned int>::size_type i=0; i < GetPHFemMesh()->vertices.size() ; i++){
		vertexVars[i].temp = temp;
		SetTempToTVecAll(i);	// �v�����FAftersetdesk�̒��ŌĂ΂�鎞�ATVecAll�̗v�f�̐����ł܂��Ă��炸�A�A�N�Z�X�ᔽ�̉\��������
	}
}

void PHFemThermo::AddvecFAll(unsigned id,double dqdt){
	vecFAllSum[id] += dqdt;		//	+=�ɕύX
}

void PHFemThermo::SetvecFAll(unsigned id,double dqdt){
	vecFAllSum[id] = dqdt;		//	+=�ɕύX���ׂ��ŁA�폜�\��
}

void PHFemThermo::InitAllVertexTemp(){
	//	���̃��b�V���̑S���_�̉��x��0�ɂ���
	for(unsigned i=0; i < GetPHFemMesh()->vertices.size(); i++){
		vertexVars[i].temp = 0.0;
		//	�ǂ̃��b�V���Ń��Z�b�g���������@GetMe()->
	}
}

void PHFemThermo::InitMoist(){

	PHFemMeshNew* mesh = GetPHFemMesh();

	for(unsigned id =0; id < mesh->tets.size(); id++){
		tetVars[id].wratio = 0.917;
		double rho = 970;
		if(tetVars[id].volume){
			tetVars[id].tetsMg = tetVars[id].volume * rho;	//����*���x
			tetVars[id].wmass = tetVars[id].tetsMg * tetVars[id].wratio;
		}else if(tetVars[id].volume < 0.0){
			DSTR << "tets[" << id << "]�̑̐ς��v�Z����Ă��܂���" << std::endl;
			CalcTetrahedraVolume(mesh->tets[id]);
		}
	}
}


void PHFemThermo::DecrMoist_velo(double vel){
	
	PHFemMeshNew* mesh = GetPHFemMesh();
	
	//�������xver
	for(unsigned id =0; id < mesh->tets.size() ; id++){
		////���_��100�x�ȏ�Ŏc���ʂ��O�ł͂Ȃ��Ƃ�
		double tempAdd = 0.0;	//	���Z���x
		for(unsigned i=0; i < 4; i++){
			tempAdd += vertexVars[mesh->tets[id].vertexIDs[i]].temp;
		}
		//�P�ʊ��Z�͍����Ă��邩�H
		double wlatheat = 2.26 * 1000;		//��1kg������̐��M(latent heat)[W�Es]=[J] ���̐��M��540cal/g���J�ɕϊ����Ďg�p   W=J/s 2.26[kJ/kg]
		tetVars[id].tetsMg = tetVars[id].volume * rho;		//�l�ʑ̂̎���
		//�P�ʎ��Ԃ���������ʂ́Adw/dt = A/W * Rc  A:�������\�ʐρAW:�������̖���������
		double faceS=0.0;
		for(int ii=0;ii<4;ii++){
			int facet = mesh->tets[id].faceIDs[ii];
			if(facet <= (int)mesh->nSurfaceFace){
				faceS += faceVars[facet].area;
			}
		}
		//double surfaceS = tets[id].faces[  //	�l�ʑ̂ɑ�����face��nSurfaceface��菬�����ԍ���face�Ȃ�A�\�ʂ�face
		double sokudo = vel;
		double dwdt = faceS / tetVars[id].tetsMg * sokudo;//�P�ʎ��Ԃ�����Ȃ̂�dt�v�f������		//1step�Ō��鐅�̗�
		//double wlat = (tempAdd / 4.0) - 100.0;	//100�x�𒴂���������������B
		//double dw = dwdt * specificHeat *  tets[id].tetsMg / wlatheat;	//	����������	//	(���x�ƕ��_100�x�Ƃ̍����̔M��)�����̐��M�ŏ������鐅�̗ʂ�������B;		//	����������
		double exwater	= 0;	//���o���鐅�̗ʑS��(���� + ���o + �����ړ�)
		//���ω��x��100�x����

		//100�x�����F���C�����@�ɂ������E����
			//���ʂ�����
			tetVars[id].tetsMg -= dwdt;
			//�M�ʂ�D��	�l�ʑ̂̎��ʂ������Ă���M�ʂ���Adwdt�������		//	4�ߓ_�̕��ω��x�ł����̂��H	//�����ɖ��������肻���B
			double tetsheating = rho * specificHeat * tetVars[id].volume * (vertexVars[mesh->tets[id].vertexIDs[0]].temp + vertexVars[mesh->tets[id].vertexIDs[1]].temp + vertexVars[mesh->tets[id].vertexIDs[2]].temp + vertexVars[mesh->tets[id].vertexIDs[3]].temp)/4.0;

			//


		//100�x�ȏ�F���M�ɂ������E����


		//�ϐ��Ɉ˂闬�o�@�^���p�N���ϐ��E�\���ω��Ɛ������o



		//	�����A�ȉ��̃R�[�h�����B
		//if( tempAdd / 4.0 >= 100.0){
		//	//dw�̕������A���ʂ␅���ʂ������
		//	//double delw = (dt / 0.01 * 1.444*(0.000235/0.29)  / 10000000)*100;
		//	double delw = (1.444*(0.000235/0.29)  / 10000000)*100;
		//	exwater = delw * 500;
		//	tets[id].tetsMg -= dw - exwater;
		//	if(tets[id].wmass > dw - exwater){
		//		tets[id].wmass -= dw - exwater;
		//	}else{
		//		DSTR << "�������o�ʂ��������܂�" << std::endl;
		//	}
		//	//���؂���:�ЂƂ܂��Aexwater���O�łȂ���΁A�����Đ������邱�Ƃɂ��悤���B�����o������A���̃��b�V����exwater�̒l���O�ɂ��悤�B
		//	//wlat�̕������A���x�������
		//	for(unsigned j=0; j < 4; j++){
		//		vertices[j].temp -= dwdt;
		//	}
		//}
		//�Ƃ肠�����A�ȒP�ɁA���������炷�R�[�h
		//if(tets[id].wmass >= tets[id].wmass_start *0.01){
		//	tets[id].wmass -= tets[id].wmass_start * 0.01;
		//}
	}
}


void PHFemThermo::DecrMoist_vel(double dt){

	PHFemMeshNew* mesh = GetPHFemMesh();

	//�������xver
	for(unsigned id =0; id < mesh->tets.size() ; id++){
		////���_��100�x�ȏ�Ŏc���ʂ��O�ł͂Ȃ��Ƃ�
		double tempAdd = 0.0;	//	���Z���x
		for(unsigned i=0; i < 4; i++){
			tempAdd += vertexVars[mesh->tets[id].vertexIDs[i]].temp;
		}
		//�P�ʊ��Z�͍����Ă��邩�H
		double wlatheat = 2.26 * 1000;		//��1kg������̐��M(latent heat)[W�Es]=[J] ���̐��M��540cal/g���J�ɕϊ����Ďg�p   W=J/s 2.26[kJ/kg]
		tetVars[id].tetsMg = tetVars[id].volume * rho;		//�l�ʑ̂̎���
		//�P�ʎ��Ԃ���������ʂ́Adw/dt = A/W * Rc  A:�������\�ʐρAW:�������̖���������
		double faceS=0.0;
		for(int ii=0;ii<4;ii++){
			int facet = mesh->tets[id].faceIDs[ii];
			if(facet <= (int)mesh->nSurfaceFace){
				faceS += faceVars[facet].area;
			}
		}
		//double surfaceS = tets[id].faces[  //	�l�ʑ̂ɑ�����face��nSurfaceface��菬�����ԍ���face�Ȃ�A�\�ʂ�face
		double sokudo = 1.0;
		double dwdt = faceS / tetVars[id].tetsMg * sokudo;//�P�ʎ��Ԃ�����Ȃ̂�dt�v�f������
		//double wlat = (tempAdd / 4.0) - 100.0;	//100�x�𒴂���������������B
		double dw = dwdt * specificHeat *  tetVars[id].tetsMg / wlatheat;	//	����������	//	(���x�ƕ��_100�x�Ƃ̍����̔M��)�����̐��M�ŏ������鐅�̗ʂ�������B;		//	����������
		double exwater	= 0;	//���o���鐅�̗ʑS��(���� + ���o + �����ړ�)
		//���ω��x��100�x����
		if( tempAdd / 4.0 >= 100.0){
			//dw�̕������A���ʂ␅���ʂ������
			//double delw = (dt / 0.01 * 1.444*(0.000235/0.29)  / 10000000)*100;
			double delw = (1.444*(0.000235/0.29)  / 10000000)*100;
			exwater = delw * 500;
			tetVars[id].tetsMg -= dw - exwater;
			if(tetVars[id].wmass > dw - exwater){
				tetVars[id].wmass -= dw - exwater;
			}else{
				DSTR << "�������o�ʂ��������܂�" << std::endl;
			}
			//���؂���:�ЂƂ܂��Aexwater���O�łȂ���΁A�����Đ������邱�Ƃɂ��悤���B�����o������A���̃��b�V����exwater�̒l���O�ɂ��悤�B
			//wlat�̕������A���x�������
			for(unsigned j=0; j < 4; j++){
				vertexVars[j].temp -= dwdt;
			}
		}
		//�Ƃ肠�����A�ȒP�ɁA���������炷�R�[�h
		//if(tets[id].wmass >= tets[id].wmass_start *0.01){
		//	tets[id].wmass -= tets[id].wmass_start * 0.01;
		//}
	}
}


void PHFemThermo::DecrMoist(){

	PHFemMeshNew* mesh = GetPHFemMesh();

	for(unsigned id =0; id < mesh->tets.size() ; id++){
		////���_��100�x�ȏ�Ŏc���ʂ��O�ł͂Ȃ��Ƃ�
		double tempAdd = 0.0;	//	���Z���x
		for(unsigned i=0; i < 4; i++){
			tempAdd += vertexVars[mesh->tets[id].vertexIDs[i]].temp;
		}
		//�P�ʊ��Z�͍����Ă��邩�H
		double wlatheat = 2.26 * 1000;		//��1kg������̐��M(latent heat)[W�Es]=[J] ���̐��M��540cal/g���J�ɕϊ����Ďg�p   W=J/s 2.26[kJ/kg]
		tetVars[id].tetsMg = tetVars[id].volume * rho;		//�l�ʑ̂̎���
		//�P�ʎ��Ԃ���������ʂ́A
		double wlat = (tempAdd / 4.0) - 100.0;	//100�x�𒴂���������������B
		double dw = wlat * specificHeat *  tetVars[id].tetsMg / wlatheat;	//	����������	//	(���x�ƕ��_100�x�Ƃ̍����̔M��)�����̐��M�ŏ������鐅�̗ʂ�������B;		//	����������
		double exwater	= 0;	//���o���鐅�̗ʑS��(���� + ���o + �����ړ�)
		//���ω��x��100�x����
		if( tempAdd / 4.0 >= 100.0){
			//dw�̕������A���ʂ␅���ʂ������
			//double delw = (dt / 0.01 * 1.444*(0.000235/0.29)  / 10000000)*100;
			double delw = (1.444*(0.000235/0.29)  / 10000000)*100;
			exwater = delw * 500;
			tetVars[id].tetsMg -= dw - exwater;
			if(tetVars[id].wmass > dw - exwater){
				tetVars[id].wmass -= dw - exwater;
			}else{
				DSTR << "�������o�ʂ��������܂�" << std::endl;
			}
			//���؂���:�ЂƂ܂��Aexwater���O�łȂ���΁A�����Đ������邱�Ƃɂ��悤���B�����o������A���̃��b�V����exwater�̒l���O�ɂ��悤�B
			//wlat�̕������A���x�������
			for(unsigned j=0; j < 4; j++){
				vertexVars[j].temp -= wlat;
			}
		}
		//�Ƃ肠�����A�ȒP�ɁA���������炷�R�[�h
		//if(tets[id].wmass >= tets[id].wmass_start *0.01){
		//	tets[id].wmass -= tets[id].wmass_start * 0.01;
		//}
	}
}

int PHFemThermo::NFaces(){
		return GetPHFemMesh()->NFaces();
	} 

std::vector<Vec3d> PHFemThermo::GetFaceEdgeVtx(unsigned id){
		return GetPHFemMesh()->GetFaceEdgeVtx(id);
	}

Vec3d PHFemThermo::GetFaceEdgeVtx(unsigned id, unsigned vtx){
	return GetPHFemMesh()->GetFaceEdgeVtx(id, vtx);
	}

}

