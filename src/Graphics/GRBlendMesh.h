/*
 *  Copyright (c) 2003-2006, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#ifndef GRBlendMesh_H
#define GRBlendMesh_H

#include <SprGraphics.h>
#include "GRFrame.h"

namespace Spr{;

class GRMaterial;

/** @class GRBlendMesh
	@brief グラフィックスシーングラフでの座標系を表す．	*/
class GRBlendMesh: public InheritGRVisual<GRBlendMeshIf, GRVisual>, public GRBlendMeshDesc{
	std::vector<unsigned int> list;				///< ディスプレイリストの識別子
	GRRenderIf* render;
	
	/// 頂点フォーマット GRVertexElement に合わせ、ディスプレイリストを作成する．
	void CreateList(GRRenderIf* r);
	/// MaterialListのインデックスに合わせ、ディスプレイリストを細分化．
	void CreateListElement(void* vtx);
public:
	OBJECT_DEF(GRBlendMesh);
	ACCESS_DESC(GRBlendMesh);

	std::vector< UTRef<GRMaterial> > material;				///< マテリアル
	std::vector<int>                 materialList;			///< マテリアルのインデックスリスト
	/**
	   facesと、三角形分割する前のXファイルで指定された面との関連付け \n
	  （Xファイルからロードされた面は、すべて三角形分割されているため、
	   どの面がどのマテリアルを適用するのか判断するためのもの）			*/
	std::vector<int> originalFaces;				///< 面を構成するための頂点インデックス（三角形分割前の面に対するインデックス）
	std::vector<unsigned int> elementIndex;		///< 面を構成するための頂点インデックス（三角形分割後の面に対するインデックス）
	std::vector<int> faceNormals;				///< 法線インデックス

	GRBlendMesh(const GRBlendMeshDesc& desc=GRBlendMeshDesc());
	~GRBlendMesh();
	void Render(GRRenderIf* r);
	void Rendered(GRRenderIf* r);
};
}
#endif
