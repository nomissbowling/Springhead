/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#ifndef PH_IKACTUATOR_H
#define PH_IKACTUATOR_H

#include <SprPhysics.h>
#include <Foundation/Object.h>
#include <Physics/PHEngine.h>
#include <Physics/PHIKEndEffector.h>

#include "PhysicsDecl.hpp"

#include <vector>
#include <set>
#include <map>

namespace Spr{;

class PHIKActuator;

// ---- ---- ---- ---- ---- ---- ---- ---- ---- ----
// IKActuator：IKに使用できるアクチュエータ（作動部品）
// 

class PHIKActuator : public SceneObject{
public:
	SPR_OBJECTDEF(PHIKActuator);
	SPR_DECLMEMBEROF_PHIKActuatorDesc;

	/// 簡略表記用typedef
	typedef std::set<PHIKEndEffector*>	ESet;
	typedef ESet::iterator				ESetIter;
	typedef std::set<PHIKActuator*>		ASet;
	typedef ASet::iterator				ASetIter;
	
	/// このアクチュエータで動かせるエンドエフェクタのリスト
	ESet linkedEndEffectors;

	/// 「このアクチュエータで動かせるいずれかのエンドエフェクタ」を動かせる他のアクチュエータ
	ASet linkedActuators;

	// --- --- --- --- ---

	/// 番号
	int number;

	/// 自由度
	int ndof;

	/// 自由度変化フラグ
	bool bNDOFChanged;

	/// エンドエフェクタ追加フラグ
	bool bEndEffectorAdded;

	/// アクチュエータ追加フラグ
	bool bActuatorAdded;

	// --- --- --- --- ---

	/// 計算用の一時変数
	PTM::VVector<double>						alpha, beta;
	std::map< int, PTM::VMatrixRow<double> >	gamma;

	/// ヤコビアン
	std::map< int,PTM::VMatrixRow<double> > Mj;

	/// IKのIterationの一回前の計算結果（収束判定用）
	PTM::VVector<double> omega_prev;

	/// IKの計算結果（角度）
	PTM::VVector<double> omega;

	/// IDの計算結果（トルク）
	PTM::VVector<double> tau;

	// --- --- --- --- --- --- --- --- --- ---

	/** @brief 初期化
	*/
	virtual void Init() {
		number = -1;
		bNDOFChanged = true;
		bEndEffectorAdded = false;
		bActuatorAdded = false;
	}

	/** @brief デフォルトコンストラクタ
	*/
	PHIKActuator(){
		Init();
	}

	/** @brief コンストラクタ
	*/
	PHIKActuator(const PHIKActuatorDesc& desc){
		Init();
		SetDesc(&desc);
	}

	// --- --- --- --- ---

	/** @brief IKの計算準備をする
	*/
	virtual void PrepareSolve();

	/** @brief IKの計算繰返しの１ステップを実行する
	*/
	virtual void ProceedSolve();

	/** @brief 擬似逆解を解いたままの"生の"計算結果を取得する
	*/
	virtual PTM::VVector<double> GetRawSolution(){ return omega; }

	// --- --- --- --- ---

	/** @brief 計算結果に従って制御対象を動かす
	*/
	virtual void Move(){}

	/** @brief 自然位置に戻る
	*/
	virtual void MoveToNaturalPosition(){}

	// --- --- --- --- ---

	/** @brief このアクチュエータを使って動かせるエンドエフェクタ、を登録する
	*/
	void RegisterEndEffector(PHIKEndEffectorIf* endeffector);

	// --- --- --- --- ---

	/** @brief 動かしにくさを設定する
	*/
	void SetBias(float bias){ this->bias = bias; }

	/** @brief 動かしにくさを取得する
	*/
	float GetBias(){ return bias; }

	/** @brief 駆動のためのバネ係数を設定する
	*/
	void SetSpring(double spring){ this->spring = spring; }

	/** @brief 駆動のためのバネ係数を取得
	*/
	double GetSpring(){ return spring; }

	/** @brief 駆動のためのダンパ係数を設定する
	*/
	void SetDamper(double damper){ this->damper = damper; }

	/** @brief 駆動のためのダンパ係数を取得
	*/
	double GetDamper(){ return damper; }

	/** @brief 有効・無効を設定する
	*/
	void Enable(bool enable){ bEnabled = enable; if (!enable) { MoveToNaturalPosition(); } }

	/** @brief 有効・無効を取得する
	*/
	bool IsEnabled(){ return bEnabled; }

	// --- --- --- --- ---

	virtual bool AddChildObject(ObjectIf* o){
		PHIKEndEffectorIf* cp = o->Cast();
		if (cp) {
			RegisterEndEffector(cp);
			return true;
		}
		return false;
	}

	virtual ObjectIf* GetChildObject(size_t pos){
		for (ESetIter it=linkedEndEffectors.begin(); it!=linkedEndEffectors.end(); ++it) {
			if (pos == 0) {
				return (*it)->Cast();
			}
			pos--;
		}
		return NULL;
	}

	size_t NChildObject() {
		int cnt = 0;
		for (ESetIter it=linkedEndEffectors.begin(); it!=linkedEndEffectors.end(); ++it) {
			cnt++;
		}
		return cnt;
	}

	// --- --- --- --- --- --- --- --- --- ---
	// Non API Methods

	/** @brief ヤコビアン計算前の処理
	*/
	virtual void BeforeCalcAllJacobian() {}

	/** @brief 関係するすべての制御点とのヤコビアンをそれぞれ求める
	*/
	void CalcAllJacobian();

	/** @brief 計算用変数を準備する前の処理
	*/
	virtual void BeforeSetupMatrix() {}

	/** @brief 計算用変数を準備する
	*/
	void SetupMatrix();

	/** @brief 指定した制御点との間のヤコビアンを計算する
	*/
	virtual void CalcJacobian(PHIKEndEffector* endeffector) {}

	/** @brief 繰返し計算の一ステップの後に行う処理
	*/
	virtual void AfterProceedSolve() {}

	/** @brief 接続した剛体を直接目標姿勢に移動する
	*/
	virtual void MoveStatic() {}

};

// ---- ---- ---- ---- ---- ---- ---- ---- ---- ----

class PHIKBallActuator : public PHIKActuator{
public:
	SPR_OBJECTDEF(PHIKBallActuator);
	SPR_DECLMEMBEROF_PHIKBallActuatorDesc;

	/// 制御対象の関節
	PHBallJointIf* joint;

	/// 制御対象関節のバネダンパ初期値と基準姿勢
	double			jSpring;
	double			jDamper;
	Quaterniond		jGoal;

	/// IKの回転軸
	Vec3d e[3];

	// --- --- --- --- --- --- --- --- --- ---

	/** @brief 初期化
	*/
	virtual void Init() {
		ndof = 2;
		joint = NULL;
		PHIKActuator::Init();
	}

	/** @brief デフォルトコンストラクタ
	*/
	PHIKBallActuator(){
		Init();
	}

	/** @brief コンストラクタ
	*/
	PHIKBallActuator(const PHIKBallActuatorDesc& desc) {
		Init();
		SetDesc(&desc);
	}

	// --- --- --- --- ---

	/** @brief 計算結果に従って制御対象を動かす
	*/
	virtual void Move();

	/** @brief 自然位置に戻る
	*/
	virtual void MoveToNaturalPosition();

	// --- --- --- --- ---

	/** @brief 動作対象の関節を設定する（１アクチュエータにつき１関節が必ず対応する）
	*/
	virtual void SetJoint(PHBallJointIf* joint) { this->joint = joint; }

	/** @brief 動作対象として設定された関節を取得する
	*/
	virtual PHBallJointIf* GetJoint() { return this->joint; }

	// --- --- --- --- ---

	virtual bool AddChildObject(ObjectIf* o){
		PHBallJointIf* jo = o->Cast();
		if (jo) {
			this->joint = jo;
			PHBallJointDesc dJ; DCAST(PHBallJointIf,this->joint)->GetDesc(&dJ);
			this->jSpring = dJ.spring;
			this->jDamper = dJ.damper;
			this->jGoal   = dJ.targetPosition;
			return true;
		}
		return PHIKActuator::AddChildObject(o);
	}

	virtual ObjectIf* GetChildObject(size_t pos){
		if (pos == 0 && this->joint != NULL) { return this->joint; }
		if (this->joint != NULL) {
			return PHIKActuator::GetChildObject(pos - 1);
		} else {
			return PHIKActuator::GetChildObject(pos);
		}
		return NULL;
	}

	size_t NChildObject() {
		if (this->joint != NULL) { return 1 + PHIKActuator::NChildObject(); }
		return PHIKActuator::NChildObject();
	}

	// --- --- --- --- --- --- --- --- --- ---
	// Non API Methods

	/** @brief 回転軸を計算する
	*/
	virtual void CalcAxis();

	/** @brief ヤコビアン計算前の処理
	*/
	virtual void BeforeCalcAllJacobian();

	/** @brief 計算用変数を準備する前の処理
	*/
	virtual void BeforeSetupMatrix();

	/** @brief 指定した制御点との間のヤコビアンを計算する
	*/
	virtual void CalcJacobian(PHIKEndEffector* endeffector);

	/** @brief 接続した剛体を直接目標姿勢に移動する
	*/
	virtual void MoveStatic();

};

// ---- ---- ---- ---- ---- ---- ---- ---- ---- ----

class PHIKHingeActuator : public PHIKActuator{
public:
	SPR_OBJECTDEF(PHIKHingeActuator);
	SPR_DECLMEMBEROF_PHIKBallActuatorDesc;

	/// 制御対象の関節
	PHHingeJointIf *joint;

	/// 制御対象関節のバネダンパ初期値と基準姿勢
	double		jSpring;
	double		jDamper;
	double		jGoal;

	// --- --- --- --- --- --- --- --- --- ---

	/** @brief 初期化
	*/
	virtual void Init() {
		ndof = 1;
		joint = NULL;
		PHIKActuator::Init();
	}

	/** @brief デフォルトコンストラクタ
	*/
	PHIKHingeActuator(){
		Init();
	}

	/** @brief コンストラクタ
	*/
	PHIKHingeActuator(const PHIKHingeActuatorDesc& desc) {
		Init();
		SetDesc(&desc);
	}

	// --- --- --- --- ---

	/** @brief 計算結果に従って制御対象を動かす
	*/
	virtual void Move();

	/** @brief 自然位置に戻る
	*/
	virtual void MoveToNaturalPosition();

	// --- --- --- --- ---

	/** @brief 動作対象の関節を設定する（１アクチュエータにつき１関節が必ず対応する）
	*/
	virtual void SetJoint(PHHingeJointIf* joint) { this->joint = joint; }

	/** @brief 動作対象として設定された関節を取得する
	*/
	virtual PHHingeJointIf* GetJoint() { return this->joint; }

	// --- --- --- --- ---

	virtual bool AddChildObject(ObjectIf* o){
		PHHingeJointIf* jo = o->Cast();
		if (jo) {
			this->joint = jo;
			this->jSpring = DCAST(PHHingeJointIf,this->joint)->GetSpring();
			this->jDamper = DCAST(PHHingeJointIf,this->joint)->GetDamper();
			this->jGoal   = DCAST(PHHingeJointIf,this->joint)->GetTargetPosition();
			return true;
		}
		return PHIKActuator::AddChildObject(o);
	}
	virtual ObjectIf* GetChildObject(size_t pos){
		if (pos == 0 && this->joint != NULL) { return this->joint; }
		if (this->joint != NULL) {
			return PHIKActuator::GetChildObject(pos - 1);
		} else {
			return PHIKActuator::GetChildObject(pos);
		}
		return NULL;
	}
	size_t NChildObject() {
		if (this->joint != NULL) { return 1 + PHIKActuator::NChildObject(); }
		return PHIKActuator::NChildObject();
	}

	// --- --- --- --- --- --- --- --- --- ---
	// Non API Methods

	/** @brief 指定した制御点との間のヤコビアンを計算する
	*/
	virtual void CalcJacobian(PHIKEndEffector* endeffector);

	/** @brief 繰返し計算の一ステップの後に行う処理
	*/
	virtual void AfterProceedSolve();

	/** @brief 接続した剛体を直接目標姿勢に移動する
	*/
	virtual void MoveStatic();

};

}

#endif
