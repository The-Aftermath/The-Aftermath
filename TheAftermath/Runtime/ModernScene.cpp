#include "Scene.h"
namespace TheAftermath {
	class AModernScene : public Scene {
	public:
		AModernScene(SceneDesc* pDesc) {
			pDevice = pDesc->pDevice;
			GBufferDesc gbufferDesc;
			gbufferDesc.pDevice = pDevice;
			gbufferDesc.mWidth = pDevice->GetViewportWidth();
			gbufferDesc.mHeight = pDevice->GetViewportHeight();
			pGbuffer = CreateGBuffer(&gbufferDesc);
		}
		~AModernScene() {
			RemoveGBuffer(pGbuffer);
		}

		void Update() {}


		GraphicsDevice* pDevice;
		GBuffer* pGbuffer;
	};

	Scene* CreateScene(SceneDesc* pDesc) {
       return new AModernScene(pDesc);
    }
    void RemoveScene(Scene* pScene) {
		auto temp_ptr = dynamic_cast<AModernScene*>(pScene);
		delete temp_ptr;
    }
}