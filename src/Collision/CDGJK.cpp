﻿/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
/*	このファイルのライセンスについての注意
	このソースは，一部 SOLID (Software Library for Interference Detection) 2.0
	http://www.win.tue.nl/~gino/solid の src/Convex.cpp を参考に書いています．
	論文とソースを公開されている Bergen さんに感謝します．

	長谷川は，このソースがSolid2.0の一部の派生物ではないと信じています．
	しかし，似ている箇所があります．もし派生物だと認定された場合，
	ライセンスがLGPLとなります．ご注意ください．
*/
#include <Collision/CDBox.h>
#include <Collision/CDSphere.h>
#include <Collision/CDCapsule.h>
#include <Collision/CDRoundCone.h>
#include <Collision/CDConvexMesh.h>
#include <Collision/CDDetectorImp.h>
#include <Physics/SprPHSdk.h>
#include <fstream>

#include <string.h>  // strcmp

bool bGJKDebug;


namespace Spr{;

void SaveMaterial(std::ostream& file, PHMaterial& m){
	file << m.e << " ";
	file << m.mu << " " << m.mu0 << std::endl;
}
void LoadMaterial(std::istream& file, PHMaterial& m){
	file >> m.e;
	file >> m.mu >> m.mu0;
}
void SaveShape(std::ostream& file, CDShape* a){
	CDConvexMesh* mesh = a->Cast();
	if (mesh){
		file << "mesh" << std::endl;
		CDConvexMeshDesc desc;
		mesh->GetDesc(&desc);
		SaveMaterial(file, desc.material);
		file << desc.vertices.size() << " ";
		for(unsigned i=0; i<desc.vertices.size(); ++i){
			file << desc.vertices[i] << std::endl;
		}
	}
	CDBox* box = a->Cast();
	if (box){
		file << "box" << std::endl;
		CDBoxDesc desc;
		box->GetDesc(&desc);
		SaveMaterial(file, desc.material);
		file << desc.boxsize << std::endl;
	}		
	CDCapsule* cap = a->Cast();
	if (cap){
		file << "cap" << std::endl;
		CDCapsuleDesc desc;
		cap->GetDesc(&desc);
		SaveMaterial(file, desc.material);
		file << desc.radius << std::endl;
		file << desc.length << std::endl;
	}		
	CDSphere* sph = a->Cast();
	if (sph){
		file << "sph" << std::endl;
		CDSphereDesc desc;
		sph->GetDesc(&desc);
		SaveMaterial(file, desc.material);
		file << desc.radius << std::endl;
	}
	CDRoundCone* rc = a->Cast();
	if(rc){
		file << "rc" << std::endl;
		CDRoundConeDesc desc;
		rc->GetDesc(&desc);
		SaveMaterial(file, desc.material);
		file << desc.radius << std::endl;
		file << desc.length << std::endl;
	}
}
CDConvex* LoadShape(std::istream& file, PHSdkIf* sdk){
	CDConvex* rv = NULL;
	char type[100];
	file >> type;
	if( strcmp(type, "mesh") == 0 ){
		CDConvexMeshDesc desc;
		LoadMaterial(file, desc.material);			
		int size;
		file >> size;
		desc.vertices.resize(size);
		for(int i=0; i<size; ++i){
			file >> desc.vertices[i];
		}
		rv = sdk->CreateShape(CDConvexMeshIf::GetIfInfoStatic(), desc)->Cast();
	}
	if( strcmp(type, "box") == 0){
		CDBoxDesc desc;
		LoadMaterial(file, desc.material);			
		file >> desc.boxsize;
		rv = sdk->CreateShape(CDBoxIf::GetIfInfoStatic(), desc)->Cast();
	}
	if( strcmp(type, "cap") == 0){
		CDCapsuleDesc desc;
		LoadMaterial(file, desc.material);			
		file >> desc.radius;
		file >> desc.length;
		rv = sdk->CreateShape(CDCapsuleIf::GetIfInfoStatic(), desc)->Cast();
	}
	if( strcmp(type, "sph") == 0){
		CDSphereDesc desc;
		LoadMaterial(file, desc.material);			
		file >> desc.radius;
		rv = sdk->CreateShape(CDSphereIf::GetIfInfoStatic(), desc)->Cast();
	}
	if( strcmp(type, "rc") == 0){
		CDRoundConeDesc desc;
		LoadMaterial(file, desc.material);
		file >> desc.radius;
		file >> desc.length;
		rv = sdk->CreateShape(CDRoundConeIf::GetIfInfoStatic(), desc)->Cast();
	}
	return rv;
}

void FASTCALL ContFindCommonPointSaveParam(const CDConvex* a, const CDConvex* b,
	const Posed& a2w, const Posed& b2w, const Vec3d& dir, double start, double end,
	Vec3d& normal, Vec3d& pa, Vec3d& pb, double& dist){
	std::ofstream file("ContFindCommonPointSaveParam.txt");
	SaveShape(file, (CDConvex*)a);
	SaveShape(file, (CDConvex*)b);
	file << a2w << std::endl;
	file << b2w << std::endl;
	file << dir << std::endl;
	file << start << std::endl;
	file << end << std::endl;
	file << normal << std::endl;
	file << pa << std::endl;
	file << pb << std::endl;
	file << dist << std::endl;
}
void ContFindCommonPointCall(std::istream& file, PHSdkIf* sdk){
	bGJKDebug = true;
	const CDConvex* a;
	const CDConvex* b;
	Posed a2w, b2w;
	Vec3d dir, normal, pa, pb;
	double dist, start, end;
	a = LoadShape(file, sdk);
	b = LoadShape(file, sdk);
	file >> a2w;
	file >> b2w;
	file >> dir;
	file >> start;
	file >> end;
	file >> normal;
	file >> pa;
	file >> pb;
	file >> dist;

	Vec3f capdir = b2w.Ori() * Vec3f(0,0,1);
	DSTR << "dir of capsule = " << capdir << std::endl;
	DSTR << "center of capsule = " << b2w.Pos()  << std::endl;
	ContFindCommonPoint(a, b, a2w, b2w, dir, start, end, normal, pa, pb, dist);
	DSTR << "normal = " << normal << std::endl;
	DSTR << "pa and pb in W = "<< std::endl;
	DSTR << a2w * pa << std::endl;
	DSTR << b2w * pb << std::endl;
}


const double sqEpsilon = 1e-3;
const double epsilon   = 1e-6;  // sが2e-6になることもあった．まだだめかもしれない．（mitake）
const double epsilon2  = epsilon*epsilon;

namespace GJK{
	Vec3f p[4];			///<	Aのサポートポイント(ローカル系)
	Vec3f q[4];			///<	Bのサポートポイント(ローカル系)
	int p_id[4];		///<	Aのサポートポイントの頂点番号（球など頂点がない場合は -1）
	int q_id[4];		///<	Bのサポートポイントの頂点番号（球など頂点がない場合は -1）
	Vec3d w[4];			///<	ContFindCommonPointで使用する速度向き座標系でのサポートポイント
	Vec3d v[4];			///<	ContFindCommonPointで使用するv
	int nSupport;		///<	何点のsupportから最近傍点を計算したかを表す。(ContFindCommonPoint専用)
	Vec3d dec;			///<	内分の割合
	int ids[4];			///<	頂点ID対応表

	Vec3d p_q[4];		///<	ミンコスキー和上でのサポートポイント(ワールド系) ContでないFindXXで使用
}	//	namespace GJK
using namespace GJK;

const char vacants[] = {
	0, 1, 0, 2, 0, 1, 0, 3,
	0, 1, 0, 2, 0, 1, 0, 4,
};
inline char FindVacantId(char a, char b){
	char bits = (1<<a) | (1<<b);
	return vacants[(int)bits];
}
inline char FindVacantId(char a, char b, char c){
	char bits = (1<<a) | (1<<b) | (1<<c);
	return vacants[(int)bits];
}

inline Vec3d TriDecompose(Vec2d p1, Vec2d p2, Vec2d p3){
	double det = p1.x*(p2.y-p3.y) + p2.x*(p3.y-p1.y) + p3.x*(p1.y-p2.y);
	Vec3d r;
	if (abs(det) > epsilon){
		double detInv = 1/det;
		r.x = detInv * (p2.x*p3.y-p3.x*p2.y);
		r.y = detInv * (p3.x*p1.y-p1.x*p3.y);
		r.z = 1 - r.x - r.y;
	}else{
		double ip[3] = {p1*p2, p2*p3, p3*p1};
		int j=-1;
		for(int i=0; i<3; ++i){
			if (ip[i]<=0 && ip[(i+1)%3]<=0){
				if (ip[i] > ip[(i+1)%3]) j = i;
				else j = (i+1)%3;
				break;
			}
		}
		if(j < 0){
			if (ip[0] < ip[1] && ip[0] < ip[2]) j=0;
			else if (ip[1] < ip[2]) j=1;
			else j = 2;
		}
		if (j==0){
			double a = p1*p1; double b = -p1*p2;
			if (a+b == 0) a = b = 1;
			r.x = b/(a+b);
			r.y = a/(a+b);
		}else if (j==1){
			double a = p2*p2; double b = -p2*p3;
			if (a+b == 0) a = b = 1;
			r.y = b/(a+b);
			r.z = a/(a+b);
		}else{	//	j==2
			double a = p3*p3; double b = -p3*p1;
			if (a+b == 0) a = b = 1;
			r.z = b/(a+b);
			r.x = a/(a+b);
		}
	}
	return r;
}

//	以下、ContFindCommonPoint()関数専用のマクロ
#define XY()	sub_vector( PTM::TSubVectorDim<0,2>() )
#define CalcSupport(n)														\
	p_id[n] = a->Support(p[n], a2z.Ori().Conjugated() * (v[n]));			\
	q_id[n] = b->Support(q[n], b2z.Ori().Conjugated() * -(v[n]));			\
	w[n] = b2z * ((Vec3d)q[n]) - a2z * ((Vec3d)p[n]);

int FASTCALL ContFindCommonPoint(const CDConvex* a, const CDConvex* b,
	const Posed& a2w, const Posed& b2w, const Vec3d& dir, double start, double end,
	Vec3d& normal, Vec3d& pa, Vec3d& pb, double& dist){
	nSupport = 0;
	//	range が+Zになるような座標系を求める．
	Quaterniond w2z;
	Vec3d u = -dir;	//	u: 物体ではなく原点の速度の向きなので - がつく．
	if (u.Z() < -1+epsilon){
		w2z = Quaterniond::Rot(Rad(180), 'x');
	}else if (u.Z() < 1-epsilon){
		Matrix3d matW2z = Matrix3d::Rot(u, Vec3f(0,0,1), 'z');
		w2z.FromMatrix(matW2z);
		w2z = w2z.Inv();
	}
	Posed a2z;
	a2z.Ori() = w2z * a2w.Ori();
	a2z.Pos() = w2z * a2w.Pos();
	Posed b2z;
	b2z.Ori() = w2z * b2w.Ori();
	b2z.Pos() = w2z * b2w.Pos();

	
	//	GJKと似た方法で，交点を求める
	//	まず、2次元で見たときに、原点が含まれるような三角形または線分を作る

	//	w0を求める
	v[0] = Vec3d(0,0,1);
	CalcSupport(0);
	if (w[0].Z() > end) 
		return -1;	//	範囲内では接触しないが，endより先で接触するかもしれない．

	if (w[0].Z() < start){	//	反対側のsupportを求めてみて，範囲外か確認
		v[3] = Vec3d(0,0,-1);
		CalcSupport(3);
		if (w[3].Z() < start){
			//	範囲内では接触しないが，後ろに延長すると接触するかもしれない．
			return -2;
		}
	}

	//	w1を求める
	v[1] = Vec3d(w[0].X(), w[0].Y(), 0);
	if (v[1].XY().square() < epsilon2){		//	w0=衝突点の場合
		normal = u.unit();
		pa = p[0]; pb = q[0];
		dist = w[0].Z();
		nSupport = 1;
		dec[0] = 1; dec[1] = 0; dec[2] = 0;
		if (dist > end) return -1;
		if (dist < start) return -2;
		return 1;
	}
	CalcSupport(1);
	if (w[1].XY() * v[1].XY() > 0) 
		return 0;	//	w[1]の外側にOがあるので触ってない
	
	
	//	w[0]-w[1]-w[0] を三角形と考えてスタートして，oが三角形の内部に入るまで繰り返し
	ids[0] = 1;	//	新しい頂点
	ids[1] = 0;	//	もとの線分
	ids[2] = 0;	//	もとの線分
	while(1){
		double s;
		Vec3d vNew;
		if ((s = w[(int)ids[0]].XY() ^ w[(int)ids[1]].XY()) > epsilon){
			//	点Oが、線分1-0から、三角形の外にはみ出している場合  
			//		... epsilon=1e-8だと無限ループ，1e-7でも稀に無限ループ
			//	1-0の法線の向きvNewでsupport pointを探し、新しい三角形にする。
			Vec2d l = w[(int)ids[1]].XY() - w[(int)ids[0]].XY();
			assert(l.square() >= epsilon2);		//	w0=w1ならば，すでに抜けているはず．
			double ll_inv = 1/l.square();
			vNew.XY() = (w[(int)ids[1]].XY()*l*ll_inv) * w[(int)ids[0]].XY()
				   - (w[(int)ids[0]].XY()*l*ll_inv) * w[(int)ids[1]].XY();
			vNew.Z() = 0;
			ids[2] = ids[0];
			ids[0] = FindVacantId(ids[1], ids[2]);
		}else if ((s = w[(int)ids[2]].XY() ^ w[(int)ids[0]].XY()) > epsilon){
			//	点Oが、線分2-0から、三角形の外にはみ出している場合
			//	2-0の法線の向きvでsupport pointを探し、新しい三角形にする。
			Vec2d l = w[(int)ids[2]].XY() - w[(int)ids[0]].XY();
			assert(l.square() >= epsilon2);		//	w0=w1ならば，すでに抜けているはず．
			double ll_inv = 1/l.square();
			vNew.XY() = (w[(int)ids[2]].XY()*l*ll_inv) * w[(int)ids[0]].XY()
				   - (w[(int)ids[0]].XY()*l*ll_inv) * w[(int)ids[2]].XY();
			vNew.Z() = 0;
			ids[1] = ids[0];
			ids[0] = FindVacantId(ids[1], ids[2]);
		}else{
			//	点Oは三角形の内側にある。
			if (ids[1] == ids[2]){
				//	1と2が同じ点=最初からonlineだったため、3角形ができなかった。
				assert(ids[0] == 1);
				assert(ids[1] == 0);
				ids[2] = 2;
				w[2] = w[0];
				v[2] = v[0];
				p[2] = p[0];
				q[2] = q[0];
			}
			break;
		}
		v[ids[0]] = vNew;
		CalcSupport(ids[0]);	//	法線の向きvNewでサポートポイントを探す
		if (w[ids[0]].XY() * v[ids[0]].XY() > -epsilon2){	//	0の外側にoがあるので触ってない
			return 0;
		}
		//	新しいsupportが1回前の線分からまったく動いていない → 点Oは外側
		double d1 = -vNew.XY() * (w[(int)ids[0]].XY()-w[(int)ids[1]].XY());
		double d2 = -vNew.XY() * (w[(int)ids[0]].XY()-w[(int)ids[2]].XY());
		if (d1 < epsilon2 || d2 < epsilon2) 
			return 0;
	}
	ids[3] = 3;
	//	三角形 ids[0-1-2] の中にoがある．ids[0]が最後に更新した頂点w
#if USERNAME==hase	//	長谷川専用デバッグコード．三角形が原点を含むことを確認
	int sign[3];
	double d[3];
	for(int i=0; i<3; ++i){
		Vec2d edge = w[ids[(i+1)%3]].XY() - w[ids[i]].XY();
		Vec2d n = Vec2d(-edge.Y(), edge.X());
		d[i] = n * w[ids[i]].XY();
		double epsilon = 1e-5;
		sign[i] = d[i] > epsilon ? 1 : d[i] < -epsilon ? -1 : 0;
	}
	if (sign[0] * sign[1] < 0 || sign[1] * sign[2] < 0){
		DSTR << "tri: 0-2:" << std::endl;
		for(int i=0; i<3; ++i){
			DSTR << w[ids[i]].X() << "\t" << w[ids[i]].Y() << std::endl;
		}
		DSTR << "dist: " << std::endl;
		for(int i=0; i<3; ++i){
			DSTR << d[i] << std::endl;
		}
		DSTR << "Error could not find a traiangle including origin." << std::endl;
	}
#endif
	
	//	三角形を小さくしていく
	int notuse = -1;
	int count = 0;
	Vec3d lastV;
	Vec3d lastTriV;
	double lastZ = DBL_MAX;
	while(1){
		count ++;
		if (count > 1000) {
#if 1	//	USERNAME==hase	//	長谷川専用デバッグコード。現在当たり判定Debug中。			
			DSTR << "Too many loop in CCDGJK." << std::endl;
			ContFindCommonPointSaveParam(a, b, a2w, b2w, dir, start, end, normal, pa, pb, dist);			
			//DebugBreak();
			bGJKDebug = true;
#endif
		}
		Vec3d s;		//	三角形の有向面積
		s = (w[ids[1]]-w[ids[0]]) % (w[ids[2]]-w[ids[0]]);
		if (s.Z() > epsilon*100.0 || -s.Z() > epsilon*100.0){
			if (s.Z() < 0){		//	逆向きの場合、ひっくり返す
				std::swap(ids[1], ids[2]);
				s *= -1;
			}
			if (bGJKDebug) DSTR << "TRI ";
			//	三角形になる場合
			notuse = -1;
			lastTriV = v[ids[3]] = s.unit();	//	3角形の法線を使う

			//	新しい w w[3] を求める
			CalcSupport(ids[3]);
		}else{
			if (bGJKDebug) DSTR << "LINE";
			int id0, id1;
			if (notuse >= 0){	
				//	前回も線分だった場合。新しい点と古い線分のどちらかの頂点で新たな線分を作る。
				double ip1 = w[ids[notuse]].XY() * w[ids[(notuse+1)%3]].XY();
				double ip2 = w[ids[notuse]].XY() * w[ids[(notuse+2)%3]].XY();
				double ipN = w[ids[(notuse+1)%3]].XY() * w[ids[(notuse+2)%3]].XY();
				if (ip1 < ip2){
					id0 = notuse;
					id1 = (notuse+1)%3;
					notuse = (notuse+2)%3;
				}else{
					id0 = notuse;
					id1 = (notuse+2)%3;
					notuse = (notuse+1)%3;
				}
			}else{
				//	初めて線分になる場合。
				//	まず、裏側の線分（z軸向きに見て、他の1or2辺の影に隠れる線分)にdoutUseマークをつける。
				bool dontUse[3] = {false, false, false};
				for(int i=0; i<3; ++i){
					Vec3d dir = w[ids[(i+1)%3]] - w[ids[i]];
					double len = dir.norm();
					if (len < 1e-15){
						dontUse[i] = true;
					}else{
						dir /= len;
						Vec3d n = Vec3d(0,0,1) - dir.z * dir;
						if (n * (w[ids[(i+2)%3]] - w[ids[i]]) < -1e-8){
							dontUse[i] = true;
						}
					}
				}
				//	使える線分のうち、原点を含むものを探す。
				double ipMin = DBL_MAX;
				int use = -1;
				for(int i=0; i<3; ++i){
					if (dontUse[i]) continue;
					double ip = w[ids[i]].XY() * w[ids[(i+1)%3]].XY();
					if (ip < ipMin){
						ipMin = ip;
						use = i;
					}
				}
				id0 = use;
				id1 = (use+1)%3;
				notuse = (use+2)%3;
			}
			//	support vector用法線には、前回の(最後に作った三角形の)法線を使う。
			if (lastTriV.square() != 0){
				v[ids[3]] = lastTriV;
			}else{
				//	初めてならば、2頂点の法線の平均の線分に垂直な成分をつかう。
				Vec3d ave = v[ids[id0]] + v[ids[id1]];
				Vec3d line = (w[ids[id1]] - w[ids[id0]]);
				double len = line.norm();
				if (len == 0){
					DSTR << "id0:" << id0 << " id1:" << id1 << std::endl;
					DSTR << "ids:"; for(int i=0; i<4; ++i) DSTR << ids[i]; DSTR << std::endl;
					DSTR << "w:"; for(int i=0; i<4; ++i) DSTR << w[i]; DSTR << std::endl;
					DSTR << "v:"; for(int i=0; i<4; ++i) DSTR << v[i]; DSTR << std::endl;
#ifdef _MSC_VER
					__debugbreak();
#endif
				}else{
					line /= len;
					ave = ave - (ave * line) * line;
				}
				v[ids[3]] = ave.unit();
			}
			CalcSupport(ids[3]);
		}
		if (bGJKDebug){
			DSTR << "v:" << v[ids[3]];
			for(int i=0; i<4; ++i){
				DSTR << "  w[" << (int) ids[i] << "] = " << w[ids[i]];
			}
			DSTR << std::endl;
			/*	//	ExcelでWとVを見るための出力	
			for(int j=0; j<3; ++j) DSTR << v[ids[3]][j] << "\t";
			DSTR << std::endl;
			for(int i=0; i<4; ++i){
				if (i==notuse) continue;
				for(int j=0; j<3; ++j) DSTR << w[ids[i]][j] << "\t";
				DSTR << std::endl;
			}
			DSTR << std::endl;
			*/

			DSTR << "notuse:" << notuse;
			for(int i=0; i<4; ++i) DSTR << " " << ids[i];
		}
		if (notuse>=0){	//	線分の場合、使った2点と新しい点で三角形を作る
			int nid[3];
			nid[0] = ids[(notuse+1)%3];
			nid[1] = ids[(notuse+2)%3];
			nid[2] = ids[3];
			Vec3d dec = TriDecompose(w[nid[0]].XY(), w[nid[1]].XY(), w[nid[2]].XY());
			double newZ = w[nid[0]].z * dec[0] + w[nid[1]].z * dec[1] + w[nid[2]].z * dec[2];
			if (bGJKDebug){
				DSTR << " newZ:" << newZ << "  dec:"<< dec << std::endl;
			}
			if (newZ + epsilon >= lastZ) goto final;
			lastZ = newZ;
			std::swap(ids[notuse], ids[3]);
		}else{
			//	どの2点とw[3]で三角形を作れるか確認する
			Vec3d decs[3];
			double minDec[3];
			int i;
			for(i=0; i<3; ++i){
				int nid0 = ids[i];
				int nid1 = ids[(i+1)%3];
				int nid2 = ids[3];
				decs[i] = TriDecompose(w[nid0].XY(), w[nid1].XY(), w[nid2].XY());
				minDec[i] = std::min(decs[i][0], std::min(decs[i][1], decs[i][2]));
			}
			if (minDec[0] > minDec[1]){
				if (minDec[0] > minDec[2]) i = 0;
				else i=2;
			}else{
				if (minDec[1] > minDec[2]) i = 1;
				else i=2;
			}
			//	ここで改善したかチェックする
			int nid0 = ids[i];
			int nid1 = ids[(i+1)%3];
			int nid2 = ids[3];
			Vec3d dec = decs[i];
			double newZ = w[nid0].z * dec[0] + w[nid1].z * dec[1] + w[nid2].z * dec[2];
			if (bGJKDebug){
				DSTR << " newZ:" << newZ << std::endl;
			}
			if (newZ + epsilon >= lastZ) goto final;
			lastZ = newZ;
			std::swap(ids[(i+2)%3], ids[3]);
		}
	}
	//	無事停止
final:
	if (notuse >=0){
		int id0 = ids[(notuse+1)%3];	int id1 = ids[(notuse+2)%3];
		double a = w[id0].norm();		double b = w[id1].norm();
		if (a+b > 1e-10){ dec[0] = b/(a+b); dec[1] = a/(a+b); }
		else {dec[0] =0.5; dec[1]=0.5;}
		dec[2]=0;
		pa = dec[0]*(Vec3d)p[id0] + dec[1]*(Vec3d)p[id1];
		pb = dec[0]*(Vec3d)q[id0] + dec[1]*(Vec3d)q[id1];
		dist = dec[0]*w[id0].z + dec[1]*w[id1].z;
		ids[0] = id0; ids[1] = id1;
		nSupport = 2;
	}else{
		dec = TriDecompose(w[ids[0]].XY(), w[ids[1]].XY(), w[ids[2]].XY());
		pa = dec[0]*(Vec3d)p[ids[0]] + dec[1]*(Vec3d)p[ids[1]] + dec[2]*(Vec3d)p[ids[2]];
		pb = dec[0]*(Vec3d)q[ids[0]] + dec[1]*(Vec3d)q[ids[1]] + dec[2]*(Vec3d)q[ids[2]];
		dist = dec[0]*w[ids[0]].z + dec[1]*w[ids[1]].z + dec[2]*w[ids[2]].z;
		nSupport = 3;
	}
	normal = w2z.Conjugated() * v[ids[3]];
	normal.unitize();
	//	HASE_REPORT
	static bool bSave = false;
	if (bSave){
		ContFindCommonPointSaveParam(a, b, a2w, b2w, dir, start, end, normal, pa, pb, dist);
	}
	if (dist > end) return -1;
	if (dist < start) return -2;
	return 1;
}

#define USE_NON_CONTINUOUS_DETECTORS
#ifdef USE_NON_CONTINUOUS_DETECTORS
/*	GJKアルゴリズム(FindCommonPoint(), FindClosestPoint())の実装．
	SOLIDのソースを参考にして書いた部分です．
	FindClosestPointは使っていません．
	
	FindCommonPointは，
		CDShapePair::Detect()
		CDContactAnalysis::CalcNormal(CDShapePair* cp)
	で使っています．ペナルティ法を使うと呼び出されます．	*/

//	過去のSupportPointが張る形状(点・線分・3角形，4面体)．最大4点まで．
static int usedBits;		//	下位4ビットが，どの4点で現在の形状が構成されるかを表す．
static int lastId;			//	最後に見つけたSupportPointをしまった場所(0～3)
static int lastBit;			//	lastBit = 1<<lastId (ビットで表したもの）
static int allUsedBits;		//	usedBitsにlastBitを加えたもの
static double det[16][4];	//	係数
//	det[3][0] * p[0] + det[3][1]*p[1] + det[3][2]*p[2] / sum で最近傍点が求まる
//	p_qベクトルたちが張る形状の体積(or長さor面積)．4点から作れる16通りについて

/*
      a   |
     /    | v = (1-k) a + k b
    /     | va = (1-k)aa + kba = vb = (1-k)ab + kbb
   /v     | -kaa + kba + kab - kbb = -aa + ab
  b       | k = a^2-ab / (a^2+b^2-2ab), 1-k = b^2-ab / (a^2+b^2-2ab)
----------+ なので， k = a^2-ab を と (k-1) = b^2-abを記録しておく	*/

///	det(長さ・面積・体積)の計算．垂線の足が頂点を何:何に内分するかも計算
inline void CalcDet() {
	static double dotp[4][4];	//	p_q[i] * p_q[j] 

	//	新しく増えた点(lastId)について，内積を計算
	for (int i = 0, curPoint = 1; i < 4; ++i, curPoint <<=1){
		if (usedBits & curPoint) 
			dotp[i][lastId] = dotp[lastId][i] = p_q[i] * p_q[lastId];
	}
	dotp[lastId][lastId] = p_q[lastId] * p_q[lastId];

	det[lastBit][lastId] = 1;
	for (int j = 0, sj = 1; j < 4; ++j, sj <<= 1) {
		if (usedBits & sj) {		
			int s2 = sj|lastBit;	//	新しく増えた点について係数の計算
			det[s2][j] = dotp[lastId][lastId] - dotp[lastId][j];	//	a^2-ab
			det[s2][lastId] = dotp[j][j] - dotp[j][lastId];			//	b^2-ab
			for (int k = 0, sk = 1; k < j; ++k, sk <<= 1) {	//	3点の場合
				if (usedBits & sk) {
					int s3 = sk|s2;
					det[s3][k] = det[s2][j] * (dotp[j][j] - dotp[j][k]) + 
						det[s2][lastId] * (dotp[lastId][j] - dotp[lastId][k]);
					det[s3][j] = det[sk|lastBit][k] * (dotp[k][k] - dotp[k][j]) + 
						det[sk|lastBit][lastId] * (dotp[lastId][k] - dotp[lastId][j]);
					det[s3][lastId] = det[sk|sj][k] * (dotp[k][k] - dotp[k][lastId]) + 
						det[sk|sj][j] * (dotp[j][k] - dotp[j][lastId]);
				}
			}
		}
	}
	if (allUsedBits == 15) {	//	4点の場合
		det[15][0] =	det[14][1] * (dotp[1][1] - dotp[1][0]) + 
						det[14][2] * (dotp[2][1] - dotp[2][0]) + 
						det[14][3] * (dotp[3][1] - dotp[3][0]);
		det[15][1] =	det[13][0] * (dotp[0][0] - dotp[0][1]) + 
						det[13][2] * (dotp[2][0] - dotp[2][1]) + 
						det[13][3] * (dotp[3][0] - dotp[3][1]);
		det[15][2] =	det[11][0] * (dotp[0][0] - dotp[0][2]) + 
						det[11][1] * (dotp[1][0] - dotp[1][2]) +	
						det[11][3] * (dotp[3][0] - dotp[3][2]);
		det[15][3] =	det[ 7][0] * (dotp[0][0] - dotp[0][3]) + 
						det[ 7][1] * (dotp[1][0] - dotp[1][3]) + 
						det[ 7][2] * (dotp[2][0] - dotp[2][3]);
	}
}


//	係数から，最近傍点 v を計算
inline void CalcVector(int usedBits, Vec3d& v) {
	double sum = 0;
	v.clear();
	for (int i = 0; i < 4; ++i) {
		if (usedBits & (1<<i)) {
			sum += det[usedBits][i];
			v += p_q[i] * det[usedBits][i];
		}
	}
	v *= 1 / sum;
}

//	係数から，最近傍点と，元の2つの図形上での，その点の位置を計算
inline void CalcPoints(int usedBits, Vec3d& p1, Vec3d& p2) {
	double sum = 0;
	p1.clear();
	p2.clear();
	for (int i = 0, curPoint = 1; i < 4; ++i, curPoint <<= 1) {
		if (usedBits & curPoint) {
			sum += det[usedBits][i];
			p1 += (Vec3d)p[i] * det[usedBits][i];
			p2 += (Vec3d)q[i] * det[usedBits][i];
		}
	}
	if (sum){
		double s = 1 / sum;
		p1 *= s;
		p2 *= s;
	}else{
		p1 = p[0];
		p2 = q[0];
	}
}

//	最近傍点を返す．
inline bool CalcClosest(Vec3d& v) {
	CalcDet();
	if (!usedBits){
		usedBits = lastBit;
		v = p_q[lastId];
		return true;
	}
	int simplex[5][4];
	int nSimplex[5];
	const char numVertices[] = {
		0, 1, 1, 2, 1, 2, 2, 3,
		1, 2, 2, 3, 2, 3, 3, 4
	};
	int nVtx = numVertices[allUsedBits];
	simplex[nVtx][0] = allUsedBits;
	nSimplex[nVtx] = 1;
	
	for(; nVtx>1; --nVtx){
		for(int sid=0; sid<nSimplex[nVtx]; ++sid){
			int s = simplex[nVtx][sid];
			nSimplex[nVtx-1]=0;
			for(int i = 0; i!=4; ++i){
				int bit = 1<<i;
				if ((s&bit) && det[s][i] <= 0){
					simplex[nVtx-1][nSimplex[nVtx-1]] = s & ~bit;
					++ nSimplex[nVtx-1];
				}
			}
			if (nSimplex[nVtx-1] == 0){
				usedBits = s;
				CalcVector(usedBits, v);	//	最近傍点を計算して返す．			
				return true;
			}
		}
	}
	return false;
}

//	新しい点wが，今までの点と等しい場合
inline bool HasSame(const Vec3d& w) {
	for (int i = 0; i < 4; ++i){
		if ((allUsedBits & (1<<i)) && (p_q[i]-w).square() < epsilon2) return true;
	}
	return false;
}
inline char VacantIdFromBits(char bits){
	return vacants[(int)bits];
}

bool FASTCALL FindCommonPoint(const CDConvex* a, const CDConvex* b,
	const Posed& a2w, const Posed& b2w, Vec3d& v, Vec3d& pa, Vec3d& pb) {
	Vec3d w;

	usedBits = 0;
	allUsedBits = 0;
	int count = 0;
	do {
		lastId = VacantIdFromBits(usedBits);
		lastBit = 1 << lastId;

		p_id[lastId] = a->Support(p[lastId], a2w.Ori().Conjugated() * (-v));
		q_id[lastId] = b->Support(q[lastId], b2w.Ori().Conjugated() * v);
		w = a2w * p[lastId]  -  b2w * q[lastId];
		if (v*w > 0) return false;			//	原点がsupport面の外側
		if (HasSame(w)) return false;		//	supportが1点に集中＝原点は外にある．
		p_q[lastId] = w;					//	新しい点を代入
		allUsedBits = usedBits|lastBit;//	使用中頂点リストに追加
		if (!CalcClosest(v)) return false;

		count ++;
		if (count > 100){
			Vec3d vtxs[3];
			Vec3d notUsed[3];
			int nUsed=0, nNotUsed=0;
			for(int i=0; i<4; ++i){
				if (usedBits & (1<<i)){
					vtxs[nUsed] = p_q[i];
					nUsed++;
				}else{
					notUsed[nNotUsed] = p_q[i];
					nNotUsed++;
				}
			}
			if (nUsed == 3 && allUsedBits == 15){
				Vec3d n = (vtxs[2]-vtxs[0]) ^ (vtxs[1]-vtxs[0]);
				n.unitize();
				if (n * (notUsed[0] - vtxs[0]) > 0){
					n *= -1;
				}
				double dist = n * (-vtxs[0]);
/*				DSTR << "n:\t" << nUsed << "Dist:\t" << dist << std::endl;
				DSTR << "v:\t" << v.x << "\t" << v.y << "\t" << v.z << std::endl; 
				DSTR << "Normal:\t" << n.x << "\t" << n.y << "\t" << n.z << std::endl;
				for(int i=0; i<nUsed; ++i){
					DSTR << "Use:\t" << vtxs[i].x << "\t" << vtxs[i].y << "\t" << vtxs[i].z << std::endl;
				}
				for(int i=0; i<nNotUsed; ++i){
					DSTR << "Not:\t" << notUsed[i].x << "\t" << notUsed[i].y << "\t" << notUsed[i].z << std::endl;
				}
				*/
			}else{
				/*	DSTR << "n:\t" << nUsed << std::endl;
				DSTR << "v:\t" << v.x << "\t" << v.y << "\t" << v.z << std::endl;
				for(int i=0; i<nUsed; ++i){
					DSTR << "Use:\t" << vtxs[i].x << "\t" << vtxs[i].y << "\t" << vtxs[i].z << std::endl;
				}
				for(int i=0; i<nNotUsed; ++i){
					DSTR << "Not:\t" << notUsed[i].x << "\t" << notUsed[i].y << "\t" << notUsed[i].z << std::endl;
				}
				*/
			}
		}
	} while ( usedBits < 15 && !(v.square() < epsilon2) ) ;
	CalcPoints(usedBits, pa, pb);
	return true;
}


#define USE_FIND_CLOSEST_POINT
#ifdef USE_FIND_CLOSEST_POINT

inline bool IsDegenerate(const Vec3d& w) {
	for (int i = 0, curPoint = 1; i < 4; ++i, curPoint <<= 1){
		if ((allUsedBits & curPoint) && (p_q[i]-w).square() < 1e-6){
//		if ((allUsedBits & curPoint) && (p_q[i]-w).square() < epsilon2){
			return true;
		}
	}
	return false;
}
double FASTCALL FindClosestPoints(const CDConvex* a, const CDConvex* b,
					  const Posed& a2w, const Posed& b2w,
					  Vec3d& v, Vec3d& pa, Vec3d& pb) {
	Vec3f p_0, q_0;
	a->Support(p_0, Vec3d());
	b->Support(q_0, Vec3d());
	v = a2w * p_0 - b2w * q_0;	
	double dist = v.norm();	
	Vec3d w;				
	double maxSupportDist = 0.0f;

	usedBits = 0;
	allUsedBits = 0;

	int count = 0;
	while (usedBits < 15 && dist > epsilon) {
		lastId = 0;
		lastBit = 1;
		while (usedBits & lastBit) { ++lastId; lastBit <<= 1; }
		p_id[lastId] = a->Support(p[lastId], a2w.Ori().Conjugated() * (-v));
		q_id[lastId] = b->Support(q[lastId], b2w.Ori().Conjugated() * v);
		w = a2w * p[lastId]  -  b2w * q[lastId];
		double supportDist = w*v/dist;
		if (maxSupportDist < supportDist) maxSupportDist= supportDist;
		if (dist - maxSupportDist <= dist * epsilon*10) break;
		if (IsDegenerate(w)) break;
		p_q[lastId] = w;
		allUsedBits = usedBits|lastBit;
		if (!CalcClosest(v)) break;
		dist = v.norm();

		count++;
		if(count == 100){
			DSTR << "Too many loop in FindClosestPoints!!" << std::endl;		
			break;
		}
	}
	CalcPoints(usedBits, pa, pb);
	return dist;
}
#endif
#endif

}
