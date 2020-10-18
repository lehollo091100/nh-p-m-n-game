#pragma once
#include "GameObject.h"
#include "Utils.h"

#define MARIO_WALKING_SPEED		0.01f 
#define MARIO_AX				0.04f
#define MARIO_JUMP_SPEED_Y		0.5f
#define MARIO_JUMP_DEFLECT_SPEED 0.2f
#define MARIO_GRAVITY			0.0015f
#define MARIO_DIE_DEFLECT_SPEED	 0.5f

#define MARIO_STATE_IDLE			0
#define MARIO_STATE_WALKING_RIGHT	100
#define MARIO_STATE_WALKING_LEFT	200
#define MARIO_STATE_JUMP			300
#define MARIO_STATE_DIE				400
#define MARIO_STATE_RUN_RIGHT       500
#define MARIO_STATE_RUN_LEFT        600
//#define MARIO_STATE_TAIL			700
#define MARIO_STATE_ATTACK_TAIL		800
#define MARIO_STATE_SIT				900
#define MARIO_STATE_FLY				1000
#define MARIO_STATE_TURN			700
#define MARIO_STATE_SPEEDDOWNRIGHT		1100
#define MARIO_STATE_SPEEDDOWNLEFT		1200


#define MARIO_ANI_BIG_IDLE_RIGHT		0
#define MARIO_ANI_BIG_IDLE_LEFT			1
#define MARIO_ANI_SMALL_IDLE_RIGHT		2
#define MARIO_ANI_SMALL_IDLE_LEFT			3

#define MARIO_ANI_BIG_WALKING_RIGHT			4
#define MARIO_ANI_BIG_WALKING_LEFT			5
#define MARIO_ANI_SMALL_WALKING_RIGHT		6
#define MARIO_ANI_SMALL_WALKING_LEFT		7

#define MARIO_ANI_SMALL_JUMP_RIGHT		8
#define MARIO_ANI_SMALL_JUMP_LEFT		9
#define MARIO_ANI_BIG_JUMP_RIGHT		10
#define MARIO_ANI_BIG_JUMP_LEFT		11//

#define MARIOTAIL_ANI_IDLE_RIGHT	12
#define MARIOTAIL_ANI_IDLE_LEFT		13

#define MARIOTAIL_ANI_WALK_RIGHT	14
#define MARIOTAIL_ANI_WALK_LEFT		15

#define MARIOTAIL_ANI_JUMP_RIGHT	16
#define MARIOTAIL_ANI_JUMP_LEFT		17

#define MARIO_ANI_BIG_WALKRIGHT_TURNLEFT	18
#define MARIO_ANI_BIG_WALKLEFT_TURNRIGHT	19
#define MARIO_ANI_SMALL_WALKRIGHT_TURNLEFT	20
#define MARIO_ANI_SMALL_WALKLEFT_TURNRIGHT	21

#define MARIO_ANI_DIE				22

#define	MARIO_LEVEL_SMALL	1
#define	MARIO_LEVEL_BIG		2
#define MARIO_LEVEL_TAIL	3

#define MARIO_BIG_BBOX_WIDTH  15
#define MARIO_BIG_BBOX_HEIGHT 27

#define MARIO_SMALL_BBOX_WIDTH  13
#define MARIO_SMALL_BBOX_HEIGHT 15

#define MARIO_UNTOUCHABLE_TIME 5000


class CMario : public CGameObject
{
	int level;
	int untouchable;
	DWORD untouchable_start;
	float start_x;			// initial position of Mario at scene
	float start_y;
	
public:
	bool IsWalkingR, IsSpeedup, IsWalkingL, IsSitting, IsRollback, IsSlowDown;
	bool IsJumping;
	int nxbackup = 1;
	float maxSpeed;
	DWORD RollbackDt;
	DWORD RollBackTime;
	DWORD rightdown,leftdown;


public:
	CMario(float x = 0.0f, float y = 0.0f);
	virtual void Update(DWORD dt, vector<LPGAMEOBJECT> *colliable_objects = NULL);
	virtual void Render();

	void SetState(int state);
	void SetLevel(int l) { level = l; }
	void StartUntouchable() { untouchable = 1; untouchable_start = GetTickCount(); }

	void Reset();

	virtual void GetBoundingBox(float &left, float &top, float &right, float &bottom);
};