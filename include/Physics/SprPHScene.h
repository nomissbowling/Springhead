#ifndef SPR_PHSCENEIF_H
#define SPR_PHSCENEIF_H

namespace Spr{;

struct PHSolidIf;
struct PHSolidDesc;
///	シーン
struct PHSceneIf : public ObjectIf{
	///	Solid作成
	virtual PHSolidIf* CreateSolid()=0;
	virtual PHSolidIf* CreateSolid(const PHSolidDesc& desc)=0;

	/// 積分ステップを返す
	virtual double GetTimeStep()const=0;
	/// 積分ステップを設定する
	virtual void SetTimeStep(double dt)=0;
	/// カウント数を返す
	virtual unsigned GetCount()const=0;
	/// カウント数を設定する
	virtual void SetCount(unsigned c)=0;
	///	シーンの時刻を進める ClearForce(); GenerateForce(); Integrate(); と同じ
	virtual void Step()=0;
	///	シーンの時刻を進める（力のクリア）
	virtual void ClearForce()=0;
	///	シーンの時刻を進める（力の生成）
	virtual void GenerateForce()=0;
	///	シーンの時刻を進める（力と速度を積分して，速度と位置を更新）
	virtual void Integrate()=0;
	///	シーンを空にする．
	virtual void Clear()=0;
};

}	//	namespace Spr
#endif
