#include "GameWorld.h"
#include <iostream>

GameWorld::GameWorld()
{
}

void GameWorld::Clear() {
	this->enemies.clear();
	this->EnemyBullets.clear();
	this->Bullets.clear();
	this->enemyShootPermission = false;
	this->deltaTimeCounter = 0.0f;
}

//void GameWorld::EnemyShoot(GLfloat x, GLfloat y)
//{
//	Model* b = new Model(x, y, 0.025f);
//	b->texture = this->bulletTexture;
//	EnemyBullets.push_back(*b);
//	delete b;
//}
//
//void GameWorld::Shoot(Model gameObject)
//{
//	if(this->Bullets.size() <= 3){
//		Model* b = new Model(this->player.pos.x, this->player.pos.y, 0.025f, 0.12f);
//		b->texture = this->bulletTexture;
//		Bullets.push_back(*b);
//		delete b;
//	}
//}

GameWorld::~GameWorld()
{
}