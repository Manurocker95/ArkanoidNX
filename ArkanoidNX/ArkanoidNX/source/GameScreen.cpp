/* This file is part of Manurocker95's Template!

this is made for my tutorial: https://gbatemp.net/threads/tutorial-setting-up-visual-studio-2017-environment-for-nintendo-switch-homebrew-development.525977/#post-8439059

Copyright (C) 2018/2019 Manuel Rodríguez Matesanz
>    This program is free software: you can redistribute it and/or modify
>    it under the terms of the GNU General Public License as published by
>    the Free Software Foundation, either version 3 of the License, or
>    (at your option) any later version.
>
>    This program is distributed in the hope that it will be useful,
>    but WITHOUT ANY WARRANTY; without even the implied warranty of
>    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
>    GNU General Public License for more details.
>
>    You should have received a copy of the GNU General Public License
>    along with this program.  If not, see <http://www.gnu.org/licenses/>.
>    See LICENSE for information.
*/

#include "GameScreen.hpp"
#include "SceneManager.hpp"
#include "Filepaths.h"
#include "Colors.h"
#include <string>
#include <random>
#include <ctime>
#include <cstdlib>

GameScreen::GameScreen(Settings * _settings) : Scene(_settings)
{
	this->m_settings = _settings;
	this->m_changeScene = false;
	this->m_score = 0;
	this->m_paddleDisplacement = _settings->m_paddleDisplacement; 
	this->m_dx = 6;
	this->m_dy = 5;
	this->m_blocksDestroyed = 0;
	srand(time(NULL));
	
}

GameScreen::~GameScreen()
{
	this->m_music->End(this->m_helper);
	delete(this->m_music);

	this->m_buttonTapSFX->End(this->m_helper);
	delete(this->m_buttonTapSFX);

	this->m_background->End(this->m_helper);
	delete(this->m_background);

	this->m_text->End(this->m_helper);
	delete(this->m_text);	
	
	this->m_paddle->End(this->m_helper);
	delete(this->m_paddle);	
	
	this->m_ball->End(this->m_helper);
	delete(this->m_ball);

	for (auto & block : m_blocks)
	{
		block->End(this->m_helper);
		delete(block);
	}
	m_blocks.clear();

}

void GameScreen::Start(SDL_Helper * helper)
{

	this->m_helper = helper;
	this->m_background = new Sprite(0, 0, helper, IMG_BACKGROUND, 1, 1, SWITCH_SCREEN_WIDTH, SWITCH_SCREEN_HEIGHT, 0, 0, false, false, 1);

	this->m_text = new Text(helper, std::string(SceneManager::Instance()->GetText("scoreText")) + std::to_string(this->m_score), 525, 670, 45, true, FONT_ARKANOID_2, C_BLACK);
	
	int _x = 10;
	int type = 0;

	for (int i = 0; i < this->m_settings->m_yBlocks; i++)
	{
		_x = 10;
		for (int j = 0; j < this->m_settings->m_xBlocks; j++)
		{
			type = rand() % 6 + 1;

			this->m_blocks.push_back(new Block(type, _x, i * 30, helper, IMG_BLOCKS, 63, 30, this->m_settings));
			_x += 63;
		}
	}
	this->m_paddle = new Sprite(450, 550, helper, IMG_PADDLE, 1, 1, 135, 14, 0, 0, false, false, 1);
	this->m_ball = new Sprite(468, 512, helper, IMG_BALL, 1, 1, 18, 18, 0, 0, false, false, 1);
	this->m_buttonTapSFX = new SfxSound(this->m_helper, SND_SFX_TAP, false, 2);
	this->m_music = new MusicSound(this->m_helper, SND_BGM_GAME, true, 1);
	this->m_music->Play(this->m_helper);
	
	if (this->m_settings->m_muted)
		this->m_helper->SDL_PauseMusic();

}

void GameScreen::Draw()
{
	this->m_background->Draw(this->m_helper);

	for (auto & block : m_blocks)
	{
		block->Draw(this->m_helper);
	}

	this->m_paddle->Draw(this->m_helper);
	this->m_ball->Draw(this->m_helper);

	this->m_text->Draw(this->m_helper);
}

void GameScreen::Update()
{
	if (this->m_ball->GetX() < 0 || this->m_ball->GetX() > SWITCH_SCREEN_WIDTH)
	{
		this->m_dx = -this->m_dx;

		if (!this->m_muted)
			this->m_buttonTapSFX->Play(this->m_helper);
	}

	if (this->m_ball->GetY() < 0)
	{
		this->m_dy = -this->m_dy;

		if (!this->m_muted)
			this->m_buttonTapSFX->Play(this->m_helper);
	}
	else if (this->m_ball->GetY() > SWITCH_SCREEN_HEIGHT)
	{
		NextScene();
		return;
	}

	this->m_paddle->Update();

	if (this->m_paddle->CheckOverlap(m_ball))
	{
		this->m_dy = -(rand() % 5 + 2);

		if (!this->m_muted)
			this->m_buttonTapSFX->Play(this->m_helper);
	}

	for (auto & block : m_blocks)
	{
		if (block->IsActive() && block->CheckOverlap(this->m_ball))
		{
			++this->m_blocksDestroyed;
			this->m_score += block->GetScore();
			this->m_text->SetText(std::string(SceneManager::Instance()->GetText("scoreText")) + std::to_string(this->m_score));
			block->SetActive(false);
			this->m_dx = -this->m_dx;
			this->m_dy = -this->m_dy;

			if (!this->m_muted)
				this->m_buttonTapSFX->Play(this->m_helper);

			if (this->m_blocksDestroyed >= this->m_blocks.size())
				this->m_changeScene = true;

			break;
		}
	}

	this->m_ball->MoveX(this->m_dx);
	this->m_ball->MoveY(this->m_dy);

	if (this->m_changeScene)
		NextScene();
}

void GameScreen::CheckInputs(u64 kDown, u64 kHeld, u64 kUp)
{
	if (kHeld & KEY_TOUCH)
	{
		u32 i;
		hidTouchRead(&this->touch, i);

		this->m_paddle->SetX(this->touch.px - 67);
	}
	else if (kUp & KEY_TOUCH)
	{
		u32 i;
		hidTouchRead(&this->touch, i);


	}

	if (kHeld & KEY_RIGHT)
	{
		this->m_paddle->MoveX(this->m_paddleDisplacement);
	}
	else if (kHeld & KEY_LEFT)
	{
		this->m_paddle->MoveX(-this->m_paddleDisplacement);
	}

	if (kDown & KEY_PLUS)
	{
		m_changeScene = true;
	}
}

// * We go to the next scene = GameScreen
void GameScreen::NextScene()
{
	SceneManager::Instance()->SaveData(this->m_score);
	SceneManager::Instance()->LoadScene(SceneManager::TITLE);
}

