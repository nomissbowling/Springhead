/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#ifndef PHCONSTRAINTENGINE_H
#define PHCONSTRAINTENGINE_H

#include <Physics/PHContactDetector.h>
#include <Physics/PHConstraint.h>
#include <Physics/PHGear.h>

namespace Spr{;

class PHSolid;
class PHJoint;
class PHRootNode;
class PHConstraintEngine;

///	形状の組
class PHShapePairForLCP : public CDShapePair{
public:
	std::vector<Vec3d>	section;	///< 交差断面の頂点．個々がPHContactPointとなる．可視化のために保持
	///	接触解析．接触部分の切り口を求めて，切り口を構成する凸多角形の頂点をengineに拘束として追加する．
	void EnumVertex(PHConstraintEngine* engine, unsigned ct, PHSolid* solid0, PHSolid* solid1);
};

/// Solidの組
class PHConstraintEngine;
class PHSolidPairForLCP : public PHSolidPair<PHShapePairForLCP, PHConstraintEngine>{
public:
	virtual void OnDetect(PHShapePairForLCP* cp, PHConstraintEngine* engine, unsigned ct, double dt);
	virtual void OnContDetect(PHShapePairForLCP* cp, PHConstraintEngine* engine, unsigned ct, double dt);
};

class PHConstraintEngine : public PHContactDetector<PHShapePairForLCP, PHSolidPairForLCP, PHConstraintEngine>, public PHConstraintEngineIfInit{
	friend class PHConstraint;
	friend class PHShapePairForLCP;
	//OBJECTDEF_NOIF(PHConstraintEngine, PHEngine);
	OBJECTDEF(PHConstraintEngine, PHEngine);
public:
	typedef PHContactDetector<PHShapePairForLCP, PHSolidPairForLCP, PHConstraintEngine> Detector;
	int		numIter;					///< 速度更新LCPの反復回数
	int		numIterCorrection;			///< 誤差修正LCPの反復回数
	int		numIterContactCorrection;	///< 接触点の誤差修正LCPの反復回数
	double	velCorrectionRate;			///< 速度のLCPで関節拘束の誤差を修正する場合の誤差修正比率
	double	posCorrectionRate;			///< 位置のLCPで，関節拘束の誤差を修正する場合の誤差修正比率
	double  contactCorrectionRate;		///< 接触の侵入解消のための，速度のLCPでの補正比率．
	double	shrinkRate;					///< LCP初期値を前回の解に対して縮小させる比率
	double	shrinkRateCorrection;
	double	freezeThreshold;			///< 剛体がフリーズする閾値
	bool	bGearNodeReady;				///< ギアノードがうまく構成されているかのフラグ．ノードやギアを追加・削除するたびにfalseになる
	
	PHConstraintEngine();
	~PHConstraintEngine();
	
	PHJoint* CreateJoint(const IfInfo* ii, const PHJointDesc& desc, PHSolid* lhs = NULL, PHSolid* rhs = NULL);	///< 関節の追加する
	PHRootNode* CreateRootNode(const PHRootNodeDesc& desc, PHSolid* solid = NULL);	///< ツリー構造のルートノードを作成
	PHTreeNode* CreateTreeNode(const PHTreeNodeDesc& desc, PHTreeNode* parent = NULL, PHSolid* solid = NULL);	///< ツリー構造の中間ノードを作成
	PHGear*		CreateGear(const PHGearDesc& desc, PHJoint1D* lhs = NULL, PHJoint1D* rhs = NULL);	///< ギアを作成
	PHPath*		CreatePath(const PHPathDesc& desc);
	void		UpdateGearNode();
	virtual int GetPriority() const {return SGBP_CONSTRAINTENGINE;}
	virtual void Step();			///< 
	//virtual void Dynamics(double dt, int ct);		///< 
	//virtual void Correction(double dt, int ct);		///< 
	void UpdateSolids();			///< 結果をSolidに反映する
	void Clear();

	PHConstraints	points;			///< 接触点の配列
	PHConstraints	joints;			///< 関節の配列
	typedef std::vector< UTRef<PHRootNode> > PHRootNodes;
	PHRootNodes		trees;			///< Articulated Body Systemの配列
	PHGears			gears;
	typedef std::vector< UTRef<PHPath> > PHPaths;
	PHPaths			paths;
	
	void SetupLCP();				///< 速度更新LCPの準備
	void IterateLCP();				///< 速度更新LCPの一度の反復
	void SetupCorrectionLCP();		///< 誤差修正LCPの準備
	void IterateCorrectionLCP();	///< 誤差修正LCPの一度の反復

	virtual PHConstraintsIf* GetContactPoints();
	virtual void SetVelCorrectionRate(double value){velCorrectionRate = value;}
	virtual void SetPosCorrectionRate(double value){posCorrectionRate = value;}
	virtual void SetContactCorrectionRate(double value){contactCorrectionRate = value;}

	virtual bool AddChildObject(ObjectIf* o);
	virtual bool DelChildObject(ObjectIf* o);
};

}	//	namespace Spr
#endif
