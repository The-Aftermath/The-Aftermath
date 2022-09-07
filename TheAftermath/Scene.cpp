#pragma once
#include "Scene.h"

#include <vector>

namespace TheAftermath {

	struct AScene : public Scene {
		AScene(SceneDesc* pDecs) {
			pDevice = pDecs->pDevice;
		}
		void Release() { delete this; }

		void Update() {

		}
		void LoadModel(const wchar_t* modelFilePath) {

		}

		Device* pDevice;
	};

	Scene* CreateScene(SceneDesc* pDecs) {
		return new AScene(pDecs);
	}
}
