#include "Enemy.h"
#include <ModelManager.h>
#include <Tools/AssetGenerator/engine/math/LoadResourceID.h>

using namespace LoadResourceID;

Enemy::~Enemy() {}

void Enemy::Initialize() {
	ModelManager::GetInstance()->LoadModel(model::Enemy);
	transform_ = { { 1.0f, 1.0f, 1.0f }, { 0.0f, 0.0f, 0.0f },{0.0f,20.0f,200.0f} };
	object = Object3d::Create(model::Enemy, transform_);
}

void Enemy::Update() {
	object->Update();
}

void Enemy::Draw() {
	object->Draw();
}