#include "GameScene.h"
#define CGLTF_IMPLEMENTATION
#include "cgltf.h"

GameScene::GameScene(UINT w, UINT h) {
	m = DirectX::XMFLOAT4X4(
		1.f, 0.f, 0.f, 0.f,
		0.f, 1.f, 0.f, 0.f,
		0.f, 0.f, 1.f, 0.f,
		0.f, 0.f, 0.f, 1.f
	);

	pos.x = 0.f;
	pos.y = 0.f;
	pos.z = 0.f;

	focus.x = 0.f;
	focus.y = 0.f;
	focus.z = 1.f;

	up.x = 0.f;
	up.y = 1.f;
	up.z = 0.f;

	auto _pos = DirectX::XMLoadFloat3(&pos);
	auto _focus = DirectX::XMLoadFloat3(&focus);
	auto _up = DirectX::XMLoadFloat3(&up);
	auto _v = DirectX::XMMatrixLookAtLH(
		_pos,
		_focus,
		_up
	);
	
	DirectX::XMStoreFloat4x4(&v, _v);
	auto _p = DirectX::XMMatrixPerspectiveFovLH(DirectX::XM_PIDIV4, (float)w / (float)h, 0.1f, 1000.f);
	DirectX::XMStoreFloat4x4(&p, _p);
}

GameScene::~GameScene() {

}

void GameScene::loadStaticModel(const char* path) {
	cgltf_options options{};
	cgltf_data* data = nullptr;
	cgltf_result result = cgltf_parse_file(&options, path, &data);


	
	cgltf_free(data);
}