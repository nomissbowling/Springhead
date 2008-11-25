#ifndef PHYSICS_PROCESS_H
#define PHYSICS_PROCESS_H

#include "Synchronization.h"
#include <Framework/SprFWAppGLUT.h>

using namespace Spr;
using namespace PTM;
using namespace std;

struct Edge{
	float edge;				///<	端の位置(グローバル系)
	int	index;				///<	元の solidの位置
	bool bMin;				///<	初端ならtrue
	bool operator < (const Edge& s) const { return edge < s.edge; }
};
typedef vector<Edge> Edges;

class PhysicsProcess : public FWAppGLUT, public UTRefCount{
public:
	//　プロセス間の同期に使う変数
	volatile bool bsync;
	bool calcPhys;
	volatile int hapticcount;
	double dt;
	Vec3d gravity;
	double nIter;
	bool bGravity;
	// 剛体に使う変数
	PHSolidIf* soPointer;
	PHSolid phpointer;

	// 近傍物体探索に使う変数
	double range;
	vector<ExpandedObject> expandedObjects; 

	// 予測シミュレーションに使う変数
	UTRef<ObjectStatesIf> states, states2;
	//デバック表示に使う変数
	bool bDebug;
	bool bStep;
	bool bOneStep;

	PhysicsProcess();	
	void Init(int argc, char* argv[]);				
	void InitCameraView();										
	void DesignObject();
	void Idle();
	void Start();
	void PhysicsStep();
	void Display();		
	void UpdateHapticPointer();
	void ExpandSolidInfo();
	void FindNearestObject();
	void PredictSimulation();
	void DisplayContactPlane();
	void DisplayLineToNearestPoint();
	void Keyboard(unsigned char key);
}; 
extern PhysicsProcess pprocess;

#endif