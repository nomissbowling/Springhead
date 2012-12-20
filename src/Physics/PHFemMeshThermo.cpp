/*
 *  Copyright (c) 2003 - 2011, Fumihiro Kato, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#include <SciLab/SprSciLab.h>
#include <Physics/PHFemMeshThermo.h>
#include <Base/Affine.h>
//#include <Framework/FWObject.h>
//#include <Framework/sprFWObject.h>

#include <GL/glew.h>
#include <GL/glut.h>

#include "windows.h"

using namespace PTM;

namespace Spr{;

#define UseMatAll
//#define DEBUG
//#define DumK



PHFemMeshThermoDesc::PHFemMeshThermoDesc(){
	Init();
}
void PHFemMeshThermoDesc::Init(){
	thConduct = 0.574;
	rho = 970;
	heatTrans = 25;
	specificHeat = 0.196;		//1960
}

///////////////////////////////////////////////////////////////////
//	PHFemMeshThermo

PHFemMeshThermo::PHFemMeshThermo(const PHFemMeshThermoDesc& desc, SceneIf* s){
	deformed = true;			//変数の初期化、形状が変わったかどうか
	SetDesc(&desc);
	if (s){ SetScene(s); }
	StepCount =0;				// ステップ数カウンタ
	StepCount_ =0;				// ステップ数カウンタ
	//phFloor =  DCAST(FWObjectIf, GetSdk()->GetScene()->FindObject("fwPan"));
	//GetFramePosition();
}

void PHFemMeshThermo::CalcVtxDisFromOrigin(){
	//>	nSurfaceの内、x,z座標から距離を求めてsqrt(2乗和)、それをFemVertexに格納する
	//> 同心円系の計算に利用する　distance from origin
	
	/// 判定フラグの初期化
	for(unsigned i=0; i<nSurfaceFace; i++){
		faces[i].mayIHheated = false;
	}
	/// 初期化
	for(unsigned i =0;i<vertices.size();i++){
		vertices[i].disFromOrigin =0.0;
	}

	/// debug
	//DSTR << "faces.size(): " << faces.size() << std::endl;

	//> 表面faceの内、原点から各faceの節点のローカル(x,z)座標系での平面上の距離の計算を、faceの全節点のy座標が負のものに対して、IH加熱の可能性を示すフラグを設定
	for(unsigned i=0;i<nSurfaceFace;i++){
		//> 表面のfaceの全節点のy座標が負ならば、そのfaceをIH加熱のface面と判定し、フラグを与える
		if(vertices[faces[i].vertices[0]].pos.y < 0.0 && vertices[faces[i].vertices[1]].pos.y < 0.0 && vertices[faces[i].vertices[2]].pos.y < 0.0){
			faces[i].mayIHheated = true;
			//	(x,z)平面におけるmayIHheatedのface全節点の原点からの距離を計算する
			for(unsigned j=0; j<3; j++){
				vertices[faces[i].vertices[j]].disFromOrigin = sqrt(vertices[faces[i].vertices[j]].pos.x * vertices[faces[i].vertices[j]].pos.x + vertices[faces[i].vertices[j]].pos.z * vertices[faces[i].vertices[j]].pos.z);
			}
		}
	}

	//	debug		//>	高速化対応時にはコメントアウトする
	//>	座標値を確認する
	for(unsigned i=0; i < nSurfaceFace; i++){
		if(faces[i].mayIHheated){
			/// 3つの頂点の組み＝辺のx,zが同じで、y座標だけが異なる点の組みがないことを確認する
			for(unsigned j=0;j<3;j++){
				if(vertices[faces[i].vertices[j]].pos.x == vertices[faces[i].vertices[(j+1)%3]].pos.x
					&& vertices[faces[i].vertices[j]].pos.z == vertices[faces[i].vertices[(j+1)%3]].pos.z
					 && vertices[faces[i].vertices[j]].pos.y != vertices[faces[i].vertices[(j+1)%3]].pos.y){
					DSTR <<i << "th: " << vertices[faces[i].vertices[j]].pos << " : " << vertices[faces[i].vertices[(j+1)%3]].pos << " : " << vertices[faces[i].vertices[(j+2)%3]].pos<<std::endl;
					DSTR << "CalcVtxDisFromOrigin() error" << std::endl;
					assert(0);
				}
			}
		}
	}
	/// debug	
	//unsigned katoonNum =0;
	//for(unsigned i=0;i < nSurfaceFace;i++){
	//	if(faces[i].mayIHheated){katoonNum +=1;}
	//}
	//DSTR << "number of faces.mayIHheated: " << katoonNum << std::endl;		///		174/980(nSurfaceFace)
}

void PHFemMeshThermo::SetThermalBoundaryCondition(){
	
}

void PHFemMeshThermo::CreateMatKAll(){

}

void PHFemMeshThermo::CreateMatCAll(){

}

void PHFemMeshThermo::ScilabTest(){
	if (!ScilabStart()) std::cout << "Error : ScilabStart \n";

	//	行列の読み書き
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

	//	グラフ描画
	ScilabJob("t = 0:0.01:2*3.141592653;");
	ScilabJob("x = sin(t);");
	ScilabJob("y = cos(t);");
	ScilabJob("plot2d(x, y);");
	for(int i=0; i<100000; ++i){
		ScilabJob("");
	}
//	ScilabEnd();
}

void PHFemMeshThermo::UsingFixedTempBoundaryCondition(unsigned id,double temp){
	//温度固定境界条件
	SetVertexTemp(id,temp);
	//for(unsigned i =0;i < vertices.size()/3; i++){
	//	SetVertexTemp(i,temp);
	//}
}

void PHFemMeshThermo::UsingHeatTransferBoundaryCondition(unsigned id,double temp,double heatTransRatio){
	//熱伝達境界条件
	//節点の周囲流体温度の設定(K,C,Fなどの行列ベクトルの作成後に実行必要あり)
//	if(vertices[id].Tc != temp){					//更新する節点のTcが変化した時だけ、TcやFベクトルを更新する
		SetLocalFluidTemp(id,temp);
		vertices[id].heatTransRatio = heatTransRatio;
		//熱伝達境界条件が使われるように、する。				///	＋＝してベクトルを作っているので、下のコードでは、余計に足してしまっていて、正しい行列を作れない。
		//for(unsigned i =0;i < vertices[id].tets.size();i++){
		//	CreateVecfLocal(tets[vertices[id].tets[i]]);		//	Tcを含むベクトルを更新する
		//}

		///	熱伝達率を含む項(K2,f3)のみ再計算
		InitCreateVecf_();				///	変更する必要のある項のみ、入れ物を初期化
		InitCreateMatk_();
		for(unsigned i =0; i < edges.size();i++){
			edges[i].k = 0.0;
		}
		for(unsigned i=0; i< this->tets.size();i++){
			//DSTR << "this->tets.size(): " << this->tets.size() <<std::endl;			//12
			CreateVecFAll(i);				///	VecFの再作成
			CreateMatkLocal(i);				///	MatK2の再作成 →if(deformed==true){matk1を生成}		matK1はmatk1の変数に入れておいて、matk2だけ、作って、加算
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
		///	節点の属する面のalphaUpdatedをtrueにする
		for(unsigned i=0;i<vertices[id].faces.size();i++){
			faces[vertices[id].faces[i]].alphaUpdated = true;
			alphaUpdated = true;
		}
}

void PHFemMeshThermo::UsingHeatTransferBoundaryCondition(unsigned id,double temp){
	//熱伝達境界条件
	//節点の周囲流体温度の設定(K,C,Fなどの行列ベクトルの作成後に実行必要あり)
//	if(vertices[id].Tc != temp){					//更新する節点のTcが変化した時だけ、TcやFベクトルを更新する
		SetLocalFluidTemp(id,temp);
		//熱伝達境界条件が使われるように、する。
		//for(unsigned i =0;i < vertices[id].tets.size();i++){
		//	CreateVecfLocal(tets[vertices[id].tets[i]]);		//	Tcを含むベクトルを更新する
		//}
		InitCreateVecf_();
		for(unsigned i=0; i < this->tets.size();i++){
			CreateVecFAll(i);				///	VeecFの再作成
													///	MatK2の再作成→matK1はmatk1の変数に入れておいて、matk2だけ、作って、加算
		}
//	}
}

void PHFemMeshThermo::SetRhoSpheat(double r,double Spheat){
	//> 密度、比熱 of メッシュのグローバル変数(=メッシュ固有の値)を更新
	rho = r;
	specificHeat = Spheat;
}

std::vector<Vec2d> PHFemMeshThermo::CalcIntersectionPoint2(unsigned id0,unsigned id1,double r,double R){
	//	2点を通る直線は1つ	2つの定数を求める
	double constA = 0.0;
	double constB = 0.0;
	///	rと交点
	double constX1 = 0.0;
	double constX1_ = 0.0;
	double constY1 = 0.0;
	///	Rと交点
	double constX2 = 0.0;
	double constX2_ = 0.0;
	double constY2 = 0.0;

	//> 引数の代替処理	関数化したときに、変換する↓
	unsigned vtxId0 = id0;
	unsigned vtxId1 = id1;
	/// 原点に近い順に並び替え
	if(vertices[vtxId0].disFromOrigin > vertices[vtxId1].disFromOrigin){
		unsigned tempId = vtxId0;
		vtxId0 = vtxId1;
		vtxId1 = tempId;
	}
	/// vtxId0 < vtxId1 が保証されている

	//	2点で交わることが前提
	//> 2点のdisFromOriginをr,Rと比較してどちらと交わるかを判別する。
	//> 円環との交点を求める
	// x-z平面で考えている
	/// constA,B:vtxId0.vtxId1を通る直線の傾きと切片　/// aconsta,constbは正負構わない
	DSTR << "id0: " << id0 << ", id1: " << id1 <<std::endl;
	constA = ( vertices[vtxId0].pos.z - vertices[vtxId1].pos.z) / ( vertices[vtxId0].pos.x - vertices[vtxId1].pos.x);
	DSTR << "vertices[vtxId0].pos.z: " << vertices[vtxId0].pos.z <<std::endl;
	DSTR << "vertices[vtxId0].pos.z: " << vertices[vtxId1].pos.z <<std::endl;
	DSTR << "dz: vertices[vtxId0].pos.z - vertices[vtxId1].pos.z : " << vertices[vtxId0].pos.z - vertices[vtxId1].pos.z << std::endl;

	DSTR << "vertices[vtxId0].pos.x: " << vertices[vtxId0].pos.x << std::endl;
	DSTR << "vertices[vtxId1].pos.x: " << vertices[vtxId1].pos.x << std::endl;
	DSTR << "dx: vertices[vtxId0].pos.x - vertices[vtxId1].pos.x: " << vertices[vtxId0].pos.x - vertices[vtxId1].pos.x << std::endl;

	DSTR << "constA = dz / dx: " << constA << std::endl;
	if(vertices[vtxId0].pos.z == vertices[vtxId1].pos.z && vertices[vtxId0].pos.x == vertices[vtxId1].pos.x){
		DSTR << "vertices[vtxId0].pos.y: " << vertices[vtxId0].pos.y << ", vertices[vtxId1].pos.y: " << vertices[vtxId1].pos.y << std::endl;
		if(vertices[vtxId0].pos.y == vertices[vtxId1].pos.y)
			DSTR << "id[" << id0 <<"], id[" << id1 << "] は同じ頂点 !" << std::endl;
	}
	
	constB = vertices[vtxId0].pos.z - constA * vertices[vtxId0].pos.x;
	DSTR << "constB = vertices[vtxId0].pos.z - constA * vertices[vtxId0].pos.x : " << vertices[vtxId0].pos.z - constA * vertices[vtxId0].pos.x << std::endl;

	//DSTR << "constA: " << constA << std::endl;
	//DSTR << "constB: " << constB << std::endl;
	//DSTR << std::endl;

	///	交点の座標を計算
	if(vertices[vtxId0].disFromOrigin < r){		/// 半径rの円と交わるとき
		//CalcYfromXatcross(vtxId0,vtxId1,r);	//関数化しない
		//> 以下、関数化,vtxId0,1,r:引数、constYを返す
		constX1 = (- constA * constB + sqrt(r * r *(constA * constA + 1.0) - constB * constB));
		constX1_ = (- constA * constB - sqrt(r * r *(constA * constA + 1.0) - constB * constB));
		// どちらかが頂点の間にある　大小がわからないので、orで、点1のx座標、2のx座標と、その入れ替えと、作る
		//> 線分の両端の点の間にあるとき
		if( (vertices[vtxId0].pos.x <= constX1 && constX1 <= vertices[vtxId1].pos.x) || (vertices[vtxId1].pos.x <= constX1 && constX1 <= vertices[vtxId0].pos.x) ){
			constY1 = sqrt(r * r - constX1 * constX1 );
		}else{
			constY1 = sqrt(r * r - constX1_ * constX1_ );
			constX1 = constX1_;		///		点のx座標はconstX_が正しい事がわかった。
		}
	}else if(vertices[vtxId0].disFromOrigin < R){		/// 半径Rの円と交わるとき
		constX1 = (- constA * constB + sqrt(R * R *(constA * constA + 1.0) - constB * constB));
		constX1_ = (- constA * constB - sqrt(R * R *(constA * constA + 1.0) - constB * constB));
		// どちらかが頂点の間にある　大小がわからないので、orで、点1のx座標、2のx座標と、その入れ替えと、作る
		//> 線分の両端の点の間にあるとき
		if( (vertices[vtxId0].pos.x <= constX1 && constX1 <= vertices[vtxId1].pos.x) || (vertices[vtxId1].pos.x <= constX1 && constX1 <= vertices[vtxId0].pos.x) ){
			constY1 = sqrt(R * R - constX1 * constX1 );
		}else{
			constY1 = sqrt(R * R - constX1_ * constX1_ );
			constX1 = constX1_;		///		点のx座標はconstX_が正しい事がわかった。
		}
		
	}
	//> どちらとも交わるとき
	else if(vertices[vtxId0].disFromOrigin < r && R < vertices[vtxId1].disFromOrigin){
		//> 定数が2つ欲しい
		constX1 = (- constA * constB + sqrt(r * r *(constA * constA + 1.0) - constB * constB));
		constX1_ = (- constA * constB - sqrt(r * r *(constA * constA + 1.0) - constB * constB));
		// どちらかが頂点の間にある　大小がわからないので、orで、点1のx座標、2のx座標と、その入れ替えと、作る
		//> 線分の両端の点の間にあるとき
		if( (vertices[vtxId0].pos.x <= constX1 && constX1 <= vertices[vtxId1].pos.x) || (vertices[vtxId1].pos.x <= constX1 && constX1 <= vertices[vtxId0].pos.x) ){
			constY1 = sqrt(r * r - constX1 * constX1 );
		}else{
			constY1 = sqrt(r * r - constX1_ * constX1_ );
			constX1 = constX1_;		///		点のx座標はconstX_が正しい事がわかった。
		}
		constX2 = (- constA * constB + sqrt(R * R *(constA * constA + 1.0) - constB * constB));
		constX2_ = (- constA * constB - sqrt(R * R *(constA * constA + 1.0) - constB * constB));
		//> 線分の両端の点の間にあるとき
		if( (vertices[vtxId0].pos.x <= constX2 && constX2 <= vertices[vtxId1].pos.x) || (vertices[vtxId1].pos.x <= constX2 && constX2 <= vertices[vtxId0].pos.x) ){
			constY2 = sqrt(R * R - constX2 * constX2 );
		}else{
			constY2 = sqrt(R * R - constX2_ * constX2_ );
			constX2 = constX2_;		///		点のx座標はconstX_が正しい事がわかった。
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

Vec2d PHFemMeshThermo::CalcIntersectionPoint(unsigned id0,unsigned id1,double r,double R){
	double constA = 0.0;
	double constB = 0.0;
	double constX = 0.0;
	double constX_ = 0.0;
	double constY = 0.0;

	//> 引数の代替処理	関数化したときに、変換する↓
	unsigned vtxId0 = id0;
	unsigned vtxId1 = id1;
	/// 原点に近い順に並び替え
	if(vertices[vtxId0].disFromOrigin > vertices[vtxId1].disFromOrigin){
		unsigned tempId = vtxId0;
		vtxId0 = vtxId1;
		vtxId1 = tempId;
	}
	/// vtxId0 < vtxId1 が保証されている

	//> 2点のdisFromOriginをr,Rと比較してどちらと交わるかを判別する。
	if( (r <= vertices[vtxId0].disFromOrigin && vertices[vtxId0].disFromOrigin <= R) ^ (r <= vertices[vtxId1].disFromOrigin && vertices[vtxId1].disFromOrigin <= R)){
		//> 円環との交点を求める
		// x-z平面で考えている
		/// constA,B:vtxId0.vtxId1を通る直線の傾きと切片　/// aconsta,constbは正負構わない
		constA = ( vertices[vtxId0].pos.z - vertices[vtxId1].pos.z) / ( vertices[vtxId0].pos.x - vertices[vtxId1].pos.x);
		constB = vertices[vtxId0].pos.z - constA * vertices[vtxId0].pos.x;

		///	交点の座標を計算
		if(vertices[vtxId0].disFromOrigin < r){		/// 半径rの円と交わるとき
			//CalcYfromXatcross(vtxId0,vtxId1,r);	//関数化しない
			//> 以下、関数化,vtxId0,1,r:引数、constYを返す
			constX = (- constA * constB + sqrt(r * r *(constA * constA + 1.0) - constB * constB));
			constX_ = (- constA * constB - sqrt(r * r *(constA * constA + 1.0) - constB * constB));
			// どちらかが頂点の間にある　大小がわからないので、orで、点1のx座標、2のx座標と、その入れ替えと、作る
			//> 線分の両端の点の間にあるとき
			if( (vertices[vtxId0].pos.x <= constX && constX <= vertices[vtxId1].pos.x) || (vertices[vtxId1].pos.x <= constX && constX <= vertices[vtxId0].pos.x) ){
				constY = sqrt(r * r - constX * constX );
			}else{
				constY = sqrt(r * r - constX_ * constX_ );
				constX = constX_;		///		点のx座標はconstX_が正しい事がわかった。
			}
		}else if(r < vertices[vtxId0].disFromOrigin && vertices[vtxId0].disFromOrigin < R){		/// 半径Rの円と交わるとき
			constX = (- constA * constB + sqrt(r * R *(constA * constA + 1.0) - constB * constB));
			constX_ = (- constA * constB - sqrt(r * R *(constA * constA + 1.0) - constB * constB));
			// どちらかが頂点の間にある　大小がわからないので、orで、点1のx座標、2のx座標と、その入れ替えと、作る
			//> 線分の両端の点の間にあるとき
			if( (vertices[vtxId0].pos.x <= constX && constX <= vertices[vtxId1].pos.x) || (vertices[vtxId1].pos.x <= constX && constX <= vertices[vtxId0].pos.x) ){
				constY = sqrt(R * R - constX * constX );
			}else{
				constY = sqrt(R * R - constX_ * constX_ );
				constX = constX_;		///		点のx座標はconstX_が正しい事がわかった。
			}
		}
		//> どちらとも交わるとき
		else if(vertices[vtxId0].disFromOrigin < r && R < vertices[vtxId1].disFromOrigin){
			//> 定数が2つ欲しい
		}
	}
		Vec2d interSection;
		interSection[0] = constX;
		interSection[1] = constY;
		DSTR << __FILE__  << "(" <<  __LINE__ << "):"<< "interSection: " << interSection << std::endl;
		//":" <<  __TIME__ << 
		return interSection;
}

void PHFemMeshThermo::ArrangeFacevtxdisAscendingOrder(int faceID){
	///	3点を原点に近い順に並べる		//>	クイックソートにしたいかも？
	int vtxmin[3];		///	通しの頂点番号を入れる
	vtxmin[0] = faces[faceID].vertices[0];
	vtxmin[1] = 0;
	vtxmin[2] = 0;
	if(vertices[faces[faceID].vertices[1]].disFromOrigin < vertices[faces[faceID].vertices[0]].disFromOrigin ){
		vtxmin[0] = faces[faceID].vertices[1];
		vtxmin[1] = faces[faceID].vertices[0];
	}else{
		vtxmin[1] = faces[faceID].vertices[1];
	}
	if(vertices[faces[faceID].vertices[2]].disFromOrigin < vertices[vtxmin[0]].disFromOrigin){
		vtxmin[2] = vtxmin[1];
		vtxmin[1] = vtxmin[0];
		vtxmin[0] = faces[faceID].vertices[2];
	}else if(vertices[vtxmin[0]].disFromOrigin < vertices[faces[faceID].vertices[2]].disFromOrigin && vertices[faces[faceID].vertices[2]].disFromOrigin < vertices[vtxmin[1]].disFromOrigin){
		vtxmin[2] = vtxmin[1];
		vtxmin[1] = faces[faceID].vertices[2];
	}else if(vertices[vtxmin[1]].disFromOrigin < vertices[faces[faceID].vertices[2]].disFromOrigin ){
		vtxmin[2] = faces[faceID].vertices[2];
	}
	//>	小さい順になっていないときは、assert(0)
	if( !(vertices[vtxmin[0]].disFromOrigin < vertices[vtxmin[1]].disFromOrigin && vertices[vtxmin[1]].disFromOrigin < vertices[vtxmin[2]].disFromOrigin )
		){	assert(0);}
	/// debug
	//DSTR << "小さい順 ";
	//for(unsigned j=0; j <3; j++){
	//	DSTR << vertices[vtxmin[j]].disFromOrigin;
	//	if(j<2){ DSTR << ", ";}
	//}
	//DSTR << std::endl;
	/// face内の配列にface内での原点から近い順番を格納
	for(unsigned i=0;i<3;i++){
		faces[faceID].ascendVtx[i] = vtxmin[i];
	}
	//DSTR << "vtxmin[0~2]:  " << vtxmin[0] <<" ," << vtxmin[1] << " ,"  << vtxmin[2] << std::endl;	
	//vtxmin[0~2]:  11 ,324 ,281 ,vtxmin[0~2]:  353 ,11 ,324 この順に原点に近い

	//	以下、削除
	//> 返す準備
	//Vec3i vtxarray = Vec3i(vtxmin[0],vtxmin[1],vtxmin[2]);
	//	ex	vtxarray: (    11    324    281)	,vtxarray: (   353     11    324)
	//DSTR << "vtxarray: " << vtxarray << std::endl; 
	//return vtxarray;		///	通しの頂点番号を返す
}
Vec2d PHFemMeshThermo::CalcIntersectionOfCircleAndLine(unsigned id0,unsigned id1,double radius){
	//	x-z平面での半径rの円環と線分の交点の座標を求める
	//	関数が呼ばれる条件：交わることが明白なとき、交わる円弧の半径と、円弧の内・外側の頂点を引受け計算

	// ...2点を通る直線の方程式の傾きと切片を求める
	double constA = 0.0;	//	傾き
	double constB = 0.0;	//	切片
	double constX1 = 0.0;	//	交点のx 座標１
	double constX1_ = 0.0;	//		  x 座標候補２
	double constZ1 = 0.0;	//		  Y 座標
	double constZ1_ = 0.0;	//		  Y 座標候補２
	double constx = 0.0;	//	1次関数にならない場合の定数
	double constz = 0.0;	//　		同上

	//	...頂点番号を原点に近い＝小さい順( id0 < id1 )に並び替え
	if( vertices[ id1 ].disFromOrigin	<	vertices[ id0 ].disFromOrigin ){
		unsigned farfromOriginId = id0;
		id0 = id1;
		id1 = farfromOriginId;
	}	// id0 < id1 を保証

	// 1次関数にならない場合
	// .. x == const
	if(vertices[id0].pos.x == vertices[id1].pos.x && vertices[id0].pos.z != vertices[id1].pos.z ){
		//	x == const の式

	}
	// .. z == const

	// ...傾きと切片を求める
	//
	constA = ( vertices[id0].pos.z - vertices[id1].pos.z) / ( vertices[id0].pos.x - vertices[id1].pos.x);
	constB = vertices[id0].pos.z - constA * vertices[id0].pos.x;
	
	
	// ..次の条件は、以降の処理でエラーが出る場合を知らせるための、デバッグ用
	if(!constA && !constB){
		//if( id0 != id1)
		//	vertices[id0].pos.z != 0 && vertices[id1].pos.z!= 0 && vertices[id0].pos.x
		// z == 0という直線の式である。とｘ＝＝０という直線の式の時もあろう。
		//	この条件分岐の意味は、線分の両端の点のx,z座標が(0,0)や、直線の式が１次関数にならないと記を除くことができるときに、用いる。
		DSTR << "直線の式の切片と傾きが共に0" << std::endl;
		DSTR << "id0: " << id0 << ", id1: " << id1 << "radius: " << radius << std::endl;
		DSTR << "vertices[id0].pos.x: " << vertices[id0].pos.x << ", vertices[id1].pos.x: " << vertices[id1].pos.x << std::endl;
		DSTR << "vertices[id0].pos.z: " << vertices[id0].pos.z << ", vertices[id1].pos.z: " << vertices[id1].pos.z << std::endl;
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
	//		DSTR << "id[" << id0 <<"], id[" << id1 << "] は同じ頂点 !" << std::endl;
	//}
	//DSTR << "constB = vertices[id0].pos.z - constA * vertices[id0].pos.x : " << vertices[id0].pos.z - constA * vertices[id0].pos.x << std::endl;
	//DSTR << "constA: " << constA << std::endl;
	//DSTR << "constB: " << constB << std::endl;
	//DSTR << std::endl;

	///	.交点の座標を計算
	// .１次関数の場合、x,z軸に平行な直線の場合がある
	if(vertices[id0].disFromOrigin <= radius && radius <= vertices[id1].disFromOrigin ){		/// 半径rの円と交わるべき
		double radius2 = radius * radius;
		constX1  = (- constA * constB + sqrt(radius2 * (constA * constA + 1.0) - constB * constB )  )  /  (constA * constA + 1);
		constX1_ = (- constA * constB - sqrt(radius2 * (constA * constA + 1.0) - constB * constB )  )  /  (constA * constA + 1);
		// 交点のx座標が線分の両端点のx座標間にあるとき
		if( (vertices[id0].pos.x <= constX1 && constX1 <= vertices[id1].pos.x) || (vertices[id1].pos.x <= constX1 && constX1 <= vertices[id0].pos.x) ){
			constZ1 = sqrt(radius2 - constX1  * constX1  );
			constZ1_ = - sqrt(radius2 - constX1  * constX1  );
			//	交点のz座標も両端点のz座標間にあるとき
			if( (vertices[id0].pos.z <= constZ1_ && constZ1_ <= vertices[id1].pos.z) || (vertices[id1].pos.z <= constZ1_ && constZ1_ <= vertices[id0].pos.z) ){
				constZ1 = constZ1_;
			}
		}else{
			constX1 = constX1_;		///		点のx座標はconstX_が正しい事がわかった。
			constZ1  =   sqrt(radius2 - constX1  * constX1  );
			constZ1_ = - sqrt(radius2 - constX1  * constX1  );
			//	交点のz座標も両端点のz座標間にあるとき
			if( (vertices[id0].pos.z <= constZ1_ && constZ1_ <= vertices[id1].pos.z) || (vertices[id1].pos.z <= constZ1_ && constZ1_ <= vertices[id0].pos.z) ){
				constZ1 = constZ1_;
			}
		}
	}else{
		DSTR << "CalcVtxCircleAndLine()関数のこの頂点組みと円弧は交わりません" << std::endl;
		constX1 = 0.0;
		constZ1 = 0.0;
		DSTR << "(id0, vertices[id0].disFromOrigin): (" << id0 << ", " << vertices[id0].disFromOrigin << "), (id1, vertices[id1].disFromOrigin): (" << id1 << ", " << vertices[id1].disFromOrigin << "), radius: " << radius << std::endl;  
		assert(0);
	}
	Vec2d intersection = Vec2d(constX1,constZ1);
	//":" <<  __TIME__ << 
	return intersection;
}		//	CalcVtxCircleAndLine() :difinition

void PHFemMeshThermo::ShowIntersectionVtxDSTR(unsigned faceID,unsigned faceVtxNum,double radius){
	unsigned i = faceID;
	unsigned j = faceVtxNum;
	DSTR << "ascendVtx[" << j << "]: " << faces[i].ascendVtx[j] << ", " << "[ " << (j+1)%3 << "]: " << faces[i].ascendVtx[(j+1)%3] << "; ";
	DSTR << " (vertices[" << faces[i].ascendVtx[j] << "].pos.x, .z) = ( " <<  vertices[faces[i].ascendVtx[j]].pos.x << ", "<<  vertices[faces[i].ascendVtx[j]].pos.z  << "), " ;
	DSTR << " (vertices[" << faces[i].ascendVtx[(j+1)%3] << "].pos.x, .z) : ( " <<  vertices[faces[i].ascendVtx[(j+1)%3]].pos.x << ", "<<  vertices[faces[i].ascendVtx[(j+1)%3]].pos.z << "), " <<std::endl;
	DSTR <<"face[i].[(" << j << "], [" << (j+1)%3 << "]：各々の原点からの距離" << vertices[faces[i].ascendVtx[j]].disFromOrigin << ", " << vertices[faces[i].ascendVtx[(j+1)%3]].disFromOrigin << ", "; 
	DSTR << " radius: " << radius <<" と2点で構成される線分との交点は下記"<< std::endl;
	DSTR << __FILE__  << "(" <<  __LINE__ << "):"<< "Intersection Vtx (x,z)= " << CalcIntersectionOfCircleAndLine( faces[i].ascendVtx[ j ] , faces[i].ascendVtx[ (j+1)%3 ] , radius) << std::endl;
	DSTR << std::endl;
}

void PHFemMeshThermo::CalcIHarea(double radius,double Radius,double dqdtAll){
	//	face構造体メンバihareaを計算
	//、メッシュ全体のihareaの合計を計算
	//	...dqdtRatio(熱流束率)を総面積に対するiharea面積比から弱火の時に、faceが受け取るべきdqdtを計算して返す

	//	形状関数の計算は、iharea を用いて、CalcVecf2surface()等で行う,face.shapefunkで行う		→クラス化しようか？
		//	.... 形状関数を格納する		// ここで分かる形状関数は、頂点間の距離から分かる線形補間係数　すなわち、割合　０＜＝〜＜＝１で良くて、最後に、行列に入れる前に、割合以外を入れればいいのかな？

	// radius value check
	if(Radius <= radius){
		DSTR << "inner radius size is larger than outer Radius " << std::endl;
		DSTR << "check and set another value" << std::endl;
		assert(0);
	}
	///	内半径と外半径の間の節点に熱流束境界条件を設定
	//> 四面体面の三角形と円環領域の重複部分の形状・面積を求める当たり判定を計算する。
	//>	切り取り形状に応じた形状関数を求め、熱流束ベクトルの成分に代入し、計算する

	//> 1.円環領域と重なるface三角形の形状を算出する。領域に含まれる頂点、face三角形の辺との交点を求めてvecteorに格納する
	//>	2.vectorには、辺0,1,2の順に領域内の頂点や交点が入っているが、これを元に三角形分割を行う。三角形分割ができたら、各三角形を求める。三角形の総和を、このfaceの加熱領域とする。
	//>	3.vectorの点における形状関数を求めて、擬似体積（重なっている面積×形状関数の値）を使って、四面体内の各点における形状関数の面積分を求める。求めた値は、熱流束ベクトルの成分として要素剛性行列の成分に代入する。
	//>	4.毎ステップ、同じ熱流束の値をベクトル成分に加える
	
	/// debug
	//unsigned numIHheated0 = 0; 
	//for(unsigned i=0; i < nSurfaceFace;i++){
	//	if(faces[i].mayIHheated){	
	//		DSTR << i << " ; "  << std::endl;
	//		numIHheated0 +=1;
	//	}
	//}
	//DSTR << "numIHheated0 / nSurfaceFace: " << numIHheated0 << " / " << nSurfaceFace << std::endl;	////	761 / 980	ってほとんどじゃないか！半分位にならないとおかしいはずだが・・・　ローカルy座標値がマイナスのものを選んでいるので

	//	debug	mayIHheatedの確度を上げる前の数を知りたい
	unsigned numIHheated0 = 0; 
	for(unsigned i=0; i < nSurfaceFace;i++){
		if(faces[i].mayIHheated){	
			//DSTR << i << " ; "  << std::endl;
			numIHheated0 +=1;
		}
	}
	DSTR << "numIHheated0 / nSurfaceFace: " << numIHheated0 << " / " << nSurfaceFace << std::endl;

	//	face頂点のどれか1つが、円環領域に入っているfaceだけ、trueに、それ以外は、falseに
	//> raius,RadiusについてmayIHheatedの確度を上げてから、円環領域と重なっている形状を求める
	for(unsigned i=0;i < nSurfaceFace; i++){
		if(faces[i].mayIHheated){			// faceの節点のy座標が負の場合→IH加熱の対象節点 円環の範囲内に入っているとは限らない
			for(unsigned j=0;j<3;j++){
				/// 円環領域内にface頂点が含まれる
				if(radius <= vertices[faces[i].vertices[j]].disFromOrigin && vertices[faces[i].vertices[j]].disFromOrigin <= Radius){
					faces[i].mayIHheated = true;
					break;		//>	見つかったら、判定はtrueのままで良い。最内側のforを抜ける
				}
				else{
					faces[i].mayIHheated = false;
				}
				//> （円環領域には含まれず）円環領域より内側と外側にfaceの辺の頂点がある	vertices[j%3] と vertices[(j+1)%3]　で作る辺があるとき
				if(vertices[faces[i].vertices[j]].disFromOrigin < radius && Radius < vertices[faces[i].vertices[(j+1)%3]].disFromOrigin 
					|| vertices[faces[i].vertices[(j+1)%3]].disFromOrigin < radius && Radius < vertices[faces[i].vertices[j]].disFromOrigin){
						faces[i].mayIHheated = true;
						break;		//>	同上
				}else{
					faces[i].mayIHheated = false;
				}
				//>	円環領域内にface辺のどちらかの頂点が含まれるとき(r<P1<R<P2,P1<r<P2<R,(とPa1,P2を入れ替えたもの))
				if(radius <= vertices[faces[i].vertices[j]].disFromOrigin && vertices[faces[i].vertices[j]].disFromOrigin < Radius && Radius < vertices[faces[i].vertices[(j+1)%3]].disFromOrigin
					|| radius <= vertices[faces[i].vertices[(j+1)%3]].disFromOrigin && vertices[faces[i].vertices[(j+1)%3]].disFromOrigin < Radius && Radius < vertices[faces[i].vertices[j]].disFromOrigin
					|| vertices[faces[i].vertices[j]].disFromOrigin <= radius && radius < vertices[faces[i].vertices[(j+1)%3]].disFromOrigin && vertices[faces[i].vertices[(j+1)%3]].disFromOrigin < Radius
					|| vertices[faces[i].vertices[(j+1)%3]].disFromOrigin <= radius && radius < vertices[faces[i].vertices[j]].disFromOrigin && vertices[faces[i].vertices[j]].disFromOrigin < Radius){
						faces[i].mayIHheated = true;
						break;		//>	同上
				}else{
					faces[i].mayIHheated = false;
				}
			}		//	for
		}		//	if
	}		//	for
	//> debug
	//>	mayIHheatedのフラグが立っているfaceにその面積の形状関数を与えてみる。	重なる面積をきちんと計算と、少しでも引っかかっていれば、加熱面に入れてしまう計算、試す
	//> CalcIHdqdt3 or 4

	/// debug
	unsigned numIHheated = 0; 
	for(unsigned i=0; i < nSurfaceFace;i++){
		if(faces[i].mayIHheated){	
			//DSTR << i << " ; "  << std::endl;
			numIHheated +=1;
		}
	}
	DSTR << "numIHheated / nSurfaceFace: " << numIHheated << " / " << nSurfaceFace << std::endl;		///:	表面faceの内、加熱節点を含むfaceの数、鉄板:264/980　こんなもんかな 
	
	//	.. 交点を求め、faces構造体のvectorに領域内の頂点や交点を格納
	for(unsigned i=0;i < nSurfaceFace; i++){
		if(faces[i].mayIHheated){		//	may → 「確実」に変化済みのフラグ
			//	area:face面積を計算されてなければ、計算
			if(faces[i].area==0) faces[i].area = CalcTriangleArea(faces[i].vertices[0],faces[i].vertices[1],faces[i].vertices[2]);
			////	face内の頂点のdisFromOriginの値でソート
			//unsigned nearestvtxnum		=	0;				///	原点に一番近い頂点のface頂点番号(0~2)
			//for(unsigned j=0;j<3;j++){
			//	double hikaku = DBL_MAX;
			//	if(hikaku > vertices[faces[i].vertices[j]].disFromOrigin){	hikaku = vertices[faces[i].vertices[j]].disFromOrigin;	nearestvtxnum = j;}
			//}
			
			// ... 3点を原点に近い順に並べる		vtxOrder:近い順に格納,番目が近い順番、格納値が頂点番号 ==	faces[i].ascendVtx[3]
			ArrangeFacevtxdisAscendingOrder(i);		///	ArrangeVtxdisAscendingOrder(int faceID,int vtx0,int vtx1,int vtx2)
			//DSTR <<  "小さい順か確認: " << vertices[vtxOrder[0]].disFromOrigin << ", "<< vertices[vtxOrder[1]].disFromOrigin << ", "<< vertices[vtxOrder[2]].disFromOrigin << std::endl;
			
			// ... face内の各頂点が属している領域を判定 0 | 1 | 2	///	faces[i].ascendVtx[0~2]に該当する頂点が　円環領域の前後のどこに存在しているか
			// ... vtxdiv[0~2]に近い順に並んだ頂点の領域ID(0~2)を割り振り ＝ faces[i].ascendVtx[j] の順と対応
			unsigned vtxdiv[3];		//	原点から近い順:0~2に並べ替えられた頂点IDに対応する領域内外の区分け　配列
			//DSTR << "faces[i].ascendVtx[0~2]:" ;
			for(unsigned j=0;j<3;j++){
				if( vertices[faces[i].ascendVtx[j]].disFromOrigin < radius){			vtxdiv[j] = 0;
				/// 円弧上を含み、円弧上も円環領域内と定義する
				}else if(radius <= vertices[faces[i].ascendVtx[j]].disFromOrigin && vertices[faces[i].ascendVtx[j]].disFromOrigin <= Radius ){	vtxdiv[j] = 1;
				}else if(Radius < vertices[faces[i].ascendVtx[j]].disFromOrigin){		vtxdiv[j] = 2;	}
				//DSTR << faces[i].ascendVtx[j] ;
				//if(j <  2) DSTR << ", " ;
			}
			//DSTR << std::endl;

			//> debug
			//DSTR << "頂点の領域番号: " ;
			//for(unsigned j =0;j<3;j++){
			//	DSTR << vtxexistarea[j];
			//	if(j<2) DSTR << ", ";
			//}
			//DSTR << std::endl;
			//int vtxexistareadebug =0;
			//... 2012.2.14ここまで...

			//...	配列の成分の値の変化を見て、始点、交点、辺対となる点を順にvectorに格納していく
			
			//	vectorに入れる際の注意!!! %%%%%%%%%%%%%%%%%%
			// %%%	vtxOrder[ 原点から近い頂点の順(0,1,2) ]:その頂点IDの原点に近い順に並べ替えてIDを格納		(例:vtxOrder[0] = (ID)278, [1] = (ID)35, [2] = (ID)76 etc)
			// %%%	vtxdiv[ 原点から近い頂点の順(0,1,2) ]:その頂点が円環領域の内側（1）か外側(0,2)かを表す

			//>	faceの辺ごとに場合分け
			///	 j と(隣の) (j+1)%3 とで対を成す辺について
			for(unsigned j=0;j<3;j++){
				double f[3]={0.0, 0.0, 0.0};	// 頂点0,1,2,3から見た形状関数 
				//debug
				//DSTR <<"j: " << j << ", faces[i].ascendVtx[j]: " << faces[i].ascendVtx[j] << ", faces[i].ascendVtx[(j+1)%3]: " << faces[i].ascendVtx[(j+1)%3] << std::endl;
				//DSTR << "vertices[faces[i].ascendVtx[j]].pos: (" << vertices[faces[i].ascendVtx[j]].pos.x  << ", "<< vertices[faces[i].ascendVtx[j]].pos.z << ") " << std::endl;
				//DSTR << "vertices[faces[i].ascendVtx[(j+1)%3]].pos: (" << vertices[faces[i].ascendVtx[(j+1)%3]].pos.x  << ", "<< vertices[faces[i].ascendVtx[(j+1)%3]].pos.z << ") " << std::endl; 
				//DSTR << std::endl;

				//	0の領域にある辺:
				if(vtxdiv[j] == 0 && vtxdiv[(j+1)%3] == 0){
					//	いずれの点をも領域内vectorには入れない
				}
				//	内半径とだけ交わる辺(内:0→外:1と外:1→内:0):
				else if(vtxdiv[j] == 0 && vtxdiv[(j+1)%3] - vtxdiv[j] > 0 || vtxdiv[j] == 1 && vtxdiv[(j+1)%3] - vtxdiv[j] < 0){
					//	(始点（↑で入れている場合には不要））と内半径とを、対の点に入れる
					if(vtxdiv[(j+1)%3] - vtxdiv[j] > 0){	//内→外
						//	内半径との交点を求めて、座標を入れる
						//%%%	線分を構成する頂点と半径、交点のチェック関数→DSTR表示	%%%//		//faceID,face内節点番号、半径を用いて、隣り合う節点で作る線分と円弧の交点を求めて表示
						ShowIntersectionVtxDSTR(i,j,radius);
						//	..内半径との交点のx,z座標を入れる
						faces[i].ihvtx.push_back( CalcIntersectionOfCircleAndLine( faces[i].ascendVtx[ j ] , faces[i].ascendVtx[ (j+1)%3 ] , radius) );
						//	..組対点の座標をintersectionに入れる
						faces[i].ihvtx.push_back( Vec2d( vertices[ faces[i].ascendVtx[ (j+1)%3 ] ].pos.x, vertices[faces[i].ascendVtx[ (j+1)%3 ] ].pos.z) );
						//	....この点位置での形状関数を導出
					}else if(vtxdiv[(j+1)%3] - vtxdiv[j] < 0){		//外→内
						//	内半径との交点を求め、交点の座標を入れる
						faces[i].ihvtx.push_back( CalcIntersectionOfCircleAndLine( faces[i].ascendVtx[ j ] , faces[i].ascendVtx[ (j+1)%3 ] , radius) );
						//..組対点は、内半径の内側：円環領域外なので、組対点をvectorには入れない
					}
				}
				//	円環領域(:1)内にある辺
				else if(vtxdiv[j] == 1 && vtxdiv[(j+1)%3] == 1){
					//	(始点を入れているのなら、)辺対点をvectorに入れる
					//	始点は入れずとも、最後に入るはず
					////intersection.push_back(Vec2d(vertices[vtxdiv[(j+1)%3]].pos.x,vertices[vtxdiv[(j+1)%3]].pos.z));
					faces[i].ihvtx.push_back(Vec2d(vertices[faces[i].ascendVtx[(j+1)%3]].pos.x,	vertices[faces[i].ascendVtx[(j+1)%3]].pos.z));
				}
				//	外半径と交わる辺(内→外、外→内)
				else if(vtxdiv[j] == 1 && vtxdiv[(j+1)%3] == 2 || vtxdiv[j] == 2 && vtxdiv[(j+1)%3] == 1){
					// 内向きか外向きかを、符号で判定することで、上の	or	のどちらかを判定し、vectorに入れる順番を変える
					//	外半径との交点を求める
					//	内→外 2 - 1 = 1 > 0
					if(vtxdiv[(j+1)%3] - vtxdiv[j] > 0){
						//	交点を格納
						faces[i].ihvtx.push_back( CalcIntersectionOfCircleAndLine( faces[i].ascendVtx[ j ] , faces[i].ascendVtx[ (j+1)%3 ] , Radius) );
						////	組対点のX,Z座標を格納→格納しちゃダメでしょ！
						//faces[i].ihvtx.push_back(Vec2d(vertices[faces[i].ascendVtx[(j+1)%3]].pos.x,	vertices[faces[i].ascendVtx[(j+1)%3]].pos.z));
						////	?→	intersection.push_back(Vec2d(vertices[vtxOrder[(j+1)%3]].pos.x,vertices[vtxOrder[(j+1)%3]].pos.z));
						
					}
					//	外→内 1 - 2 = -1 < 0
					else if(vtxdiv[(j+1)%3] - vtxdiv[j] < 0){
						// 外半径との交点の座標を格納する
						faces[i].ihvtx.push_back( CalcIntersectionOfCircleAndLine( faces[i].ascendVtx[ j ] , faces[i].ascendVtx[ (j+1)%3 ] , Radius) );
						// 組対点を格納する
						faces[i].ihvtx.push_back( Vec2d( vertices[ faces[i].ascendVtx[ (j+1)%3 ] ].pos.x, vertices[faces[i].ascendVtx[ (j+1)%3 ] ].pos.z) );
					}
					else if(vtxdiv[(j+1)%3] - vtxdiv[j] == 0) assert(0);
					//	(始点がvectorに入っていることを確認する)交点をvectorに入れる
					faces[i].ihvtx.push_back( CalcIntersectionOfCircleAndLine( faces[i].ascendVtx[ j ] , faces[i].ascendVtx[ (j+1)%3 ] , Radius) );
				}
				//	内半径と外半径とのどちらとも交わる辺(内→外、外→内)
				else if(vtxdiv[j] == 0 && vtxdiv[(j+1)%3] == 2 || vtxdiv[j] == 2 && vtxdiv[(j+1)%3] == 0){
					// 内向きか外向きかを、符号で判定することで、上の	or	のどちらかを判定し、vectorに入れる順番を変える
					//%%%	どちらとも交わる条件
					// 内半径円弧より原点より→外：0→２: 2 - 0 > 0
					if(vtxdiv[(j+1)%3] - vtxdiv[j] > 0){
						//内半径と交わり、交点を格納
						faces[i].ihvtx.push_back( CalcIntersectionOfCircleAndLine( faces[i].ascendVtx[ j ] , faces[i].ascendVtx[ (j+1)%3 ] , radius) );
						//外板系と交わり、交点を格納
						faces[i].ihvtx.push_back( CalcIntersectionOfCircleAndLine( faces[i].ascendVtx[ j ] , faces[i].ascendVtx[ (j+1)%3 ] , Radius) );
					}else{	// 外→内半径円弧より原点より：2→０
						//外半径と交わり、交点を格納
						faces[i].ihvtx.push_back( CalcIntersectionOfCircleAndLine( faces[i].ascendVtx[ j ] , faces[i].ascendVtx[ (j+1)%3 ] , Radius) );
						//内板系と交わり、交点を格納
						faces[i].ihvtx.push_back( CalcIntersectionOfCircleAndLine( faces[i].ascendVtx[ j ] , faces[i].ascendVtx[ (j+1)%3 ] , radius) );
					}
				}
				//	内半径と外半径と交わる辺(外側から内側へ)
				//else if(){
				//}
					//	if文の中で、差分がプラスかマイナスで判定できそう
				
				//	外半径の外側にある辺
				else if(vtxdiv[j] == 2 && vtxdiv[(j+1)%3] == 2){
					//	いずれの点をも領域内vectorには入れない
				}
				//	内側から外側に行く辺は上で記述できるが、外側から内側に向かう辺をこれで記述できるのか？
			}	//	for(unsigned j=0;j<3;j++){

			//	..vectorを三角形分割する
			//	..分割した三角形の面積を各々求める
			// ..IH面積に追加

			//faces[i].iharea = 

			//	デバッグ項目：faceのihvtx(vector)に、mayihheatedでフラグの立ったface又は節点が全て含まれているか。


// 以下　旧コード
			/// 旧コード
			/// 領域内にスタート頂点があるとき
			//if(radius < vertices[faces[i].vertices[nearestvtxnum]].disFromOrigin && vertices[faces[i].vertices[(nearestvtxnum+1)%3]].disFromOrigin < Radius){	///	頂点が領域内にあるとき
			//	intersection.push_back( Vec2d(vertices[faces[i].vertices[nearestvtxnum]].pos.x,vertices[faces[i].vertices[nearestvtxnum]].pos.z) );
			//}
			///// 内円とだけ交点をもつとき
			//else if(vertices[faces[i].vertices[nearestvtxnum]].disFromOrigin < radius && vertices[faces[i].vertices[(nearestvtxnum+1)%3]].disFromOrigin < Radius){
			//	//Vec2d vtxXZ = CalcIntersectionPoint(faces[i].vertices[nearestvtxnum],faces[i].vertices[(nearestvtxnum+1)%3],radius,Radius);
			//	tempXZ.push_back( CalcIntersectionPoint(faces[i].vertices[nearestvtxnum],faces[i].vertices[(nearestvtxnum+1)%3],radius,Radius) );
			//	DSTR << "case1 tempXZ[0]" << tempXZ[0] << std::endl;
			//}
			////>	外円とだけ交点をもつとき
			//else if(radius < vertices[faces[i].vertices[nearestvtxnum]].disFromOrigin && vertices[faces[i].vertices[(nearestvtxnum+1)%3]].disFromOrigin < Radius){
			//	//Vec2d vtxXZ = CalcIntersectionPoint(faces[i].vertices[nearestvtxnum],faces[i].vertices[(nearestvtxnum+1)%3],radius,Radius);
			//	tempXZ.push_back( CalcIntersectionPoint(faces[i].vertices[nearestvtxnum],faces[i].vertices[(nearestvtxnum+1)%3],radius,Radius) );
			//	DSTR << "case2 tempXZ[0]" << tempXZ[0] << std::endl;
			//}
			////>		内円と外円と交点を持つ場合
			//else if(vertices[faces[i].vertices[nearestvtxnum]].disFromOrigin < radius  && Radius < vertices[faces[i].vertices[(nearestvtxnum + 1)%3]].disFromOrigin ){	//>	頂点が領域0と1にあるとき
			//	//> 交点を求め、交点の値をvectorに代入する
			//	//Vec4d vtxXZ = CalcIntersectionPoint2(faces[i].vertices[nearestvtxnum],faces[i].vertices[(nearestvtxnum+1)%3],radius,Radius);
			//	DSTR << "nearestvtxnum: " << nearestvtxnum <<std::endl;
			//	DSTR << "(nearestvtxnum+1)%3: " << (nearestvtxnum+1)%3 <<std::endl;
			//	tempXZ = CalcIntersectionPoint2(faces[i].vertices[nearestvtxnum],faces[i].vertices[(nearestvtxnum+1)%3],radius,Radius);
			//	DSTR << "case3 tempXZ[0]" << tempXZ[0] << std::endl;
			//	if(tempXZ.size() >2){DSTR << "case3 tempXZ[1]" << tempXZ[1] << std::endl;}
			//	//> 交点は、内円と、外円と2つ存在する
			//	// 2つの交点に分割
			//	//Vec2d vtxXZ0;
			//	//vtxXZ0[0] = vtxXZ[0];
			//	//vtxXZ0[1] = vtxXZ[1];
			//	//Vec2d vtxXZ1;
			//	//vtxXZ1[2] = vtxXZ[2];
			//	//vtxXZ1[3] = vtxXZ[3];
			//}
			//for(unsigned j=0; j< tempXZ.size();j++){
			//	DSTR << "tempXZ[" << j << "]" << tempXZ[j] << std::endl;
			//}
			
		}	//	if(mayIHheated)
	}	//	for(faces[i]


	//>	Step	1.の実装
	//>	radius:内半径、Radius:外半径,　dqdtAll:単位時間あたりに入ってくる全熱量

	//%%	手順 
	//> isHeated(->nearIHHeater)のfacesの内、3つの節点全部について、特定半径の中に入っているものがあるかどうかをチェックする。
	//>	入っているものを、見つけたら、面積計算をしたいが、ここで、何個の節点が入っているかによって場合分けを行う。
	//>	３つ:faceの面積をfaceSに代入する	1~2つ:面積を近似計算するために、3節点の内、どの節点と節点で構成された辺が内半径rまたは、外半径Rと交わっているか判定
	//>	交わっている場合には、その交点を求める。求めた交点と内半径以上外半径以内の領域にあるface内の節点を用いて、三角形を作り、その面積を求める。

//> 以下コピペ元のソース、実装後に消す
	//>	1.1領域と重複する面積をface毎に求める
	double faceS = 0.0;
	for(unsigned i=0;i < nSurfaceFace; i++){
		if(faces[i].mayIHheated){			// faceの節点のy座標が負の場合→IH加熱の対象節点 円環の範囲内に入っているとは限らない
			unsigned nObinnerVtx = 0;

			unsigned innervtx[4] = {0,0,0,0};
			//>	頂点を入れるvector or 配列		//>	円環領域と重なる形状を計算するために、重なる領域内にあるface頂点、辺との交点をfaceの辺0~2の順にvectorに格納していく。格納時に重複が無いようにする
			std::vector<Vec2d> intersectVtx;
			//Vec2d innertVtx[12];		//>	vectorから配列に変更する→静的メモリ確保,けど、vectorの方がコードは楽?メモリの開放を忘れずに
			unsigned judge[2] = {0,0};		///	judge[0],[1]の順に原点に近い点の判定結果
			if(faces[i].area==0) faces[i].area = CalcTriangleArea(faces[i].vertices[0],faces[i].vertices[1],faces[i].vertices[2]);		// 面積計算が済んでなければ計算する
			
			//> 領域内に入っている点が見つかり次第、faceの頂点を含め、face.vectorに入れる。faceの頂点を入れる場合には、重複がないかを調べる。

			/// faceの各辺の両端の点について条件判定 ////	jは0から順に書いているだけで、faceの辺番号ではない
			for(unsigned j=0; j < 3; j++){
				///	face内の点の組(辺)について、その辺の組が
				/// vtx[(0+j)%3]について,原点に近い順に並び替え	//> == :同じ距離の時、何もしない。
				unsigned vtx0 = faces[i].vertices[(j+0)%3];
				unsigned vtx1 = faces[i].vertices[(j+1)%3];
				if(vertices[vtx0].disFromOrigin > vertices[vtx1].disFromOrigin){
					vtx0 = faces[i].vertices[(j+1)%3];
					vtx1 = faces[i].vertices[(j+0)%3];
				}
				///		   ＼      ＼	
				///  ・	  ・ )  ・   )	 ・
				///			/       /	 	
				///  ↑     ↑   　↑	 ↑	
				/// 原点　内半径　外半径 face頂点	
				/// judge[0]:vtx0, judge[1]:vtx1が領域内にあるかどうか (O(=Origin) < vtx0.disFromOrigin < vtx1.disFromOrigin )
				///	vtx0が r<, < R, else 領域ごとにjudge[]

				///	3点全部が領域内にあるとき、ただvectorに格納するだけでは、重複する頂点が出てきてしまう。
				///	ので、3点が全部領域内にあるときは、push_backの前に、除外する必要がある。
				///	同じ座標の点がvectorに入っていないか確認してから格納する

				//> 関数化する
				if(vertices[vtx0].disFromOrigin < radius){judge[0] =0;}
				else if(Radius < vertices[vtx0].disFromOrigin){judge[0] =2;}
				else{
					///	faces三角形頂点の格納
					judge[0] = 1;
					//> 領域内の頂点を格納
					Vec2d tempcoord = Vec2d(vertices[vtx0].pos.x,vertices[vtx0].pos.z);		
					//>	同じ座標値の点がなければ、格納。findっていう関数があったような気がする.	map ? list?
					unsigned tempjudge =0;
					for(unsigned k =0; k < intersectVtx.size(); k++){
						if(intersectVtx[k] == tempcoord){
							tempjudge += 1;
						}
					}
					if(tempjudge == 0){
						//faces[i].innerIH.push_back(tempcoord);
						faces[i].ihvtx.push_back(tempcoord);
//						DSTR << "tempcoord: " << tempcoord <<std::endl;
					}
					for(unsigned k=0;k < intersectVtx.size();k++){
						DSTR << k << "th: " << intersectVtx[k] << std::endl;
					}
					//DSTR << "faces[i].innerIH: " << faces[i].innerIH.end() << std::endl;
				}
				
				///	vtx1が r<, < R, else 領域ごとにjudge[]
				if(vertices[vtx1].disFromOrigin < radius){judge[1] =0;}
				else if(Radius < vertices[vtx1].disFromOrigin){judge[1] =2;}
				else{
					///	faces三角形頂点の格納
					judge[1] = 1;
					//> 領域内の頂点を格納
					Vec2d tempcoord = Vec2d(vertices[vtx1].pos.x,vertices[vtx1].pos.z);
					//>	同じ座標値の点がなければ、格納。findっていう関数があったような気がする.	map ? list?
					unsigned tempjudge =0;
					for(unsigned k =0; k < intersectVtx.size(); k++){
						if(intersectVtx[k] == tempcoord){
							tempjudge += 1;
						}
					}
					if(tempjudge == 0){
						faces[i].ihvtx.push_back(tempcoord);
//						DSTR << "tempcoord: " << tempcoord <<std::endl;
					}
					for(unsigned k=0;k < intersectVtx.size();k++){
						DSTR << k << "th: " << intersectVtx[k] << std::endl;
					}
					//DSTR << "faces[i].innerIH: " << faces[i].innerIH.end() << std::endl;					//DSTR << "faces[i].innerIH: " << faces[i].innerIH[i].end() << std::endl;
				}

				//> 交点の格納
				/// faceの辺の両端の頂点でjudgeの値が異なる	→	円との交点を求める
				/// 内半径(r),外半径(R)との交点を求める	[0]!=[1]:交点を持つ、どちらかが、r,Rとの交点を持つ
				if(judge[0] == 0 && judge [1]==1 )
				{
					//> radiusとの交点を求める
					//> 求めた交点を代入する
					Vec2d vtxXZ = Vec2d(0.0,0.0);		//> 交点の座標を代入
					//> 交点のvectorに格納
					faces[i].ihvtx.push_back(vtxXZ);
					//faces[i].innerIH.push_back(vtxXZ);
					//intersectVtx.push_back(vtxXZ);
				}
				else if(judge[0] == 1 && judge [1]==2 )
				{
					//> Rとの交点を求める
					//> 求めた交点をvectorに格納する
				}
				else if(judge[0] == 1 && judge [1]==2 )
				{
					//> r,Rとの交点を求める
					//> 求めた交点をvectorに格納する
				}
			}
		}
	}
}

//void PHFemMeshThermo::CalcIHdqdt4(double radius,double Radius,double dqdtAll){
//	///	内半径と外半径の間の節点に熱流束境界条件を設定
//	//> 四面体面の三角形と円環領域の重複部分の形状・面積を求める当たり判定を計算する。
//	//>	切り取り形状に応じた形状関数を求め、熱流束ベクトルの成分に代入し、計算する
//
//	//> 1.円環領域と重なるface三角形の形状を算出する。領域に含まれる頂点、face三角形の辺との交点を求めてvecteorに格納する
//	//>	2.vectorには、辺0,1,2の順に領域内の頂点や交点が入っているが、これを元に三角形分割を行う。三角形分割ができたら、各三角形を求める。三角形の総和を、このfaceの加熱領域とする。
//	//>	3.vectorの点における形状関数を求めて、擬似体積（重なっている面積×形状関数の値）を使って、四面体内の各点における形状関数の面積分を求める。求めた値は、熱流束ベクトルの成分として要素剛性行列の成分に代入する。
//	//>	4.毎ステップ、同じ熱流束の値をベクトル成分に加える
//
//	//>	Step	1.の実装
//	//>	radius:内半径、Radius:外半径,　dqdtAll:単位時間あたりに入ってくる全熱量
//
//	//%%	手順 
//	//> isHeated(->nearIHHeater)のfacesの内、3つの節点全部について、特定半径の中に入っているものがあるかどうかをチェックする。
//	//>	入っているものを、見つけたら、面積計算をしたいが、ここで、何個の節点が入っているかによって場合分けを行う。
//	//>	３つ:faceの面積をfaceSに代入する	1~2つ:面積を近似計算するために、3節点の内、どの節点と節点で構成された辺が内半径rまたは、外半径Rと交わっているか判定
//	//>	交わっている場合には、その交点を求める。求めた交点と内半径以上外半径以内の領域にあるface内の節点を用いて、三角形を作り、その面積を求める。
//
//	//>	1.1領域と重複する面積をface毎に求める
//	double faceS = 0.0;
//	for(unsigned i=0;i < nSurfaceFace; i++){
//		if(faces[i].mayIHheated){			// faceの節点のy座標が負の場合→IH加熱の対象節点 円環の範囲内に入っているとは限らない
//			unsigned nObinnerVtx = 0;
//
//			unsigned innervtx[4] = {0,0,0,0};
//			//>	頂点を入れるvector or 配列		//>	円環領域と重なる形状を計算するために、重なる領域内にあるface頂点、辺との交点をfaceの辺0~2の順にvectorに格納していく。格納時に重複が無いようにする
//			std::vector<Vec2d> intersectVtx;
//			//Vec2d innertVtx[12];		//>	vectorから配列に変更する→静的メモリ確保,けど、vectorの方がコードは楽?メモリの開放を忘れずに
//			unsigned judge[2] = {0,0};		///	judge[0],[1]の順に原点に近い点の判定結果
//			if(faces[i].area==0) faces[i].area = CalcTriangleArea(faces[i].vertices[0],faces[i].vertices[1],faces[i].vertices[2]);		// 面積計算が済んでなければ計算する
//			
//			//> 領域内に入っている点が見つかり次第、faceの頂点を含め、face.vectorに入れる。faceの頂点を入れる場合には、重複がないかを調べる。
//
//			/// faceの各辺の両端の点について条件判定 ////	jは0から順に書いているだけで、faceの辺番号ではない
//			for(unsigned j=0; j < 3; j++){
//				///	face内の点の組(辺)について、その辺の組が
//				/// vtx[(0+j)%3]について,原点に近い順に並び替え	//> == :同じ距離の時、何もしない。
//				unsigned vtx0 = faces[i].vertices[(j+0)%3];
//				unsigned vtx1 = faces[i].vertices[(j+1)%3];
//				if(vertices[vtx0].disFromOrigin > vertices[vtx1].disFromOrigin){
//					vtx0 = faces[i].vertices[(j+1)%3];
//					vtx1 = faces[i].vertices[(j+0)%3];
//				}
//				///		   ＼      ＼	
//				///  ・	  ・ )  ・   )	 ・
//				///			/       /	 	
//				///  ↑     ↑   　↑	 ↑	
//				/// 原点　内半径　外半径 face頂点	
//				/// judge[0]:vtx0, judge[1]:vtx1が領域内にあるかどうか (O(=Origin) < vtx0.disFromOrigin < vtx1.disFromOrigin )
//				///	vtx0が r<, < R, else 領域ごとにjudge[]
//
//				///	3点全部が領域内にあるとき、ただvectorに格納するだけでは、重複する頂点が出てきてしまう。
//				///	ので、3点が全部領域内にあるときは、push_backの前に、除外する必要がある。
//				///	同じ座標の点がvectorに入っていないか確認してから格納する
//
//				//> 関数化する
//				if(vertices[vtx0].disFromOrigin < radius){judge[0] =0;}
//				else if(Radius < vertices[vtx0].disFromOrigin){judge[0] =2;}
//				else{
//					///	faces三角形頂点の格納
//					judge[0] = 1;
//					//> 領域内の頂点を格納
//					Vec2d tempcoord = Vec2d(vertices[vtx0].pos.x,vertices[vtx0].pos.z);		
//					//>	同じ座標値の点がなければ、格納。findっていう関数があったような気がする.	map ? list?
//					unsigned tempjudge =0;
//					for(unsigned k =0; k < intersectVtx.size(); k++){
//						if(intersectVtx[k] == tempcoord){
//							tempjudge += 1;
//						}
//					}
//					if(tempjudge == 0){
//						faces[i].ihvtx.push_back(tempcoord);
//						DSTR << "tempcoord: " << tempcoord <<std::endl;
//					}
//					for(unsigned k=0;k < intersectVtx.size();k++){
//						DSTR << k << "th: " << intersectVtx[k] << std::endl;
//					}
//					//DSTR << "faces[i].innerIH: " << faces[i].innerIH.end() << std::endl;
//				}
//				
//				///	vtx1が r<, < R, else 領域ごとにjudge[]
//				if(vertices[vtx1].disFromOrigin < radius){judge[1] =0;}
//				else if(Radius < vertices[vtx1].disFromOrigin){judge[1] =2;}
//				else{
//					///	faces三角形頂点の格納
//					judge[1] = 1;
//					//> 領域内の頂点を格納
//					Vec2d tempcoord = Vec2d(vertices[vtx1].pos.x,vertices[vtx1].pos.z);
//					//>	同じ座標値の点がなければ、格納。findっていう関数があったような気がする.	map ? list?
//					unsigned tempjudge =0;
//					for(unsigned k =0; k < intersectVtx.size(); k++){
//						if(intersectVtx[k] == tempcoord){
//							tempjudge += 1;
//						}
//					}
//					if(tempjudge == 0){
//						faces[i].ihvtx.push_back(tempcoord);
//						DSTR << "tempcoord: " << tempcoord <<std::endl;
//					}
//					for(unsigned k=0;k < intersectVtx.size();k++){
//						DSTR << k << "th: " << intersectVtx[k] << std::endl;
//					}
//					//DSTR << "faces[i].innerIH: " << faces[i].innerIH.end() << std::endl;					//DSTR << "faces[i].innerIH: " << faces[i].innerIH[i].end() << std::endl;
//				}
//
//				//> 交点の格納
//				/// faceの辺の両端の頂点でjudgeの値が異なる	→	円との交点を求める
//				/// 内半径(r),外半径(R)との交点を求める	[0]!=[1]:交点を持つ、どちらかが、r,Rとの交点を持つ
//				if(judge[0] == 0 && judge [1]==1 )
//				{
//					//> radiusとの交点を求める
//					//> 求めた交点を代入する
//					Vec2d vtxXZ = Vec2d(0.0,0.0);		//> 交点の座標を代入
//					//> 交点のvectorに格納
//					faces[i].ihvtx.push_back(vtxXZ);
//					//intersectVtx.push_back(vtxXZ);
//				}
//				else if(judge[0] == 1 && judge [1]==2 )
//				{
//					//> Rとの交点を求める
//					//> 求めた交点をvectorに格納する
//				}
//				else if(judge[0] == 1 && judge [1]==2 )
//				{
//					//> r,Rとの交点を求める
//					//> 求めた交点をvectorに格納する
//				}
//			}
//			
//
//
//			/// 以下、コピペ前のコードのため、多分、不要。交点を求めるアルゴリズムだけ、上に転用する
////
////			//> 置き換えと0,1を入れ替えるだけ(=ポリモーフィズム)で残りの実装も作る
////				// 下記処理を関数化する?。点のIDを入れれば、同じ処理をする関数
////				//> 0-1で交点を作っているとき true false は論演算の排他的論理和XORでtrue時、交点を計算する 00=0,01=10=1,11=0; 片方の点が範囲内で、もう一方が範囲外の場合、trueになる
////			
////				//> 円環の範囲内に入っていたら、mayIHheatedをtrueに、あるいは、falseにする(一応)
////
////				/// faces[i]の頂点が何個、領域内に入っているかを見つける
////			for(unsigned j=0;j<3;j++){
////				if( radius <= vertices[faces[i].vertices[j]].disFromOrigin && vertices[faces[i].vertices[j]].disFromOrigin <= Radius){
////					nObinnerVtx += 1;
////					innervtx[j] = 1;
////				}
////			}
////
////			//> nObinnerVtxの値で条件分岐
////			
////			///	faceの辺が、円弧と交わる交点を求める
////			if(nObinnerVtx == 1){
////				Vec2d vtxXY[3];			/// faceの辺と半径r,Rとの交点のx,z座標　最大３点
////				for(unsigned j=0;j<3;j++){		///	faceを構成する3頂点について
////					unsigned k = 0;
////					unsigned m = 0;
////					k = (j+1)%3;		///	j=0;k=1, j=1;k=2, j=2;k=0 
////					m = (j+2)%3;
////					//vtxXY[j] = CalcIntersectionPoint(faces[i].vertices[j],faces[i].vertices[k],r,R);
////					//DSTR << "vtxXY[" << j << "]: " << vtxXY[j] << std::endl; 
////					if(innervtx[j] ==1){			/// faces[i]のj番目の節点が円環領域内に入っている
////					/// j番目の頂点とエッジを構成する点(他の２点)を使って、半径r,Rと交わる点を算出する						
////					//> j 0,1,2
////						if(j == 0){	/// 頂点jと辺を作る頂点を使って、辺と交わる交点を求める
////							//k = (j+1)%3;		///	j=0;k=1, j=1;k=2, j=2;k=0 
////						vtxXY[j] = CalcIntersectionPoint(faces[i].vertices[j],faces[i].vertices[k],radius,Radius);
////						vtxXY[k] = CalcIntersectionPoint(faces[i].vertices[j],faces[i].vertices[m],radius,Radius);
////						}
////						else if(j == 1){
////						vtxXY[j] = CalcIntersectionPoint(faces[i].vertices[j],faces[0].vertices[1],radius,Radius);
////						vtxXY[j] = CalcIntersectionPoint(faces[i].vertices[j],faces[2].vertices[2],radius,Radius);
////						}
////						else if(j == 2){
////						vtxXY[j] = CalcIntersectionPoint(faces[i].vertices[j],faces[0].vertices[1],radius,Radius);
////						vtxXY[j] = CalcIntersectionPoint(faces[i].vertices[j],faces[1].vertices[2],radius,Radius);
////						}
////						else{
////							assert(0);
////							DSTR << "Error in PHFemMeshThermo::CalcIHdqdt2(hogehoge) 領域判定にミス" << std::endl;
////						}
////					}
////				}
////			}
////				//for(unsigned j=0;j<3;j++){
////				//	/// face内の節点を順番にチェックする
////				//	unsigned k =0;
////				//	k = (j+1)%3;		///	j=0;k=1, j=1;k=2, j=2;k=0 
////				//	if( r <= vertices[faces[i].vertices[j]].disFromOrigin && vertices[faces[i].vertices[j]].disFromOrigin <= R){
////				//	}
////				//	if( r <= vertices[faces[i].vertices[j]].disFromOrigin && vertices[faces[i].vertices[j]].disFromOrigin <= R){
////				//		//> j,kをfaces[i].vertices[j],faces[i].vertices[k]として代入
////				//		CalcIntersectionPoint(faces[i].vertices[0],faces[i].vertices[1],r,R);
////				//	}
////				//}
////
////				//> r -> radius
////				//> R -> Radius
////			
////				/// 座標(consX ,constY)が円と三角形の辺との交点
////			
////				//unsigned vtxId0 = faces[i].vertices[0];
////				//unsigned vtxId1 = faces[i].vertices[1];
////		
//////			free(innervtx);		//> メモリの解放	//配列は動的メモリではないのでfreeで開放できない
////		
////		
////			////> 1-2で交点を作っているとき	//
////			//else if( (r <= vertices[faces[i].vertices[1]].disFromOrigin && vertices[faces[i].vertices[1]].disFromOrigin <= R) ^ (vertices[faces[i].vertices[2]].disFromOrigin && vertices[faces[i].vertices[2]].disFromOrigin <= R)){
////			//	//> 円環との交点を求める
////			//	int katoon00 =0;
////			//}
////			////> 0-2で交点を作っているとき
////			//else if( (r <= vertices[faces[i].vertices[0]].disFromOrigin && vertices[faces[i].vertices[0]].disFromOrigin <= R) ^ (vertices[faces[i].vertices[2]].disFromOrigin && vertices[faces[i].vertices[2]].disFromOrigin <= R)){
////			//	//> 円環との交点を求める
////			//	int katoon000 =0;
////			//}
////
////			//for(unsigned j=0;j<3;j++){
////			//	if( (r <= vertices[faces[i].vertices[j]].disFromOrigin && vertices[faces[i].vertices[j]].disFromOrigin <= R) * (r <= vertices[faces[i].vertices[j+1]].disFromOrigin && vertices[faces[i].vertices[j+1]].disFromOrigin <= R)){
////			//		if(r <= vertices[faces[i].vertices[j]].disFromOrigin && vertices[faces[i].vertices[j]].disFromOrigin <= R)
////			//		nObinnerVtx += 1;
////			//	}
////			//}
////			//if( nObinnerVtx == 1)			faces[i].fluxarea = 1.5/3.0 * faces[i].area;//faces[i].fluxarea = 1.0/3.0 * faces[i].area;
////			//else if(nObinnerVtx == 2)		faces[i].fluxarea = 2.8/3.0 * faces[i].area;//faces[i].fluxarea = 2.0/3.0 * faces[i].area;
////			//else if(nObinnerVtx == 3)		faces[i].fluxarea = faces[i].area;
////			//else if(nObinnerVtx == 0)		faces[i].fluxarea = 0;
////
////			//if(faces[i].fluxarea >= 0){	
////			//	faceS += faces[i].fluxarea;
////			//}else{	assert(0);	}		//	faces[i].fluxareaに0未満の数字が入っているのに加算しようとしている
////			//DSTR << "faces[" << i << "].fluxarea: " << faces[i].fluxarea << std::endl;	
//	}
//
//	//for(unsigned i=0;i < nSurfaceFace; i++){
//	//	DSTR << "faces[" << i << "].fluxarea: " << faces[i].fluxarea << std::endl;
//	//}
//
//		if(faceS > 0){
//			//> dqdt を単位面積あたりに直す([1/m^2])
//			double dqdtds = dqdtAll / faceS;
//	//		DSTR << "dqdtds:  " << dqdtds << std::endl;
//			//>	以下、熱流束をfacesに格納する
//			//>	熱流束の面積計算はfluxareaを用いて行う
//			for(unsigned i=0;i < nSurfaceFace; i++){
//				if(faces[i].mayIHheated){
//					faces[i].heatflux = dqdtds * faces[i].fluxarea;		//	熱流束の量をheatfluxの面積から計算
//	//				DSTR << "faces[" << i <<"].heatflux: " << faces[i].heatflux <<std::endl;
//				}
//			}
//		}
//	
//		//　以上、値は入っているようだ
//
//		int katoon =0;
//		//↑をつかって、CreateMatk2tをコピーした関数で、Vecf2?を作る基に
//
//		//>	熱量は、dqdtdsを用いる
//
//		//> r <= <= Rの中心から放射状に加熱
//
//		//	節点でdqdtの値を更新する
//
//		//　以下は、ベクトルを作る関数の仕事
//		//	節点の属する表面の面で、計算する
//		//  vertices[].heatFluxValueを基に計算を進める
//		//	ガウスザイデル計算できるように処理など、準備する
//	}
//}

void PHFemMeshThermo::CalcIHdqdt3(double r,double R,double dqdtAll,unsigned num){
	///	内半径と外半径の間の節点に熱流束境界条件を設定

	//> 加熱する四面体面の面積の総和を求める
	double faceS = 0.0;
	for(unsigned i=0;i < nSurfaceFace; i++){
		if(faces[i].mayIHheated){			// faceの節点のy座標が負の場合→IH加熱の対象節点
			unsigned nObinnerVtx = 0;
			if(faces[i].area==0) faces[i].area = CalcTriangleArea(faces[i].vertices[0],faces[i].vertices[1],faces[i].vertices[2]);
			for(unsigned j=0;j<3;j++){
				if( r <= vertices[faces[i].vertices[j]].disFromOrigin && vertices[faces[i].vertices[j]].disFromOrigin <= R){
					nObinnerVtx += 1;
				}
			}
			if( nObinnerVtx == 1)			faces[i].fluxarea = 1.5/3.0 * faces[i].area;//faces[i].fluxarea = 1.0/3.0 * faces[i].area;
			else if(nObinnerVtx == 2)		faces[i].fluxarea = 2.8/3.0 * faces[i].area;//faces[i].fluxarea = 2.0/3.0 * faces[i].area;
			else if(nObinnerVtx == 3)		faces[i].fluxarea = faces[i].area;
			else if(nObinnerVtx == 0)		faces[i].fluxarea = 0;

			if(faces[i].fluxarea >= 0){	
				faceS += faces[i].fluxarea;
			}else{		assert(0);	}		//	faces[i].fluxareaに0未満の数字が入っているのに加算しようとしている
			//DSTR << "faces[" << i << "].fluxarea: " << faces[i].fluxarea << std::endl;
		}
	}

	//for(unsigned i=0;i < nSurfaceFace; i++){
	//	DSTR << "faces[" << i << "].fluxarea: " << faces[i].fluxarea << std::endl;
	//}

	if(faceS > 0){
		//> dqdt を単位面積あたりに直す([1/m^2])
		double dqdtds = dqdtAll / faceS;
//		DSTR << "dqdtds:  " << dqdtds << std::endl;
		//>	以下、熱流束をfacesに格納する
		//>	熱流束の面積計算はfluxareaを用いて行う
		for(unsigned i=0;i < nSurfaceFace; i++){
			if(faces[i].mayIHheated){
				faces[i].heatflux[num] = dqdtds * faces[i].fluxarea;		//	熱流束の量をheatfluxの面積から計算
//				DSTR << "faces[" << i <<"].heatflux: " << faces[i].heatflux <<std::endl;
			}
		}
	}
	
	//　以上、値は入っているようだ

	int katoon =0;
	//↑をつかって、CreateMatk2tをコピーした関数で、Vecf2?を作る基に

	//>	熱量は、dqdtdsを用いる

	//> r <= <= Rの中心から放射状に加熱

	//	節点でdqdtの値を更新する

	//　以下は、ベクトルを作る関数の仕事
	//	節点の属する表面の面で、計算する
	//  vertices[].heatFluxValueを基に計算を進める
	//	ガウスザイデル計算できるように処理など、準備する

}

void PHFemMeshThermo::CalcIHdqdt2(double r,double R,double dqdtAll,unsigned num){
	///	内半径と外半径の間の節点に熱流束境界条件を設定
	//> 円環で区切られる四面体面の領域を三角形で近似する

	//> 加熱する四面体面の面積の総和を求める

	//%%	手順 
	//> isHeated(->nearIHHeater)のfacesの内、3つの節点全部について、特定半径の中に入っているものがあるかどうかをチェックする。
	//>	入っているものを、見つけたら、面積計算をしたいが、ここで、何個の節点が入っているかによって場合分けを行う。
	//>	３つ:faceの面積をfaceSに代入する	1~2つ:面積を近似計算するために、3節点の内、どの節点と節点で構成された辺が内半径rまたは、外半径Rと交わっているか判定
	//>	交わっている場合には、その交点を求める。求めた交点と内半径以上外半径以内の領域にあるface内の節点を用いて、三角形を作り、その面積を求める。

	double faceS = 0.0;
	for(unsigned i=0;i < nSurfaceFace; i++){
		if(faces[i].mayIHheated){			// faceの節点のy座標が負の場合→IH加熱の対象節点 円環の範囲内に入っているとは限らない
			unsigned nObinnerVtx = 0;
			unsigned inner[3] = {0,0,0};
			unsigned judge[2] = {0,0};		///	judge[0],[1]の順に原点に近い点の判定結果
			if(faces[i].area==0) faces[i].area = CalcTriangleArea(faces[i].vertices[0],faces[i].vertices[1],faces[i].vertices[2]);		// 面積計算が済んでなければ計算する
				//> 置き換えと0,1を入れ替えるだけ(=ポリモーフィズム)で残りの実装も作る
				// 下記処理を関数化する?。点のIDを入れれば、同じ処理をする関数
				//> 0-1で交点を作っているとき true false は論演算の排他的論理和XORでtrue時、交点を計算する 00=0,01=10=1,11=0; 片方の点が範囲内で、もう一方が範囲外の場合、trueになる
			
				//> 円環の範囲内に入っていたら、mayIHheatedをtrueに、あるいは、falseにする(一応)

				/// faces[i]の頂点が何個、領域内に入っているかを見つける
				for(unsigned j=0;j<3;j++){
					if( r <= vertices[faces[i].vertices[j]].disFromOrigin && vertices[faces[i].vertices[j]].disFromOrigin <= R){
						nObinnerVtx += 1;
						inner[j] = 1;
					}
				}

				//> nObinnerVtxの値で条件分岐
			
				///	
				if(nObinnerVtx == 1){
					Vec2d vtxXY[3];			/// faceの辺と半径r,Rとの交点のx,z座標　最大３点
					for(unsigned j=0;j<3;j++){		///	faceを構成する3頂点について
						unsigned k = 0;
						unsigned m = 0;
						k = (j+1)%3;		///	j=0;k=1, j=1;k=2, j=2;k=0 
						m = (j+2)%3;
						//vtxXY[j] = CalcIntersectionPoint(faces[i].vertices[j],faces[i].vertices[k],r,R);
						//DSTR << "vtxXY[" << j << "]: " << vtxXY[j] << std::endl; 
						if(inner[j] ==1){			/// faces[i]のj番目の節点が円環領域内に入っている
						/// j番目の頂点とエッジを構成する点(他の２点)を使って、半径r,Rと交わる点を算出する						
						//> j 0,1,2
							if(j == 0){	/// 頂点jと辺を作る頂点を使って、辺と交わる交点を求める
								//k = (j+1)%3;		///	j=0;k=1, j=1;k=2, j=2;k=0 
							vtxXY[j] = CalcIntersectionPoint(faces[i].vertices[j],faces[i].vertices[k],r,R);
							vtxXY[k] = CalcIntersectionPoint(faces[i].vertices[j],faces[i].vertices[m],r,R);
							}
							else if(j == 1){
							vtxXY[j] = CalcIntersectionPoint(faces[i].vertices[j],faces[0].vertices[1],r,R);
							vtxXY[j] = CalcIntersectionPoint(faces[i].vertices[j],faces[2].vertices[2],r,R);
							}
							else if(j == 2){
							vtxXY[j] = CalcIntersectionPoint(faces[i].vertices[j],faces[0].vertices[1],r,R);
							vtxXY[j] = CalcIntersectionPoint(faces[i].vertices[j],faces[1].vertices[2],r,R);
							}
							else{
								assert(0);
								DSTR << "Error in PHFemMeshThermo::CalcIHdqdt2(hogehoge) 領域判定にミス" << std::endl;
							}
						 }
					}
				}

				//for(unsigned j=0;j<3;j++){
				//	/// face内の節点を順番にチェックする
				//	unsigned k =0;
				//	k = (j+1)%3;		///	j=0;k=1, j=1;k=2, j=2;k=0 
				//	if( r <= vertices[faces[i].vertices[j]].disFromOrigin && vertices[faces[i].vertices[j]].disFromOrigin <= R){
				//	}
				//	if( r <= vertices[faces[i].vertices[j]].disFromOrigin && vertices[faces[i].vertices[j]].disFromOrigin <= R){
				//		//> j,kをfaces[i].vertices[j],faces[i].vertices[k]として代入
				//		CalcIntersectionPoint(faces[i].vertices[0],faces[i].vertices[1],r,R);
				//	}
				//}

				//> r -> radius
				//> R -> Radius
			
				/// 座標(consX ,constY)が円と三角形の辺との交点
			
				//unsigned vtxId0 = faces[i].vertices[0];
				//unsigned vtxId1 = faces[i].vertices[1];
			}
			////> 1-2で交点を作っているとき	//
			//else if( (r <= vertices[faces[i].vertices[1]].disFromOrigin && vertices[faces[i].vertices[1]].disFromOrigin <= R) ^ (vertices[faces[i].vertices[2]].disFromOrigin && vertices[faces[i].vertices[2]].disFromOrigin <= R)){
			//	//> 円環との交点を求める
			//	int katoon00 =0;
			//}
			////> 0-2で交点を作っているとき
			//else if( (r <= vertices[faces[i].vertices[0]].disFromOrigin && vertices[faces[i].vertices[0]].disFromOrigin <= R) ^ (vertices[faces[i].vertices[2]].disFromOrigin && vertices[faces[i].vertices[2]].disFromOrigin <= R)){
			//	//> 円環との交点を求める
			//	int katoon000 =0;
			//}

			//for(unsigned j=0;j<3;j++){
			//	if( (r <= vertices[faces[i].vertices[j]].disFromOrigin && vertices[faces[i].vertices[j]].disFromOrigin <= R) * (r <= vertices[faces[i].vertices[j+1]].disFromOrigin && vertices[faces[i].vertices[j+1]].disFromOrigin <= R)){
			//		if(r <= vertices[faces[i].vertices[j]].disFromOrigin && vertices[faces[i].vertices[j]].disFromOrigin <= R)
			//		nObinnerVtx += 1;
			//	}
			//}
			//if( nObinnerVtx == 1)			faces[i].fluxarea = 1.5/3.0 * faces[i].area;//faces[i].fluxarea = 1.0/3.0 * faces[i].area;
			//else if(nObinnerVtx == 2)		faces[i].fluxarea = 2.8/3.0 * faces[i].area;//faces[i].fluxarea = 2.0/3.0 * faces[i].area;
			//else if(nObinnerVtx == 3)		faces[i].fluxarea = faces[i].area;
			//else if(nObinnerVtx == 0)		faces[i].fluxarea = 0;

			//if(faces[i].fluxarea >= 0){	
			//	faceS += faces[i].fluxarea;
			//}else{	assert(0);	}		//	faces[i].fluxareaに0未満の数字が入っているのに加算しようとしている
			//DSTR << "faces[" << i << "].fluxarea: " << faces[i].fluxarea << std::endl;
		}

	//for(unsigned i=0;i < nSurfaceFace; i++){
	//	DSTR << "faces[" << i << "].fluxarea: " << faces[i].fluxarea << std::endl;
	//}

	if(faceS > 0){
		//> dqdt を単位面積あたりに直す([1/m^2])
		double dqdtds = dqdtAll / faceS;
//		DSTR << "dqdtds:  " << dqdtds << std::endl;
		//>	以下、熱流束をfacesに格納する
		//>	熱流束の面積計算はfluxareaを用いて行う
		for(unsigned i=0;i < nSurfaceFace; i++){
			if(faces[i].mayIHheated){
				faces[i].heatflux[num] = dqdtds * faces[i].fluxarea;		//	熱流束の量をheatfluxの面積から計算
//				DSTR << "faces[" << i <<"].heatflux: " << faces[i].heatflux <<std::endl;
			}
		}
	}
	//　以上、値は入っているようだ
	int katoon =0;
	//↑をつかって、CreateMatk2tをコピーした関数で、Vecf2?を作る基に

	//>	熱量は、dqdtdsを用いる

	//> r <= <= Rの中心から放射状に加熱

	//	節点でdqdtの値を更新する

	//　以下は、ベクトルを作る関数の仕事
	//	節点の属する表面の面で、計算する
	//  vertices[].heatFluxValueを基に計算を進める
	//	ガウスザイデル計算できるように処理など、準備する
}

Vec2d PHFemMeshThermo::GetIHbandDrawVtx(){	return IHLineVtxX;	}

void PHFemMeshThermo::SetIHbandDrawVtx(double xS, double xE){
	IHLineVtxX = Vec2d(xS,xE);
}

void PHFemMeshThermo::CalcIHdqdtband_(double xS,double xE,double dqdtAll,unsigned num){
	///	x座標：xS~xEの間の節点に熱流束境界条件を設定
	// xS,ｘEの間にいずれか一点がある、
	// 
	// mayIHheatedは使わない	:この条件内で、faceの全節点のy座標が負のものについてのみ、facesSに加算

	//> 加熱する四面体面の面積の総和を求める
	double faceS = 0.0;			// initialize
	for(unsigned i=0;i < nSurfaceFace; i++){
		//if(faces[i].mayIHheated){			// faceの節点のy座標が負の場合→IH加熱の対象節点
			unsigned nObinnerVtx = 0;
			if(faces[i].area==0) faces[i].area = CalcTriangleArea(faces[i].vertices[0],faces[i].vertices[1],faces[i].vertices[2]);
			//..全節点でy<0なら、fluxareaに加算
			if(vertices[faces[i].vertices[0]].pos.y < 0 && vertices[faces[i].vertices[1]].pos.y < 0 && vertices[faces[i].vertices[2]].pos.y < 0){
				for(unsigned j=0;j<3;j++){
					// ||.. :x, |.|.:o , .|.|:o ,   .||.:o  , ..||:x 
					//. どれか１点がxS~xEの間にある、又は、隣り合う2点がバンドの外側にある、IHareaに算入
					if( xS <= vertices[faces[i].vertices[j]].pos.x && vertices[faces[i].vertices[j]].pos.x <= xE){
						faces[i].fluxarea = faces[i].area;
						break;
					}
					//.頂点対がバンドを挟むとき
					if(vertices[faces[i].vertices[j]].pos.x <= xS && xE <= vertices[faces[i].vertices[(j+1)%3]].pos.x 
						|| vertices[faces[i].vertices[(j+1)%3]].pos.x <= xS && xE <= vertices[faces[i].vertices[j]].pos.x ){
							faces[i].fluxarea = faces[i].area;
						break;
					}
				}
			}
			if(faces[i].fluxarea >= 0){	
				faceS += faces[i].fluxarea;
			}else{		assert(0);	}		//	faces[i].fluxareaに0未満の数字が入っているのに加算しようとしている
			//DSTR << "faces[" << i << "].fluxarea: " << faces[i].fluxarea << std::endl;
		//}
	}

	//..face面積に応じた熱流束を全体の面積割合より求める
	if(faceS > 0){
		//> dqdt を単位面積あたりに直す([1/m^2])
		//double dqdt_ds = dqdtAll / faceS;
//		DSTR << "dqdtds:  " << dqdtds << std::endl;
		//>	以下、熱流束をfacesに格納する
		//>	熱流束の面積計算はfluxareaを用いて行う
		for(unsigned i=0;i < nSurfaceFace; i++){
			if(faces[i].fluxarea){
				faces[i].heatflux[num] = faces[i].fluxarea / faceS * dqdtAll;		//	熱流束の量をheatfluxの面積から計算
//				DSTR << "faces[" << i <<"].heatflux: " << faces[i].heatflux <<std::endl;			
				// debug
				//for(unsigned j = 0; j < 3;++j){
 				//	vertices[faces[i].vertices[j]].temp = 100.0;
				//}
			}
		}
	}

	//	デバッグ・表示用
	SetIHbandDrawVtx(xS,xE);
}

void PHFemMeshThermo::CalcIHdqdtband(double xS,double xE,double dqdtAll,unsigned num){
	///	x座標：xS~xEの間の節点に熱流束境界条件を設定

	//> 加熱する四面体面の面積の総和を求める
	double faceS = 0.0;
	for(unsigned i=0;i < nSurfaceFace; i++){
		if(faces[i].mayIHheated){			// faceの節点のy座標が負の場合→IH加熱の対象節点
			unsigned nObinnerVtx = 0;
			if(faces[i].area==0) faces[i].area = CalcTriangleArea(faces[i].vertices[0],faces[i].vertices[1],faces[i].vertices[2]);
			for(unsigned j=0;j<3;j++){
				if( xS <= vertices[faces[i].vertices[j]].pos.x && vertices[faces[i].vertices[j]].pos.x <= xE){
					nObinnerVtx += 1;
				}
			}
			if( nObinnerVtx == 1)			faces[i].fluxarea = faces[i].area;//faces[i].fluxarea = 1.0/3.0 * faces[i].area;
			else if(nObinnerVtx == 2)		faces[i].fluxarea = faces[i].area;//faces[i].fluxarea = 2.0/3.0 * faces[i].area;
			else if(nObinnerVtx == 3)		faces[i].fluxarea = faces[i].area;
			else if(nObinnerVtx == 0)		faces[i].fluxarea = 0.0;

			if(faces[i].fluxarea >= 0){	
				faceS += faces[i].fluxarea;
			}else{		assert(0);	}		//	faces[i].fluxareaに0未満の数字が入っているのに加算しようとしている
			//DSTR << "faces[" << i << "].fluxarea: " << faces[i].fluxarea << std::endl;
		}
	}

	//for(unsigned i=0;i < nSurfaceFace; i++){
	//	DSTR << "faces[" << i << "].fluxarea: " << faces[i].fluxarea << std::endl;
	//}



	if(faceS > 0){
		//> dqdt を単位面積あたりに直す([1/m^2])
		double dqdtds = dqdtAll / faceS;
//		DSTR << "dqdtds:  " << dqdtds << std::endl;
		//>	以下、熱流束をfacesに格納する
		//>	熱流束の面積計算はfluxareaを用いて行う
		for(unsigned i=0;i < nSurfaceFace; i++){
			if(faces[i].mayIHheated){
				faces[i].heatflux[num] = dqdtds * faces[i].fluxarea;		//	熱流束の量をheatfluxの面積から計算
//				DSTR << "faces[" << i <<"].heatflux: " << faces[i].heatflux <<std::endl;
			}
		}
	}
	
	//　以上、値は入っているようだ

	int katoon =0;

	}

void PHFemMeshThermo::CalcIHdqdt_atleast(double r,double R,double dqdtAll,unsigned mode){
	///	内半径と外半径の間の節点に熱流束境界条件を設定
	//	北野天満宮祈願祈念コメント
	//	少しでも領域にかかっていれば、IH加熱に含める

	//> 加熱する四面体面の面積の総和を求める
	double faceS = 0.0;
	for(unsigned i=0;i < nSurfaceFace; i++){
		if(faces[i].mayIHheated){			// faceの節点のy座標が負の場合→IH加熱の対象節点
			unsigned nObinnerVtx = 0;
			if(faces[i].area==0) faces[i].area = CalcTriangleArea(faces[i].vertices[0],faces[i].vertices[1],faces[i].vertices[2]);
			for(unsigned j=0;j<3;j++){
				if( r <= vertices[faces[i].vertices[j]].disFromOrigin && vertices[faces[i].vertices[j]].disFromOrigin <= R){
					//nObinnerVtx += 1;
					faces[i].fluxarea = faces[i].area;
					break;
				}
			}
			//if( nObinnerVtx == 1)			faces[i].fluxarea = faces[i].area;//faces[i].fluxarea = 1.0/3.0 * faces[i].area;
			//else if(nObinnerVtx == 2)		faces[i].fluxarea = faces[i].area;//faces[i].fluxarea = 2.0/3.0 * faces[i].area;
			//else if(nObinnerVtx == 3)		faces[i].fluxarea = faces[i].area;
			//else if(nObinnerVtx == 0)		faces[i].fluxarea = 0.0;

			//if(faces[i].fluxarea >= 0){	
			faceS += faces[i].fluxarea;
			//}else{		assert(0);	}		//	faces[i].fluxareaに0未満の数字が入っているのに加算しようとしている
			//DSTR << "faces[" << i << "].fluxarea: " << faces[i].fluxarea << std::endl;
		}
	}

	//for(unsigned i=0;i < nSurfaceFace; i++){
	//	DSTR << "faces[" << i << "].fluxarea: " << faces[i].fluxarea << std::endl;
	//}

	if(faceS > 0){
		//> dqdt を単位面積あたりに直す([1/m^2])
		double dqdtds = dqdtAll / faceS;
//		DSTR << "dqdtds:  " << dqdtds << std::endl;
		//>	以下、熱流束をfacesに格納する
		//>	熱流束の面積計算はfluxareaを用いて行う
		for(unsigned i=0;i < nSurfaceFace; i++){
			if(faces[i].mayIHheated){
				faces[i].heatflux[mode] = dqdtds * faces[i].fluxarea;		//	熱流束の量をheatfluxの面積から計算
				//debug
				//if(faces[i].fluxarea > 0.0){
				//	int kattonnnn =0;
				//}
//				DSTR << "faces[" << i <<"].heatflux: " << faces[i].heatflux <<std::endl;
			}
		}
	}
	
	//　以上、値は入っているようだ

	int katoon =0;
	//↑をつかって、CreateMatk2tをコピーした関数で、Vecf2?を作る基に

	//>	熱量は、dqdtdsを用いる

	//> r <= <= Rの中心から放射状に加熱

	//	節点でdqdtの値を更新する

	//　以下は、ベクトルを作る関数の仕事
	//	節点の属する表面の面で、計算する
	//  vertices[].heatFluxValueを基に計算を進める
	//	ガウスザイデル計算できるように処理など、準備する

}

void PHFemMeshThermo::CalcIHdqdt(double r,double R,double dqdtAll,unsigned num){
	///	内半径と外半径の間の節点に熱流束境界条件を設定

	//> 加熱する四面体面の面積の総和を求める
	double faceS = 0.0;
	for(unsigned i=0;i < nSurfaceFace; i++){
		if(faces[i].mayIHheated){			// faceの節点のy座標が負の場合→IH加熱の対象節点
			unsigned nObinnerVtx = 0;
			if(faces[i].area==0) faces[i].area = CalcTriangleArea(faces[i].vertices[0],faces[i].vertices[1],faces[i].vertices[2]);
			for(unsigned j=0;j<3;j++){
				if( r <= vertices[faces[i].vertices[j]].disFromOrigin && vertices[faces[i].vertices[j]].disFromOrigin <= R){
					nObinnerVtx += 1;
				}
			}
			if( nObinnerVtx == 1)			faces[i].fluxarea = 1.5/3.0 * faces[i].area;//faces[i].fluxarea = 1.0/3.0 * faces[i].area;
			else if(nObinnerVtx == 2)		faces[i].fluxarea = 2.8/3.0 * faces[i].area;//faces[i].fluxarea = 2.0/3.0 * faces[i].area;
			else if(nObinnerVtx == 3)		faces[i].fluxarea = faces[i].area;
			else if(nObinnerVtx == 0)		faces[i].fluxarea = 0;

			if(faces[i].fluxarea >= 0){	
				faceS += faces[i].fluxarea;
			}else{		assert(0);	}		//	faces[i].fluxareaに0未満の数字が入っているのに加算しようとしている
			//DSTR << "faces[" << i << "].fluxarea: " << faces[i].fluxarea << std::endl;
		}
	}

	//for(unsigned i=0;i < nSurfaceFace; i++){
	//	DSTR << "faces[" << i << "].fluxarea: " << faces[i].fluxarea << std::endl;
	//}

	if(faceS > 0){
		//> dqdt を単位面積あたりに直す([1/m^2])
		double dqdtds = dqdtAll / faceS;
//		DSTR << "dqdtds:  " << dqdtds << std::endl;
		//>	以下、熱流束をfacesに格納する
		//>	熱流束の面積計算はfluxareaを用いて行う
		for(unsigned i=0;i < nSurfaceFace; i++){
			if(faces[i].mayIHheated){
				faces[i].heatflux[num] = dqdtds * faces[i].fluxarea;		//	熱流束の量をheatfluxの面積から計算
//				DSTR << "faces[" << i <<"].heatflux: " << faces[i].heatflux <<std::endl;
			}
		}
	}
	
	//　以上、値は入っているようだ

	int katoon =0;
	//↑をつかって、CreateMatk2tをコピーした関数で、Vecf2?を作る基に

	//>	熱量は、dqdtdsを用いる

	//> r <= <= Rの中心から放射状に加熱

	//	節点でdqdtの値を更新する

	//　以下は、ベクトルを作る関数の仕事
	//	節点の属する表面の面で、計算する
	//  vertices[].heatFluxValueを基に計算を進める
	//	ガウスザイデル計算できるように処理など、準備する

}
/// face毎に作ってしまうのが良いのか、verticesごとにやるのがいいのか。どっちがいいか分からないので、ひとまず、vertices毎に作ってしまおう

void PHFemMeshThermo::SetVertexHeatFlux(int id,double heatFlux){
	vertices[id].heatFluxValue = heatFlux;
}

void PHFemMeshThermo::SetVtxHeatFluxAll(double heatFlux){
	for(unsigned i=0; i < vertices.size() ;i++){
		SetVertexHeatFlux(i,heatFlux);
	}
}

void PHFemMeshThermo::CalcHeatTransUsingGaussSeidel(unsigned NofCyc,double dt){
	//dt = 0.0000000000001 * dt;		//デバッグ用に、dtをものすごく小さくしても、節点0がマイナスになるのか、調べた
	
	//dtはPHFemEngine.cppで取得する動力学シミュレーションのステップ時間
	bool DoCalc =true;											//初回だけ定数ベクトルbの計算を行うbool		//NofCycが0の時にすればいいのかも
	std::ofstream ofs("log.txt");
	for(unsigned i=0; i < NofCyc; i++){							//ガウスザイデルの計算ループ
		if(DoCalc){		
			if(deformed || alphaUpdated){												//D_iiの作成　形状や熱伝達率が更新された際に1度だけ行えばよい
				for(unsigned j =0; j < vertices.size() ; j++){
					//for(unsigned k =0;k < vertices.size(); k++){
					//	DSTR << "dMatCAll "<< k << " : " << dMatCAll[0][k] << std::endl;
					//}
					_dMatAll.resize(1,vertices.size());
					_dMatAll[0][j] = 1.0/ ( 1.0/2.0 * dMatKAll[0][j] + 1.0/dt * dMatCAll[0][j] );		//1 / D__ii	を求める
					//1.0/dt = 500 d
					//DSTR << "dMatKAll : "  << dMatKAll << std::endl;
					//DSTR << "dMatCAll : "  << dMatCAll << std::endl;
					//DSTR << "1.0/dt : " << 1.0/dt <<std::endl;
					//DSTR <<  1.0/dt *dMatCAll[0][j] << std::endl;		//0.001のオーダー
					//DSTR << 1.0/2.0 * dMatKAll[0][j] << std::endl;		//0.0003前後のオーダー
					//値が入っているかをチェック
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
			//	D_jj    2       �冲
			//

			for(unsigned j =0; j < vertices.size() ; j++){		//初回ループだけ	係数ベクトルbVecAllの成分を計算
				bVecAll[j][0] = 0.0;							//bVecAll[j][0]の初期化
				bVecAll_IH[j][0] = 0.0;
				//節点が属すedges毎に　対角成分(j,j)と非対角成分(j,?)毎に計算
				//対角成分は、vertices[j].k or .c に入っている値を、非対角成分はedges[hoge].vertices[0] or vertices[1] .k or .cに入っている値を用いる
				//�@)非対角成分について
				for(unsigned k =0;k < vertices[j].edges.size() ; k++){
					unsigned edgeId = vertices[j].edges[k];
					//リファクタリング	以下の条件分岐についてj>edges[edgeId].vertices[0] とそうでない時とで分けたほうが漏れが出る心配はない？
					if( j != edges[edgeId].vertices[0]){					//節点番号jとedges.vertices[0]が異なる節点番号の時:非対角成分
						unsigned vtxid0 = edges[edgeId].vertices[0];
						bVecAll[j][0] += (-1.0/2.0 * edges[edgeId].k + 1.0/dt * edges[edgeId].c ) * TVecAll[vtxid0];
					}
					else if( j != edges[edgeId].vertices[1] ){			//節点番号jとedges.vertices[1]が異なる節点番号の時:非対角成分
						unsigned vtxid1 = edges[edgeId].vertices[1];
						bVecAll[j][0] += (-1.0/2.0 * edges[edgeId].k + 1.0/dt * edges[edgeId].c ) * TVecAll[vtxid1];

					}
					else{
						//上記のどちらでもない場合、エラー
						DSTR << "edges.vertex has 3 vertexies or any other problem" <<std::endl;
					}
					//	for Debug
					//DSTR << "edges[" << edgeId << "].vertices[0] : " << edges[edgeId].vertices[0] << std::endl;
					//DSTR << "edges[" << edgeId << "].vertices[1] : " << edges[edgeId].vertices[1] << std::endl;
					//int hogeshi =0;
				}
				//�A)対角成分について
				bVecAll[j][0] += (-1.0/2.0 * dMatKAll[0][j] + 1.0/dt * dMatCAll[0][j] ) * TVecAll[j];
				ofs << "bVecAll[" << j <<"][0] : " << bVecAll[j][0] << std::endl;			// DSTR
				//{F}を加算
				bVecAll[j][0] += vecFAllSum[j];		//Fを加算
				//DSTR << " vecFAllSum[" << j << "] : "  << vecFAllSum[j] << std::endl;
				//DSTR << std::endl;
				//D_iiで割る ⇒この場所は、ここで良いの？どこまで掛け算するの？
				bVecAll[j][0] = bVecAll[j][0] * _dMatAll[0][j];
				ofs << "bVecAll[" << j <<"][0] * _dMatAll : " << bVecAll[j][0] << std::endl;
				//	DSTR <<  "bVecAll[" << j <<"][0] * _dMatAll : " << bVecAll[j][0] << std::endl;
				ofs << "TVecAll[" << j <<"] : " << TVecAll[j] << std::endl;
				//	DSTR << "TVecAll[" << j <<"] : " << TVecAll[j] << std::endl;
			}
			DoCalc = false;			//初回のループだけで利用
			//値が入っているか、正常そうかをチェック
			//DSTR << "bVecAll[j][0] : " << std::endl;
			//for(unsigned j =0;j <vertices.size() ; j++){
			//	DSTR << j << " : "<< bVecAll[j][0] << std::endl;
			//}
			int debughogeshi =0;
		}		//if(DoCalc){...}

#ifdef DEBUG
		//	念のため、計算前の初期温度を0にしている。
		if(i == 0){
				for(unsigned j=0;j <vertices.size() ;j++){
					TVecAll[j] = 0.0;
				}
		}
#endif
		//	 1      
		//	--- [F]{T(t+dt)}
		//	D_jj 		
		//[F] = 1/2 [K] +1/dt [C] から対角成分を除し(-1)をかけたもの
		//エッジに入っている成分に-1をかけるのではなく、最後に-1をかける。
		//
		for(unsigned j =0; j < vertices.size() ; j++){
			//T(t+dt) = の式
			//	まずtempkjを作る
			double tempkj = 0.0;			//ガウスザイデルの途中計算で出てくるFの成分計算に使用する一時変数
			for(unsigned k =0;k < vertices[j].edges.size() ; k++){
				unsigned edgeId = vertices[j].edges[k]; 
				if( j != edges[edgeId].vertices[0]){					//節点番号jとedges.vertices[0]が異なる節点番号の時:非対角成分		//OK
					unsigned vtxid0 = edges[edgeId].vertices[0];
					//DSTR << "TVecAll["<< vtxid0<<"] : " << TVecAll[vtxid0] <<std::endl;
					//TVecAll[j] +=_dMatAll[j][0] * -(1.0/2.0 * edges[edgeId].k + 1.0/dt * edges[edgeId].c ) * TVecAll[vtxid0] + bVecAll[j][0]; 
					//DSTR << "j : " << j << ", vtxid0 : " << vtxid0 <<", edges[edgeId].vertices[0] : " << edges[edgeId].vertices[0] <<  std::endl;
					tempkj += (1.0/2.0 * edges[edgeId].k + 1.0/dt * edges[edgeId].c ) * TVecAll[vtxid0];
				}
				else if( j != edges[edgeId].vertices[1] ){			//節点番号jとedges.vertices[1]が異なる節点番号の時:非対角成分
					unsigned vtxid1 = edges[edgeId].vertices[1];
					//DSTR << "TVecAll["<< vtxid1<<"] : " << TVecAll[vtxid1] <<std::endl;
					tempkj += (1.0/2.0 * edges[edgeId].k + 1.0/dt * edges[edgeId].c ) * TVecAll[vtxid1];
				}
				else{
					//上記のどちらでもない場合、エラー
					DSTR << "edges.vertex has 3 vertexies or any other problem" <<std::endl;
				}
				//	for Debug
				//DSTR << "TVecAll:"
				//DSTR << "edges[" << edgeId << "].vertices[0] : " << edges[edgeId].vertices[0] << std::endl;
				//DSTR << "edges[" << edgeId << "].vertices[1] : " << edges[edgeId].vertices[1] << std::endl;
				//int hogeshi =0;
			}
			//	TVecAllの計算
			TVecAll[j] =	_dMatAll[0][j] * ( -1.0 * tempkj) + bVecAll[j][0];			//	-b = D^(-1) [ (-1/2 * K + 1/dt * C ){T(t+dt)} + {F} ]なので、bVecAllはただの加算でよい
			//TVecAll[j] =	_dMatAll[0][j] * ( -1.0 * tempkj) + bVecAll[j][0];   // -b = D^(-1) [ (-1/2 * K + 1/dt * C ){T(t+dt)} + {F} ]なので、bVecAllはただの加算でよい
//			TVecAll[j] =	_dMatAll[0][j] * ( -1.0 * tempkj) + bVecAll[j][0];			//この計算式だと、まともそうな値が出るが・・・理論的にはどうなのか、分からない。。。
			////	for DEBUG
			//int hofgeshi =0;
			//if(TVecAll[j] != 0.0){
			//	DSTR << "!=0 TVecAll["<< j<<"] : " << TVecAll[j] <<std::endl;
			//}
			//DSTR << i << "回目の計算、" << j <<"行目のtempkj: " << tempkj << std::endl;
			//tempkj =0.0;
			ofs << j << std::endl;
			ofs << "tempkj: "<< tempkj << std::endl;
			ofs << "DMatAll[0][j] * ( -1.0 * tempkj) :" <<_dMatAll[0][j] * ( -1.0 * tempkj) << std::endl;
			ofs << "bVecAll[j][0] :  " <<  bVecAll[j][0] << std::endl;
			ofs << "  TVecAll[j] : " << TVecAll[j] << std::endl;
			ofs << std::endl;
		}
		////	for Debug
		//for(unsigned j=0;j < vertices.size();j++){
		//	//DSTR << "tempk" << j << " : " << tempkj << std::endl;
		//	int hogeshi__ =0;
		//	//TVecAll[j]の計算結果を代入する
		//	//定数ベクトルbを上で計算、毎行でbVecAllを減算すればよい。
		//	DSTR << i << "回目の計算の " << "TVecAll[" << j << "] : " << TVecAll[j] << std::endl;
		//}
		//for(unsigned j=0;j < vertices.size();j++){
		//	//DSTR << "tempk" << j << " : " << tempkj << std::endl;
		//	int hogeshi__ =0;
		//	//TVecAll[j]の計算結果を代入する
		//	//定数ベクトルbを上で計算、毎行でbVecAllを減算すればよい。
		//	DSTR << i << "回目の計算の " << "bVecAll[" << j << "][0] : " << bVecAll[j][0] << std::endl;
		//}

		//DSTR << i <<  "th Cyc" << std::endl; 
		//DSTR << i << "回目の計算、TVecAll : " <<std::endl;
		//DSTR << TVecAll << std::endl;
		ofs << i <<  "th Cyc" << std::endl;
		ofs << i << "回目の計算、TVecAll : " <<std::endl;
		ofs << TVecAll << std::endl;
		ofs << "bVecAll: " <<std::endl;
		ofs << bVecAll << std::endl;
		ofs << "_dMatAll: " <<std::endl; 
		ofs << _dMatAll <<std::endl;
		int piyopiyoyo =0;
		double tempTemp=0.0;
		for(unsigned j=0;j <vertices.size() ; j++){
			tempTemp += TVecAll[j];
		}
		//	DSTR
		ofs << i <<"回目の計算時の　全節点の温度の和 : " << tempTemp << std::endl;
		ofs << std::endl;
	}
//	deformed = true;
}

#define FEMLOG(x)
//#define FEMLOG(x) x
void PHFemMeshThermo::CalcHeatTransUsingGaussSeidel(unsigned NofCyc,double dt,double eps){
	//dt = 0.0000000000001 * dt;		//デバッグ用に、dtをものすごく小さくしても、節点0がマイナスになるのか、調べた
	double _eps = 1-eps;			// 1-epsの計算に利用
	//dtはPHFemEngine.cppで取得する動力学シミュレーションのステップ時間
	bool DoCalc =true;											//初回だけ定数ベクトルbの計算を行うbool		//NofCycが0の時にすればいいのかも
	FEMLOG( std::ofstream ofs("log.txt") ) ;
	for(unsigned i=0; i < NofCyc; i++){							//ガウスザイデルの計算ループ
		if(DoCalc){												
			if(deformed || alphaUpdated ){												//D_iiの作成　形状が更新された際に1度だけ行えばよい
				for(unsigned j =0; j < vertices.size() ; j++){
					//for(unsigned k =0;k < vertices.size(); k++){
					//	DSTR << "dMatCAll "<< k << " : " << dMatCAll[0][k] << std::endl;
					//}
					_dMatAll.resize(1,vertices.size());
					_dMatAll[0][j] = 1.0/ ( eps * dMatKAll[0][j] + 1.0/dt * dMatCAll[0][j] );		//1 / D__ii	を求める
					//1.0/dt = 500 d
					//DSTR << "dMatKAll : "  << dMatKAll << std::endl;
					//DSTR << "dMatCAll : "  << dMatCAll << std::endl;
					//DSTR << "1.0/dt : " << 1.0/dt <<std::endl;
					//DSTR <<  1.0/dt *dMatCAll[0][j] << std::endl;		//0.001のオーダー
					//DSTR << 1.0/2.0 * dMatKAll[0][j] << std::endl;		//0.0003前後のオーダー
					//値が入っているかをチェック
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
			//	D_jj    2       �冲
			//

			for(unsigned j =0; j < vertices.size() ; j++){		//初回ループだけ	係数ベクトルbVecAllの成分を計算
				bVecAll[j][0] = 0.0;							//bVecAll[j][0]の初期化
				//節点が属すedges毎に　対角成分(j,j)と非対角成分(j,?)毎に計算
				//対角成分は、vertices[j].k or .c に入っている値を、非対角成分はedges[hoge].vertices[0] or vertices[1] .k or .cに入っている値を用いる
				//�@)非対角成分について
				for(unsigned k =0;k < vertices[j].edges.size() ; k++){
					unsigned edgeId = vertices[j].edges[k];
					//リファクタリング	以下の条件分岐についてj>edges[edgeId].vertices[0] とそうでない時とで分けたほうが漏れが出る心配はない？
					if( j != edges[edgeId].vertices[0]){					//節点番号jとedges.vertices[0]が異なる節点番号の時:非対角成分
						unsigned vtxid0 = edges[edgeId].vertices[0];
						bVecAll[j][0] += (-_eps * edges[edgeId].k + 1.0/dt * edges[edgeId].c ) * TVecAll[vtxid0];
					}
					else if( j != edges[edgeId].vertices[1] ){			//節点番号jとedges.vertices[1]が異なる節点番号の時:非対角成分
						unsigned vtxid1 = edges[edgeId].vertices[1];
						bVecAll[j][0] += (-_eps * edges[edgeId].k + 1.0/dt * edges[edgeId].c ) * TVecAll[vtxid1];

					}
					else{
						//上記のどちらでもない場合、エラー
						DSTR << "edges.vertex has 3 vertexies or any other problem" <<std::endl;
					}
					//	for Debug
					//DSTR << "edges[" << edgeId << "].vertices[0] : " << edges[edgeId].vertices[0] << std::endl;
					//DSTR << "edges[" << edgeId << "].vertices[1] : " << edges[edgeId].vertices[1] << std::endl;
					//int hogeshi =0;
				}
				//�A)対角成分について
				bVecAll[j][0] += (-_eps * dMatKAll[0][j] + 1.0/dt * dMatCAll[0][j] ) * TVecAll[j];
				FEMLOG(ofs << "bVecAll[" << j <<"][0] : " << bVecAll[j][0] << std::endl;)			// DSTR
				FEMLOG(ofs << "dMatKAll[0][" << j <<"] : " << dMatKAll[0][j] << std::endl;)			// DSTR
				FEMLOG(ofs << "dMatCAll[0][" << j <<"] : " << dMatCAll[0][j] << std::endl;)			// DSTR
				//{F}を加算
				bVecAll[j][0] += vecFAllSum[j];		//Fを加算
				//DSTR << " vecFAllSum[" << j << "] : "  << vecFAllSum[j] << std::endl;
				//DSTR << std::endl;
				//D_iiで割る ⇒この場所は、ここで良いの？どこまで掛け算するの？
				bVecAll[j][0] = bVecAll[j][0] * _dMatAll[0][j];
				FEMLOG(ofs << "bVecAll[" << j <<"][0] * _dMatAll : " << bVecAll[j][0] << std::endl);
				FEMLOG(ofs << "TVecAll[" << j <<"] : " << TVecAll[j] << std::endl);
			}
			DoCalc = false;			//初回のループだけで利用
			//値が入っているか、正常そうかをチェック
			//DSTR << "bVecAll[j][0] : " << std::endl;
			//for(unsigned j =0;j <vertices.size() ; j++){
			//	DSTR << j << " : "<< bVecAll[j][0] << std::endl;
			//}
			int debughogeshi =0;
		}		//if(DoCalc){...}

#ifdef DEBUG
		//	念のため、計算前の初期温度を0にしている。
		if(i == 0){
				for(unsigned j=0;j <vertices.size() ;j++){
					TVecAll[j] = 0.0;
				}
		}
#endif
		//	 1      
		//	--- [F]{T(t+dt)}
		//	D_jj 		
		//[F] = eps(ilon) [K] +1/dt [C] から対角成分を除し(-1)をかけたもの
		//エッジに入っている成分に-1をかけるのではなく、最後に-1をかける。
		//
		for(unsigned j =0; j < vertices.size() ; j++){
			//T(t+dt) = の式
			//	まずtempkjを作る
			double tempkj = 0.0;			//ガウスザイデルの途中計算で出てくるFの成分計算に使用する一時変数
			for(unsigned k =0;k < vertices[j].edges.size() ; k++){
				unsigned edgeId = vertices[j].edges[k]; 
				if( j != edges[edgeId].vertices[0]){					//節点番号jとedges.vertices[0]が異なる節点番号の時:非対角成分		//OK
					unsigned vtxid0 = edges[edgeId].vertices[0];
					//DSTR << "TVecAll["<< vtxid0<<"] : " << TVecAll[vtxid0] <<std::endl;
					//TVecAll[j] +=_dMatAll[j][0] * -(1.0/2.0 * edges[edgeId].k + 1.0/dt * edges[edgeId].c ) * TVecAll[vtxid0] + bVecAll[j][0]; 
					//DSTR << "j : " << j << ", vtxid0 : " << vtxid0 <<", edges[edgeId].vertices[0] : " << edges[edgeId].vertices[0] <<  std::endl;
					tempkj += (eps * edges[edgeId].k + 1.0/dt * edges[edgeId].c ) * TVecAll[vtxid0];
				}
				else if( j != edges[edgeId].vertices[1] ){			//節点番号jとedges.vertices[1]が異なる節点番号の時:非対角成分
					unsigned vtxid1 = edges[edgeId].vertices[1];
					//DSTR << "TVecAll["<< vtxid1<<"] : " << TVecAll[vtxid1] <<std::endl;
					tempkj += (eps * edges[edgeId].k + 1.0/dt * edges[edgeId].c ) * TVecAll[vtxid1];
				}
				else{
					//上記のどちらでもない場合、エラー
					DSTR << "edges.vertex has 3 vertexies or any other problem" <<std::endl;
				}
				//	for Debug
				//DSTR << "TVecAll:"
				//DSTR << "edges[" << edgeId << "].vertices[0] : " << edges[edgeId].vertices[0] << std::endl;
				//DSTR << "edges[" << edgeId << "].vertices[1] : " << edges[edgeId].vertices[1] << std::endl;
				//int hogeshi =0;
			}
			//	TVecAllの計算
			TVecAll[j] =	_dMatAll[0][j] * ( -1.0 * tempkj) + bVecAll[j][0];			//	-b = D^(-1) [ (-1/2 * K + 1/dt * C ){T(t+dt)} + {F} ]なので、bVecAllはただの加算でよい
			//TVecAll[j] =	_dMatAll[0][j] * ( -1.0 * tempkj) + bVecAll[j][0];   // -b = D^(-1) [ (-1/2 * K + 1/dt * C ){T(t+dt)} + {F} ]なので、bVecAllはただの加算でよい
//			TVecAll[j] =	_dMatAll[0][j] * ( -1.0 * tempkj) + bVecAll[j][0];			//この計算式だと、まともそうな値が出るが・・・理論的にはどうなのか、分からない。。。
			////	for DEBUG
			//int hofgeshi =0;
			//if(TVecAll[j] != 0.0){
			//	DSTR << "!=0 TVecAll["<< j<<"] : " << TVecAll[j] <<std::endl;
			//}
			//DSTR << i << "回目の計算、" << j <<"行目のtempkj: " << tempkj << std::endl;
			//tempkj =0.0;
			FEMLOG(ofs << j << std::endl);
			FEMLOG(ofs << "tempkj: "<< tempkj << std::endl);
			FEMLOG(ofs << "DMatAll[0][j] * ( -1.0 * tempkj) :" <<_dMatAll[0][j] * ( -1.0 * tempkj) << std::endl;)
			FEMLOG(ofs << "bVecAll[j][0] :  " <<  bVecAll[j][0] << std::endl;)
			FEMLOG(ofs << "  TVecAll[j] : " << TVecAll[j] << std::endl;)
			FEMLOG(ofs << std::endl;)
		}
		////	for Debug
		//for(unsigned j=0;j < vertices.size();j++){
		//	//DSTR << "tempk" << j << " : " << tempkj << std::endl;
		//	int hogeshi__ =0;
		//	//TVecAll[j]の計算結果を代入する
		//	//定数ベクトルbを上で計算、毎行でbVecAllを減算すればよい。
		//	DSTR << i << "回目の計算の " << "TVecAll[" << j << "] : " << TVecAll[j] << std::endl;
		//}
		//for(unsigned j=0;j < vertices.size();j++){
		//	//DSTR << "tempk" << j << " : " << tempkj << std::endl;
		//	int hogeshi__ =0;
		//	//TVecAll[j]の計算結果を代入する
		//	//定数ベクトルbを上で計算、毎行でbVecAllを減算すればよい。
		//	DSTR << i << "回目の計算の " << "bVecAll[" << j << "][0] : " << bVecAll[j][0] << std::endl;
		//}

		//DSTR << i <<  "th Cyc" << std::endl; 
		//DSTR << i << "回目の計算、TVecAll : " <<std::endl;
		//DSTR << TVecAll << std::endl;
		FEMLOG(ofs << i <<  "th Cyc" << std::endl;)
		FEMLOG(ofs << i << "回目の計算、TVecAll : " <<std::endl;)
		FEMLOG(ofs << TVecAll << std::endl;)
		FEMLOG(ofs << "bVecAll: " <<std::endl;)
		FEMLOG(ofs << bVecAll << std::endl;)
		FEMLOG(ofs << "_dMatAll: " <<std::endl;) 
		FEMLOG(ofs << _dMatAll <<std::endl;)
		int piyopiyoyo =0;
		double tempTemp=0.0;
		for(unsigned j=0;j <vertices.size() ; j++){
			tempTemp += TVecAll[j];
		}
		//	DSTR
		FEMLOG(ofs << i <<"回目の計算時の　全節点の温度の和 : " << tempTemp << std::endl;)
		FEMLOG(ofs << std::endl;)
	}
//	deformed = true;
}

void PHFemMeshThermo::UpdateVertexTempAll(unsigned size){
	for(unsigned i=0;i < size;i++){
		vertices[i].temp = TVecAll[i];
	}
}
void PHFemMeshThermo::UpdateVertexTemp(unsigned vtxid){
		vertices[vtxid].temp = TVecAll[vtxid];
}

void PHFemMeshThermo::TexChange(unsigned id,double tz){

}

void PHFemMeshThermo::HeatTransFromPanToFoodShell(){
	//if(pan){
	//	Affinef afPan = pan->GetGRFrame()->GetWorldTransform();
	//	Affinef afMesh = tmesh.obj->GetGRFrame()->GetWorldTransform();
	//	Affinef afMeshToPan = afPan.inv() * afMesh;	
	//}

	//shape pair solid pair
	

	//	2物体の接触面から、加熱する節点を決める。
	//	Shape pair のSolid pare辺りに記述がある

	//	最外殻の節点を世界座標に変換し、そのフライパン又は、鉄板を基にした座標系に変換した座標値が、ある座標以下なら
	//	最外殻の節点に熱伝達する
	//PHSolidIf* phs ;
//	Affinef afPan = phs
	//Affinef afPan = pan->GetGRFrame()->GetWorldTransform();
	//Affinef afMesh = tmesh.obj->GetGRFrame()->GetWorldTransform();
	//Affinef afMeshToPan = afPan.inv() * afMesh;	
//	for(unsigned i=-0; i < surfaceVertices.size();i++){
//		if(vertices[surfaceVertices[i]].pos){};
//		//vertices[surfaceVertices[i]].Tc;
//	}

	//	接触面からの距離が一定距離以内なら

	//	熱伝達境界条件で、熱伝達

	//	熱伝達境界条件で熱伝達後、フライパンの熱は吸熱というか、減るが、それは有限要素法をどのように結合してやればいいのか？
	
	//	節点周囲のTcと熱伝達率αによって、熱が伝わるので、ここで、フライパンから熱を伝えたい節点のTcと熱伝達率を設定する

	//	熱伝達率は、相手との関係によって異なるので、節点が何と接しているかによって変更する


}

void PHFemMeshThermo::DrawEdge(unsigned id0, unsigned id1){
	//Vec3d pos0 = vertices[id0].pos;
	//Vec3d pos1 = vertices[id1].pos;
	//
	//Vec3d wpos0 = 
	//	GetWorldTransform() * pos0; //* ローカル座標を 世界座標への変換して代入
	//Vec3d wpos1 = GetWorldTransform() * pos1; //* ローカル座標を 世界座標への変換して代入
	//glColor3d(1.0,0.0,0.0);
	//glBegin(GL_LINES);
	//	glVertex3f(wpos0[0],wpos0[1],wpos0[2]);
	//	glVertex3f(wpos0[0] + wpos1[0], wpos0[1] + wpos1[1], wpos0[2] + wpos1[2]);
	//glEnd();
	//glFlush();
}




void PHFemMeshThermo::Step(double dt){

	//// cps表示用
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
	////途中時間
	//if (bOneSecond) {
	//	std::cout << "1: " << GetTickCount() - stepStart << std::endl;
	//	cpslog << GetTickCount() - stepStart << ",";
	//} 
	//stepStart = GetTickCount();
	//途中時間
	//if (bOneSecond) { std::cout << "1: " << GetTickCount() - stepStart << std::endl; }
	// %%% CPS表示

	//std::ofstream templog("templog.txt");

	std::ofstream ofs_("log_.txt");
//	ScilabTest();									//	Scilabを使うテスト
	//境界条件を設定:温度の設定
//	UsingFixedTempBoundaryCondition(0,200.0);

	//	実験用のコード
	//	a) 時間離散化時に台形公式利用、前進・後退差分で振動する加熱方式
	//if(StepCount == 0){
	//	unsigned texid_ =7;
	//	UsingFixedTempBoundaryCondition(texid_,200.0);
	//}

	//	b) 断熱過程の実験
	//	熱伝達率を0にする。温度固定境界条件で加熱。

	//	UsingFixedTempBoundaryCondition(3,50.0);
	//for(unsigned i=0 ;i<1;i++){
	//	UsingFixedTempBoundaryCondition(i,200.0);
	//}

	//%%%%		熱伝達境界条件		%%%%//
	//	食材メッシュの表面の節点に、周囲の流体温度を与える
	//	周囲の流体温度は、フライパンの表面温度や、食材のUsingFixedTempBoundaryCondition(0,200.0);液体内の温度の分布から、その場所での周囲流体温度を判別する。
	//	位置座標から判別するコードをここに記述
	//UsingHeatTransferBoundaryCondition(unsigned id,double temp);
	//エネルギー保存則より、周囲流体温度の低下や、流体への供給熱量は制限されるべき

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

	//	test　shapepairを取ってくる
	//GetScene()->
	
	//dt = dt *0.01;		誤差1度程度になる
	//dt = dt;				収束した時の、計算誤差？（マイナスになっている節点温度がそれなりに大きくなる。）
	
	///>	ガウスザイデル法の設定
	//	CalcHeatTransUsingGaussSeidel(20,dt);			//ガウスザイデル法で熱伝導計算を解く　クランクニコルソン法のみを使いたい場合

//	dNdt = 10.0 * dt;

#if 0
	// 解く前にかならず行う
	UpdateVecFAll_frypan(WEEK);				// 引数に加熱強さを与える。(OFF/WEEK/MIDDLE/HIGH)
#endif
	//ガウスザイデル法で解く
	CalcHeatTransUsingGaussSeidel(1,dt,1.0);			//ガウスザイデル法で熱伝導計算を解く 第三引数は、前進・クランクニコルソン・後退積分のいずれかを数値で選択

	//温度を表示してみる
	//DSTR << "vertices[3].temp : " << vertices[3].temp << std::endl;

	//温度のベクトルから節点へ温度の反映
	UpdateVertexTempAll((unsigned)vertices.size());
	//for(unsigned i =0;i<vertices.size();i++){
	//	DSTR << "vertices[" << i << "].temp : " << vertices[i].temp << std::endl;
	//}
	int templogkatoon =0;

	for(unsigned i =0;i<vertices.size();i++){
		if(vertices[i].temp !=0){
			ofs_ << "vertices[" << i << "].temp : " << vertices[i].temp << std::endl;
		}
	}
	int hogehoge=0;

	//	節点の温度の推移履歴の保存
	if(StepCount ==0){
		templog <<"ステップ数"<<",";
		templog <<"熱シミュレーション時間"<<",";
		for(unsigned i=0; i<vertices.size();i++){
			templog << "節点" << i << "の温度" << ",";
		}
		templog <<"," << std::endl;
		}
	templog << StepCount << ", ";
	templog << StepCount * dt << ", ";
	for(unsigned i=0; i<vertices.size();i++){
		templog << vertices[i].temp << ",";
	}
	templog <<"," << std::endl;
	//templog;
	//節点温度を画面に表示する⇒3次元テクスチャを使ったテクスチャ切り替えに値を渡す⇒関数化

	//DSTR << "TVecAll : " <<std::endl;
	//DSTR << TVecAll << std::endl;
	//DSTR << "bVecAll : " <<std::endl;
	//DSTR << bVecAll << std::endl;


	//	for	DEBUG	節点3とエッジ対を作る節点を表示
	//for(unsigned i =0; i < vertices[3].edges.size();i++){
	//	DSTR << "edges[vertices[3].edges[" << i << "]].vertices[0] : " << edges[vertices[3].edges[i]].vertices[0] << std::endl;
	//	DSTR << "edges[vertices[3].edges[" << i << "]].vertices[1] : " << edges[vertices[3].edges[i]].vertices[1] << std::endl;
	//}
	//for(unsigned i =0; i < vertices[3].edges.size();i++){
	//	DSTR << "vertices[3].edges[" << i << "] : " << vertices[3].edges[i] << std::endl;
	//}
	int hogeshi = 0;
	//	for DEBUG
	//（形状が変わったら、マトリクスやベクトルを作り直す）
	//温度変化・最新の時間の{T}縦ベクトルに記載されている節点温度を基に化学変化シミュレーションを行う
		//SetChemicalSimulation();
		//化学変化シミュレーションに必要な温度などのパラメータを渡す
	//温度変化や化学シミュレーションの結果はグラフィクス表示を行う
	StepCount += 1;
	if(StepCount >= 1000*1000*1000){
		StepCount = 0;
		StepCount_ += 1;
	}
	int temphogeshi =0;

}

void PHFemMeshThermo::CreateMatrix(){
}

void PHFemMeshThermo::InitTcAll(double temp){
	for(unsigned i =0; i <vertices.size();i++){
		vertices[i].Tc = temp;
	}
}

void PHFemMeshThermo::InitCreateMatC(){
	/// MatCについて	//	使用する行列の初期化
	//dMatCAll：対角行列の成分の入った行列のサイズを定義:配列として利用	幅:vertices.size(),高さ:1
	dMatCAll.resize(1,vertices.size()); //(h,w)
	dMatCAll.clear();								///	値の初期化
	//matcの初期化は、matcを作る関数でやっているので、省略
}

void PHFemMeshThermo::InitCreateMatk_(){
	///	MatKについて
	//matkの初期化
	for(unsigned i =0; i < 4 ; i++){
		for(unsigned j =0; j < 4 ; j++){
			matk[i][j] = 0.0;
		}
	}
	dMatKAll.clear();		///	初期化
#ifdef UseMatAll
	//matKAll.resize(vertices.size(),vertices.size());	///	matKAllのサイズの代入
	matKAll.clear();									///	matKAllの初期化
	//DSTR << "matKAll: " << matKAll <<std::endl;
#endif UseMatAll

#ifdef DumK
	//matKAllの初期化
	matKAll.resize(vertices.size(),vertices.size());
	matkAll.clear();					///	初期化、下の初期化コードは不要
#endif


}
void PHFemMeshThermo::InitCreateMatk(){
	///	MatKについて
	//matkの初期化
	for(unsigned i =0; i < 4 ; i++){
		for(unsigned j =0; j < 4 ; j++){
			matk[i][j] = 0.0;
		}
	}
	//for(unsigned i=0;i<3;i++){
	//tets.matk[i]
	//}
	//DMatAll：対角行列の成分の入った行列のサイズを定義:配列として利用	幅:vertices.size(),高さ:1
	dMatKAll.resize(1,vertices.size()); //(h,w)
	dMatKAll.clear();		///	初期化
	////値の初期化
	//for(unsigned i=0;i<vertices.size();i++){
	//	dMatKAll[0][i] = 0.0;
	//}
#ifdef UseMatAll
	
	matKAll.resize(vertices.size(),vertices.size());	///	matKAllのサイズの代入
	matKAll.clear();									///	matKAllの初期化
	//for(unsigned i=0;i<vertices.size();i++){
	//	for(unsigned j=0;j<vertices.size();j++){
	//		matKAll[i][j] = 0.0;
	//	}
	//}
#endif UseMatAll

#ifdef DumK
	//matKAllの初期化
	matKAll.resize(vertices.size(),vertices.size());
	matkAll.clear();					///	初期化、下の初期化コードは不要
	//for(unsigned i=0;i<vertices.size();i++){
	//	for(unsigned j=0;j<vertices.size();j++){
	//		matKAll[i][j] = 0.0;
	//	}
	//}
#endif
}

void PHFemMeshThermo::InitCreateVecf_(){
	///	Vecfについて
	//Vecfの初期化
	for(unsigned i =0; i < 4 ; i++){
			vecf[i] = 0.0;
	}
	vecFAllSum.clear();						///	初期化
}

void PHFemMeshThermo::InitVecFAlls(){
	for(unsigned i =0; i < 4 ; i++){ vecf[i] = 0.0;}	/// Vecfの初期化
	vecFAllSum.resize(vertices.size());					///	全体剛性ベクトルFのサイズを規定
	vecFAllSum.clear();									///		〃			の初期化

	for(unsigned i=0;i < HIGH +1 ; i++){				/// IH加熱モードの各ベクトルを初期化
		vecFAll_f2IH[i].resize(vertices.size(),1);
		vecFAll_f2IH[i].clear();
	}
	// tets.vecf[4]の初期化
	for(unsigned i=0;i<tets.size();i++){
		for(unsigned j=0; j <4;j++){
			tets[i].vecf[j].clear();
		}
	}
	// vecFAllの初期化
	//initialize
	for(unsigned i =0;i<4;i++){
		vecFAll[i].resize(vertices.size());
		vecFAll[i].clear();
	}
}

void PHFemMeshThermo::UpdateVecF_frypan(){
	// modeは必要か？
	
	//	Initialize
	InitVecFAlls();		// この中の初期化のすべてが必要か？

//. 1)最初の2つ({F2},{F3})は、F2,F3のどちらかだけ更新すれば良い場合に用いる
#if 0
	// {F2}
	for(unsigned tetsid = 0; tetsid < tets.size();tetsid++){
		unsigned id = tetsid;
		CreateVecf2surface(id);			// tets[id].vecf[1];に結果格納
		for(unsigned j =0;j < 4; j++){
			int vtxid0 = tets[id].vertices[j];
			//vecFAll[1][vtxid0] += vecf[j];
			vecFAll[1][vtxid0] += tets[id].vecf[1][j];
		}
	}	
#endif

#if 0
	// {F3}
	for(unsigned tetsid = 0; tetsid < tets.size();tetsid++){
		unsigned id = tetsid;
		CreateVecf3(id);		// tets[id].vecf[2];に結果格納
		//vecf = tets[id].vecf[2];
		for(unsigned j =0;j < 4; j++){
			int vtxid0 = tets[id].vertices[j];
			//vecFAll[1][vtxid0] += vecf[j];
			vecFAll[2][vtxid0] += tets[id].vecf[2][j];
		}
	}	
#endif

//. 2) {F2,F3}の両方共更新して良い場合
#if 1
	for(unsigned tetsid = 0; tetsid < tets.size();tetsid++){
		unsigned id = tetsid;
		CreateVecf2surface(id);
		CreateVecf3(id);		// tets[id].vecf[2];に結果格納
		//vecf = tets[id].vecf[2];
		for(unsigned j =0;j < 4; j++){
			int vtxid0 = tets[id].vertices[j];
			//vecFAll[1][vtxid0] += vecf[j];
			vecFAll[1][vtxid0] += tets[id].vecf[1][j];
			vecFAll[2][vtxid0] += tets[id].vecf[2][j];
		}
	}
#endif

int debugParam =0;

	vecFAllSum.clear();	// 前Stepでの熱入出力を消去
	//Σ{F[i]}_{i=1}^{4}
	vecFAllSum = vecFAll[1] + vecFAll[2];

//%%%% この関数はここまででとりあえず完成 2012.10.09

	//	消去予定
#if 0
//depend on mode, I don't need to use mode state.Because mode state cause different calc result of heatflus.
// I just use the result of IHdqdt Function.
	//vecFAll[1],[2]に代入
	for(unsigned id = 0; id < vertices.size();id++){
		if(mode == OFF){ 
			// F2は加算しない
			vecFAllSum[id] = vecFAllSum[id]; //F3
		}
		else if(mode == WEEK){
			vecFAllSum[id] =  vecFAll_f2IH[mode][id][0] + vecFAll_f3[id][0];//F2+F3		//mode=0 -> F2のWEEKの強さ
		}
		else if(mode == MIDDLE){
			vecFAllSum[id] = vecFAll_f2IH[mode][id][0];//F2+F3		//mode=1 -> F2のmiddleの強さ
		}
		else if(mode == HIGH){
			vecFAllSum[id] = vecFAll_f2IH[mode][id][0];//F2+F3		//mode=2 -> F2のhighの強さ
		}
	}

#endif

#if 0
	for(unsigned i =0; i< 4;i++){
		vecFAllSum += vecFAll[i];				//全体剛性行列の和を取る
	}
	// F2,F3を加算する
#endif
}


void PHFemMeshThermo::UpdateVecF(){
	// modeは必要か？
	
	//	Initialize
	//InitVecFAlls();		// この中の初期化のすべてが必要か？

//. 1)最初の2つ({F2},{F3})は、F2,F3のどちらかだけ更新すれば良い場合に用いる
#if 0
	// {F2}
	for(unsigned tetsid = 0; tetsid < tets.size();tetsid++){
		unsigned id = tetsid;
		CreateVecf2surface(id);			// tets[id].vecf[1];に結果格納
		for(unsigned j =0;j < 4; j++){
			int vtxid0 = tets[id].vertices[j];
			//vecFAll[1][vtxid0] += vecf[j];
			vecFAll[1][vtxid0] += tets[id].vecf[1][j];
		}
	}	
#endif

#if 0
	// {F3}
	for(unsigned tetsid = 0; tetsid < tets.size();tetsid++){
		unsigned id = tetsid;
		CreateVecf3(id);		// tets[id].vecf[2];に結果格納
		//vecf = tets[id].vecf[2];
		for(unsigned j =0;j < 4; j++){
			int vtxid0 = tets[id].vertices[j];
			//vecFAll[1][vtxid0] += vecf[j];
			vecFAll[2][vtxid0] += tets[id].vecf[2][j];
		}
	}	
#endif

//. 2) {F2,F3}の両方共更新して良い場合
#if 0
	for(unsigned tetsid = 0; tetsid < tets.size();tetsid++){
		unsigned id = tetsid;
		CreateVecf2surface(id);
		CreateVecf3(id);		// tets[id].vecf[2];に結果格納
		//vecf = tets[id].vecf[2];
		for(unsigned j =0;j < 4; j++){
			int vtxid0 = tets[id].vertices[j];
			//vecFAll[1][vtxid0] += vecf[j];
			vecFAll[1][vtxid0] += tets[id].vecf[1][j];
			vecFAll[2][vtxid0] += tets[id].vecf[2][j];
		}
	}
#endif

int debugParam =0;

	vecFAllSum.clear();	// 前Stepでの熱入出力を消去
	//Σ{F[i]}_{i=1}^{4}
//	vecFAllSum = vecFAll[1] + vecFAll[2];

//%%%% この関数はここまででとりあえず完成 2012.12.03

#if 0
	for(unsigned i =0; i< 4;i++){
		vecFAllSum += vecFAll[i];				//全体剛性行列の和を取る
	}
	// F2,F3を加算する
#endif
}

void PHFemMeshThermo::UpdateIHheat(unsigned heatingMODE){

	//熱伝導率、密度、比熱、熱伝達率　のパラメーターを設定・代入
		//PHFemMEshThermoのメンバ変数の値を代入 CADThermoより、0.574;//玉ねぎの値//熱伝導率[W/(ｍ・K)]　Cp = 1.96 * (Ndt);//玉ねぎの比熱[kJ/(kg・K) 1.96kJ/(kg K),（玉ねぎの密度）食品加熱の科学p64より970kg/m^3
		//熱伝達率の単位系　W/(m^2 K)⇒これはSI単位系なのか？　25は論文(MEAT COOKING SIMULATION BY FINITE ELEMENTS)のオーブン加熱時の実測値
		//SetInitThermoConductionParam(0.574,970,1.96,25);
	//SetInitThermoConductionParam(0.574,970,0.1960,25 * 0.01);		//> thConduct:熱伝導率 ,roh:密度,	specificHeat:比熱 J/ (K・kg):1960 ,　heatTrans:熱伝達率 W/(m^2・K)
		//これら、変数値は後から計算の途中で変更できるようなSetParam()関数を作っておいたほうがいいかな？

	//.		熱流束の設定
	//..	初期化
	//SetVtxHeatFluxAll(0.0);

	//1.フライパン位置を取ってくる
		//ih加熱円環中心からの同心円状加熱領域を計算し、ihdqdtに当てはめるメッシュ情報を生成
		//　if(フライパンが動いたか)	動いていなければ、vecfも、1step前の値を使えるようにしておきたい。

	//2...	face面での熱流束量を計算（フライパン位置又はポインタを引数に代入：毎回フライパンの位置が変化するので、フライパン位置の変化の度に生成する）
	if(heatingMODE == OFF){
		CalcIHdqdt_atleast(0.0,0.0,0.0, OFF);		//	IH加熱行列の係数0となるため、計算されない
	}
	else if(heatingMODE == WEEK){	
		CalcIHdqdt_atleast(0.11,0.14,231.9 * 5e1, WEEK);		//
	}
	else if(heatingMODE == MIDDLE){
		CalcIHdqdt_atleast(0.11,0.14,231.9 * 0.005 * 1e4, MIDDLE);		//
	}
	else if(heatingMODE == HIGH){
		CalcIHdqdt_atleast(0.11,0.14,231.9 * 0.005 * 1e5, HIGH);		//
	}

	//3.各面での熱流束量から全体剛性ベクトルを作る。{F}に代入

#if 1			// switch1
	UpdateVecF_frypan();
#endif
	//%%	IH加熱のモード切替
	//	ライン状に加熱
	//	CalcIHdqdtband_(0.09,0.10,231.9 * 5e3);		//*0.5*1e4	値を変えて実験	//*1e3　//*1e4 //5e3
	//	円環状に加熱

	//	この後で、熱流束ベクトルを計算する関数を呼び出す
	///	熱伝達率を各節点に格納
	//SetHeatTransRatioToAllVertex();
#if 0			//!switch1
	InitVecFAlls();
	for(unsigned i=0; i < this->tets.size(); i++){
		CreateVecFAll(i);
	}
#endif

#if 0
	CreateVecF2surfaceAll();		//	CreateVecFAll(i);の代わり
	CreateVecF3surfaceAll();		//	CreateVecFAll(i);の代わり
#endif
}

void PHFemMeshThermo::SetParamAndReCreateMatrix(double thConduct0,double roh0,double specificHeat0){
	for(unsigned i =0; i < edges.size();i++){
		edges[i].c = 0.0;
		edges[i].k = 0.0;
	}

	///	faces
	for(unsigned i=0;i<faces.size();i++){
		faces[i].alphaUpdated = true;
		faces[i].area = 0.0;
		faces[i].heatTransRatio = 0.0;
		faces[i].deformed = true;				//初期状態は、変形後とする
		faces[i].fluxarea =0.0;
		for(unsigned j =0; j < HIGH +1 ; j++){			// 加熱モードの数だけ、ベクトルを生成
			faces[i].heatflux[j] = 0.0;
		}
	}

	//行列の成分数などを初期化
	bVecAll.resize(vertices.size(),1);

	//節点温度の初期設定(行列を作る前に行う)
	SetVerticesTempAll(0.0);			///	初期温度の設定

	//周囲流体温度の初期化(temp度にする)
	InitTcAll(0.0);

	//dmnN 次元の温度の縦（列）ベクトル
	CreateTempMatrix();

	//熱伝導率、密度、比熱、熱伝達率　のパラメーターを設定・代入
		//PHFemMEshThermoのメンバ変数の値を代入 CADThermoより、0.574;//玉ねぎの値//熱伝導率[W/(ｍ・K)]　Cp = 1.96 * (Ndt);//玉ねぎの比熱[kJ/(kg・K) 1.96kJ/(kg K),（玉ねぎの密度）食品加熱の科学p64より970kg/m^3
		//熱伝達率の単位系　W/(m^2 K)⇒これはSI単位系なのか？　25は論文(MEAT COOKING SIMULATION BY FINITE ELEMENTS)のオーブン加熱時の実測値
	//. 熱伝達する SetInitThermoConductionParam(0.574,970,0.1960,25 * 0.001 );		//> thConduct:熱伝導率 ,roh:密度,	specificHeat:比熱 J/ (K・kg):1960 ,　heatTrans:熱伝達率 W/(m^2・K)
	//. 熱伝達しない
	SetInitThermoConductionParam(thConduct0,roh0,specificHeat0,0);		// 熱伝達率=0;にしているw
	
	//> 熱流束の初期化
	SetVtxHeatFluxAll(0.0);

	//>	熱放射率の設定
	SetThermalEmissivityToVerticesAll(0.0);				///	暫定値0.0で初期化	：熱放射はしないｗ

	//> IH加熱するfaceをある程度(表面face && 下底面)絞る、関係しそうなface節点の原点からの距離を計算し、face[].mayIHheatedを判定
	CalcVtxDisFromOrigin();
	//>	IHからの単位時間当たりの加熱熱量	//単位時間当たりの総加熱熱量	231.9; //>	J/sec
	
	//	この後で、熱流束ベクトルを計算する関数を呼び出す
	InitCreateMatC();					///	CreateMatCの初期化
	InitVecFAlls();					///	VecFAll類の初期化
	InitCreateMatk();					///	CreateMatKの初期化

	///	熱伝達率を各節点に格納
	SetHeatTransRatioToAllVertex();
	for(unsigned i=0; i < this->tets.size(); i++){
		//各行列を作って、ガウスザイデルで計算するための係数の基本を作る。Timestepの入っている項は、このソース(SetDesc())では、実現できないことが分かった(NULLが返ってくる)
		CreateMatkLocal(i);				///	Matk1 Matk2(更新が必要な場合がある)を作る	//ifdefスイッチで全体剛性行列も(表示用だが)生成可能
		CreatedMatCAll(i);
		CreateVecFAll(i);
	}
	
	// カウントの初期化
	//Ndt =0;

	//水分蒸発周りの初期化
	InitMoist();

	SetVerticesTempAll(0.0);

}

void PHFemMeshThermo::AfterSetDesc() {	

	//%%%	初期化類		%%%//

	//各種メンバ変数の初期化⇒コンストラクタでできたほうがいいかもしれない。
	///	Edges
	for(unsigned i =0; i < edges.size();i++){
		edges[i].c = 0.0;	
		edges[i].k = 0.0;
	}

	///	faces
	for(unsigned i=0;i<faces.size();i++){
		faces[i].alphaUpdated = true;
		faces[i].area = 0.0;
		faces[i].heatTransRatio = 0.0;
		faces[i].deformed = true;				//初期状態は、変形後とする
		faces[i].fluxarea =0.0;
//		faces[i].heatflux =0.0;
		//faces[i].heatflux.clear();				// 初期化
		//faces[i].heatflux[hum]の領域確保：配列として、か、vetorとしてのpush_backか、どちらかを行う。配列ならここに記述。
		for(unsigned j =0; j < HIGH +1 ; j++){			// 加熱モードの数だけ、ベクトルを生成
			faces[i].heatflux[j] = 0.0;
		}
	}

	///	vertex

	///	tets

	//行列の成分数などを初期化
	bVecAll.resize(vertices.size(),1);

	//行列を作る
		//行列を作るために必要な節点や四面体の情報は、PHFemMeshThermoの構造体に入っている。
			//PHFemMeshThermoのオブジェクトを作る際に、ディスクリプタに値を設定して作る
		
	//節点温度の初期設定(行列を作る前に行う)
	SetVerticesTempAll(0.0);			///	初期温度の設定

	//周囲流体温度の初期化(temp度にする)
	InitTcAll(0.0);

	//節点の初期温度を設定する⇒{T}縦ベクトルに代入
		//{T}縦ベクトルを作成する。以降のK,C,F行列・ベクトルの節点番号は、この縦ベクトルの節点の並び順に合わせる?
		
	//dmnN 次元の温度の縦（列）ベクトル
	CreateTempMatrix();

	//熱伝導率、密度、比熱、熱伝達率　のパラメーターを設定・代入
		//PHFemMEshThermoのメンバ変数の値を代入 CADThermoより、0.574;//玉ねぎの値//熱伝導率[W/(ｍ・K)]　Cp = 1.96 * (Ndt);//玉ねぎの比熱[kJ/(kg・K) 1.96kJ/(kg K),（玉ねぎの密度）食品加熱の科学p64より970kg/m^3
		//熱伝達率の単位系　W/(m^2 K)⇒これはSI単位系なのか？　25は論文(MEAT COOKING SIMULATION BY FINITE ELEMENTS)のオーブン加熱時の実測値
		//SetInitThermoConductionParam(0.574,970,1.96,25);
	//. 熱伝達する SetInitThermoConductionParam(0.574,970,0.1960,25 * 0.001 );		//> thConduct:熱伝導率 ,roh:密度,	specificHeat:比熱 J/ (K・kg):1960 ,　heatTrans:熱伝達率 W/(m^2・K)
	//. 熱伝達しない
	SetInitThermoConductionParam(0.574,970,0.1960,0 );		// 熱伝達率=0;にしているw
	


	//断熱過程
	//SetInitThermoConductionParam(0.574,970,0.1960,0.0);		//> thConduct:熱伝導率 ,roh:密度,	specificHeat:比熱 J/ (K・kg):1960 ,　heatTrans:熱伝達率 W/(m^2・K)
	//これら、変数値は後から計算の途中で変更できるようなSetParam()関数を作っておいたほうがいいかな？

	//> 熱流束の初期化
	SetVtxHeatFluxAll(0.0);

	//>	熱放射率の設定
	SetThermalEmissivityToVerticesAll(0.0);				///	暫定値0.0で初期化	：熱放射はしないｗ

	//>	行列の作成　行列の作成に必要な変数はこの行以前に設定が必要
		//計算に用いるマトリクス、ベクトルを作成（メッシュごとの要素剛性行列/ベクトル⇒全体剛性行列/ベクトル）
		//{T}縦ベクトルの節点の並び順に並ぶように、係数行列を加算する。係数行列には、面積や体積、熱伝達率などのパラメータの積をしてしまったものを入れる。


	//> IH加熱するfaceをある程度(表面face && 下底面)絞る、関係しそうなface節点の原点からの距離を計算し、face[].mayIHheatedを判定
	CalcVtxDisFromOrigin();
	//>	IHからの単位時間当たりの加熱熱量
	//単位時間当たりの総加熱熱量	231.9; //>	J/sec
	
	//円環加熱：IH
	//CalcIHdqdt(0.04,0.095,231.9 * 0.005 * 1e6);		/// 単位 m,m,J/sec		//> 0.002:dtの分;Stepで用いるdt倍したいが...	// 0.05,0.11は適当値
	//CalcIHdqdt_atleast(0.06,0.095,231.9 * 0.005 * 1e5);		///	少しでも円環領域にかかっていたら、そのfaceの面積全部にIH加熱をさせる
	
	//	重要
	//20120811
	//CalcIHdqdt_atleast(0.11,0.14,231.9 * 0.005 * 1e5);		//mainの中に実装、phPanにだけ実行させたい
	
	
	//..debug 
	//バンド状加熱
//	CalcIHdqdtband_(-0.02,0.20,231.9 * 0.005 * 1e6);

	
	//%%	IH加熱のモード切替
	//	ライン状に加熱
	//	CalcIHdqdtband_(0.09,0.10,231.9 * 5e3);		//*0.5*1e4	値を変えて実験	//*1e3　//*1e4 //5e3
	//	円環状に加熱
	
	//CalcIHarea(0.04,0.095,231.9 * 0.005 * 1e6);




	//	この後で、熱流束ベクトルを計算する関数を呼び出す

	InitCreateMatC();					///	CreateMatCの初期化
	InitVecFAlls();					///	VecFAll類の初期化
	InitCreateMatk();					///	CreateMatKの初期化
	//..	CreateLocalMatrixAndSet();			//> 以上の処理を、この関数に集約

	///	熱伝達率を各節点に格納
	SetHeatTransRatioToAllVertex();
	for(unsigned i=0; i < this->tets.size(); i++){
		//各行列を作って、ガウスザイデルで計算するための係数の基本を作る。Timestepの入っている項は、このソース(SetDesc())では、実現できないことが分かった(NULLが返ってくる)
		CreateMatkLocal(i);				///	Matk1 Matk2(更新が必要な場合がある)を作る	//ifdefスイッチで全体剛性行列も(表示用だが)生成可能
		//CreateMatKall();		//CreateMatkLocal();に実装したので、後程分ける。
		CreatedMatCAll(i);
		CreateVecFAll(i);
	}

	int hogeshidebug =0;
	//	節点温度推移の書き出し
	templog.open("templog.csv");

	//	CPSの経時変化を書き出す
	//cpslog.open("cpslog.csv");

	// カウントの初期化
	Ndt =0;

	//水分蒸発周りの初期化
	InitMoist();

}

//void PHFemMeshThermo::CreateLocalMatrixAndSet(){
//	//K,C,Fの行列を作る関数を呼び出して、作らせる
//	for(unsigned i = 0; i< tets.size() ; i++){
//		//tetsを引数にいれると、その行列・ベクトルを作ってくれて、できた行列、ベクトルを基に係数を設定しくれる
//		//こうすれば、各要素剛性行列でfor文を回さなくてもよくなる
//		//CreateMatkLocal(tets);
//		//CreateMatcLocal(tets);
//		//CreateVecfLocal(tets);
//
//		//tetsを入れて作らせる
////		SetkcfParam(tets);
//
//	}
//
//}
void PHFemMeshThermo::SetkcfParam(Tet tets){
}

void PHFemMeshThermo::CreateMatc(unsigned id){
	//最後に入れる行列を初期化
	for(unsigned i =0; i < 4 ;i++){
		for(unsigned j =0; j < 4 ;j++){
			matc[i][j] = 0.0;
		}
	}
	//matc に21でできた行列を入れる
	matc = Create44Mat21();
	//	for debug
		//DSTR << "matc " << matc << " ⇒ ";
	matc = rho * specificHeat * CalcTetrahedraVolume(tets[id]) / 20.0 * matc;
	//	debug	//係数の積をとる
		//DSTR << matc << std::endl;
		//int hogemat =0 ;
}

void PHFemMeshThermo::CreatedMatCAll(unsigned id){
	//すべての要素について係数行列を作る
		//c
	CreateMatc(id);
	int mathoge=0;
	//	(ガウスザイデルを使った計算時)要素毎に作った行列の成分より、エッジに係数を格納する
	//	or	(ガウスザイデルを使わない計算時)要素ごとの計算が終わるたびに、要素剛性行列の成分だけをエッジや点に作る変数に格納しておく	#ifedefでモード作って、どちらもできるようにしておいても良いけどw
	for(unsigned j=1; j < 4; j++){
		int vtxid0 = tets[id].vertices[j];
		//	下三角行列部分についてのみ実行
		//	j==1:k=0, j==2:k=0,1, j==3:k=0,1,2
		for(unsigned k = 0; k < j; k++){
			int vtxid1 = tets[id].vertices[k];
				for(unsigned l =0; l < vertices[vtxid0].edges.size(); l++){
					for(unsigned m =0; m < vertices[vtxid1].edges.size(); m++){
						if(vertices[vtxid0].edges[l] == vertices[vtxid1].edges[m]){
							edges[vertices[vtxid0].edges[l]].c += matc[j][k];		//同じものが二つあるはずだから半分にする。上三角化下三角だけ走査するには、どういうfor文ｓにすれば良いのか？
							//DSTR << edges[vertices[vtxid0].edges[l]].k << std::endl;
						}
					}
				}
		}
	}
	//対角成分を対角成分の全体剛性行列から抜き出した1×nの行列に代入する
	//j=0~4まで代入(上のループでは、jは対角成分の範囲しかないので、値が入らない成分が出てしまう)
	for(unsigned j =0;j<4;j++){
		dMatCAll[0][tets[id].vertices[j]] += matc[j][j];
	}

	////	for debug
	//DSTR << "dMatCAll : " << std::endl;
	//for(unsigned j =0;j < vertices.size();j++){
	//	DSTR << j << "th : " << dMatCAll[0][j] << std::endl;
	//}
	// ネギについて非0成分になった。

	//	調べる
	//dMatKAllの成分のうち、0となる要素があったら、エラー表示をするコードを書く
	// try catch文にする
	//for(unsigned j = 0; j < vertices.size() ; j++){
	//	if(dMatCAll[0][j] ==0.0){
	//		DSTR << "dMatCAll[0][" << j << "] element is blank" << std::endl;
	//	}
	//}
	int piyodebug =0;
}

void PHFemMeshThermo::CreateVecFAll(unsigned id){

	//	注意
	//	f3を使用する場合:周囲流体温度Tcが0の節点の要素は0になるため、温度の設定が必要
	
	//すべての要素について係数行列を作る
	//f1を作る
	//>	熱流束境界条件	vecf2を作る			
	//CreateVecf2(id);				//>	tets[id].vecf[1] を初期化,代入		熱流束は相加平均で求める
	CreateVecf2surface(id);			
	//>	熱伝達境界条件	f3を作る
	CreateVecf3(id);			// surface化すべきだよね	//>	tets[id].vecf[2] を初期化,代入		熱伝達率は相加平均、周囲流体温度は節点の形状関数？ごとに求める
	//CreateVecf3_(id);			//>	tets[id].vecf[2] を初期化,代入		熱伝達率、周囲流体温度を相加平均で求める
	//f4を作る
	//f1:vecf[0],f2:vecf[1],f3:vecf[2],f4:vecf[3]を加算する
	vecf = tets[id].vecf[1] + tets[id].vecf[2];		//>	+ tets[id].vecf[0] +  tets[id].vecf[3] の予定
	//	(ガウスザイデルを使った計算時)要素毎に作った行列の成分より、エッジに係数を格納する
	//	or	(ガウスザイデルを使わない計算時)要素ごとの計算が終わるたびに、要素剛性行列の成分だけをエッジや点に作る変数に格納しておく	#ifedefでモード作って、どちらもできるようにしておいても良いけどw

	//要素の節点番号の場所に、その節点のfの値を入れる
	//j:要素の中の何番目か
	for(unsigned j =0;j < 4; j++){
		int vtxid0 = tets[id].vertices[j];
		vecFAllSum[vtxid0] += vecf[j];
		//vecFAll_f2IH[num][vtxid0][0] += vecf[j];
	}
	//	for debug
	//vecFAllSumに値が入ったのかどうかを調べる 2011.09.21全部に値が入っていることを確認した
	//DSTR << "vecFAllSum : " << std::endl;
	//for(unsigned j =0; j < vertices.size() ; j++){
	//	DSTR << j << " ele is :  " << vecFAllSum[j] << std::endl;
	//}

	////	調べる
	////vecFAllSumの成分のうち、0となる要素があったら、エラー表示をするコードを書く
	//// try catch文にする
	//for(unsigned j = 0; j < vertices.size() ; j++){
	//	if(vecFAllSum[j] ==0.0){
	//		DSTR << "vecFAllSum[" << j << "] element is blank" << std::endl;
	//	}
	//}

}

void PHFemMeshThermo::CreateMatkLocal(unsigned id){
	//すべての四面体要素について係数行列を作る

	//	k1を作る	k1kでも、k1bでもどちらでも構わない	どりらが速いか調べる
	///	変形した時だけ生成する
	if(deformed){	CreateMatk1k(id);}			//  K第一項の行列作成	//k理論を根拠に、加筆して、形状関数を導出	
//	if(deformed){	CreateMatk1b(id);}			//	書籍の理論を根拠に、公式を用いて形状関数を導出
	//DSTR << "tets[id].matk1: " << tets[id].matk1 << std::endl;

	//熱伝達境界条件に必要な、k2を作るか否か
	CreateMatk2t(id);					///	熱伝達境界条件
	//CreateMatk2(id,tets[id]);			///	この関数は使用しない
	//DSTR << "tets[id].matk2: " << tets[id].matk2 << std::endl;
	int hogehogehoge=0;

	//k1,k2,k3を加算する(使っている数値だけ)
	matk = tets[id].matk[0] + tets[id].matk[1];	
	//DSTR << "matk: " << matk << std::endl;

	//	(ガウスザイデルを使った計算時)要素毎に作った行列の成分より、エッジに係数を格納する
	//	or	(ガウスザイデルを使わない計算時)要素ごとの計算が終わるたびに、要素剛性行列の成分だけをエッジや点に作る変数に格納しておく	#ifedefでモード作って、どちらもできるようにしておいても良いけどw
	//	Edges のこの要素で計算したK行列の成分をkに係数として格納する
		
	//matkの対角成分以外で、下三角の部分の値を、edgeのkに代入する
	//
	//DSTR << i <<"th tetrahedra element'edges[vertices[vtxid0].edges[l]].k (All edges.k' value): " << std::endl;
	for(unsigned j=1; j < 4; j++){
		int vtxid0 = tets[id].vertices[j];
		//	下三角行列部分についてのみ実行
		//	j==1:k=0, j==2:k=0,1, j==3:k=0,1,2
		for(unsigned k = 0; k < j; k++){
			int vtxid1 = tets[id].vertices[k];
			for(unsigned l =0; l < vertices[vtxid0].edges.size(); l++){
				for(unsigned m =0; m < vertices[vtxid1].edges.size(); m++){
					if(vertices[vtxid0].edges[l] == vertices[vtxid1].edges[m]){
						edges[vertices[vtxid0].edges[l]].k += matk[j][k];		//同じものが二つあるはずだから半分にする。上三角化下三角だけ走査するには、どういうfor文ｓにすれば良いのか？
						//DSTR << edges[vertices[vtxid0].edges[l]].k << std::endl;
#ifdef DumK
						edges[vertices[vtxid0].edges[l]].k = 0.0;
#endif DumK
					}
				}
			}
		}
	}

#ifdef UseMatAll
	//SciLabで使うために、全体剛性行列を作る
	//matkから作る
	for(unsigned j=0; j<4 ; j++){
		for(unsigned k=0; k<4 ;k++){
			matKAll[tets[id].vertices[j]][tets[id].vertices[k]] += matk[j][k];
		}
	}

	////edgesに入った係数から作る
	//for(unsigned j=1; j < 4; j++){
	//	int vtxid0 = tets[i].vertices[j];
	//	//	下三角行列部分についてのみ実行
	//	//	j==1:k=0, j==2:k=0,1, j==3:k=0,1,2
	//	for(unsigned k = 0; k < j; k++){
	//		int vtxid1 = tets[i].vertices[k];
	//			for(unsigned l =0; l < vertices[vtxid0].edges.size(); l++){
	//				for(unsigned m =0; m < vertices[vtxid1].edges.size(); m++){
	//					if(vertices[vtxid0].edges[l] == vertices[vtxid1].edges[m]){
	//						edges[vertices[vtxid0].edges[l]].k += matk[j][k];		//同じものが二つあるはずだから半分にする。上三角化下三角だけ走査するには、どういうfor文ｓにすれば良いのか？
	//						//DSTR << edges[vertices[vtxid0].edges[l]].k << std::endl;
	//					}
	//				}
	//			}
	//	}
	//}

#endif UseMatAll

#ifdef DumK
	//SciLabで使うために、全体剛性行列を作る
	//matkから作る
	for(unsigned j=0; j<4 ; j++){
		for(unsigned k=0; k<4 ;k++){
			matKAll[tets.vertices[j]][tets.vertices[k]] = 0.0;;
		}
	}
#endif

	//対角成分を対角成分の全体剛性行列から抜き出した1×nの行列に代入する
	//j=0~4まで代入(上のループでは、jは対角成分の範囲しかないので、値が入らない成分が出てしまう)
	for(unsigned j =0;j<4;j++){
		dMatKAll[0][tets[id].vertices[j]] += matk[j][j];
		//DSTR << "matk[" << j << "][" << j << "] : " << matk[j][j] << std::endl;
		//DSTR << "dMatKAll[0][" << tets[i].vertices[j] << "] : " << dMatKAll[0][tets[i].vertices[j]] << std::endl;
		int hoge4 =0;
	}
	//DSTR << std::endl;	//改行

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
	//要素25の0~3番目の節点が何か表示する
	//if(i == 25){
	//		for(unsigned n=0;n < 4;n++){
	//			DSTR << n << " : " << tets[25].vertices[n] << std::endl;	//要素25の0番目の節点が63である。
	//		}
	//}
	//節点番号が63の点がどの要素に入っているのかを調べる⇒25,57だった
	//for(unsigned j=0;j < vertices[63].tets.size();j++){
	//	DSTR << vertices[63].tets[j] <<std::endl;
	//}
		
	//	調べる
	//dMatKAllの成分のうち、0となる要素があったら、エラーか？　入っていない成分があっても、問題ない気もする
	// try catch文にする
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
	for(unsigned j =0;j<vertices.size();j++){
		if(matKAll[j][j] != dMatKAll[0][j]){
			DSTR << j <<" 成分の要素はおかしい！調査が必要である。 " <<std::endl;
		}
	}
#endif UseMatAll
	int hoge5 =0;

}

void PHFemMeshThermo::CreateMatk1b(unsigned id){
	//yagawa1983を基にノートに式展開した計算式
	unsigned i=0;
	unsigned j=0;
	unsigned k=0;
	unsigned l=0;

	//a_1~a_4, ... , c_4	を作成
	//係数(符号)×行列式の入れ物
	double a[4];
	double b[4];
	double c[4];

	//要素を構成する節点の座標の入れ物
	double x[4];
	double y[4];
	double z[4];
	//要素内の4節点のx,y,z座標を格納
	for(unsigned m=0; m < 4;m++){
		x[m] = vertices[tets[id].vertices[m]].pos.x;
		y[m] = vertices[tets[id].vertices[m]].pos.y;
		z[m] = vertices[tets[id].vertices[m]].pos.z;
	}

	//mata~matc
	//a[i]
	for(unsigned i =0;i<4;i++){
		double fugou =0.0;				// (-1)^i の符号の定義

		//	fugou の符号判定
		if(i == 0 || i == 2){		//0,2の時、(-1)^1,3 = -1
			fugou = -1.0;
		}
		else{					//1,3の時、(-1)^0,2 = 1
			fugou = 1.0;
		}
		
		//i,j,k,lの関係セット⇒配列の要素にしてもいいかも。i[4],if(i[0]=0){i[1](=j)=1, i[2](=k)=2, i[3](=l)=3}	if(i[0]=1){i[1](=j)=2, i[2](=k)=3, i[3](=l)=0}
		if(i==0){		j=1;	k=2;	l=3;	}
		else if(i==1){	j=2;	k=3;	l=0;	}
		else if(i==2){	j=3;	k=0;	l=1;	}
		else if(i==3){	j=0;	k=1;	l=2;	}

		// a_iの作成
		for(unsigned m =0;m<3;m++){						//	1の成分への代入はついで
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


		// b_iの作成
		matb[0][0]=x[j];
		matb[1][0]=x[k];
		matb[2][0]=x[l];

		for(unsigned m =0;m<3;m++){						//	1の成分への代入はついで
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

		// c_iの作成
		matcc[0][0]=x[j];
		matcc[1][0]=x[k];
		matcc[2][0]=x[l];

		matcc[0][1]=y[j];
		matcc[1][1]=y[k];
		matcc[2][1]=y[l];

		for(unsigned m =0;m<3;m++){						//	1の成分への代入はついで
			matcc[m][2] = 1.0;
		}

		//DSTR << "matcc : " <<std::endl;
		//DSTR << matcc << std::endl;
		//DSTR << "matcc.det() : " <<std::endl;
		//DSTR << matcc.det() << std::endl;

		c[i] = fugou * matcc.det();
		
		//	for debug　要素ごとのa_i~c_iの算出
		//DSTR << "a["<< i << "] : " << a[i] << std::endl;
		//DSTR << "b["<< i << "] : " << b[i] << std::endl;
		//DSTR << "c["<< i << "] : " << c[i] << std::endl;
		//DSTR << std::endl;
		int debughogeshi =0;
	}
	
	//	matk1の成分にa_i ~ c_iの多項式を代入	きれいすぎるが、下のコードで良い！	対角成分も非対角成分も、全部、下のコード
	//	改善案		下三角と対角成分だけ、計算し、上三角は下三角を代入でもよい。
	for(unsigned i =0;i<4;i++){
		for(unsigned j =0;j<4;j++){
			tets[id].matk[0][i][j] = a[i] * a[j] +b[i] * b[j] + c[i] * c[j];
		}
	}

	////	上記よりコストの少ないコード?
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
	
	//係数の積
	tets[id].matk[0]= thConduct / (36 *  CalcTetrahedraVolume(tets[id])) * tets[id].matk[0];		//理論が間違っていたので、修正

	//	for DEBUG
	//DSTR << "係数積後の matk1 : " << std::endl;
	//DSTR << matk1 << std::endl;
	int debughogeshi3 =0;

	DSTR << "Inner Function matk1b _ tets["<< id << "].matk[0] "<< tets[id].matk[0] << std::endl;  
	//a~cの多項式をK1に代入
	//matk1(4x4)に代入

}

void PHFemMeshThermo::CreateMatk1k(unsigned id){
	//この計算を呼び出すときに、各四面体ごとに計算するため、四面体の0番から順にこの計算を行う
	//四面体を構成する4節点を節点の配列(Tetsには、節点の配列が作ってある)に入っている順番を使って、面の計算を行ったり、行列の計算を行ったりする。
	//そのため、この関数の引数に、四面体要素の番号を取る

	//最後に入れる行列を初期化
	tets[id].matk[0].clear();
	//for(unsigned i =0; i < 4 ;i++){
	//	for(unsigned j =0; j < 4 ;j++){
	//		//tets[id].matk1[i][j] = 0.0;
	//		tets[id].matk[0][i][j] = 0.0;
	//	}
	//}

	//	A行列　=	a11 a12 a13
	//				a21 a22 a23
	//				a31 a32 a33
	//を生成
	PTM::TMatrixRow<4,4,double> matk1A;
	FemVertex p[4];
	for(unsigned i = 0; i< 4 ; i++){
		p[i]= vertices[tets[id].vertices[i]];
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

	//a11 ~ a33 を行列に入れて、[N~T] [N] を計算させる
	
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

	//	Km の算出
	//tets[id].matk1 = Nx.trans() * Nx + Ny.trans() * Ny + Nz.trans() * Nz;
	tets[id].matk[0] = Nx.trans() * Nx + Ny.trans() * Ny + Nz.trans() * Nz;

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
	tets[id].matk[0] = thConduct / (36 * CalcTetrahedraVolume(tets[id]) ) * tets[id].matk[0];
	//DSTR << "Inner Function _tets[id].matk1 : " << tets[id].matk1 << std::endl;

}

void PHFemMeshThermo::CreateVecf2surface(unsigned id,unsigned num){
	// 初期化
	tets[id].vecf[1].clear();
	//l=0の時f21,1の時:f22, 2の時:f23, 3の時:f24	を生成
	///	..j番目の行列の成分が0のベクトルを作成
	for(unsigned l= 0 ; l < 4; l++){
		vecf2array[l] = Create41Vec1();
		vecf2array[l][l] = 0.0;			//	l行を0に
	}
	for(unsigned l= 0 ; l < 4; l++){
		///	四面体の各面(l = 0 〜 3) についてメッシュ表面かどうかをチェックする。表面なら、行列を作ってvecf2arrayに入れる
		//faces[tets.faces[i]].sorted;		/// 1,24,58みたいな節点番号が入っている
		///	..行列型の入れ物を用意
		//faces[tets.faces[l]].vertices;
		if(tets[id].faces[l] < (int)nSurfaceFace && faces[tets[id].faces[l]].fluxarea > 0 ){			///	外殻の面 且つ 熱伝達率が更新されたら matk2を更新する必要がある
			///	四面体の三角形の面積を計算		///	この関数の外で面積分の面積計算を実装する。移動する
			if(faces[tets[id].faces[l]].area ==0 || faces[tets[id].faces[l]].deformed ){		///	面積が計算されていない時（はじめ） or deformed(変形した時・初期状態)がtrueの時		///	条件の追加	面積が0か ||(OR) αが更新されたか
				faces[tets[id].faces[l]].area = CalcTriangleArea(faces[tets[id].faces[l]].vertices[0], faces[tets[id].faces[l]].vertices[1], faces[tets[id].faces[l]].vertices[2]);
				faces[tets[id].faces[l]].deformed = false;
			}
			///	計算結果を行列に代入
			///	areaの計算に使っていない点が入っている行を除いたベクトルの積をとる
			///	積分計算を根本から考える
			unsigned vtx = tets[id].vertices[0] + tets[id].vertices[1] + tets[id].vertices[2] + tets[id].vertices[3];			
			///	area計算に使われていない節点ID：ID
			unsigned ID = vtx -( faces[tets[id].faces[l]].vertices[0] + faces[tets[id].faces[l]].vertices[1] + faces[tets[id].faces[l]].vertices[2] );
			for(unsigned j=0;j<4;j++){
				if(tets[id].vertices[j] == ID){					///	形状関数が１、（すなわち）このfaceに対面する頂点　と一致したら　その時のfaceで面積分する
					///	外殻にないメッシュ面の面積は0で初期化しておく
					///	以下の[]は上までの[l]と異なる。
					///	IDが何番目かによって、形状関数の係数が異なるので、
					tets[id].vecf[1] += faces[tets[id].faces[l]].heatflux[num] * (1.0/3.0) * faces[tets[id].faces[l]].area * vecf2array[j];
					//DSTR << "tets[id].matk2にfaces[tets[id].faces[l]].heatTransRatio * (1.0/12.0) * faces[tets[id].faces[l]].area * matk2array[" << j << "]"<< "を加算: " <<faces[tets[id].faces[l]].heatTransRatio * (1.0/12.0) * faces[tets[id].faces[l]].area * matk2array[j] << std::endl;
					//DSTR << "tets[id].matk2 +=  " << tets[id].matk2 << std::endl;
				}
				//else{
				//	///	IDと一致しない場合には、matk2array[j]には全成分0を入れる
				//	///	としたいところだが、
				//	//matk2array[j] =0.0 * matk2array[j];
				//	//DSTR << "matk2array[" << j << "]: " << matk2array[j] << std::endl;
				//}
			}
		}
		///	SurfaceFaceじゃなかったら、matk2arrayには0を入れる
		//else{
		//	//matk2array[l];
		//}
	}


}

void PHFemMeshThermo::CreateVecf2surface(unsigned id){
	// 初期化
	tets[id].vecf[1].clear();
	//l=0の時f21,1の時:f22, 2の時:f23, 3の時:f24	を生成
	///	..j番目の行列の成分が0のベクトルを作成
	for(unsigned l= 0 ; l < 4; l++){
		vecf2array[l] = Create41Vec1();
		vecf2array[l][l] = 0.0;			//	l行を0に
	}
	for(unsigned l= 0 ; l < 4; l++){
		///	四面体の各面(l = 0 〜 3) についてメッシュ表面かどうかをチェックする。表面なら、行列を作ってvecf2arrayに入れる
		//faces[tets.faces[i]].sorted;		/// 1,24,58みたいな節点番号が入っている
		///	..行列型の入れ物を用意
		//faces[tets.faces[l]].vertices;
		if(tets[id].faces[l] < (int)nSurfaceFace && faces[tets[id].faces[l]].fluxarea > 0 ){			///	外殻の面 且つ 熱伝達率が更新されたら matk2を更新する必要がある
			///	四面体の三角形の面積を計算		///	この関数の外で面積分の面積計算を実装する。移動する
			if(faces[tets[id].faces[l]].area ==0 || faces[tets[id].faces[l]].deformed ){		///	面積が計算されていない時（はじめ） or deformed(変形した時・初期状態)がtrueの時		///	条件の追加	面積が0か ||(OR) αが更新されたか
				faces[tets[id].faces[l]].area = CalcTriangleArea(faces[tets[id].faces[l]].vertices[0], faces[tets[id].faces[l]].vertices[1], faces[tets[id].faces[l]].vertices[2]);
				faces[tets[id].faces[l]].deformed = false;
			}
			///	計算結果を行列に代入
			///	areaの計算に使っていない点が入っている行を除いたベクトルの積をとる
			///	積分計算を根本から考える
			unsigned vtx = tets[id].vertices[0] + tets[id].vertices[1] + tets[id].vertices[2] + tets[id].vertices[3];			
			///	area計算に使われていない節点ID：ID
			unsigned ID = vtx -( faces[tets[id].faces[l]].vertices[0] + faces[tets[id].faces[l]].vertices[1] + faces[tets[id].faces[l]].vertices[2] );
			for(unsigned j=0;j<4;j++){
				if(tets[id].vertices[j] == ID){					///	形状関数が１、（すなわち）このfaceに対面する頂点　と一致したら　その時のfaceで面積分する
					///	外殻にないメッシュ面の面積は0で初期化しておく
					///	以下の[]は上までの[l]と異なる。
					///	IDが何番目かによって、形状関数の係数が異なるので、
					tets[id].vecf[1] += faces[tets[id].faces[l]].heatflux[1] * (1.0/3.0) * faces[tets[id].faces[l]].area * vecf2array[j];
				}
			}
		}
		///	SurfaceFaceじゃなかったら、matk2arrayには0を入れる
		//else{
		//	//matk2array[l];
		//}
	}


}
#if 0
void PHFemMeshThermo::CreateVecF3surfaceAll(){
	//	初期化
	//	弱火、中火、強火について初期化(ベクトルの行数設定、初期化)
	vecFAll_f3.resize(vertices.size(),1);			//表面だけでなく、全節点について計算しないと、ベクトル×行列の計算が不成立のため。
	vecFAll_f3.clear();
	
	//四面体要素ごとに行列を作り、どこかで合成する
	//idを入れて、再帰的に作っている
	for(unsigned id =0; id < tets.size();id++){ 
		//行列を作る
		CreateVecf3(id);//;		// f3surfaceではないけれど、いいのか？	//CreateVecf2surface(id,num);	//	この関数も、引数に指定したベクトルに入れられるようにする?
		for(unsigned j =0;j < 4; j++){
			int vtxid0 = tets[id].vertices[j];
			//vecFAllSum[vtxid0] = vecf[j];			//全体剛性ベクトルを作成：ガウスザイデル計算内でやっている処理・これを行う。ここまでをVecf2でやる。
			vecFAll_f3[vtxid0][0] += tets[id].vecf[2][j];		//	+= じゃなくてもいいのか？同様に、元のソースでも += の必要があるのでは？
		}
	}
	//作った後に、ガウスザイデル計算で、VecFAllにセットする関数を作る。
	//vecFAllSumに加算とかすると、どんどん増えてしまうし、逆に、他の、変化しない要素{F_3}など、全体ベクトルも作って、保存しておく必要
	//ガウスザイデルの計算の中で、これまでの計算でFベクトルを使うのに代えて、マイステップで、VecFをF1,F2から作る必要がある。
}

void PHFemMeshThermo::CreateVecF2surfaceAll(){
	//	初期化
	//	弱火、中火、強火について初期化(ベクトルの行数設定、初期化) initVecFAlls()で実行
	//for(unsigned i =0; i < 4 ;i++){
	//	vecFAll_f2IH[i].resize(vertices.size(),1);			//表面だけでなく、全節点について計算しないと、ベクトル×行列の計算が不成立のため。
	//	vecFAll_f2IH[i].clear();
	//}
	
	//四面体要素ごとに行列を作り、どこかで合成する
	//idを入れて、再帰的に作っている
	for(unsigned id =0; id < tets.size();id++){ 
		//行列を作る
		for(unsigned num =0; num <4 ; num++){	//全火力(OFF/WEEK/MIDDLE/HIGH)について
			CreateVecf2surface(id,num);			//	この関数も、引数に指定したベクトルに入れられるようにする?
			//num毎に、入れ物に入れる。
			for(unsigned j =0;j < 4; j++){
				int vtxid0 = tets[id].vertices[j];
				//vecFAllSum[vtxid0][0] = vecf[j];			//全体剛性ベクトルを作成：ガウスザイデル計算内でやっている処理・これを行う。ここまでをVecf2でやる。
				//vecFAll_f2IHw[vtxid0][0] = vecf[j];
				vecFAll_f2IH[num][vtxid0][0] += tets[id].vecf[1][j];		//f2の[num(火力)]	+= じゃなくてもいいのか？同様に、元のソースでも += の必要があるのでは？
			}

		}


	
		////要素の節点番号の場所に、その節点のfの値を入れる
		////j:要素の中の何番目か
		//for(unsigned j =0;j < 4; j++){
		//	int vtxid0 = tets[id].vertices[j]
		//	vecFAll_f2IHw[vtxid0][0] = vecf[j];tets[id].vecf[1]//のj要素
		//}

		//
		////入れ物に入れる
		////j:要素の中の何番目か
		//for(unsigned j =0;j < 4; j++){
		//	int vtxid0 = tets[id].vertices[j];
		//	vecFAllSum[vtxid0] = vecf[j];			//全体剛性ベクトルを作成：ガウスザイデル計算内でやっている処理・これを行う。ここまでをVecf2でやる。
		//	vecFAll_f2IHw[vtxid0][0] = vecf[j];
		//	
		//}
	

	//作った後に、ガウスザイデル計算で、vecFAllSumにセットする関数を作る。
	//vecFAllSumに加算とかすると、どんどん増えてしまうし、逆に、他の、変化しない要素{F_3}など、全体ベクトルも作って、保存しておく必要
	//ガウスザイデルの計算の中で、これまでの計算でFベクトルを使うのに代えて、マイステップで、VecFをF1,F2から作る必要がある。

	}

	//以下、CreateVecfLocalからコピペ　2012.9.25
	//すべての要素について係数行列を作る
	//f1を作る
	//>	熱流束境界条件	vecf2を作る			


	



}
#endif
void PHFemMeshThermo::CreateVecf2(unsigned id){
	//	初期化
	for(unsigned i =0; i < 4 ;i++){
		//最後に入れる行列を初期化
		tets[id].vecf[1][i] =0.0;				//>	f3 = vecf[1] 
	}	
	//l=0の時f31,1:f32, 2:f33, 3:f34	を生成
	for(unsigned l= 0 ; l < 4; l++){
		//matk2array[l] = matk2temp;
		vecf2array[l] = Create41Vec1();
		//	l行を0に
		vecf2array[l][l] = 0.0;

		//array[n][m][l]	= narray[n],m行l列
		//	f_3	(vecf3array[0], vecf3array[1],..)
		// =	| 0 | + | 1 |+...
		//		| 1 |   | 0 |
		//		| 1 |   | 1 |
		//		| 1 |   | 1 |

		//	for debug
		//DSTR << "vecf3array[" << l << "] : " << std::endl;
		//DSTR << vecf3array[l] << std::endl;

		//係数の積をとる
		//この節点で構成される四面体の面積の積をとる
		//四面体の節点1,2,3(0以外)で作る三角形の面積
		//l==0番目の時、 123	を代入する
		//l==1			0 23
		//l==2			01 3
		//l==3			012
		//をCalcTriangleAreaに入れることができるようにアルゴリズムを考える。

		//>	CreateMatk2tのようなアルゴリズムに変更予定
		//k21
		if(l==0){
			//>	三角形面を構成する3頂点の熱流束の相加平均
			double tempHF = (vertices[tets[id].vertices[1]].heatFluxValue + vertices[tets[id].vertices[2]].heatFluxValue + vertices[tets[id].vertices[3]].heatFluxValue ) / 3.0;		//HTR:HeatTransRatio
			vecf2array[l] = tempHF * (1.0/3.0) * CalcTriangleArea( tets[id].vertices[1],tets[id].vertices[2],tets[id].vertices[3] ) * vecf2array[l];
			//DSTR << "vecf2array[" << l << "] : " << vecf2array[l] << std::endl;
			//Vecの節点毎にその節点での周囲流体温度Tcとの積を行う
			
			////>	不要？
			//for(unsigned m=0; m<4; m++){
			//	vecf2array[l][m] = vertices[tets[id].vertices[m]].Tc * vecf2array[l][m];
			//}
		}
		//	k22
		else if(l==1){
			double tempHF = (vertices[tets[id].vertices[0]].heatFluxValue + vertices[tets[id].vertices[2]].heatFluxValue + vertices[tets[id].vertices[3]].heatFluxValue ) / 3.0;		//HTR:HeatTransRatio
			vecf2array[l] = tempHF * (1.0/3.0) * CalcTriangleArea( tets[id].vertices[0],tets[id].vertices[2],tets[id].vertices[3] ) * vecf2array[l];
			//vecf3array[l] = heatTrans * (1.0/3.0) * CalcTriangleArea( tets[id].vertices[0],tets[id].vertices[2],tets[id].vertices[3] ) * vecf3array[l];
			//DSTR << "vecf3array[" << l << "] : " << vecf3array[l] << std::endl;
			//Vecの節点毎にその節点での周囲流体温度Tcとの積を行う
			//for(unsigned m=0; m<4; m++){
			//	vecf2array[l][m] = vertices[tets[id].vertices[m]].Tc * vecf2array[l][m];
			//}
		}
		//	k23
		else if(l==2){
			double tempHF = (vertices[tets[id].vertices[0]].heatFluxValue + vertices[tets[id].vertices[1]].heatFluxValue + vertices[tets[id].vertices[3]].heatFluxValue ) / 3.0;		//HTR:HeatTransRatio
			vecf2array[l] = tempHF * (1.0/3.0) * CalcTriangleArea( tets[id].vertices[0],tets[id].vertices[1],tets[id].vertices[3] ) * vecf2array[l];
			//vecf3array[l] = heatTrans * (1.0/3.0) * CalcTriangleArea( tets[id].vertices[0],tets[id].vertices[1],tets[id].vertices[3] ) * vecf3array[l];
			//DSTR << "vecf3array[" << l << "] : " << vecf3array[l] << std::endl;
			//Vecの節点毎にその節点での周囲流体温度Tcとの積を行う
			//for(unsigned m=0; m<4; m++){
			//	vecf2array[l][m] = vertices[tets[id].vertices[m]].Tc * vecf2array[l][m];
			//}
		}
		//	k24
		else if(l==3){
			double tempHF = (vertices[tets[id].vertices[0]].heatFluxValue + vertices[tets[id].vertices[1]].heatFluxValue + vertices[tets[id].vertices[2]].heatFluxValue ) / 3.0;		//HTR:HeatTransRatio
			vecf2array[l] = tempHF * (1.0/3.0) * CalcTriangleArea( tets[id].vertices[0],tets[id].vertices[1],tets[id].vertices[2] ) * vecf2array[l];
			//vecf3array[l] = heatTrans * (1.0/3.0) * CalcTriangleArea( tets[id].vertices[0],tets[id].vertices[1],tets[id].vertices[2] ) * vecf3array[l];
			//DSTR << "vecf3array[" << l << "] : " << vecf3array[l] << std::endl;
			//Vecの節点毎にその節点での周囲流体温度Tcとの積を行う
			//for(unsigned m=0; m<4; m++){
			//	vecf2array[l][m] = vertices[tets[id].vertices[m]].Tc * vecf2array[l][m];
			//}
		}
		//for debug
		//DSTR << "vecf3array[" << l << "]の完成版は↓" << std::endl;
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
		tets[id].vecf[1] += vecf2array[i];
		//	for debug
		//DSTR << "vecf3 に vecf3array = f3" << i+1 <<"まで加算した行列" << std::endl;
		//DSTR << vecf3 << std::endl;
	}
	
	//	f1,f2,f3,f4	を計算する際に、[0][0]成分から[3][0]成分までの非0成分について、先にTcをかけてしまう


	//for debug
	//DSTR << "節点（";
	//for(unsigned i =0; i < 4; i++){
	//	DSTR << tets[id].vertices[i] << "," ;
	//}
	//DSTR << ")で構成される四面体の" << std::endl;
	//DSTR << "vecf3 : " << std::endl;
	//DSTR << vecf3 << std::endl;
	//int hogeshishi =0;
}

void PHFemMeshThermo::CreateVecf3_(unsigned id){
	//	初期化
	for(unsigned i =0; i < 4 ;i++){
		//vecf3[i] = 0.0;		//最後に入れる行列を初期化
		tets[id].vecf[2][i] =0.0;
	}	
	//l=0の時f31,1:f32, 2:f33, 3:f34	を生成
	for(unsigned l= 0 ; l < 4; l++){
		//matk2array[l] = matk2temp;
		vecf3array[l] = Create41Vec1();
		//	l行を0に
		//for(int i=0;i<4;i++){
		//	vecf3array[l][l][i] = 0.0;
		//}
		vecf3array[l][l] = 0.0;
		//array[n][m][l]	= narray[n],m行l列
		//	f_3	(vecf3array[0], vecf3array[1],..)
		// =	| 0 | + | 1 |+...
		//		| 1 |   | 0 |
		//		| 1 |   | 1 |
		//		| 1 |   | 1 |
		//	for debug
		//DSTR << "vecf3array[" << l << "] : " << std::endl;
		//DSTR << vecf3array[l] << std::endl;

		//係数の積をとる
		//この節点で構成される四面体の面積の積をとる
		//四面体の節点1,2,3(0以外)で作る三角形の面積
		//l==0番目の時、 123	を代入する
		//l==1			0 23
		//l==2			01 3
		//l==3			012
		//をCalcTriangleAreaに入れることができるようにアルゴリズムを考える。
		//k21
		if(l==0){
			//三角形面を構成する3頂点の熱伝達率の相加平均
			double tempHTR = (vertices[tets[id].vertices[1]].heatTransRatio + vertices[tets[id].vertices[2]].heatTransRatio + vertices[tets[id].vertices[3]].heatTransRatio ) / 3.0;		//HTR:HeatTransRatio
			double avgTc = (vertices[tets[id].vertices[1]].Tc + vertices[tets[id].vertices[2]].Tc + vertices[tets[id].vertices[3]].Tc ) / 3.0;
			vecf3array[l] = tempHTR * avgTc * (1.0/3.0) * CalcTriangleArea( tets[id].vertices[1],tets[id].vertices[2],tets[id].vertices[3] ) * vecf3array[l];
			//DSTR << "vecf3array[" << l << "] : " << vecf3array[l] << std::endl;
			
			//>	↓は本当？
			//Vecの節点毎にその節点での周囲流体温度Tcとの積を行う
			//for(unsigned m=0; m<4; m++){
			//	vecf3array[l][m] = vertices[tets[id].vertices[m]].Tc * vecf3array[l][m];
			//}
		}
		//	k22
		else if(l==1){
			double tempHTR = (vertices[tets[id].vertices[0]].heatTransRatio + vertices[tets[id].vertices[2]].heatTransRatio + vertices[tets[id].vertices[3]].heatTransRatio ) / 3.0;		//HTR:HeatTransRatio
			double avgTc = (vertices[tets[id].vertices[0]].Tc + vertices[tets[id].vertices[2]].Tc + vertices[tets[id].vertices[3]].Tc ) / 3.0;		//HTR:HeatTransRatio
			vecf3array[l] = tempHTR * avgTc * (1.0/3.0) * CalcTriangleArea( tets[id].vertices[0],tets[id].vertices[2],tets[id].vertices[3] ) * vecf3array[l];
			//DSTR << "vecf3array[" << l << "] : " << vecf3array[l] << std::endl;
			//Vecの節点毎にその節点での周囲流体温度Tcとの積を行う
			//for(unsigned m=0; m<4; m++){
			//	vecf3array[l][m] = vertices[tets[id].vertices[m]].Tc * vecf3array[l][m];
			//}
		}
		//	k23
		else if(l==2){
			double tempHTR = (vertices[tets[id].vertices[0]].heatTransRatio + vertices[tets[id].vertices[1]].heatTransRatio + vertices[tets[id].vertices[3]].heatTransRatio ) / 3.0;		//HTR:HeatTransRatio
			double avgTc = (vertices[tets[id].vertices[0]].Tc + vertices[tets[id].vertices[1]].Tc + vertices[tets[id].vertices[3]].Tc ) / 3.0;		//HTR:HeatTransRatio
			vecf3array[l] = tempHTR * avgTc * (1.0/3.0) * CalcTriangleArea( tets[id].vertices[0],tets[id].vertices[1],tets[id].vertices[3] ) * vecf3array[l];
			//vecf3array[l] = heatTrans * (1.0/3.0) * CalcTriangleArea( tets[id].vertices[0],tets[id].vertices[1],tets[id].vertices[3] ) * vecf3array[l];
			//DSTR << "vecf3array[" << l << "] : " << vecf3array[l] << std::endl;
			//Vecの節点毎にその節点での周囲流体温度Tcとの積を行う
			//for(unsigned m=0; m<4; m++){
			//	vecf3array[l][m] = vertices[tets[id].vertices[m]].Tc * vecf3array[l][m];
			//}
		}
		//	k24
		else if(l==3){
			double tempHTR = (vertices[tets[id].vertices[0]].heatTransRatio + vertices[tets[id].vertices[1]].heatTransRatio + vertices[tets[id].vertices[2]].heatTransRatio ) / 3.0;		//HTR:HeatTransRatio
			double avgTc = (vertices[tets[id].vertices[0]].Tc + vertices[tets[id].vertices[1]].Tc + vertices[tets[id].vertices[2]].Tc ) / 3.0;		//HTR:HeatTransRatio
			vecf3array[l] = tempHTR * avgTc * (1.0/3.0) * CalcTriangleArea( tets[id].vertices[0],tets[id].vertices[1],tets[id].vertices[2] ) * vecf3array[l];
			//DSTR << "vecf3array[" << l << "] : " << vecf3array[l] << std::endl;
			//Vecの節点毎にその節点での周囲流体温度Tcとの積を行う
			//for(unsigned m=0; m<4; m++){
			//	vecf3array[l][m] = vertices[tets[id].vertices[m]].Tc * vecf3array[l][m];
			//}
		}
		//for debug
		//DSTR << "vecf3array[" << l << "]の完成版は↓" << std::endl;
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
		tets[id].vecf[2] += vecf3array[i];
		//	for debug
		//DSTR << "vecf3 に vecf3array = f3" << i+1 <<"まで加算した行列" << std::endl;
		//DSTR << vecf3 << std::endl;
	}
	
	//	f1,f2,f3,f4	を計算する際に、[0][0]成分から[3][0]成分までの非0成分について、先にTcをかけてしまう


	//for debug
	//DSTR << "節点（";
	//for(unsigned i =0; i < 4; i++){
	//	DSTR << tets[id].vertices[i] << "," ;
	//}
	//DSTR << ")で構成される四面体の" << std::endl;
	//DSTR << "vecf3 : " << std::endl;
	//DSTR << vecf3 << std::endl;
	//int hogeshishi =0;
}

void PHFemMeshThermo::CreateVecf3(unsigned id){
	//	初期化
	for(unsigned i =0; i < 4 ;i++){
		//vecf3[i] = 0.0;		//最後に入れる行列を初期化
		tets[id].vecf[2][i] =0.0;
	}	
	//l=0の時f31,1:f32, 2:f33, 3:f34	を生成
	for(unsigned l= 0 ; l < 4; l++){
		//matk2array[l] = matk2temp;
		vecf3array[l] = Create41Vec1();
		//	l行を0に
		//for(int i=0;i<4;i++){
		//	vecf3array[l][l][i] = 0.0;
		//}
		vecf3array[l][l] = 0.0;
		//array[n][m][l]	= narray[n],m行l列
		//	f_3	(vecf3array[0], vecf3array[1],..)
		// =	| 0 | + | 1 |+...
		//		| 1 |   | 0 |
		//		| 1 |   | 1 |
		//		| 1 |   | 1 |
		//	for debug
		//DSTR << "vecf3array[" << l << "] : " << std::endl;
		//DSTR << vecf3array[l] << std::endl;

		//係数の積をとる
		//この節点で構成される四面体の面積の積をとる
		//四面体の節点1,2,3(0以外)で作る三角形の面積
		//l==0番目の時、 123	を代入する
		//l==1			0 23
		//l==2			01 3
		//l==3			012
		//をCalcTriangleAreaに入れることができるようにアルゴリズムを考える。
		//k21
		if(l==0){
			//三角形面を構成する3頂点の熱伝達率の相加平均
			double tempHTR = (vertices[tets[id].vertices[1]].heatTransRatio + vertices[tets[id].vertices[2]].heatTransRatio + vertices[tets[id].vertices[3]].heatTransRatio ) / 3.0;		//HTR:HeatTransRatio
			vecf3array[l] = tempHTR * (1.0/3.0) * CalcTriangleArea( tets[id].vertices[1],tets[id].vertices[2],tets[id].vertices[3] ) * vecf3array[l];
			//vecf3array[l] = heatTrans * (1.0/3.0) * CalcTriangleArea( tets.vertices[1],tets.vertices[2],tets.vertices[3] ) * vecf3array[l];
			//DSTR << "vecf3array[" << l << "] : " << vecf3array[l] << std::endl;
			//Vecの節点毎にその節点での周囲流体温度Tcとの積を行う
			for(unsigned m=0; m<4; m++){
				vecf3array[l][m] = vertices[tets[id].vertices[m]].Tc * vecf3array[l][m];
			}
		}
		//	k22
		else if(l==1){
			double tempHTR = (vertices[tets[id].vertices[0]].heatTransRatio + vertices[tets[id].vertices[2]].heatTransRatio + vertices[tets[id].vertices[3]].heatTransRatio ) / 3.0;		//HTR:HeatTransRatio
			vecf3array[l] = tempHTR * (1.0/3.0) * CalcTriangleArea( tets[id].vertices[0],tets[id].vertices[2],tets[id].vertices[3] ) * vecf3array[l];
			//vecf3array[l] = heatTrans * (1.0/3.0) * CalcTriangleArea( tets[id].vertices[0],tets[id].vertices[2],tets[id].vertices[3] ) * vecf3array[l];
			//DSTR << "vecf3array[" << l << "] : " << vecf3array[l] << std::endl;
			//Vecの節点毎にその節点での周囲流体温度Tcとの積を行う
			for(unsigned m=0; m<4; m++){
				vecf3array[l][m] = vertices[tets[id].vertices[m]].Tc * vecf3array[l][m];
			}
		}
		//	k23
		else if(l==2){
			double tempHTR = (vertices[tets[id].vertices[0]].heatTransRatio + vertices[tets[id].vertices[1]].heatTransRatio + vertices[tets[id].vertices[3]].heatTransRatio ) / 3.0;		//HTR:HeatTransRatio
			vecf3array[l] = tempHTR * (1.0/3.0) * CalcTriangleArea( tets[id].vertices[0],tets[id].vertices[1],tets[id].vertices[3] ) * vecf3array[l];
			//vecf3array[l] = heatTrans * (1.0/3.0) * CalcTriangleArea( tets[id].vertices[0],tets[id].vertices[1],tets[id].vertices[3] ) * vecf3array[l];
			//DSTR << "vecf3array[" << l << "] : " << vecf3array[l] << std::endl;
			//Vecの節点毎にその節点での周囲流体温度Tcとの積を行う
			for(unsigned m=0; m<4; m++){
				vecf3array[l][m] = vertices[tets[id].vertices[m]].Tc * vecf3array[l][m];
			}
		}
		//	k24
		else if(l==3){
			double tempHTR = (vertices[tets[id].vertices[0]].heatTransRatio + vertices[tets[id].vertices[1]].heatTransRatio + vertices[tets[id].vertices[2]].heatTransRatio ) / 3.0;		//HTR:HeatTransRatio
			vecf3array[l] = tempHTR * (1.0/3.0) * CalcTriangleArea( tets[id].vertices[0],tets[id].vertices[1],tets[id].vertices[2] ) * vecf3array[l];
			//vecf3array[l] = heatTrans * (1.0/3.0) * CalcTriangleArea( tets[id].vertices[0],tets[id].vertices[1],tets[id].vertices[2] ) * vecf3array[l];
			//DSTR << "vecf3array[" << l << "] : " << vecf3array[l] << std::endl;
			//Vecの節点毎にその節点での周囲流体温度Tcとの積を行う
			for(unsigned m=0; m<4; m++){
				vecf3array[l][m] = vertices[tets[id].vertices[m]].Tc * vecf3array[l][m];
			}
		}
		//for debug
		//DSTR << "vecf3array[" << l << "]の完成版は↓" << std::endl;
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
		tets[id].vecf[2] += vecf3array[i];
		//	for debug
		//DSTR << "vecf3 に vecf3array = f3" << i+1 <<"まで加算した行列" << std::endl;
		//DSTR << vecf3 << std::endl;
	}
	
	//	f1,f2,f3,f4	を計算する際に、[0][0]成分から[3][0]成分までの非0成分について、先にTcをかけてしまう


	//for debug
	//DSTR << "節点（";
	//for(unsigned i =0; i < 4; i++){
	//	DSTR << tets[id].vertices[i] << "," ;
	//}
	//DSTR << ")で構成される四面体の" << std::endl;
	//DSTR << "vecf3 : " << std::endl;
	//DSTR << vecf3 << std::endl;
	//int hogeshishi =0;
}

double PHFemMeshThermo::CalcTetrahedraVolume(Tet tets){
	PTM::TMatrixRow<4,4,double> tempMat44;
	for(unsigned i =0; i < 4; i++){
		for(unsigned j =0; j < 4; j++){
			if(i == 0){
				tempMat44[i][j] = 1.0;
			}
			else if(i == 1){
				tempMat44[i][j] = vertices[tets.vertices[j]].pos.x;
			}
			else if(i == 2){
				tempMat44[i][j] = vertices[tets.vertices[j]].pos.y;
			}
			else if(i == 3){
				tempMat44[i][j] = vertices[tets.vertices[j]].pos.z;
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

double PHFemMeshThermo::CalcTriangleArea(int id0, int id1, int id2){
	double area=0.0;								///	要改善	faces[id].areaに値を入れる 

	//行列式の成分を用いて面積を求める
	//triarea =
	//|  1     1     1   |
	//|x2-x1 y2-y1 z2-z1 |
	//|x3-x1 y3-y1 z3-z1 |
	//|
	PTM::TMatrixRow<3,3,double> triarea;		//三角形の面積　= tri + area
	for(unsigned i =0 ; i < 3 ; i++){
		triarea[0][i] = 1.0;
	}
	for(unsigned i =0 ; i < 3 ; i++){
		//					x2(節点2のx(pos第i成分)目)	-	x1(〃)
		// i==0の時	vertices[id1].pos[i]	=>	 pos[0] == pos.x
		triarea[1][i] = vertices[id1].pos[i] - vertices[id0].pos[i];
	}
	for(unsigned i =0 ; i < 3 ; i++){
		//					x3(節点3のx(pos第i成分)目)	-	x1(〃)
		triarea[2][i] = vertices[id2].pos[i] - vertices[id0].pos[i];
	}
	double m1,m2,m3 = 0.0;
	m1 = triarea[1][1] * triarea[2][2] - triarea[1][2] * triarea[2][1];
	m2 = triarea[2][0] * triarea[1][2] - triarea[1][0] * triarea[2][2];
	m3 = triarea[1][0] * triarea[2][1] - triarea[2][0] * triarea[1][1];

	area = sqrt(m1 * m1 + m2 * m2 + m3 * m3) / 2.0;

	//	for debug
	//DSTR << "三角形の面積は : " << area << std::endl; 

	//0番目の節点は40,1番目の節点は134,2番目の節点は79 の座標で計算してみた
	//三角形を求める行列 : 2.75949e-005 * 1 = 2.75949 × 10-5(byGoogle計算機) [m^2] = 2.75949 × 10-1 [cm^2]なので、ネギのメッシュのスケールなら大体あっているはず

	return area;
}

PTM::TMatrixCol<4,1,double> PHFemMeshThermo::Create41Vec1(){
	PTM::TMatrixCol<4,1,double> Mat1temp;
	for(int i =0; i <4 ; i++){
				Mat1temp[i][0] = 1.0;
	}
	return Mat1temp;
}

PTM::TMatrixRow<4,4,double> PHFemMeshThermo::Create44Mat21(){
	//|2 1 1 1 |
	//|1 2 1 1 |
	//|1 1 2 1 |
	//|1 1 1 2 |	を作る
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
void PHFemMeshThermo::CreateMatk3t(unsigned id){
	//l=0の時k21,1の時:k22, 2の時:k23, 3の時:k24	を生成
	for(unsigned l= 0 ; l < 4; l++){
		//matk2array[l] = matk2temp;
		matk3array[l] = Create44Mat21();
		//	1行i列を0に
		for(int i=0;i<4;i++){
			matk3array[l][l][i] = 0.0;
		}
		//	i行1列を0に
		for(int i=0;i<4;i++){
			matk3array[l][i][l] = 0.0;
		}
	}

	///	初期化
	tets[id].matk[2].clear();
	
	for(unsigned l= 0 ; l < 4; l++){
		///	四面体の各面(l = 0 〜 3) についてメッシュ表面かどうかをチェックする。表面なら、行列を作ってmatk2arrayに入れる
		//faces[tets.faces[i]].sorted;		/// 1,24,58みたいな節点番号が入っている
		///	行列型の入れ物を用意

		//faces[tets.faces[l]].vertices;
		if(tets[id].faces[l] < (int)nSurfaceFace && faces[tets[id].faces[l]].alphaUpdated ){			///	外殻の面 且つ 熱伝達率が更新されたら matk2を更新する必要がある
			//最後に入れる行列を初期化
			for(unsigned i =0; i < 4 ;i++){
				for(unsigned j =0; j < 4 ;j++){
					tets[id].matk[2][i][j] = 0.0;
				}
			}
			///	四面体の三角形の面積を計算		///	この関数の外で面積分の面積計算を実装する。移動する
			if(faces[tets[id].faces[l]].area ==0 || faces[tets[id].faces[l]].deformed ){		///	面積が計算されていない時（はじめ） or deformed(変形した時・初期状態)がtrueの時		///	条件の追加	面積が0か ||(OR) αが更新されたか
				faces[tets[id].faces[l]].area = CalcTriangleArea(faces[tets[id].faces[l]].vertices[0], faces[tets[id].faces[l]].vertices[1], faces[tets[id].faces[l]].vertices[2]);
				faces[tets[id].faces[l]].deformed = false;
			}
			///	計算結果を行列に代入
			///	areaの計算に使っていない点が入っている行と列を除いた行列の積をとる
			///	積分計算を根本から考える
			unsigned vtx = tets[id].vertices[0] + tets[id].vertices[1] + tets[id].vertices[2] + tets[id].vertices[3];
			//DSTR << "vtx: " << vtx <<std::endl;
				///	area計算に使われていない節点ID：ID
			unsigned ID = vtx -( faces[tets[id].faces[l]].vertices[0] + faces[tets[id].faces[l]].vertices[1] + faces[tets[id].faces[l]].vertices[2] );
			//DSTR << "メッシュ表面の面は次の3頂点からなる。" << std::endl;
			//DSTR << "faces[tets.faces[l]].vertices[0]: " << faces[tets.faces[l]].vertices[0] <<std::endl;
			//DSTR << "faces[tets.faces[l]].vertices[1]: " << faces[tets.faces[l]].vertices[1] <<std::endl;
			//DSTR << "faces[tets.faces[l]].vertices[2]: " << faces[tets.faces[l]].vertices[2] <<std::endl;
			//DSTR << "ID: " << ID <<"のときの節点と対面する面で面積分を計算する"<<std::endl;
			for(unsigned j=0;j<4;j++){
				if(tets[id].vertices[j] == ID){					///	形状関数が１、（すなわち）このfaceに対面する頂点　と一致したら　その時のfaceで面積分する
					///	j番目の行列の成分を0にしたmatk2arrayで計算する
					///	外殻にないメッシュ面の面積は0で初期化しておく
					faces[tets[id].faces[l]].thermalEmissivity = (vertices[faces[tets[id].faces[l]].vertices[0]].thermalEmissivity + vertices[faces[tets[id].faces[l]].vertices[1]].thermalEmissivity 
						+ vertices[faces[tets[id].faces[l]].vertices[2]].thermalEmissivity ) / 3.0;		///	当該faceの熱伝達率を構成節点での値の相加平均をとる
					///	熱輻射率を計算、熱輻射率も、節点の熱輻射率から計算？

					///	以下の[]は上までの[l]と異なる。
					///	IDが何番目かによって、形状関数の係数が異なるので、
					tets[id].matk[2] += faces[tets[id].faces[l]].thermalEmissivity * (1.0/12.0) * faces[tets[id].faces[l]].area * matk2array[j];
					//DSTR << "tets[id].matk2にfaces[tets[id].faces[l]].heatTransRatio * (1.0/12.0) * faces[tets[id].faces[l]].area * matk2array[" << j << "]"<< "を加算: " <<faces[tets[id].faces[l]].heatTransRatio * (1.0/12.0) * faces[tets[id].faces[l]].area * matk2array[j] << std::endl;
					//DSTR << "tets[id].matk2 +=  " << tets[id].matk2 << std::endl;
				}
			}
		}
	}
}
void PHFemMeshThermo::CreateMatk2t(unsigned id){

	//l=0の時k21,1の時:k22, 2の時:k23, 3の時:k24	を生成
	for(unsigned l= 0 ; l < 4; l++){
		//matk2array[l] = matk2temp;
		matk2array[l] = Create44Mat21();
		//	1行i列を0に
		for(int i=0;i<4;i++){
			matk2array[l][l][i] = 0.0;
		}
		//	i行1列を0に
		for(int i=0;i<4;i++){
			matk2array[l][i][l] = 0.0;
		}
	}

	///	初期化
	tets[id].matk[1].clear();
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
		///	四面体の各面(l = 0 〜 3) についてメッシュ表面かどうかをチェックする。表面なら、行列を作ってmatk2arrayに入れる
		//faces[tets.faces[i]].sorted;		/// 1,24,58みたいな節点番号が入っている
		///	行列型の入れ物を用意

		//faces[tets.faces[l]].vertices;
		if(tets[id].faces[l] < (int)nSurfaceFace && faces[tets[id].faces[l]].alphaUpdated ){			///	外殻の面 且つ 熱伝達率が更新されたら matk2を更新する必要がある
			//最後に入れる行列を初期化
			for(unsigned i =0; i < 4 ;i++){
				for(unsigned j =0; j < 4 ;j++){
					//matk2[i][j] = 0.0;
					//tets[id].matk2[i][j] = 0.0;
					tets[id].matk[1][i][j] = 0.0;
				}
			}
			///	四面体の三角形の面積を計算		///	この関数の外で面積分の面積計算を実装する。移動する
			if(faces[tets[id].faces[l]].area ==0 || faces[tets[id].faces[l]].deformed ){		///	面積が計算されていない時（はじめ） or deformed(変形した時・初期状態)がtrueの時		///	条件の追加	面積が0か ||(OR) αが更新されたか
				faces[tets[id].faces[l]].area = CalcTriangleArea(faces[tets[id].faces[l]].vertices[0], faces[tets[id].faces[l]].vertices[1], faces[tets[id].faces[l]].vertices[2]);
				faces[tets[id].faces[l]].deformed = false;
			}
			///	計算結果を行列に代入
			///	areaの計算に使っていない点が入っている行と列を除いた行列の積をとる
			///	積分計算を根本から考える
			unsigned vtx = tets[id].vertices[0] + tets[id].vertices[1] + tets[id].vertices[2] + tets[id].vertices[3];
			//DSTR << "vtx: " << vtx <<std::endl;
			
			///	area計算に使われていない節点ID：ID
			unsigned ID = vtx -( faces[tets[id].faces[l]].vertices[0] + faces[tets[id].faces[l]].vertices[1] + faces[tets[id].faces[l]].vertices[2] );
			//DSTR << "メッシュ表面の面は次の3頂点からなる。" << std::endl;
			//DSTR << "faces[tets.faces[l]].vertices[0]: " << faces[tets.faces[l]].vertices[0] <<std::endl;
			//DSTR << "faces[tets.faces[l]].vertices[1]: " << faces[tets.faces[l]].vertices[1] <<std::endl;
			//DSTR << "faces[tets.faces[l]].vertices[2]: " << faces[tets.faces[l]].vertices[2] <<std::endl;
			//DSTR << "ID: " << ID <<"のときの節点と対面する面で面積分を計算する"<<std::endl;
			for(unsigned j=0;j<4;j++){
				if(tets[id].vertices[j] == ID){					///	形状関数が１、（すなわち）このfaceに対面する頂点　と一致したら　その時のfaceで面積分する
					///	j番目の行列の成分を0にしたmatk2arrayで計算する
					///	外殻にないメッシュ面の面積は0で初期化しておく
					faces[tets[id].faces[l]].heatTransRatio = (vertices[faces[tets[id].faces[l]].vertices[0]].heatTransRatio + vertices[faces[tets[id].faces[l]].vertices[1]].heatTransRatio 
						+ vertices[faces[tets[id].faces[l]].vertices[2]].heatTransRatio ) / 3.0;		///	当該faceの熱伝達率を構成節点での値の相加平均をとる
					///	以下の[]は上までの[l]と異なる。
					///	IDが何番目かによって、形状関数の係数が異なるので、
					tets[id].matk[1] += faces[tets[id].faces[l]].heatTransRatio * (1.0/12.0) * faces[tets[id].faces[l]].area * matk2array[j];		//元はtets[id].matk2 +=
					//DSTR << "tets[id].matk2にfaces[tets[id].faces[l]].heatTransRatio * (1.0/12.0) * faces[tets[id].faces[l]].area * matk2array[" << j << "]"<< "を加算: " <<faces[tets[id].faces[l]].heatTransRatio * (1.0/12.0) * faces[tets[id].faces[l]].area * matk2array[j] << std::endl;
					//DSTR << "tets[id].matk2 +=  " << tets[id].matk2 << std::endl;
				}
				//else{
				//	///	IDと一致しない場合には、matk2array[j]には全成分0を入れる
				//	///	としたいところだが、
				//	//matk2array[j] =0.0 * matk2array[j];
				//	//DSTR << "matk2array[" << j << "]: " << matk2array[j] << std::endl;
				//}
			}
		}
		///	SurfaceFaceじゃなかったら、matk2arrayには0を入れる
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
	//	//DSTR << "matk2 に matk2array = k2" << i+1 <<"まで加算した行列" << std::endl;
	//	//DSTR << matk2 << std::endl;
	//}
	
	//for debug
	//DSTR << "節点（";
	//for(unsigned i =0; i < 4; i++){
	//	DSTR << tets[id].vertices[i] << "," ;
	//}
	//DSTR << ")で構成される四面体の" << std::endl;
	//DSTR << "matk2 : " << std::endl;
	//DSTR << matk2 << std::endl;
	//int hogeshishi =0;
	
	//DSTR << "Inner Function _ matk2t tets[id].matk2: " <<tets[id].matk2 << std::endl;
}
void PHFemMeshThermo::CreateMatk2(unsigned id,Tet tets){
	//この計算を呼び出すときに、各四面体ごとに計算するため、四面体の0番から順にこの計算を行う
	//四面体を構成する4節点を節点の配列(Tetsには、節点の配列が作ってある)に入っている順番を使って、面の計算を行ったり、行列の計算を行ったりする。
	//そのため、この関数の引数に、四面体要素の番号を取る

	//最後に入れる行列を初期化
	for(unsigned i =0; i < 4 ;i++){
		for(unsigned j =0; j < 4 ;j++){
			//matk2[i][j] = 0.0;
			tets.matk[1][i][j] = 0.0;
		}
	}

	//2)	
	//l=0の時k21,1:k22, 2:k23, 3:k24	を生成
	for(unsigned l= 0 ; l < 4; l++){
		//matk2array[l] = matk2temp;
		matk2array[l] = Create44Mat21();
		//	1行i列を0に
		for(int i=0;i<4;i++){
			matk2array[l][l][i] = 0.0;
		}
		//	i行1列を0に
		for(int i=0;i<4;i++){
			matk2array[l][i][l] = 0.0;
		}
		//	k_21	
		// =	|0 0 0 0 |
		//		|0 2 1 1 |
		//		|0 1 2 1 |
		//		|0 1 1 2 |
		//	for debug
		//DSTR <<"matk2array[" << l << "] : " << std::endl;
		//DSTR << matk2array[l] << std::endl;

		//係数の積をとる
		//この節点で構成される四面体の面積の積をとる

		//節点を見てみよう♪
		//for(unsigned i =0; i < 4 ; i++){
		//	DSTR << "k2"<< l << "行列の "<< i <<"番目の節点は" << tets.vertices[i] << std::endl;
		//}
	
		//四面体の節点1,2,3(0以外)で作る三角形の面積
		//l==0番目の時、 123	を代入する
		//l==1			0 23
		//l==2			01 3
		//l==3			012
		//をCalcTriangleAreaに入れることができるようにアルゴリズムを考える。
		//k21
		//この面は外殻か？外殻なら、計算を実行
			///	vertices
			///	surfaceVerticesかどうかで判定
		//areaは計算されていないか？されているのなら、faces.areaの値を使う
		if(l==0){
			///	面の熱伝達係数を、
			double tempHTR = (vertices[tets.vertices[1]].heatTransRatio + vertices[tets.vertices[2]].heatTransRatio + vertices[tets.vertices[3]].heatTransRatio ) / 3.0;
			matk2array[l] = tempHTR * (1.0/12.0) * CalcTriangleArea( tets.vertices[1],tets.vertices[2],tets.vertices[3] ) * matk2array[l];
		}
		//	k22
		else if(l==1){
			double tempHTR = (vertices[tets.vertices[0]].heatTransRatio + vertices[tets.vertices[2]].heatTransRatio + vertices[tets.vertices[3]].heatTransRatio ) / 3.0;
			matk2array[l] = tempHTR * (1.0/12.0) * CalcTriangleArea( tets.vertices[0],tets.vertices[2],tets.vertices[3] ) * matk2array[l];
		}
		//	k23
		else if(l==2){
			double tempHTR = (vertices[tets.vertices[0]].heatTransRatio + vertices[tets.vertices[1]].heatTransRatio + vertices[tets.vertices[3]].heatTransRatio ) / 3.0;
			matk2array[l] = tempHTR * (1.0/12.0) * CalcTriangleArea( tets.vertices[0],tets.vertices[1],tets.vertices[3] ) * matk2array[l];
		}
		//	k24
		else if(l==3){
			double tempHTR = (vertices[tets.vertices[0]].heatTransRatio + vertices[tets.vertices[1]].heatTransRatio + vertices[tets.vertices[2]].heatTransRatio ) / 3.0;
			matk2array[l] = tempHTR * (1.0/12.0) * CalcTriangleArea( tets.vertices[0],tets.vertices[1],tets.vertices[2] ) * matk2array[l];
		}
		//for debug
		//DSTR << "matk2array[" << l << "]の完成版は↓" << std::endl;
		//DSTR << matk2array[l] << std::endl;
	}

	//k2 = k21 + k22 + k23 + k24
	for(unsigned i=0; i < 4; i++){
		tets.matk[1] += matk2array[i];
		//	for debug
		//DSTR << "matk2 に matk2array = k2" << i+1 <<"まで加算した行列" << std::endl;
		//DSTR << matk2 << std::endl;
	}
	
	//for debug
	//DSTR << "節点（";
	//for(unsigned i =0; i < 4; i++){
	//	DSTR << tets.vertices[i] << "," ;
	//}
	//DSTR << ")で構成される四面体の" << std::endl;
	//DSTR << "matk2 : " << std::endl;
	//DSTR << matk2 << std::endl;
	//int hogeshishi =0;
}

void PHFemMeshThermo::SetInitThermoConductionParam(double thConduct0,double roh0,double specificHeat0,double heatTrans0){
	thConduct = thConduct0;
	rho = roh0;
	specificHeat = specificHeat0;
	heatTrans = heatTrans0;
}
void PHFemMeshThermo::SetThermalEmissivityToVtx(unsigned id,double thermalEmissivity){
	vertices[id].thermalEmissivity = thermalEmissivity;
}
void PHFemMeshThermo::SetThermalEmissivityToVerticesAll(double thermalEmissivity){
	for(unsigned i =0; i < vertices.size(); i++){
		vertices[i].thermalEmissivity = thermalEmissivity;
	}
}
void PHFemMeshThermo::SetHeatTransRatioToAllVertex(){
	for(unsigned i =0; i < vertices.size() ; i++){
		vertices[i].heatTransRatio = heatTrans;
	}
}

void PHFemMeshThermo::SetTempAllToTVecAll(unsigned size){
	for(unsigned i =0; i < size;i++){
		TVecAll[i] = vertices[i].temp;
	}
}

void PHFemMeshThermo::CreateTempMatrix(){
	unsigned dmnN = (unsigned)vertices.size();
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

void PHFemMeshThermo::SetTempToTVecAll(unsigned vtxid){
	if(0 <= vtxid && vtxid < TVecAll.size()){
		TVecAll[vtxid] = vertices[vtxid].temp;
	}
}

void PHFemMeshThermo::UpdateheatTransRatio(unsigned id,double heatTransRatio){
	//if(vertices[id].heatTransRatio != heatTransRatio){	//異なっていたら
	//	vertices[id].heatTransRatio = heatTransRatio;	
	//	///	αを含む行列の更新	K2,f3
	//	///	f3
	//	for(unsigned i =0; i < vertices[1].tets.size(); i++){
	//		CreateVecf3(tets[vertices[id].tets[i]]);
	//	}
	//	///	K3

	//}
	///	同じなら何もしない
}

void PHFemMeshThermo::SetLocalFluidTemp(unsigned i,double temp){
	vertices[i].Tc = temp;			//節点の周囲流体温度の設定
}

void PHFemMeshThermo::SetVertexTemp(unsigned i,double temp){
	vertices[i].temp = temp;
	SetTempToTVecAll(i);
}

void PHFemMeshThermo::SetVerticesTempAll(double temp){
	for(std::vector<unsigned int>::size_type i=0; i < vertices.size() ; i++){
		vertices[i].temp = temp;
		SetTempToTVecAll(i);	// 要検討：Aftersetdeskの中で呼ばれる時、TVecAllの要素の数が固まっておらず、アクセス違反の可能性がある
	}
}

void PHFemMeshThermo::AddvecFAll(unsigned id,double dqdt){
	vecFAllSum[id] += dqdt;		//	+=に変更
}

void PHFemMeshThermo::SetvecFAll(unsigned id,double dqdt){
	vecFAllSum[id] = dqdt;		//	+=に変更すべきで、削除予定
}

void PHFemMeshThermo::InitAllVertexTemp(){
	//	このメッシュの全長点の温度を0にする
	for(unsigned i=0; i < vertices.size(); i++){
		vertices[i].temp = 0.0;
		//	どのメッシュでリセットかけたか　GetMe()->
	}
}

void PHFemMeshThermo::InitMoist(){
	for(unsigned id =0; id < tets.size(); id++){
		tets[id].wratio = 0.917;
		double rho = 970;
		if(tets[id].volume){
			tets[id].tetsMg = tets[id].volume * rho;	//質量*密度
			tets[id].wmass = tets[id].tetsMg * tets[id].wratio;
		}else if(tets[id].volume < 0.0){
			DSTR << "tets[" << id << "]の体積が計算されていません" << std::endl;
			CalcTetrahedraVolume(tets[id]);
		}
	}
}

void PHFemMeshThermo::DecrMoist(){
	for(unsigned id =0; id < tets.size() ; id++){
		////頂点が100度以上で残水量が０ではないとき
		double tempAdd = 0.0;	//	加算温度
		for(unsigned i=0; i < 4; i++){
			tempAdd += vertices[tets[id].vertices[i]].temp;
		}
		//単位換算は合っているか？
		double wlatheat = 2.26 * 1000;		//水1kg当たりの潜熱(latent heat)[W・s]=[J] 水の潜熱が540cal/gよりJに変換して使用   W=J/s 2.26[kJ/kg]
		tets[id].tetsMg = tets[id].volume * rho;		//四面体の質量
		double wlat = (tempAdd / 4.0) - 100.0;	//100度を超えただけ蒸発する。
		double dw = wlat * specificHeat *  tets[id].tetsMg / wlatheat;	//	水分蒸発量	//	(温度と沸点100度との差分の熱量)÷水の潜熱で蒸発する水の量が分かる。;		//	水分蒸発量
		double exwater	= 0;	//流出する水の量全体(蒸発 + 流出 + 水分移動)
		//平均温度が100度超過
		if( tempAdd / 4.0 >= 100.0){
			//dwの分だけ、質量や水分量から引く
			//double delw = (dt / 0.01 * 1.444*(0.000235/0.29)  / 10000000)*100;
			double delw = (1.444*(0.000235/0.29)  / 10000000)*100;
			exwater = delw * 500;
			tets[id].tetsMg -= dw - exwater;
			if(tets[id].wmass > dw - exwater){
				tets[id].wmass -= dw - exwater;
			}else{
				DSTR << "水分流出量が多すぎます" << std::endl;
			}
			//検証する:ひとまず、exwaterが０でなければ、音を再生させることにしようか。音を出したら、そのメッシュのexwaterの値を０にしよう。
			//wlatの分だけ、温度から引く
			for(unsigned j=0; j < 4; j++){
				vertices[j].temp -= wlat;
			}
		}
		//とりあえず、簡単に、水分を減らすコード
		//if(tets[id].wmass >= tets[id].wmass_start *0.01){
		//	tets[id].wmass -= tets[id].wmass_start * 0.01;
		//}
	}
}

}


