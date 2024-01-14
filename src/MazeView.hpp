#pragma once

#pragma warning (push)
#pragma warning (disable: 4245)
#pragma warning (disable: 4201)
#ifndef OLC_PGE_APPLICATION
#define OLC_PGE_APPLICATION
#endif
#include "olcPixelGameEngine.h"
#pragma warning (pop)

#include "MazeModel.hpp"
#include <unordered_map>
#include <condition_variable>
#include <mutex>
#include <thread>
#include <chrono>
#include <semaphore>

static const std::unordered_map<CellType, olc::Pixel> CELL_COLOUR_MAP = {
	{CellType::LEFT    , olc::YELLOW},
	{CellType::DOWN    , olc::YELLOW},
	{CellType::RIGHT   , olc::YELLOW},
	{CellType::UP      , olc::YELLOW},

	{CellType::BLOCK   , olc::DARK_BLUE},
	{CellType::EMPTY   , olc::DARK_GREY},

	{CellType::START   , olc::YELLOW},
	{CellType::MAXDIST , olc::BLACK},
	{CellType::EXIT    , olc::YELLOW},
};

class MazeView
	: public olc::PixelGameEngine
{
private:
	using Super = olc::PixelGameEngine;

	Grid m_grid;

	// the time between frames
	std::chrono::nanoseconds m_delay{ 1000 };

	// controls the redrawing of the maze
	std::counting_semaphore<1> m_redrawMazeSem{ 0 };
	std::counting_semaphore<1> m_continueConstructingMazeSem{ 0 };

public:
	MazeView(const int rows, const int cols)
		: m_grid(rows, cols)
	{
		sAppName = "Maze";
	}

	void SetDelay(std::chrono::nanoseconds deltaTime)
	{
		m_delay = deltaTime;
	}

	Grid& GetGrid()
	{
		return m_grid;
	}

	void TriggerDraw()
	{
		m_redrawMazeSem.release();
		std::this_thread::sleep_for(m_delay);
		m_continueConstructingMazeSem.acquire();
	}

	bool OnUserUpdate(float) override
	{
		// wait for TriggerDraw to be called by the Creator thread
		m_redrawMazeSem.acquire();

		Super::Clear(olc::BLACK);

		// Draw entire matrix
		for (int i = 1; i < m_grid.rows - 1; i++)
		{
			for (int j = 1; j < m_grid.cols - 1; j++)
			{
				auto it = CELL_COLOUR_MAP.find(m_grid.Get({ i, j }));
				// if it's a distance => colour it as empty
				if (it == CELL_COLOUR_MAP.end())
					it = CELL_COLOUR_MAP.find(CellType::EMPTY);

				// Draws a "pixel" scaled by cellSize 
				Super::Draw({ i , j }, it->second);
			}
		}

		m_continueConstructingMazeSem.release();

		return true;
	}

	// mandatory to override
	bool OnUserCreate() override
	{
		return true;
	}

	// mandatory to override
	bool OnUserDestroy() override
	{
		return false;
	}
};
