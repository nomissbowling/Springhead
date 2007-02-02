#ifndef SPR_FWSDKIF_H
#define SPR_FWSDKIF_H

#include <Framework/SprFWScene.h>
#include <Physics/SprPHSdk.h>
#include <Graphics/SprGRSdk.h>
#include <FileIO/SprFISdk.h>

namespace Spr{;

struct FWSdkDesc{
};

struct FWSdkIf : ObjectIf {
	IF_DEF(FWSdk);

	/** @brief SDKオブジェクトを作成する
	 */
	static FWSdkIf* SPR_CDECL CreateSdk();

	/** @brief 空のシーンを作成する
		空のシーンを作成し、シーンリストに追加する．
		作成されたシーンはアクティブとなる．
	 */
	virtual FWSceneIf* CreateScene(const FWSceneDesc& desc)=0;

	/** @brief シーンをファイルからロードする
		@param filename ファイル名
		指定されたファイルからシーンをロードし、シーンリストに追加する．
		ロード後は，ロードされたシーンがアクティブとなる．
		ファイル中に複数のシーンが含まれていた場合は，最後にロードされたシーンがアクティブとなる．
	 */
	virtual void LoadScene(UTString filename)=0;

	/// シーンの個数を取得する
	virtual int NScene() const=0;

	/** @brief シーンを切り替える
		@param index
		index番目のシーンをアクティブにする
	 */
	virtual void	SwitchScene(FWSceneIf* scene)=0;

	/** @brief シーンを取得する
		@param index 取得するシーンのインデックス．省略するとアクティブシーンを取得する．
	 */
	virtual FWSceneIf* GetScene(int index = -1)=0;

	/// PHSdkオブジェクトを取得する
	virtual PHSdkIf* GetPHSdk()=0;

	/// GRSdkオブジェクトを取得する
	virtual GRSdkIf* GetGRSdk()=0;

	/// FISdkオブジェクトを取得する
	virtual FISdkIf* GetFISdk()=0;

	/// レンダラを取得する
	virtual GRRenderIf* GetRender() = 0;
	/// レンダラを設定する
	virtual void SetRender(GRRenderIf* render) = 0;
	/// デバイスを取得する
	virtual GRDeviceIf* GetDevice() = 0;
	/// デバイスを設定する
	virtual void SetDevice(GRDeviceIf* device) = 0;

	/** @brief デバッグ描画モードの取得
	 */
	virtual bool GetDebugMode() = 0;

	/** @brief デバッグ描画モードの設定
	 */
	virtual void SetDebugMode(bool debug = true) = 0;

	/** @brief シミュレーションの実行
	 */
	virtual void Step()=0;

	/** @brief 描画を実行
	 */
	virtual void Draw() = 0;

	/** @brief 描画領域のサイズを設定する
		@param w 描画領域の横幅
		@param h 描画領域の縦幅
	 */
	virtual void Reshape(int w, int h)=0;

	static void SPR_CDECL RegisterSdk();
};

}

#endif
