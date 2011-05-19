#include "FWGLUISample.h"
#include "SampleModel.h"
#include <iostream>
#include <sstream>
#include <GL/glut.h>
#include <GL/glui.h>

#define ESC 27

FWGLUISample::FWGLUISample(){
	bDrawInfo = false;
}

void FWGLUISample::Init(int argc, char* argv[]){
	SetGRAdaptee(TypeGLUI); // GLUIを使うことを指定
	GRInit(argc, argv);		// GLUIの初期化				
	CreateSdk();			// Sdkの作成
	GetSdk()->Clear();		// SDKの初期化
	GetSdk()->CreateScene(PHSceneDesc(), GRSceneDesc());	// Sceneの作成
	GetSdk()->GetScene()->GetPHScene()->SetTimeStep(0.05);	// シミュレーションの刻み時間を設定

	FWWinDesc windowDesc;					// GLのウィンドウディスクリプタ
	windowDesc.title = "FWAppSample";		// ウィンドウのタイトル
	AssignScene(CreateWin(windowDesc));		// ウィンドウの作成、シーンの割り当て
	
	FWGLUIDesc uiDesc;
	{
		uiDesc.fromLeft = 510;	uiDesc.fromTop	=  30;
	}
	// UIを作る場合の処理（if(glui)と同義）
	if(glui = ((FWGLUI*)GetGRAdaptee())->CreateGUI(GetWin(0)->GetID(), uiDesc)){
		DesignGUI();
	}

	InitCameraView();				// カメラビューの初期化
	CreateObjects();				// 剛体を作成
	CreateTimer();					// タイマを作成
}

void FWGLUISample::InitCameraView(){
	//	Affinef 型が持つ、ストリームから行列を読み出す機能を利用して視点行列を初期化
	std::istringstream issView(
		"((0.9996 0.0107463 -0.0261432 -0.389004)"
		"(-6.55577e-010 0.924909 0.380188 5.65711)"
		"(0.0282657 -0.380037 0.92454 13.7569)"
		"(     0      0      0      1))"
	);
	issView >> cameraInfo.view;
}

void FWGLUISample::CreateObjects(){
	PHSceneIf* phscene = GetSdk()->GetScene()->GetPHScene();
	PHSolidDesc desc;
	CDBoxDesc bd;

	// 床(物理法則に従わない，運動が変化しない)
	{
		// 剛体(soFloor)の作成
		desc.mass = 1e20f;
		desc.inertia *= 1e30f;
		PHSolidIf* soFloor = phscene->CreateSolid(desc);		// 剛体をdescに基づいて作成
		soFloor->SetDynamical(false);
		soFloor->SetGravity(false);
		// 形状(shapeFloor)の作成
		bd.boxsize = Vec3f(50, 10, 50);
		CDShapeIf* shapeFloor = GetSdk()->GetPHSdk()->CreateShape(bd);
		// 剛体に形状を付加する
		soFloor->AddShape(shapeFloor);
		soFloor->SetFramePosition(Vec3d(0, -5, 0));
	}

	// 箱(物理法則に従う，運動が変化)
	{
		// 剛体(soBox)の作成
		desc.mass = 0.5;
		desc.inertia *= 0.033;
		PHSolidIf* soBox = phscene->CreateSolid(desc);
		// 形状(shapeBox)の作成
		bd.boxsize = Vec3f(2,2,2);
		CDShapeIf* shapeBox = GetSdk()->GetPHSdk()->CreateShape(bd);
		// 剛体に形状を付加
		soBox->AddShape(shapeBox);
		soBox->SetFramePosition(Vec3d(0, 10, 0));
	}
}

void FWGLUISample::TimerFunc(int id){
	GetSdk()->Step();
	PostRedisplay();
}

void FWGLUISample::Display(){
		FWWin* win = GetCurrentWin();
	if(!win)
		return;

	GRRenderIf* render = win->GetRender();
	FWSceneIf*  scene  = win->GetScene();
	
	/// 描画モードの設定
	GetSdk()->SetDebugMode(true);
	scene->SetRenderMode(true, false);
	scene->EnableRenderForce(bDrawInfo, bDrawInfo);
	scene->EnableRenderContact(bDrawInfo);
	
	/// カメラ座標の指定
	if (win->scene){
		GRCameraIf* cam = win->scene->GetGRScene()->GetCamera();
		if (cam && cam->GetFrame()){
			cam->GetFrame()->SetTransform(cameraInfo.view);
		}else{
			render->SetViewMatrix(cameraInfo.view.inv());
		}
	}

	/// 描画の実行
	GetSdk()->SwitchScene(scene);
	GetSdk()->SwitchRender(render);
	GetSdk()->Draw();
	render->SwapBuffers();
}

void FWGLUISample::Reset(){
	GetSdk()->GetScene()->GetPHScene()->Clear();
	CreateObjects();
}

void FWGLUISample::Keyboard(int key, int x, int y){
	switch (key) {
		case ESC:
		case 'q':
			exit(0);
			break;
		case 'r':
			Reset();
			break;
		case 'd':
			bDrawInfo = !bDrawInfo;
			break;
		case '1':
			DSTR << "box" << std::endl;
			CreateBox(GetSdk());
			break;
		case '2':
			DSTR << "sphere" << std::endl;
			CreateSphere(GetSdk());
			break;
		case '3':
			DSTR << "capsule" << std::endl;
			CreateCapsule(GetSdk());
			break;
		case '4':
			DSTR << "roundcone" << std::endl;
			CreateRoundCone(GetSdk());
			break;
		default:
			break;
	}
}


void FWGLUISample::DesignGUI(){
	panel = glui->add_panel("Sample", true);
	button1 = glui->add_button_to_panel(panel, "Create Box", 1, GLUI_CB(CallButton1));
}

void FWGLUISample::CallButton1(int control){
	((FWGLUISample*)FWApp::instance)->Button1(control);
}

void FWGLUISample::Button1(int control){
	Keyboard('1', 0, 0);
}

