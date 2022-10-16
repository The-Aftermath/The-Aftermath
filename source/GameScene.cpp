#include "GameScene.h"
#define CGLTF_IMPLEMENTATION
#include "cgltf.h"

GameScene::GameScene(nvrhi::DeviceHandle device) {
	mDevice = device;
}

GameScene::~GameScene() {

}

void GameScene::loadStaticModel(const char* path) {
	cgltf_options options{};
	cgltf_data* data = nullptr;
	cgltf_result result = cgltf_parse_file(&options, path, &data);
	
	cgltf_free(data);
}