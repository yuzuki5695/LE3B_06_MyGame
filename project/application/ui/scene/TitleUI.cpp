#include "TitleUI.h"
#include <TextureManager.h>
//#include <Input.h>
//#include <CameraManager.h>
//#include <Easing.h>
//#ifdef min
//#undef min
//#endif
//#ifdef max
//#undef max
//#endif
// AssetGeneratorからインクルード
#include <subproject/AssetGenerator/engine/generator/LoadResourceID.h>

using namespace MyEngine;
using namespace AssetGen::LoadResourceID::Textures;

namespace MyGame {
	void TitleUI::Initialize() {

		const std::vector<std::string> textures = {
		Ui::titile_01,
		Ui::titile_02,
		Ui::titile_03,
		Ui::titile_04,
		Ui::titile_05,
		Ui::titile_06,
		Ui::titile_07,
		};
		for (const auto& t : textures) {
			TextureManager::GetInstance()->LoadTexture(t);
		}



	}

















}