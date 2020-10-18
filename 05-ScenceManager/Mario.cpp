#include <algorithm>
#include <assert.h>
#include <iostream>
#include <fstream>
#include "Utils.h"

#include "Mario.h"
#include "Game.h"

#include "Goomba.h"
#include "Brick.h"
#include "Portal.h"
#include "Sprites.h"

CMario::CMario(float x, float y) : CGameObject()
{
	level = MARIO_LEVEL_BIG;
	untouchable = 0;
	SetState(MARIO_STATE_IDLE);
	maxSpeed = 0.05f;
	start_x = x;
	start_y = y;
	this->x = x;
	this->y = y;
	IsWalkingR = IsWalkingL = IsJumping = IsSlowDown = false;
}

void CMario::Update(DWORD dt, vector<LPGAMEOBJECT> *coObjects)
{
	DWORD now = GetTickCount64();
	if (nx != 0 && IsSlowDown) {
		DebugOut(L"Slow down\n");
		vx += nx * -1 * 0.0011f;
		if ((nx == 1 && vx <= 0) || (nx == -1 && vx >= 0)) {
			vx = 0;
			IsSlowDown = false;
		}
	}
	if (IsWalkingR && vx < 0.08f)
		vx += 0.003f;
	if (IsWalkingL && vx > -0.08f)
		vx -= 0.003f;
	if (IsRollback && now - RollBackTime >= 190)
		IsRollback = false;
	if (IsRollback) {
		vx += -nx * 0.0008f;
	}
	

	CGameObject::Update(dt);
	//DebugOut(L"%f\n", vx);
	vy += MARIO_GRAVITY * dt;

	vector<LPCOLLISIONEVENT> coEvents;
	vector<LPCOLLISIONEVENT> coEventsResult;

	coEvents.clear();

	// turn off collision when die 
	if (state != MARIO_STATE_DIE)
		CalcPotentialCollisions(coObjects, coEvents);

	// reset untouchable timer if untouchable time has passed
	if (GetTickCount() - untouchable_start > MARIO_UNTOUCHABLE_TIME)
	{
		untouchable_start = 0;
		untouchable = 0;
	}

	// No collision occured, proceed normally
	if (coEvents.size() == 0)
	{
		x += dx;
		y += dy;
	}
	else
	{
		float min_tx, min_ty, nx = 0, ny;
		float rdx = 0;
		float rdy = 0;

		// TODO: This is a very ugly designed function!!!!
		FilterCollision(coEvents, coEventsResult, min_tx, min_ty, nx, ny, rdx, rdy);

		// how to push back Mario if collides with a moving objects, what if Mario is pushed this way into another object?
		//if (rdx != 0 && rdx!=dx)
		//	x += nx*abs(rdx); 

		// block every object first!
		x += min_tx * dx + nx * 0.4f;
		y += min_ty * dy + ny * 0.4f;


		if (ny != 0) vy = 0;


		//
		// Collision logic with other objects
		//
		for (UINT i = 0; i < coEventsResult.size(); i++)
		{
			LPCOLLISIONEVENT e = coEventsResult[i];

			if (dynamic_cast<CGoomba *>(e->obj)) // if e->obj is Goomba 
			{
				CGoomba *goomba = dynamic_cast<CGoomba *>(e->obj);

				// jump on top >> kill Goomba and deflect a bit 
				if (e->ny < 0)
				{
					if (goomba->GetState() != GOOMBA_STATE_DIE)
					{
						goomba->SetState(GOOMBA_STATE_DIE);
						vy = -MARIO_JUMP_DEFLECT_SPEED;
					}
				}
				else if (e->nx != 0)
				{
					if (untouchable == 0)
					{
						if (goomba->GetState() != GOOMBA_STATE_DIE)
						{
							if (level > MARIO_LEVEL_SMALL)
							{
								level = MARIO_LEVEL_SMALL;
								StartUntouchable();
							}
							else
								SetState(MARIO_STATE_DIE);
						}
					}
				}
			}
			if (dynamic_cast<CBrick *>(e->obj)) // if e->obj is Goomba 
			{
				CBrick *goomba = dynamic_cast<CBrick *>(e->obj);
				if (e->ny < 0)
				{
					IsJumping = false;
				}
			}


			else if (dynamic_cast<CPortal *>(e->obj))
			{
				CPortal *p = dynamic_cast<CPortal *>(e->obj);
				CGame::GetInstance()->SwitchScene(p->GetSceneId());
			}
		}
	}

	for (UINT i = 0; i < coEvents.size(); i++) delete coEvents[i];
	DebugOut(L"2: %f\n", vx);
}

void CMario::Render()
{
	int ani = -1;
	if (state == MARIO_STATE_DIE)
		ani = MARIO_ANI_DIE;
	else
		if (level == MARIO_LEVEL_BIG)
		{
			if (vx == 0)
			{
				if (nx > 0) {
					ani = MARIO_ANI_BIG_IDLE_RIGHT;
					if (vy < 0)
					{
						ani = MARIO_ANI_BIG_JUMP_RIGHT;
					}
				}
				else {

					ani = MARIO_ANI_BIG_IDLE_LEFT;
					if (vy < 0) {
						ani = MARIO_ANI_BIG_JUMP_LEFT;
					}
				}
			}
			else if (vx > 0)
			{

				if (vy < 0)
					ani = MARIO_ANI_BIG_JUMP_RIGHT;
				else {
					ani = MARIO_ANI_BIG_WALKING_RIGHT;
				}
				if (IsRollback == true)
				{
					DebugOut(L"ROLLBACK\n");
					ani = MARIO_ANI_BIG_WALKLEFT_TURNRIGHT;
				}
				//ani = MARIO_ANI_BIG_WALKING_RIGHT;
			}

			else {
				if (vy < 0)
					ani = MARIO_ANI_BIG_JUMP_LEFT;
				else
					ani = MARIO_ANI_BIG_WALKING_LEFT;
				if (IsRollback == true)
				{
					DebugOut(L"ROLLBACK\n");
					ani = MARIO_ANI_BIG_WALKRIGHT_TURNLEFT;
				}
			}
		}
		else if (level == MARIO_LEVEL_SMALL)
		{
			if (vx == 0)
			{
				if (nx > 0) {
					ani = MARIO_ANI_SMALL_IDLE_RIGHT;
					if (vy < 0)
					{
						ani = MARIO_ANI_SMALL_JUMP_RIGHT;
					}
				}
				else {
					ani = MARIO_ANI_SMALL_IDLE_LEFT;
					if (vy < 0) {
						ani = MARIO_ANI_SMALL_JUMP_LEFT;
					}
				}
			}
			else if (vx > 0)
			{
				if (vy < 0)
					ani = MARIO_ANI_SMALL_JUMP_RIGHT;
				else {
					ani = MARIO_ANI_SMALL_WALKING_RIGHT;
				}
				//ani = MARIO_ANI_BIG_WALKING_RIGHT;
			}
			else {
				if (vy < 0)
					ani = MARIO_ANI_SMALL_JUMP_LEFT;
				else {
					ani = MARIO_ANI_SMALL_WALKING_LEFT;
				}
			}
			if (state == MARIO_STATE_TURN)
			{
				if (vx > 0 && nxbackup == 1)
				{
					ani = MARIO_ANI_SMALL_WALKLEFT_TURNRIGHT;
					//nx = -1;
				}
				else if (vx < 0 && nxbackup == -1) {
					ani = MARIO_ANI_SMALL_WALKRIGHT_TURNLEFT;
					//nx = 1;
				}
			}
		}
		else if (level == MARIO_LEVEL_TAIL)
		{
			if (vx == 0)
			{
				if (nx > 0) {

					ani = MARIOTAIL_ANI_IDLE_RIGHT;
					if (vy < 0)
					{
						ani = MARIOTAIL_ANI_JUMP_RIGHT;
					}
				}
				else {

					ani = MARIOTAIL_ANI_IDLE_LEFT;
					if (vy < 0) {
						ani = MARIOTAIL_ANI_JUMP_LEFT;
					}
				}
			}
			else if (vx > 0)
			{
				if (vy < 0)
					ani = MARIOTAIL_ANI_JUMP_RIGHT;
				else {
					ani = MARIOTAIL_ANI_WALK_RIGHT;
				}
				//ani = MARIO_ANI_BIG_WALKING_RIGHT;
			}

			else {
				if (vy < 0)
					ani = MARIOTAIL_ANI_JUMP_LEFT;
				else
					ani = MARIOTAIL_ANI_WALK_LEFT;
			}
		}
	if (IsRollback == true && nx > 0 && vx >0)
	{
		DebugOut(L"ROLLBACK\n");
		ani = MARIO_ANI_BIG_WALKRIGHT_TURNLEFT;
	}
	int alpha = 255;
	if (untouchable) alpha = 128;
	animation_set->at(ani)->Render(x, y, alpha);

	RenderBoundingBox();
}


void CMario::SetState(int state)
{
	CGameObject::SetState(state);
	switch (state)
	{
	case MARIO_STATE_WALKING_RIGHT:
		if (!IsWalkingL) {
			if (nx == -1 && vx < 0 && !IsRollback) {
				IsSlowDown = false;
				IsRollback = true;
				vx = -0.01f;
				RollBackTime = GetTickCount64();
			}
			if (!IsRollback)
			{
				if (!IsWalkingR) {
					IsWalkingR = true;
					vx = MARIO_WALKING_SPEED;
					nx = 1;
				}
			}
		}
		break;
	case MARIO_STATE_WALKING_LEFT:
		if (!IsWalkingR) {
			if (nx == 1 && vx > 0 && !IsRollback) {
				IsSlowDown = false;
				IsRollback = true;
				vx = 0.01f;
				RollBackTime = GetTickCount64();
			}
			if (!IsRollback)
			{
				if (!IsWalkingL) {
					IsWalkingL = true;
					vx = -MARIO_WALKING_SPEED;
					nx = -1;
				}
			}
		}
		break;
	case MARIO_STATE_RUN_RIGHT:
		DebugOut(L"Run right\n");
		vx = MARIO_WALKING_SPEED * 2;
		nx = 1;
		break;
	case MARIO_STATE_RUN_LEFT:
		vx = -MARIO_WALKING_SPEED * 2;
		nx = -1;
		break;
	case MARIO_STATE_JUMP:
		IsJumping = true;
		vy = -MARIO_JUMP_SPEED_Y;
		break;
	case MARIO_STATE_IDLE:
		//DebugOut(L"Stop\n");
		break;
	case MARIO_STATE_DIE:
		vy = -MARIO_DIE_DEFLECT_SPEED;
		break;
	case MARIO_STATE_TURN:
		break;
	case MARIO_STATE_SPEEDDOWNRIGHT:
		if (!IsRollback) {
			IsSlowDown = true;
			IsWalkingR = false;
			DebugOut(L"Slow R\n");
			nx = 1;
		}
		else {
			if (IsRollback && vx < 0.01)
			{
				IsRollback = false;
				IsSlowDown = true;
				nx = 1;
			}
		}
		break;
	case MARIO_STATE_SPEEDDOWNLEFT:
		if (!IsRollback) {
			IsSlowDown = true;
			IsWalkingL = false;
			DebugOut(L"Slow L\n");
			nx = -1;
		}else
		if (IsRollback && vx > -0.01f)
		{
			IsRollback = false;
			IsSlowDown = true;
			nx = -1;
		}
		break;

	}
}

void CMario::GetBoundingBox(float &left, float &top, float &right, float &bottom)
{
	left = x;
	top = y;

	if (level == MARIO_LEVEL_BIG || level == MARIO_LEVEL_TAIL)
	{
		right = x + MARIO_BIG_BBOX_WIDTH;
		bottom = y + MARIO_BIG_BBOX_HEIGHT;
	}
	else
	{
		right = x + MARIO_SMALL_BBOX_WIDTH;
		bottom = y + MARIO_SMALL_BBOX_HEIGHT;
	}
}

/*
	Reset Mario status to the beginning state of a scene
*/
void CMario::Reset()
{
	SetState(MARIO_STATE_IDLE);
	SetLevel(MARIO_LEVEL_BIG);
	SetPosition(start_x, start_y);
	SetSpeed(0, 0);
}

