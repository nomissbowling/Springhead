#ifndef CDSPHERE_H
#define CDSPHERE_H

#include <SprCollision.h>
#include <Foundation/Object.h>
#include "CDConvex.h"

namespace Spr{;

	
/// 球体
//class CDSphere: public InheritNamedObject<CDSphereIf, CDConvex>{
class CDSphere: public  InheritCDShape<CDSphereIf, CDConvex>{
public:
	OBJECTDEF(CDSphere);
	
	mutable Vec3f curPos;			/// 探索開始番号
	Vec3f center;					///	球体の中心座標
	float radius;					/// 球体の半径

	CDSphere();
	//CDSphere(float r = 0.0f, Vec3f c = Vec3f()):radius(r), center(c){}
	//CDSphere(Vec3f c = Vec3f(), float r = 0.0f):center(c), radius(r){}
	CDSphere(const CDSphereDesc& desc);
	
	/// ShapeType
	virtual int SphereType(){ return CDSphereDesc::SPHERE; }
	///	サポートポイントを求める．
	virtual Vec3f Support(const Vec3f& p) const;
	///	切り口を求める．接触解析に使う．
	/// ただし、球体に関しては、切り口は求めない。接触解析時には最近傍の１点さえあればいい。
	/// 球体に関してFindCutRing()が呼び出された場合には、assertionが発生する。
	virtual void FindCutRing(CDCutRing& r, const Posed& toW);
	
	/// 球体の中心座標を取得
	virtual Vec3f GetCenter();
	/// 球体の半径を取得
	virtual float GetRadius();
};

	
}	//	namespace Spr

#endif	// CDSPHERE_H
