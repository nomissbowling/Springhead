//	Do not edit. MakeTypeDesc.bat will update this file.
	
	GRFrameTransformMatrix* pGRFrameTransformMatrix = NULL;
	desc = DBG_NEW UTTypeDesc("GRFrameTransformMatrix");
	desc->size = sizeof(GRFrameTransformMatrix);
	desc->access = DBG_NEW FIAccess<GRFrameTransformMatrix>;
	field = desc->AddField("", "Affinef", "transform", "");
	field->offset = (char*)&(pGRFrameTransformMatrix->transform) - (char*)pGRFrameTransformMatrix;
	db->RegisterDesc(desc);
	
	GRFrameDesc* pGRFrameDesc = NULL;
	desc = DBG_NEW UTTypeDesc("GRFrameDesc");
	desc->size = sizeof(GRFrameDesc);
	desc->ifInfo = GRFrameIf::GetIfInfoStatic();
	((IfInfo*)GRFrameIf::GetIfInfoStatic())->desc = desc;
	desc->access = DBG_NEW FIAccess<GRFrameDesc>;
	field = desc->AddField("", "Affinef", "transform", "");
	field->offset = (char*)&(pGRFrameDesc->transform) - (char*)pGRFrameDesc;
	db->RegisterDesc(desc);
	
	GRMeshDesc* pGRMeshDesc = NULL;
	desc = DBG_NEW UTTypeDesc("GRMeshDesc");
	desc->size = sizeof(GRMeshDesc);
	desc->ifInfo = GRMeshIf::GetIfInfoStatic();
	((IfInfo*)GRMeshIf::GetIfInfoStatic())->desc = desc;
	desc->access = DBG_NEW FIAccess<GRMeshDesc>;
	field = desc->AddField("vector", "Vec3f", "vertices", "");
	field->offset = (char*)&(pGRMeshDesc->vertices) - (char*)pGRMeshDesc;
	field = desc->AddField("vector", "Vec3f", "normals", "");
	field->offset = (char*)&(pGRMeshDesc->normals) - (char*)pGRMeshDesc;
	field = desc->AddField("vector", "int", "faces", "");
	field->offset = (char*)&(pGRMeshDesc->faces) - (char*)pGRMeshDesc;
	db->RegisterDesc(desc);
	
	GRLight* pGRLight = NULL;
	desc = DBG_NEW UTTypeDesc("GRLight");
	desc->size = sizeof(GRLight);
	desc->access = DBG_NEW FIAccess<GRLight>;
	field = desc->AddField("", "Vec4f", "ambient", "");
	field->offset = (char*)&(pGRLight->ambient) - (char*)pGRLight;
	field = desc->AddField("", "Vec4f", "diffuse", "");
	field->offset = (char*)&(pGRLight->diffuse) - (char*)pGRLight;
	field = desc->AddField("", "Vec4f", "specular", "");
	field->offset = (char*)&(pGRLight->specular) - (char*)pGRLight;
	field = desc->AddField("", "Vec4f", "position", "");
	field->offset = (char*)&(pGRLight->position) - (char*)pGRLight;
	field = desc->AddField("", "float", "range", "");
	field->offset = (char*)&(pGRLight->range) - (char*)pGRLight;
	field = desc->AddField("", "float", "attenuation0", "");
	field->offset = (char*)&(pGRLight->attenuation0) - (char*)pGRLight;
	field = desc->AddField("", "float", "attenuation1", "");
	field->offset = (char*)&(pGRLight->attenuation1) - (char*)pGRLight;
	field = desc->AddField("", "float", "attenuation2", "");
	field->offset = (char*)&(pGRLight->attenuation2) - (char*)pGRLight;
	field = desc->AddField("", "Vec3f", "spotDirection", "");
	field->offset = (char*)&(pGRLight->spotDirection) - (char*)pGRLight;
	field = desc->AddField("", "float", "spotFalloff", "");
	field->offset = (char*)&(pGRLight->spotFalloff) - (char*)pGRLight;
	field = desc->AddField("", "float", "spotInner", "");
	field->offset = (char*)&(pGRLight->spotInner) - (char*)pGRLight;
	field = desc->AddField("", "float", "spotCutoff", "");
	field->offset = (char*)&(pGRLight->spotCutoff) - (char*)pGRLight;
	db->RegisterDesc(desc);
	
	GRMaterialDesc* pGRMaterialDesc = NULL;
	desc = DBG_NEW UTTypeDesc("GRMaterialDesc");
	desc->size = sizeof(GRMaterialDesc);
	desc->ifInfo = GRMaterialIf::GetIfInfoStatic();
	((IfInfo*)GRMaterialIf::GetIfInfoStatic())->desc = desc;
	desc->access = DBG_NEW FIAccess<GRMaterialDesc>;
	field = desc->AddField("", "Vec4f", "ambient", "");
	field->offset = (char*)&(pGRMaterialDesc->ambient) - (char*)pGRMaterialDesc;
	field = desc->AddField("", "Vec4f", "diffuse", "");
	field->offset = (char*)&(pGRMaterialDesc->diffuse) - (char*)pGRMaterialDesc;
	field = desc->AddField("", "Vec4f", "specular", "");
	field->offset = (char*)&(pGRMaterialDesc->specular) - (char*)pGRMaterialDesc;
	field = desc->AddField("", "Vec4f", "emissive", "");
	field->offset = (char*)&(pGRMaterialDesc->emissive) - (char*)pGRMaterialDesc;
	field = desc->AddField("", "float", "power", "");
	field->offset = (char*)&(pGRMaterialDesc->power) - (char*)pGRMaterialDesc;
	field = desc->AddField("", "string", "texture", "");
	field->offset = (char*)&(pGRMaterialDesc->texture) - (char*)pGRMaterialDesc;
	db->RegisterDesc(desc);
	
	GRCamera* pGRCamera = NULL;
	desc = DBG_NEW UTTypeDesc("GRCamera");
	desc->size = sizeof(GRCamera);
	desc->access = DBG_NEW FIAccess<GRCamera>;
	field = desc->AddField("", "Vec2f", "size", "");
	field->offset = (char*)&(pGRCamera->size) - (char*)pGRCamera;
	field = desc->AddField("", "Vec2f", "center", "");
	field->offset = (char*)&(pGRCamera->center) - (char*)pGRCamera;
	field = desc->AddField("", "float", "front", "");
	field->offset = (char*)&(pGRCamera->front) - (char*)pGRCamera;
	field = desc->AddField("", "float", "back", "");
	field->offset = (char*)&(pGRCamera->back) - (char*)pGRCamera;
	db->RegisterDesc(desc);
	
	GRSceneDesc* pGRSceneDesc = NULL;
	desc = DBG_NEW UTTypeDesc("GRSceneDesc");
	desc->size = sizeof(GRSceneDesc);
	desc->ifInfo = GRSceneIf::GetIfInfoStatic();
	((IfInfo*)GRSceneIf::GetIfInfoStatic())->desc = desc;
	desc->access = DBG_NEW FIAccess<GRSceneDesc>;
	db->RegisterDesc(desc);
	
	GRSdkDesc* pGRSdkDesc = NULL;
	desc = DBG_NEW UTTypeDesc("GRSdkDesc");
	desc->size = sizeof(GRSdkDesc);
	desc->ifInfo = GRSdkIf::GetIfInfoStatic();
	((IfInfo*)GRSdkIf::GetIfInfoStatic())->desc = desc;
	desc->access = DBG_NEW FIAccess<GRSdkDesc>;
	db->RegisterDesc(desc);
