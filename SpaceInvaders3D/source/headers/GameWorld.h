#pragma once
#include <vector>
#include "Includes.h"
#include "Model.h"

class GameWorld
{
public:
	double deltaTimeCounter;
	bool enemyShootPermission = false;
	//player details
	int playerScore = 0, playerLives = 3, playerMoveSpeed = 0;
	//alien details
	
	float alienXOffset = 0.0f, alienYOffset = 0.0f, originalAlienXOffset, alienMoveSpeed = 2;
	bool isPlayerBullet = false, isEnemyBullet = false, right = true;
	Model player, background, foreground;
	float startPosX = -8.5f, startPosY = 6.5f, Xgap = 1.0f, Ygap = -1.0f;
	
	std::vector<Model> borders;
	std::vector<Model> barricades;
	std::vector<Model> enemies;
	std::vector<Model> EnemyBullets;
	std::vector<Model> Bullets;
	std::vector<Model> lifeIcons;
	std::vector<Model> scoreDigitsOutput;
	std::vector<Model> scoreDigitsItemList;
	
	GameWorld();
	void Clear();
	void EnemyShoot(GLfloat x, GLfloat y);
	void Shoot(Model gameObject);
	~GameWorld();

private:

};

