#pragma once
#include "Scene.h"
#include "Vertex.h"
#include "json.hpp"
#include <vector>

namespace TheAftermath {

	struct AScene : public Scene {
		AScene(SceneDesc* pDecs) {
			pDevice = pDecs->pDevice;
		}
		void Release() { delete this; }

		void Update() {


			pDevice->Present();
		}
		void LoadModel(const wchar_t* modelFilePath) {

		}

		Device* pDevice;

		std::vector<Vertex> mAllVertex;
		std::vector<uint32_t> mAllIndex;

		std::vector<Vertex> mVisibleVertex;
		std::vector<uint32_t> mVisibleIndex;
	};

	Scene* CreateScene(SceneDesc* pDecs) {
		return new AScene(pDecs);
	}
}
