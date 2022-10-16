#pragma once

class GameScene {

public:
	GameScene();
	~GameScene();

	GameScene(const GameScene&) = delete;
	GameScene& operator=(const GameScene&) = delete;
	GameScene(GameScene&&) noexcept = default;
	GameScene& operator=(GameScene&&) noexcept = default;

	void loadStaticModel(const char* path);
};