#include "Block.hpp"
#include <random>
#include <ctime>
#include <cstdlib>

Block::Block(int type, int _x, int _y, SDL_Helper * _helper, char * _sprite, int _sizePerFrameX, int _sizePerFrameY, Settings * _settings) : Sprite(_x, _y, _helper, _sprite, 1, 1, _sizePerFrameX, _sizePerFrameY, 0, 0, true, false, 1, false, false)
{
	int _score = 0;

	switch (type)
	{
	case 0:
		_score = 100;
		this->m_currentFrameX = 0;
		break;
	case 1:
		_score = 200;
		this->m_currentFrameX = 1;
		break;
	case 2:
		_score = 300;
		this->m_currentFrameX = 2;
		break;
	case 3:
		_score = 500;
		this->m_currentFrameX = 3;
		break;
	case 4:
		this->m_currentFrameX = 4;
		_score = 50;
		break;
	default:
		this->m_currentFrameX = 0;
		_score = 10;
		break;
	}

	this->m_score = _score;
}

int Block::GetScore()
{
	return this->m_score;
}