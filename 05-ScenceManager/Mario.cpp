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
	type = GType::MARIO;
	level = MARIO_LEVEL_BIG;
	IsWalking = false;
	untouchable = 0;
	SetState(MARIO_STATE_IDLE);
	start_x = x;
	start_y = y;
	this->x = x;
	this->y = y;
	IsWalkingR = IsWalkingL = IsJumping = IsSlowDown = IsMaxspeed = false;
}

void CMario::Update(DWORD dt, vector<LPGAMEOBJECT> *coObjects)
{
	// Calculate dx, dy 
	CGameObject::Update(dt);

	// Simple fall down
	vy += MARIO_GRAVITY * dt;

	vector<LPCOLLISIONEVENT> coEvents;
	vector<LPCOLLISIONEVENT> coEventsResult;

	coEvents.clear();

	// turn off collision when die 
	if (state != MARIO_STATE_DIE)
		CalcPotentialCollisions(coObjects, coEvents);
	if (IsWalking) {
		if (vx == 0) {
			vx = nx * MARIO_WALKING_SPEED;
			state = MARIO_STATE_WALKING_RIGHT;
		}
		/*else
			if (vx > 0)
			{
				if (nx > 0)
				{
					vx += MARIO_ACCELERATE;
					if (vx >= MARIO_WALKING_MAXSPEED)
						vx = MARIO_WALKING_MAXSPEED;
				}
				else if (nx < 0)
				{
					vx -= MARIO_ACCELERATE;
					if (vx <= 0)
					{
						vx = nx * MARIO_WALKING_SPEED;
					}
				}
				
			}
			else {
				if (nx < 0)
				{
					vx -= MARIO_ACCELERATE;
					if (vx <= -MARIO_WALKING_MAXSPEED)
						vx = -MARIO_WALKING_MAXSPEED;
				}
				else if (nx > 0)
				{
					vx += MARIO_ACCELERATE;
					if (vx >= 0)
						vx = 0;	
				}
			}*/
	}







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
		FilterCollision(coEvents, coEventsResult, min_tx, min_ty, nx, ny, rdx, rdy);
		x += min_tx * dx + nx * 0.4f;
		y += min_ty * dy + ny * 0.4f;

		//if (nx != 0) vx = 0;
		if (ny != 0) vy = 0;
		// Collision logic with other objects
		for (UINT i = 0; i < coEventsResult.size(); i++)
		{
			LPCOLLISIONEVENT e = coEventsResult[i];
			if (e->obj->GetType() == GType::BRICK)
			{
				//DebugOut(L"onground\n");
			}

		}
	}

	for (UINT i = 0; i < coEvents.size(); i++) delete coEvents[i];
}

void CMario::Render()
{
	ani = MARIO_ANI_BIG_IDLE_RIGHT;
	if (state == MARIO_STATE_WALKING_RIGHT)
		ani = MARIO_ANI_BIG_WALKING_RIGHT;
	//DebugOut(L"%f\n", x);
	int alpha = 255;
	if (untouchable) alpha = 128;
	if (nx == 1)
		DebugOut(L"xM %f\n", x);
	animation_set->at(ani)->Render(x, y, alpha, nx);
}

void CMario::DecreaseSpeed(float speedDown)
{
}
void CMario::SetState(int state)
{
	CGameObject::SetState(state);
	switch (state)
	{
	case MARIO_STATE_WALKING_RIGHT:
		IsWalking = true;
	default:
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

void CMario::Go()
{
}
void CMario::Left()
{
}

void CMario::Right()
{
}

void CMario::Jump()
{
}
void CMario::Idle() {
}
void CMario::Sit()
{
}
void CMario::Rollback()
{
}