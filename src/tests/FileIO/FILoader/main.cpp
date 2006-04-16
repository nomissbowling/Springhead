/**
 Springhead2/src/tests/FileIO/FILoader/main.cpp

【概要】
  Xファイルをロードし、Physicsエンジンと接続してシミュレーションする。
  
【終了基準】
  ・プログラムが正常終了したら0を返す。  
 
【処理の流れ】
  ・test.xファイルをロードする。
  ・ロードした情報を出力する。
  ・Physicsエンジンと接続し、シミュレーションさせる。
  
 */
#include <Springhead.h>
#include <GL/glut.h>
#define	ESC		27
namespace Spr{
	PHSdkIf* phSdk;
	PHSceneIf** scene;
	CDShapeIf** shape;
	GRSdkIf* grSdk;
	GRDeviceGLIf* grDevice;
	GRDebugRenderIf* render;
}
using namespace Spr;


// 材質の設定
GRMaterial mat_red(Vec4f(0.0, 0.0, 0.0, 1.0),		// ambient
					Vec4f(0.7, 0.0, 0.0, 1.0),		// diffuse
					Vec4f(1.0, 1.0, 1.0, 1.0),		// specular
					Vec4f(0.0, 0.0, 0.0, 1.0),		// emissive
					100.0);							// power

GRMaterial mat_green(Vec4f(0.0, 0.0, 0.0, 1.0),		// ambient
					Vec4f(0.0, 0.7, 0.0, 1.0),		// diffuse
					Vec4f(1.0, 1.0, 1.0, 1.0),		// specular
					Vec4f(0.0, 0.0, 0.0, 1.0),		// emissive
					20.0);							// power

GRMaterial mat_blue(Vec4f(0.0, 0.0, 0.0, 1.0),		// ambient
					Vec4f(0.0, 0.0, 0.7, 1.0),		// diffuse
					Vec4f(1.0, 1.0, 1.0, 1.0),		// specular
					Vec4f(0.0, 0.0, 0.0, 1.0),		// emissive
					20.0);							// power

/**
 brief     	glutDisplayFuncで指定したコールバック関数
 param	 	なし
 return 	なし
 */
void display(){
	//	バッファクリア
	render->ClearBuffer();
	render->SetMaterial(mat_red);	

	PHSceneIf* scene = NULL;
	if (phSdk->NScene()){
		PHSceneIf** s = phSdk->GetScenes();
		scene = *s;
	}
	if (!scene){
		std::cout << "scene == NULL. File may not found." << std::endl;
		exit(-1);
	}
	PHSolidIf **solids = scene->GetSolids();
	for (int num=0; num < scene->NSolids(); ++num){
		if (num == 1){
			render->SetMaterial(mat_green);	
		} else if (num == 2) {
			render->SetMaterial(mat_blue);	
		}
		Affinef af;
		solids[num]->GetPose().ToAffine(af);
		render->PushModelMatrix();
		render->MultModelMatrix(af);
		CDShapeIf** shapes = solids[num]->GetShapes();
		for(int s=0; s<solids[num]->NShape(); ++s){
			Affinef af;
			shapes[s]->GetPose().ToAffine(af);
			render->PushModelMatrix();
			render->MultModelMatrix(af);
			CDConvexMeshIf* mesh = ICAST(CDConvexMeshIf, shapes[s]);
			if (mesh){
				Vec3f* base = mesh->GetVertices();
				for (size_t f=0; f<mesh->NFace(); ++f) {	
					CDFaceIf* face = mesh->GetFace(f);
					render->DrawFace(face, base);
				}
			}
			CDSphereIf* sphere = ICAST(CDSphereIf, shapes[s]);
			if (sphere){
				float r = sphere->GetRadius();
				GLUquadricObj* quad = gluNewQuadric();
				gluSphere(quad, r, 16, 8);
				gluDeleteQuadric(quad);
			}
			render->PopModelMatrix();
		}
		render->PopModelMatrix();
	}

	render->EndScene();
}
/**
 brief     	光源の設定
 param	 	なし
 return 	なし
 */
void setLight() {
	GRLight light0;
	light0.ambient	= Vec4f(0.0, 0.0, 0.0, 1.0);
	light0.diffuse	= Vec4f(0.9, 0.9, 0.9, 1.0);
	light0.specular	= Vec4f(1.0, 1.0, 1.0, 1.0);
	light0.position = Vec4f(0.0, 10.0, -5.0, 1.0);
	render->PushLight(light0);
	
	GRLight light1;
	light1.ambient	= Vec4f(0.0, 0.0, 0.0, 1.0);
	light1.diffuse	= Vec4f(0.9, 0.9, 0.9, 1.0);
	light1.specular	= Vec4f(1.0, 1.0, 1.0, 1.0);
	light1.position = Vec4f(0.0, 5.0, 10.0, 1.0);
	render->PushLight(light1);

	GRLight light2;
	light2.ambient	= Vec4f(0.0, 0.0, 0.0, 1.0);
	light2.diffuse	= Vec4f(0.9, 0.9, 0.9, 1.0);
	light2.specular	= Vec4f(1.0, 1.0, 1.0, 1.0);
	light2.position = Vec4f(-5.0, 5.0, -10.0, 1.0);
	render->PushLight(light2);

}

/**
 brief  	glutReshapeFuncで指定したコールバック関数
 param	 	<in/--> w　　幅
 param  	<in/--> h　　高さ
 return 	なし
 */
void reshape(int w, int h){
	render->Reshape(Vec2f(w,h));
}

/**
 brief 		glutKeyboardFuncで指定したコールバック関数 
 param		<in/--> key　　 ASCIIコード
 param 		<in/--> x　　　 キーが押された時のマウス座標
 param 		<in/--> y　　　 キーが押された時のマウス座標
 return 	なし
 */
void keyboard(unsigned char key, int x, int y){
	if (key == ESC) exit(0);
}

/**
 brief  	glutIdleFuncで指定したコールバック関数
 param	 	なし
 return 	なし
 */
void idle(){
	if(scene && *scene) (*(scene))->Step();
	glutPostRedisplay();
	static int count;
	count ++;
	if (count > 1200){
		std::cout << "1200 count passed." << std::endl;
		exit(0);
	}
}

/**
 brief		メイン関数
 param		<in/--> argc　　コマンドライン入力の個数
 param		<in/--> argv　　コマンドライン入力
 return		0 (正常終了)
 */

int main(int argc, char* argv[]){
	FISdkIf* fiSdk = CreateFISdk();
	FIFileXIf* fileX = fiSdk->CreateFileX();
	ObjectIfs objs;
	if (argc>=2){
		phSdk = CreatePHSdk();		//	PHSDKを用意して，
		objs.push_back(phSdk);		
		fileX->Load(objs, argv[1]);	//	ファイルローダに渡す方式
	}else{
		fileX->Load(objs, "test.x");	//	PHSDKごとロードして，
		phSdk = ICAST(PHSdkIf, objs.front());	//	PHSDKを受け取る方式
	}
	fiSdk->Clear();	//	ファイルローダのメモリを解放．
	objs.clear();
	phSdk->Print(DSTR);

	scene = phSdk->GetScenes();		// Sceneの取得
	shape = phSdk->GetShapes();		// Shapeの取得
	DSTR << "Loaded : " << "NScene=" << phSdk->NScene() << ", NSphape=" << phSdk->NShape() << std::endl;

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	int window = glutCreateWindow("FILoad");

	// Graphics Sdk
	grSdk = CreateGRSdk();
	render = grSdk->CreateDebugRender();
	grDevice = grSdk->CreateDeviceGL(window);
	grDevice->Init();
	render->SetDevice(grDevice);

	// 視点設定
	Affinef view;
	view.Pos() = Vec3f(-2.0, 3.0, 5.0);									// eye
		view.LookAtGL(Vec3f(0.0, 0.0, 0.0), Vec3f(0.0, 1.0, 0.0));		// center, up 
	view = view.inv();	
	render->SetViewMatrix(view);

	// 光源設定
	setLight();

	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	glutIdleFunc(idle);
	glutMainLoop();		// これ追加すると大量のmemory leakが発生する

	return 0;
}

