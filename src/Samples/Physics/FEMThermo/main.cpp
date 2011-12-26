/*
 *  Copyright (c) 2003-2006, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
/**
\page pageFEMThermo FEMによる温度シミュレーションのサンプル
Springhead2/src/Samples/FEMThermo

\section secQuitBoxStack 終了基準
- Escか'q'で強制終了。
	
*/
//
#include "../../SampleApp.h"
#include <Framework/FWFemMesh.h>
#include <Collision/CDConvexMesh.h>
#include <Physics/PHConstraintEngine.h>
#include <list>

#pragma hdrstop

using namespace PTM;

using namespace Spr;
using namespace std;


class MyApp : public SampleApp{
public:
	/// ページID
	enum {
		MENU_MAIN = MENU_SCENE,
	};
	/// アクションID
	enum {
		ID_BOX,
		ID_SWITCHRENDER,
		ID_TEMPUP,
		ID_TEMPDN
	};
	bool debugRender;

public:
	MyApp():debugRender(false){
		appName = "FEMThermo";

		AddAction(MENU_MAIN, ID_TEMPUP, "Tc UP 20degree");
		AddHotKey(MENU_MAIN, ID_TEMPUP, 'o');
		AddAction(MENU_MAIN, ID_TEMPDN, "Tc DOWN 20degree");
		AddHotKey(MENU_MAIN, ID_TEMPDN, 'O');
		AddAction(MENU_MAIN, ID_BOX, "drop box");
		AddHotKey(MENU_MAIN, ID_BOX, 'b');
		AddAction(MENU_MAIN, ID_SWITCHRENDER, "switch physics(debug)/graphics rendering");
		AddHotKey(MENU_MAIN, ID_SWITCHRENDER, 'd');
	}
	~MyApp(){}

	virtual void Init(int argc, char* argv[]){
		CreateSdk();
		GetSdk()->CreateScene();
		SetGRAdaptee(TypeGLUT);
		GRInit(argc, argv);
		//CreateSdk();

		FWWinDesc windowDesc;
		windowDesc.width = 1024;
		windowDesc.height = 768;
		windowDesc.title = appName;
		CreateWin(windowDesc);
		InitWindow();


		// 形状の作成
		CDBoxDesc bd;
		bd.boxsize = Vec3f(2,2,2);
		shapeBox = GetSdk()->GetPHSdk()->CreateShape(bd)->Cast();
		
		CDSphereDesc sd;
		sd.radius = 1;
		shapeSphere = GetSdk()->GetPHSdk()->CreateShape(sd)->Cast();
		
		CDCapsuleDesc cd;
		cd.radius = 1;
		cd.length = 1;
		shapeCapsule = GetSdk()->GetPHSdk()->CreateShape(cd)->Cast();
		
		CDRoundConeDesc rcd;
		rcd.length = 3;
		shapeRoundCone= GetSdk()->GetPHSdk()->CreateShape(rcd)->Cast();
		
		//	ファイルのロード
		UTRef<ImportIf> import = GetSdk()->GetFISdk()->CreateImport();
		GetSdk()->LoadScene("sceneTHTest.spr", import);			// ファイルのロード			// scene.spr:negiをロード, scene2.spr:デバッグ用の直方体, scene3.spr:穴あきcheeseをロード, sceneTHtest.spr:フライパンなどインポートのテスト
		numScenes = GetSdk()->NScene();
		if (numScenes) SwitchScene(GetSdk()->NScene()-1);

		//	FEMMeshを保存してみる
		FWFemMeshIf* fm = GetSdk()->GetScene()->FindObject("fwNegi")->Cast();
		if (fm && fm->GetPHMesh()){
			FIFileSprIf* spr = GetSdk()->GetFISdk()->CreateFileSpr();
			ObjectIfs objs;
			objs.Push(fm->GetPHMesh());
			spr->Save(objs, "femmesh.spr");
		}

		/// 描画設定
		if (fwScene){
			fwScene->SetWireMaterial(GRRenderIf::WHITE);
			fwScene->SetRenderMode(true, true);				///< ソリッド描画，ワイヤフレーム描画
			fwScene->EnableRenderAxis(false, true, true);		///< 座標軸
			fwScene->SetAxisStyle(FWSceneIf::AXIS_LINES);	///< 座標軸のスタイル
			fwScene->EnableRenderForce(false, true);			///< 力
			fwScene->EnableRenderContact(true);				///< 接触断面
			phScene = fwScene->GetPHScene();
		}

		//	セーブ用のステートの用意
		states = ObjectStatesIf::Create();		
		// タイマ
		timer = CreateTimer(UTTimerIf::FRAMEWORK);
		timer->SetInterval(10);

		//> 原点座標の格納	
		//FWObjectIf* pan_		=	DCAST(FWObjectIf, GetSdk()->GetScene()->FindObject("Pan"));
		//Affinef afPan_ = pan_->GetGRFrame()->GetWorldTransform();
		//Affinef afPan_ = pan_->GetGRFrame()->GetWorldTransform();
		///	原点座放＝＞ローカル座標系の(0,0,0)のはず
	}

	// 描画関数．描画要求が来たときに呼ばれる
	virtual void OnDraw(GRRenderIf* render) {
		if (debugRender){
			fwScene->DrawPHScene(render);
		}else{
			fwScene->Sync();
			fwScene->GetGRScene()->Render(render);
		}

		std::ostringstream sstr;
		sstr << "NObj = " << phScene->NSolids();
		render->DrawFont(Vec2f(-21, 23), sstr.str());
	}

	//アクション処理
	virtual void OnAction(int menu, int id){
		if(menu == MENU_MAIN){
			Vec3d v, w(0.0, 0.0, 0.2), p(0.5, 20.0, 0.0);
			Quaterniond q = Quaterniond::Rot(Rad(30.0), 'y');

			if(id == ID_BOX){
				Drop(SHAPE_BOX, GRRenderIf::RED, v, w, p, q);
				message = "box dropped.";
			}else if(id==ID_SWITCHRENDER){
				debugRender = !debugRender; 
			}else if(id=ID_TEMPUP){
				FWObjectIf** fwobject = fwScene->GetObjects();	
				//fwScene
			}else if(id=ID_TEMPDN){

			}

		}
		SampleApp::OnAction(menu, id);
	}
	//ステップ実行
	virtual void OnStep() {
		SampleApp::OnStep();
		PHSceneIf* phscene = GetSdk()->GetScene()->GetPHScene();
		//if(phscene)		cout << phscene << endl;
		SceneObjectIf* phSceneObject;
		PHFemMeshIf* phFemMesh;
		size_t Nobject = GetSdk()->GetPHSdk()->GetObjectIf()->NChildObject();
		//なんかしらアドレスが取れているのは分かった
		//for(int i=0; i< Nobject ;i++){
		//	ObjectIf* object = GetSdk()->GetPHSdk()->GetChildObject(i);
		//	cout << "Object" << i << " : " << object << endl;
		//}
		//NamedObjectIf* nobject = GetSdk()->FindObject("negi");
		//phFemMeshを作って、phFemMeshの処理をを実行させてみたい。
		//
		//熱シミュレーションステップ
		HeatConductionStep();
		
	}
	inline static double dist2D2(const Vec3d& a, const Vec3d& b){
		return Square(a.x-b.x) + Square(a.y-b.y);
	}
	struct CondVtx{
		int id;
		Vec3d pos;
		double area;
		double assign;
		CondVtx():id(-1), area(0), assign(0){}
		void AddCompanion(int id, double a){
			for(unsigned i=0; i<companions.size(); ++i){
				if (companions[i].id == id){
					companions[i].area += a;
					return;
				}
			}
			companions.push_back(Companion(id, area));
		}
		struct Companion{
			int id;
			double area;
			Companion(int i, double a):id(i), area(a){}
		};
		std::vector<Companion> companions;
		struct Less{
			int axis;
			Less(int a):axis(a){}
			bool operator() (const CondVtx& a, const CondVtx& b) const { return a.pos[axis] < b.pos[axis]; }
		};
	};
	struct CondVtxs:public std::vector<CondVtx>{
		PHFemMesh* pmesh;
		std::vector<int> vtx2Cond;
	};
	//	curの隣で、usedに含まれない頂点を列挙
	void FindNext(std::vector<int>& next, const std::vector<int>& cur, const std::vector<int>& used, CondVtxs& condVtxs){
		for(unsigned i=0; i<cur.size(); ++i){
			for(unsigned e=0; e < condVtxs.pmesh->vertices[condVtxs[cur[i]].id].edges.size(); ++e){
				PHFemMesh::Edge& edge = condVtxs.pmesh->edges[condVtxs.pmesh->vertices[condVtxs[cur[i]].id].edges[e]];
				int f = edge.vertices[0] == cur[i] ? edge.vertices[1] : edge.vertices[0]; 
				int cf = condVtxs.vtx2Cond[f];
				if (cf>=0){
					if (std::find(used.begin(), used.end(), cf) == used.end()
						&& std::find(cur.begin(), cur.end(), cf) == cur.end()
						&& std::find(next.begin(), next.end(), cf) == next.end())
						next.push_back(cf);
				}
			}
		}
	}
	//	condVtxsのなかから、fromsの隣の割り当てに空きがある頂点を列挙し、posに近い順にソートして返す。
	struct Dist2Less{
		Vec3d pos;
		CondVtxs& condVtxs;
		Dist2Less(Vec3d p, CondVtxs& c):pos(p),condVtxs(c){}
		bool operator () (int a, int b)const{
			return dist2D2(condVtxs[a].pos, pos) < dist2D2(condVtxs[b].pos, pos); 
		}
	};
	void FindNearests(std::vector<int>& nears, const Vec3d& pos, CondVtxs& condVtxs, const std::vector<int>& froms){
		FindNext(nears, froms, std::vector<int>(), condVtxs);
		for(unsigned i=0; i<nears.size(); ++i){
			if (condVtxs[nears[i]].area - condVtxs[nears[i]].assign <= 0){
				nears.erase(nears.begin()+i);
				i--;
			}
		}
		std::sort(nears.begin(), nears.end(), Dist2Less(pos, condVtxs));
	}
	static void CalcWeight3(double* weights, Vec3d pos, Vec3d p0, Vec3d p1, Vec3d p2){
		Vec3d a = p1-p0;
		Vec3d b = p2-p0;
		Vec2d v(pos.x-p0.x, pos.y-p0.y);
		Matrix2d mat(a.x, b.x, a.y, b.y);
		Vec2d w = mat.inv() * v;
		weights[0] = 1- w[0] - w[1];
		weights[1] = w[0];
		weights[2] = w[1];
	}
	//	condVtxs の中から、pos に近い3点を from を起点に探索して idsに返す。posを補間する重みをwegihtsに返す。
	void FindNearest3(int *ids, double* weights, const Vec3d& pos, CondVtxs& condVtxs, int from){
		int minId = from;
		int cid;
		do{
			cid = minId;
			double minDist2 = dist2D2(condVtxs[cid].pos, pos);
			int vid = condVtxs[cid].id;
			for(unsigned e=0; e < condVtxs.pmesh->vertices[vid].edges.size(); ++e){
				PHFemMesh::Edge& edge = condVtxs.pmesh->edges[condVtxs.pmesh->vertices[vid].edges[e]];
				int next = edge.vertices[0] == vid ? edge.vertices[1] : edge.vertices[0]; 
				if (condVtxs.vtx2Cond[next]>=0){
					double d2 = dist2D2(condVtxs[condVtxs.vtx2Cond[next]].pos, pos);
					if (d2 < minDist2){
						minDist2 = d2;
						minId = next;
					}
				}
			}
		}while(cid != minId);
		//	隣の頂点で近いものを２つ見つける
		double minDist2 = DBL_MAX;
		double min2Dist2 = DBL_MAX;
		minId = -1;
		int min2Id = -1;
		int vid = condVtxs[cid].id;
		for(unsigned e=0; e < condVtxs.pmesh->vertices[vid].edges.size(); ++e){
			PHFemMesh::Edge& edge = condVtxs.pmesh->edges[condVtxs.pmesh->vertices[vid].edges[e]];
			int next = edge.vertices[0] == vid ? edge.vertices[1] : edge.vertices[0];
			int cnext = condVtxs.vtx2Cond[next];
			if (cnext>=0){
				double d2 = dist2D2(condVtxs[cnext].pos, pos);
				if (d2 < minDist2){
					min2Dist2 = minDist2;
					minDist2 = d2;
					min2Id = minId;
					minId = cnext;
				}
			}
		}
		ids[0] = cid;
		ids[1] = minId;
		ids[2] = min2Id;
		CalcWeight3(weights, pos, condVtxs[ids[0]].pos, condVtxs[ids[1]].pos, condVtxs[ids[2]].pos);
	}
	void HeatConductionStep(){
#if 1
		FWFemMesh* fmesh[2];
		fmesh[0] = GetSdk()->GetScene()->FindObject("fwNegi")->Cast();
		fmesh[1] = GetSdk()->GetScene()->FindObject("fwPan")->Cast();
		if (!(fmesh[0] && fmesh[1])){
			DSTR << "対象のFWFemMeshが見つかりません" << std::endl;
			return;
		}
		PHSolid* solids[2];
		solids[0] = fmesh[0]->GetPHSolid()->Cast();
		solids[1] = fmesh[1]->GetPHSolid()->Cast();
		//	FEMMeshは接触判定がなくCDConvexMeshにある。CDConvexMeshの位置を取ってくる
		if (solids[0]->NShape() != 1 || solids[0]->NShape() !=1){
			DSTR << "複数形状には未対応" << std::endl;
			return;
		}
		PHScene* scene = solids[0]->GetScene()->Cast();
		bool bSwap;
		PHSolidPairForLCP* pair = scene->GetSolidPair(solids[0]->Cast(), solids[1]->Cast(), bSwap)->Cast();
		PHShapePairForLCP* sp = pair->GetShapePair(0,0)->Cast();
		if (sp->state == CDShapePair::NONE){
			//	未接触なので、GJKを呼ぶ
			sp->shapePoseW[0] = solids[0]->GetPose() * solids[0]->GetShapePose(0);
			sp->shapePoseW[1] = solids[1]->GetPose() * solids[1]->GetShapePose(0);
			if (bSwap) std::swap(sp->shapePoseW[0], sp->shapePoseW[1]);	
			Vec3d sep;
			double dist = FindClosestPoints(sp->shape[0], sp->shape[1], sp->shapePoseW[0], sp->shapePoseW[1], sep, sp->closestPoint[0], sp->closestPoint[1]);
			if (dist < 1e-10){
				//	かなり近いので、法線が怪しいので、警告をだしておく。
				DSTR << "２物体が非常に近いが、接触しはしていない微妙な状態"  << std::endl;
			}
			sp->depth = -dist;
			sp->normal = sp->shapePoseW[1]*sp->closestPoint[1] - sp->shapePoseW[0]*sp->closestPoint[0];
			sp->normal.unitize();
		}
		const double isoLen = 0.02;	//	これ以上離れると伝導しない距離
		if (sp->depth > -isoLen){
			CondVtxs condVtxs[2];
			condVtxs[0].pmesh = fmesh[bSwap? 1:0]->GetPHMesh()->Cast();
			condVtxs[1].pmesh = fmesh[bSwap? 0:1]->GetPHMesh()->Cast();
			//	距離が近い頂点を列挙。ついでに法線に垂直な平面上での座標を求めておく。
			Matrix3d coords;
			if (sp->normal.x < sp->normal.y) coords.Rot(sp->normal, Vec3d(1,0,0), 'z');
			else coords.Rot(sp->normal, Vec3d(0,1,0), 'z');
			Matrix3d coords_inv = coords.inv();			
			for(int i=0; i<2; ++i){
				Vec3d normal = sp->shapePoseW[i].Ori().Inv() * sp->normal * (i==0 ? 1 : -1);
				for(unsigned v=0; v < condVtxs[i].pmesh->surfaceVertices.size(); ++v){
					double vd = (sp->closestPoint[i] - condVtxs[i].pmesh->vertices[condVtxs[i].pmesh->surfaceVertices[v]].pos) * normal;
					vd -= sp->depth;
					if (vd < isoLen) {
						CondVtx c;
						c.id = condVtxs[i].pmesh->surfaceVertices[v];
						c.pos = coords_inv * (sp->shapePoseW[i] * condVtxs[i].pmesh->vertices[c.id].pos);
						condVtxs[i].push_back(c);
					}
				}
			}
			//	法線に垂直な平面上でソートして、BBoxが重ならない部分は捨てる。
			//	本来は、CDConvex::FindCutRing()を使って良い感じにやりたいところ。凸分解ができたらやりたい。
			Vec2d bboxMin, bboxMax;
			for(int axis = 0; axis<2; ++axis){
				for(int i=0; i<2; ++i){
					std::sort(condVtxs[i].begin(), condVtxs[i].end(), CondVtx::Less(axis));
				}
				bboxMin[axis] = condVtxs[0].size() ? condVtxs[0].front().pos[axis] : DBL_MAX;
				bboxMin[axis] = std::min(bboxMin[axis], condVtxs[1].size() ? condVtxs[1].front().pos[axis] : DBL_MAX);
				for(int i=0; i<2; ++i){
					CondVtx tmp;
					tmp.pos[axis] = bboxMin[axis];
					CondVtxs::iterator it = std::lower_bound(condVtxs[i].begin(), condVtxs[i].end(), tmp, CondVtx::Less(axis));
					condVtxs[i].erase(condVtxs[i].begin(), it);
				}
				bboxMax[axis] = condVtxs[0].size() ? condVtxs[0].back().pos[axis] : DBL_MIN;
				bboxMax[axis] = std::max(bboxMax[axis], condVtxs[1].size() ? condVtxs[1].back().pos[axis] : DBL_MIN);
				for(int i=0; i<2; ++i){
					CondVtx tmp;
					tmp.pos[axis] = bboxMax[axis];
					CondVtxs::iterator it = std::upper_bound(condVtxs[i].begin(), condVtxs[i].end(), tmp, CondVtx::Less(axis));
					condVtxs[i].erase(it, condVtxs[i].end());
				}
			}
			/*	熱伝達率 α [W/(m^2 K)] を用いると、境界上で q = α(T-Tc) (T:接点温度 Tc:周囲の流体等の温度)
				２物体の接触だと、T1-α1->Tc-α2->T2 となると考えられる。
				q = α1(T1-Tc) = α2(Tc-T2) より (α1+α2)Tc = α1T1 + α2T2
				Tc = (α1T1 + α2T2)/(α1+α2)
				q = α1(T1-(α1T1 + α2T2)/(α1+α2)) = α1T1 - α1(α1T1 + α2T2)/(α1+α2)
				  = (α1α2T1 - α1α2T2)/(α1+α2) = α'(T1-T2)  α' = α1α2/(α1+α2)	*/
			/*	qとQについての考察
				qは単位面積あたりなので、頂点間の熱の移動量Qに直すには、頂点が代表する面積を掛ける必要がある。
				本来は、三角形の重なりと形状関数から求めるべきもの。
				しかし、重なり具合は毎ステップ変わるので、この計算は大変。簡略化を考える。
				頂点は頂点を含む三角形に勢力を持つ。三角形の重なりより、頂点の距離の意味が大きい。
				距離が近いものを割り当てていくが、後で飛び地が出てはいけない。
				そこで、１点から初めて徐々に割り当て領域を大きくして行く。これでずれは起きても飛び地はでない。
				
				頂点の面積は、頂点を含む三角形達の面積の和の1/3。				
			*/
			//	bboxの中心近くの頂点を見つける
			double xCenter = 0.5*(bboxMin.x + bboxMax.x);
			int centerVtx[2];
			for(int i=0; i<2; ++i){
				CondVtx tmp;
				tmp.pos[1] = 0.5*(bboxMin.y+bboxMax.y);
				CondVtxs::iterator it = std::lower_bound(condVtxs[i].begin(), condVtxs[i].end(), tmp, CondVtx::Less(1));
				int cit = it - condVtxs[i].begin();
				double minDistX = DBL_MAX;
				for(int y=0; y<5; ++y){
					if (cit-y >= 0 && cit-y < (int)condVtxs[i].size()){
						double dist = std::abs(condVtxs[i][cit-y].pos.x - xCenter);
						if (dist < minDistX){
							centerVtx[i] = cit-y;
							minDistX = dist;
						}
					}
					if (cit+y < (int)condVtxs[i].size()){
						double dist = std::abs(condVtxs[i][cit+y].pos.x - xCenter);
						if (dist < minDistX){
							centerVtx[i] = cit+y;
							minDistX = dist;
						}
					}
				}
			}
			//	centerVtx[i]と一番近い頂点を探す
			//	verticesからcondVtxへの対応表を作る
			for(int i=0; i<2; ++i){
				condVtxs[i].vtx2Cond.resize(condVtxs[i].pmesh->vertices.size(), -1);
				for(unsigned j=0; j<condVtxs[i].size(); ++j){
					condVtxs[i].vtx2Cond[condVtxs[i][j].id] = j;
				}
			}
			int closestVtx[2];
			double minDist2[2];
			for(int i=0; i<2; ++i){
				Vec3d pos = condVtxs[1-i][centerVtx[1-i]].pos;
				int minId = centerVtx[i];
				int cid;
				do {
					cid = minId;
					minDist2[i] = dist2D2(condVtxs[i][cid].pos, pos);
					int vid = condVtxs[i][cid].id;
					for(unsigned e=0; e < condVtxs[i].pmesh->vertices[vid].edges.size(); ++e){
						PHFemMesh::Edge& edge = condVtxs[i].pmesh->edges[condVtxs[i].pmesh->vertices[vid].edges[e]];
						int next = edge.vertices[0] == vid ? edge.vertices[1] : edge.vertices[0]; 
						int cnext = condVtxs[i].vtx2Cond[next];
						if (cnext>=0){
							double d2 = dist2D2(condVtxs[i][cnext].pos, pos);
							if (d2 < minDist2[i]){
								minDist2[i] = d2;
								minId = cnext;
							}
						}
					}
				} while(minId != cid);
				closestVtx[i] = cid;
			}
			//	より近いほうを closestVtx[2] に入れる。
			if (minDist2[0] < minDist2[1]) closestVtx[1] = centerVtx[1];
			else closestVtx[0] = centerVtx[0];
			//	ここから対応を広げていく
			std::vector<int> cur[2];
			cur[0].push_back(closestVtx[0]);
			cur[1].push_back(closestVtx[1]);
			//	割り当て済みの頂点はこちらに移す
			std::vector<int> used[2];
			while(cur[0].size() || cur[1].size()){
				//	curについて熱流束の係数（熱伝達率）を求め、対応する頂点を見つける
				for(int i=0; i<2; ++i){
					for(unsigned j=0; j<cur[i].size();++j){
						std::vector<int> ids(3, -1);
						double weights[3];
						FindNearest3(&ids[0], weights, condVtxs[i][cur[i][j]].pos, condVtxs[1-i], cur[1-i][0]);
						double a = condVtxs[i][cur[i][j]].area;
						double rest = a;
						while(rest > 0){
							double nextWeights[3] = {weights[0], weights[1], weights[2]};
							for(int k=0; k<3; ++k){
								if (condVtxs[1-i][ids[k]].area - condVtxs[1-i][ids[k]].assign > a * weights[k]){
									double delta = a * weights[k];
									condVtxs[1-i][ids[k]].assign += delta;
									condVtxs[1-i][ids[k]].AddCompanion(cur[i][j], delta);
									condVtxs[i][cur[i][j]].AddCompanion(ids[k], delta);
									rest -= delta;
								}else{
									double delta = condVtxs[1-i][ids[k]].area - condVtxs[1-i][ids[k]].assign;
									condVtxs[1-i][ids[k]].assign = condVtxs[1-i][ids[k]].area;
									condVtxs[1-i][ids[k]].AddCompanion(cur[i][j], delta);
									condVtxs[i][cur[i][j]].AddCompanion(ids[k], delta);
									rest -= delta;
									//	いっぱいなので、残り２つに振り分ける
									nextWeights[k] = 0;
									int k1 = (k+1)%3; int k2 = (k+2)%3;
									double wsum = nextWeights[k1]+nextWeights[k2];
									if (wsum < 0) goto filled;
									double d1 = nextWeights[k1]/wsum * weights[k];
									double d2 = nextWeights[k2]/wsum * weights[k];
									nextWeights[k1] += d1; nextWeights[k2] += d2;
								}
							}
							a = rest;
						}
filled:;
						//	３点では吸収しきれなかったので、そばの頂点をどんどん探して割り当てていく
						while (rest > 0){
							std::vector<int> nears;
							FindNearests(nears, condVtxs[i][cur[i][j]].pos, condVtxs[1-i], ids);	//	隣の空いてる頂点を近い順に返す。
							for(unsigned k=0; rest > 0 && k<nears.size(); ++k){
								if (condVtxs[1-i][nears[k]].area - condVtxs[1-i][nears[k]].assign > rest){
									condVtxs[1-i][nears[k]].assign += rest;
									condVtxs[1-i][nears[k]].AddCompanion(cur[i][j], rest);
									condVtxs[i][cur[i][j]].AddCompanion(nears[k], rest);
									rest = 0;
								}else{
									double delta = condVtxs[1-i][nears[k]].area - condVtxs[1-i][nears[k]].assign;
									condVtxs[1-i][nears[k]].AddCompanion(cur[i][j], delta);
									condVtxs[i][cur[i][j]].AddCompanion(nears[k], delta);
									rest -= delta;
									condVtxs[1-i][nears[k]].assign = condVtxs[1-i][nears[k]].area;
								}
							}
							if (rest > 0){
								ids.insert(ids.end(), nears.begin(), nears.end());
								nears.clear();
							}
						}
					}
				}
				for(int i=0; i<2; ++i){
					std::vector<int> next;
					FindNext(next, cur[i], used[i], condVtxs[i]);
					used[i].insert(used[i].end(), cur[i].begin(), cur[i].end());
					cur[i] = next;
				}
			}
		}
		
		

#else
		//	フライパンを取ってくる
		FWObjectIf* pan		=	DCAST(FWObjectIf, GetSdk()->GetScene()->FindObject("fwPan"));
		//	食材を取ってくる
		FWFemMeshIf* tmesh	= GetSdk()->GetScene()->FindObject("fwNegi")->Cast();
//		FWFemMeshIf* tmesh	= GetSdk()->GetScene()->FindObject("fwCheese")->Cast();
		//	ワールド座標に変換する

		//DSTR <<"pan: " << pan << std::endl;
		//DSTR <<"tmesh: " << tmesh << std::endl;

		//curScene = id;
		//// id番目のシーンを選択
		//fwScene = GetSdk()->GetScene(id);
		//phScene = fwScene->GetPHScene();
		//GetCurrentWin()->SetScene(fwScene);
		//editor.SetObject(phScene);

		//tmesh->GetScene();
		
		if(pan && tmesh){
			//	フライパンの世界座標を入手
			Affinef afPan = pan->GetGRFrame()->GetWorldTransform();
			//	加熱物体の世界座標を入手
			Affinef afMesh = tmesh->GetGRFrame()->GetWorldTransform();
			//	afPanから見た、afMeshの座標系への変換行列を入手
			Affinef afMeshToPan = afPan.inv() * afMesh;
			//DSTR << afPan <<std::endl;
			//DSTR << afMesh <<std::endl;
			//DSTR << afMeshToPan <<std::endl;

			PHFemMeshIf* phm = tmesh->GetPHMesh();
//			int nOfSurfaceVtx = phm->NSurfaceVertices();

			///	フライパンにとっての原点からの距離に応じて、加熱する
			////	最外殻の節点のフライパンからの座標
			double tempTc =10.0;			//negitest 10.0		///	cheese 100.0

			
			//	pfemの定義
			//	PHFemMEshThermの節点を取ってきて、加熱する
			PHFemMeshThermoIf* pfem = NULL;

			///	メッシュのPosOnPan座標を入れて、座標が小さい順に並べる
			std::list<double> posOnPany;

			for(unsigned int i=0; i<tmesh->NChildObject() && !pfem; ++i){
				pfem = tmesh->GetChildObject(i)->Cast();
				//	pfemが取れていることを確認
				if(pfem){
					///	加熱温度の上がり方を制限
					if(tempTc <= 250.0){ tempTc += tempTc * pfem->GetStepCount() * 0.02;}		//negi test 0.02 // cheese 0.01
					else{
						tempTc = 250.0;
					}

					//DSTR << pfem->NSurfaceVertices() <<std::endl;
					for(unsigned j =0; j < pfem->NSurfaceVertices(); j++){
						Vec3d pfemPose = pfem->GetPose(pfem->GetSurfaceVertex(j));
						Vec3d posOnPan = afMeshToPan * pfemPose;
						//DSTR << j <<"th pfemPose: " << pfemPose << std::endl;
						//Vec3d pfemPose_ = pfem->GetSufVtxPose(pfem->GetSurfaceVertex(j));
						//DSTR << j <<"th pfemPose_: " << pfemPose_ << std::endl;

						//> メッシュの表面の節点vertex座標があるフライパン座標系のとある範囲にある時、熱伝達境界条件で加熱する
						//if(posOnPan.y >= -0.03 && posOnPan.y <= -0.02 ){				///	pfemPose.y >= -0.0076 && pfemPose.y <= -0.0074 /// cube_test.x用	//	 -0.01 <= pfemPose.y <= 0.0
						if(posOnPan.y >= -0.03 && posOnPan.y <= -0.020 ){		///	cheese
//							pfem->SetVertexTc(j,tempTc);
							pfem->SetVertexTc(j,tempTc,25.0);
							//pfem->SetVertexTemp(j,25.0);
							//DSTR << "pfem->GetStepCount(): " << pfem->GetStepCount() << std::endl;
							//DSTR << "afPan: " << afPan <<std::endl;
							//DSTR << "afMesh: " << afMesh <<std::endl;
							posOnPany.push_back(posOnPan.y);
							//DSTR << "j: "<<j<< ", posOnPan.y: " << posOnPan.y << std::endl;
							//UsingHeatTransferBoundaryCondition		を呼び出す
							//DSTR << j << "th vertex.Tcに" << tempTc << "を設定" <<std::endl;
							//Tcの更新？
						}
					}
				}
			}
			posOnPany.sort();
	//		DSTR << "posOnPany.front(): " <<posOnPany.front() << std::endl;
			//for(unsigned i=0;i<50;i++){
			//	DSTR << posOnPany[i] << std::endl;
			//}

			int kattton =0;
		
			//for(unsigned i=-0; i < nOfSurfaceVtx; i++){
			//	DSTR <<i <<"th: phm->GetPose(i): " << phm->GetPose(i) << std::endl;
			//	if(posOnPan.y >= -1.0 && posOnPan.y <= 0.3){
			//		DSTR << i << "th posOnPan.y: " << posOnPan.y << std::endl; 
			//	}
			//}
			
		}
		int nSolidsPair	=	GetSdk()->GetPHSdk()->GetScene(0)->NSolidPairs();
		int nSolid		=	GetSdk()->GetPHSdk()->GetScene(0)->NSolids();

//		PHSolidPairForLCPIf* solidPair = GetSdk()->GetPHSdk()->GetScene(0)->GetSolidPair(

		//char grName = "";
		//char phName = "";

		//GRFrameIf* findFrame = DCAST(GRFrameIf, GetSdk()->GetScene()->GetGRScene()->FindObject(grName));
		//PHSolidIf* findSolid = DCAST(PHSolidIf, GetSdk()->GetScene()->GetPHScene()->FindObject(phName));
		//PHFemMeshThermoオブジェクトを作り、main関数で使えるようにする関数
			//CreatePHFemMeshThermo();
		//総節点数、総要素数、節点座標、要素の構成などを登録
			//PHFemMeshThermoのオブジェクトを作る際に、ディスクリプタに値を設定して作る
		
		//（形状が変わったら、マトリクスやベクトルを作り直す）
		//温度変化・最新の時間の{T}縦ベクトルに記載されている節点温度を基に化学変化シミュレーションを行う
			//SetChemicalSimulation();
			//化学変化シミュレーションに必要な温度などのパラメータを渡す
		//温度変化や化学シミュレーションの結果はグラフィクス表示を行う
			
#endif
	}
	//>	対流・放射伝熱の距離に比例・反比例して加える熱量が変わる関数の実装
	void hogehoge(){
		///	底の位置
		double topOfPan=0.0;
		///	食材の位置
		double altitudeOfFood=0.0;

		///	対流、輻射による比例？係数
		double a_0 = 0;
			///	一変数近似が良いか、多変数近似が良いか
				/// 比例係数、a * x^3 +  b * x^2  + c * x^1
		///	その食材のx,y位置の射影のフライパン表面の温度
		double tempP=0.0;
				///	食べ物と同じxy位置(世界座標)のフライパンの温度を取ってくる？
		 a_0 * (topOfPan - altitudeOfFood) * tempP; ///	単位の次元[K/m]

	}
	void CreatePHFemMeshThermo(){
		PHFemMeshThermoIf* phmesht;
		PHFemMeshThermoDesc desc;
	}
	void SetInductionHeating(double radius,double width,double heatFlux){						//>	磁束→熱流束　heatFlux
		double inRadius = radius - width /2;		//> 内半径
		double outRadius = radius + width /2; 		//>	外半径

		//>　内半径と外半径の間をtempで加熱する




	}
	void SetInductionHeating2(double radius,double width,double temp,double ){			//>	半径の内側又は外側に行くにつれて、加熱流束が減衰する
		double inRadius = radius - width /2;		//> 内半径
		double outRadius = radius + width /2; 		//>	外半径

		//>　内半径と外半径の間をtempで加熱する




	}
	void CopyTheFood(){

	}

//	bool CopyTheFood(const char* phName, const char* grName){
//	if(GetSdk()->GetScene()->GetPHScene() && GetSdk()->GetScene()->GetGRScene()){
//		std::cout << phName << " : " << GetSdk()->GetScene()->GetPHScene()->FindObject(phName) << std::endl;
//		if(GetSdk()->GetScene()->GetPHScene()->FindObject(phName)){
//			PHSolidIf* findSolid = DCAST(PHSolidIf, GetSdk()->GetScene()->GetPHScene()->FindObject(phName));
//			PHSolidDesc sDesc;
//			if (findSolid) {
//				findSolid->GetDesc(&sDesc);
//				std::cout << "dupl : solid found : " << findSolid->GetName() << std::endl;
//			} else {
//				std::cout << "dupl : solid NOT found : " << phName << std::endl;
//			}
//
//			GRFrameIf* findFrame = DCAST(GRFrameIf, GetSdk()->GetScene()->GetGRScene()->FindObject(grName));
//			GRFrameDesc fDesc;
//			
//			cout << "GRFrame Name : " << findFrame->GetName() 
//				 << " GRMesh Name  : " << DCAST(NamedObjectIf, findFrame->GetChildObject(0))->GetName() << std::endl;
//			GRMesh* origMesh = findFrame->GetChildObject(0)->Cast();
//			GRMeshDesc& md = *(GRMeshDesc*)origMesh->GetDescAddress();		//GRMeshDesc型の値(ポインタの指す値)mdに、上でアドレスを格納したポインタ meshのGetDescAddressという関数の返り値を格納する。
//			GRMesh* cm = GetSdk()->GetScene()->GetGRScene()->CreateObject(md)->Cast();	//cm:create mesh? mdに格納されたポインタのさすオブジェクトをcreate(生成)して、GRMesh*型にキャストする。
//			cm->tex3d = true;												//3次元テクスチャを使用可にする。
//			cm->material = origMesh->material;									//cmのmaterialに　meshのmaterialListの値を格納する。
//			cm->materialList = origMesh->materialList;							//cmのmaterialListにmeshのmaterialListを格納する。
//			cm->faceNormals  = origMesh->faceNormals;							//cmのmeshの面の法線ベクトルに、meshのそれを格納する。
//			cm->originalFaceIds = origMesh->originalFaceIds;					//meshのoriginalFaceIds(３次元メッシュの元の頂点番号)をcmのそれに格納する。
//			cm->originalFaces  = origMesh->originalFaces;						//meshのoriginalFacesをcmのそれに格納する。
//
//			// FWObjectIf* copied = GetSdk()->GetScene()->CreateObject(sDesc, fDesc);
//			FWObjectIf* copied = GetSdk()->GetScene()->CreateFWObject();
//			copied->SetPHSolid(GetSdk()->GetScene()->GetPHScene()->CreateSolid(sDesc));
//			copied->SetGRFrame(GetSdk()->GetScene()->GetGRScene()->CreateVisual(fDesc.GetIfInfo(), fDesc)->Cast());
//
//			for (unsigned int i=0; i<findSolid->NChildObject(); ++i) {
//				copied->GetPHSolid()->AddChildObject(findSolid->GetChildObject(i));
//			}
//			copied->GetGRFrame()->AddChildObject(cm->Cast());
//
//			std::stringstream sout;
//			sout << "soCopiedFood" << copiedNum++;
//			copied->GetPHSolid()->SetName(sout.str().c_str());
//			Posed p = pan->GetPHSolid()->GetPose();					//フライパンの上に入れる
//			//食材は常にフライパンに入れる
//			copied->GetPHSolid()->SetFramePosition(Vec3d((double)rand() / (double)RAND_MAX * 0.03 * cos((double)rand()) + p.px,
//														(double)rand() / (double)RAND_MAX * 0.03 + 0.07 + p.py,
//														(double)rand() / (double)RAND_MAX * 0.03 * sin((double)rand()) +p.pz)
//														);
//			//copied->GetPHSolid()->SetAngularVelocity(Vec3d( (double)rand() / (double)RAND_MAX * 2.9,  (double)rand() / (double)RAND_MAX * 0.7,	(double)rand() / (double)RAND_MAX * 1.7 ));
//			copied->GetPHSolid()->SetAngularVelocity(Vec3d(0.0,0.0,0.0));
//			copied->GetPHSolid()->SetOrientation(Quaternionf::Rot(Rad((double)rand() / (double)RAND_MAX) * cos((double)rand()) + Rad(15) , 'x') 
//															  * Quaternionf::Rot(Rad((double)rand()), 'y')
//															  * Quaternionf::Rot(Rad((double)rand() / (double)RAND_MAX) * sin((double)rand()), 'z'));
//			copied->GetPHSolid()->SetVelocity(Vec3d(0.0,0.0,0.0));
//			sout.str("");
//			sout << "frCopiedFood" << copiedNum;
//			copied->GetGRFrame()->SetName(sout.str().c_str());
//			
//			foodObjs.push_back(copied);
//			GRMesh* mesh = foodObjs.back()->GetGRFrame()->GetChildObject(0)->Cast();
//			mesh->Render(GetCurrentWin()->render);
//
//			if(mesh && mesh->GetNVertex()){
//			
//				GRThermoMesh tmesh;
//				tmesh.Init(mesh, foodObjs.back());
//
//				std::stringstream foodName;
//				foodName << phName;
//				//copythings = 1;					//要検討
//				if (foodName.str().find("negi")!= string::npos || foodName.str().find("Negi")!= string::npos){
//					tmesh.copyInit =true;
//					tmesh.fwobj = copied;			//foodObjsの最後の要素がFWObjectIfなら、それを格納
//					tmeshnegis.push_back(tmesh);
//					MyApp::InitNegi();
//				}
//				else if (foodName.str().find("carrot")!= string::npos || foodName.str().find("Carrot")!= string::npos){
//					tmesh.copyInit =true;
//					tmesh.fwobj = copied;			//foodObjsの最後の要素がFWObjectIfなら、それを格納
//					tmeshcarrots.push_back(tmesh);
//					MyApp::InitCarrot();
//				}
//				else if (foodName.str().find("shrimp")!= string::npos || foodName.str().find("Shrimp")!= string::npos){
//					tmesh.copyInit =true;
//					tmesh.fwobj = copied;			//foodObjsの最後の要素がFWObjectIfなら、それを格納
//					tmeshshrimps.push_back(tmesh);
//					MyApp::InitShrimp();
//				}
//				else if (foodName.str().find("asteakc")!= string::npos || foodName.str().find("Asteakc")!= string::npos){
//					tmesh.copyInit =true;
//					tmesh.fwobj = copied;			//foodObjsの最後の要素がFWObjectIfなら、それを格納
//					tmesh.swtovtx = true;			//断面と本体の同じ位置座標を持つ頂点のペアを作る処理のフラグ
//					tmeshasteaks_c.push_back(tmesh);
//					MyApp::InitAsteak_c();
//				}
//				else if (foodName.str().find("asteak")!= string::npos || foodName.str().find("Asteak")!= string::npos){
//					tmesh.copyInit =true;
//					tmesh.fwobj = copied;			//foodObjsの最後の要素がFWObjectIfなら、それを格納
//					tmesh.swtovtx = true;
//					tmeshasteaks.push_back(tmesh);
//					MyApp::InitAsteak();
//				}
//				else if (foodName.str().find("nsteakc")!= string::npos || foodName.str().find("Nsteakc")!= string::npos){
//					tmesh.copyInit =true;
//					tmesh.fwobj = copied;			//foodObjsの最後の要素がFWObjectIfなら、それを格納
//					tmesh.swtovtx = true;
//					tmeshnsteaks_c.push_back(tmesh);
//					MyApp::InitNsteak_c();
//				}
//				else if (foodName.str().find("nsteak")!= string::npos || foodName.str().find("Nsteak")!= string::npos){
//					tmesh.copyInit =true;
//					tmesh.fwobj = copied;			//foodObjsの最後の要素がFWObjectIfなら、それを格納
//					tmesh.swtovtx = true;
//					tmeshnsteaks.push_back(tmesh);
//					MyApp::InitNsteak();
//				}
//				
//				tmeshes.push_back(tmesh);
//				DSTR		<< "Copy number of the food : " << copiedNum << std::endl;
//				std::cout	<< "Copy number of the food : " << copiedNum << std::endl;
//			}
//		}
//	}
//	//コピーが失敗したときのエラー表示
//	//DSTR		<< "Failed to copy." << std::endl;
//	//std::cout	<< "Failed to copy." << std::endl;
//	return false;
//}

};


////////////////////////////////////////////////////////////////////////////////////////////////////////////

//FWMyApptest app;
MyApp app;

/**
 brief		メイン関数
 param		<in/--> argc　　コマンドライン入力の個数
 param		<in/--> argv　　コマンドライン入力
 return		0 (正常終了)
 */
int main(int argc, char *argv[]) {
	app.Init(argc, argv);
	app.StartMainLoop();
	return 0;
}

