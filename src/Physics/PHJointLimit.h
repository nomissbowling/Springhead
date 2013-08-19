﻿/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#ifndef PHJOINTLIMIT_H
#define PHJOINTLIMIT_H

#include <Base/TQuaternion.h>
#include <Physics/PhysicsDecl.hpp>
#include <Physics/PHAxisIndex.h>

#include <Physics/SprPHJointLimit.h>
#include <Foundation/Object.h>

namespace Spr{;

class PHScene;
class PHConstraintEngine;
class PH1DJoint;
class PHBallJoint;

///	1自由度関節の可動域拘束
class PH1DJointLimit : public SceneObject {
public:
	SPR_OBJECTDEF(PH1DJointLimit);
	SPR_DECLMEMBEROF_PH1DJointLimitDesc;

	/// 可動域拘束の対象となる関節
	PH1DJoint* joint;

	/// 可動範囲外に出ているか
	bool bOnLimit;

	/// 可動範囲外に出ている量
	double diff;

	/// コンストラクタ
	PH1DJointLimit(const PH1DJointLimitDesc& desc = PH1DJointLimitDesc()) { SetDesc(&desc); }
	
	// ----- Limitの機能

	virtual void SetupAxisIndex();
	virtual void CompBias();

	// ----- インタフェースの実装
	void    SetRange(Vec2d  range)    { this->range = range; }
	void    GetRange(Vec2d &range)    { range = this->range; }
	void    SetSpring(double spring)  { this->spring = spring; }
	double  GetSpring()               { return spring; }
	void    SetDamper(double damper)  { this->damper = damper; }
	double  GetDamper()               { return damper; }
	bool    IsOnLimit()               { return bOnLimit; }
};

// -----  -----  -----  -----  -----  -----  -----  -----  -----  -----  -----  -----  -----  ----- 

///	球関節の可動域拘束（ベースクラス）
class PHBallJointLimit : public SceneObject {
public:
	SPR_OBJECTDEF(PHBallJointLimit);
	SPR_DECLMEMBEROF_PHBallJointLimitState;
	SPR_DECLMEMBEROF_PHBallJointLimitDesc;

	/// 可動域拘束の対象となる関節
	PHBallJoint* joint;

	/// 可動範囲外に出ている量
	Vec3d diff;

	// ----- Joint本体と異なる座標系を使うための独自のJacobianとLCP計算変数

	/// 拘束ヤコビアン
	Matrix3d J[2];

	/// T = M.inv() * J^t ガウスザイデルで使用
	Matrix3d T[2];
	
	/// LCPのbベクトルとその補正量
	Vec3d b, db, B;
	
	/// LCPのA行列の対角成分とその補正量，逆数
	Vec3d A, dA, Ainv;	

	/// Projection用の各軸のMin/Max
	double fMaxDt[3], fMinDt[3];

	/// 拘束軸管理クラス
	AxisIndex<3> axes;

	/// コンストラクタ
	PHBallJointLimit();
	
	// ----- PHBallJointから呼び出される機能

	/// LCPを解く前段階の計算
	void SetupLCP();

	/// LCPの繰り返し計算
	void IterateLCP();

	// ----- このクラスで実装する機能

	/// Aの対角成分を計算する．A = J * M^-1 * J^T
	void CompResponseMatrix();

	/// LCPの補正値の計算．誤差修正用
	void CompBias();

	/// 拘束力変化量(df)に対する加速度変化量(dvの差分)を計算して反映
	void CompResponse(double df, int i);

	// ----- 派生クラスで実装する機能

	/// 拘束座標系のJabocianを計算
	virtual void CompJacobian(){}

	/// 可動域制限にかかっているか確認しどの自由度を速度拘束するかを設定
	virtual void SetupAxisIndex(){}

	// ----- インタフェースの実装
	void    SetSpring(double spring)  { this->spring = spring; }
	double  GetSpring()               { return spring; }
	void    SetDamper(double damper)  { this->damper = damper; }
	double  GetDamper()               { return damper; }
	void SetLimitDir(Vec3d limDir)    { this->limitDir = limDir; }
	Vec3d GetLimitDir()               { return limitDir; }

	virtual bool IsOnLimit()          { return false; }
};

// -----  -----  -----  -----  -----  -----  -----  -----  -----  -----  -----  -----  -----  ----- 

///	球関節の円形可動域拘束
class PHBallJointConeLimit : public PHBallJointLimit{
public:
	SPR_OBJECTDEF(PHBallJointConeLimit);
	SPR_DECLMEMBEROF_PHBallJointConeLimitDesc;

	/// 可動範囲外に出ているか
	bool bOnLimit;

	/// コンストラクタ
	PHBallJointConeLimit(const PHBallJointConeLimitDesc& desc = PHBallJointConeLimitDesc()) {SetDesc(&desc);}
	
	// ----- PHBallJointLimitの派生クラスで実装する機能

	/// 拘束座標系のJabocianを計算
	virtual void CompJacobian();

	/// 可動域制限にかかっているか確認しどの自由度を速度拘束するかを設定
	virtual void SetupAxisIndex();

	// ----- インタフェースの実装
	virtual bool IsOnLimit() { return bOnLimit; }
	void SetSwingRange(Vec2d range) { limitSwing = range; }
	void GetSwingRange(Vec2d& range){ range = limitSwing; }
	void SetTwistRange(Vec2d range) { limitTwist = range; }
	void GetTwistRange(Vec2d& range){ range = limitTwist; }
};

// -----  -----  -----  -----  -----  -----  -----  -----  -----  -----  -----  -----  -----  ----- 


/// スプライン閉曲線上の点（制御点ではなく）
class ClosedSplineCurve;
struct SplinePoint {
	/// 親
	ClosedSplineCurve* spline;

	/// 点の座標（極座標 : Swing, SwingDir）
	Vec2d pos;

	/// 辺番号
	int edge;

	/// パラメータt
	double t;

	/// 値の更新．tが0以下や1以上だったら隣の辺上の点に移る
	void Update();

	/// posからiやtを逆算する
	void ReverseUpdate();

	/// 接線をx軸として座標系を作る
	Matrix3d CompJacobian();
};

/// スプライン閉曲線
class ClosedSplineCurve {
public:
	/// コンストラクタ
	ClosedSplineCurve(std::vector<PHSplineLimitNode>* nds) { nodes = nds; }

	/// 制御点の配列（へのポインタ．実体はLimitのDescが持っている）
	std::vector<PHSplineLimitNode>* nodes;
	
	/// 辺の数
	int NEdges() { return int(nodes->size()-1); /* 始点と終点が被るので */ }

	/// i番目の辺のSwingスプライン係数を返す
	Vec4d GetEdgeSwingCoeff(int i);

	/// i番目の辺のSwingDirスプライン係数を返す
	Vec4d GetEdgeSwingDirCoeff(int i);

	/// 頂点を追加(posが指定された場合は挿入)する
	void AddNode(PHSplineLimitNode node, int pos=-1);

	/// i番目の辺のパラメータtにおける点の極座標を返す
	SplinePoint GetPointOnEdge(int i, double t);
};

///	球関節のスプライン閉曲線可動域拘束
class PHBallJointSplineLimit : public PHBallJointLimit{
public:
	SPR_OBJECTDEF(PHBallJointSplineLimit);
	SPR_DECLMEMBEROF_PHBallJointSplineLimitDesc;

	/// 可動域曲線
	ClosedSplineCurve limitCurve;

	/// 可動範囲外に出ているか
	bool bOnSwing, bOnTwist;
	
	/// 最後に可動域内に居たときの位置（in Swing/SwingDir/Twist）
	Vec3d lastPos;

	/// 現在の関節ポジション（in Swing/SwingDir/Twist）
	Vec3d currPos;

	/// 最近傍点
	SplinePoint neighbor;

	/// コンストラクタ
	PHBallJointSplineLimit(const PHBallJointSplineLimitDesc& desc = PHBallJointSplineLimitDesc()) : limitCurve(&nodes) {
		SetDesc(&desc);

		bOnSwing = false;
		bOnTwist = false;

		lastPos = Vec3d(0,0,0);
	}

	// ----- PHBallJointLimitの派生クラスで実装する機能

	/// 拘束座標系のJabocianを計算
	virtual void CompJacobian();

	/// どの自由度を速度拘束するかを設定
	virtual void SetupAxisIndex();

	// ----- このクラスの機能

	/// Swing-SwingDirの可動域チェック（bOnSwing（とneighbor）を決定する）
	void CheckSwingLimit();

	/// Twistの可動域チェック（bOnTwistを決定する）
	void CheckTwistLimit();

	/// 交点チェック
	int CheckIntersection(Vec2d base, Vec2d curr, double& minDist);

	// ----- インタフェースの実装
	virtual bool IsOnLimit() { return(bOnSwing || bOnTwist); }
	void AddNode(PHSplineLimitNode node)          { limitCurve.AddNode(node, -1);  }
	void AddNode(PHSplineLimitNode node, int pos) { limitCurve.AddNode(node, pos); }
	void AddNode(double S, double SD, double dS, double dSD, double tMin, double tMax) {
		AddNode(PHSplineLimitNode(S,SD,dS,dSD,tMin,tMax));
	}
	void AddNode(double S, double SD, double dS, double dSD, double tMin, double tMax, int pos) {
		AddNode(PHSplineLimitNode(S,SD,dS,dSD,tMin,tMax), pos);
	}
	void SetPoleTwistRange(Vec2d range) { poleTwist = range; }
	void GetPoleTwistRange(Vec2d& range){ range = poleTwist; }
};

// ----- ユーティリティ

/// ３次方程式を解く関数
Vec3d SolveCubicFunction(Vec4d eq3);

/// Swing-SwingDir極座標をユークリッド座標に直す関数
Vec3d FromPolar(Vec2d pos);

}

#endif
