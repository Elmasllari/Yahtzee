#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"

#include <set>
#include <numeric>
#include <stack>
#include <time.h>
#include <stdlib.h>  


class FiveDice : public olc::PixelGameEngine
{
public:
	FiveDice()
	{
		sAppName = "Yahtzee";
	}

	std::vector<std::pair<std::string, int>> vScores;
	std::vector<std::pair<std::string, int>> vScores_after;
	std::vector<uint8_t> vRolled;		// our 5 dice values
	std::vector<uint8_t> vSorted;		// our 5 dice values sorted
	bool bSelected_Dice[5] = { false };
	bool bWhat_to_score[13] = { false };
	bool bPoint[13] = { false };   // if we have scored
	bool bPoint_after[13] = { false };	
	bool bReleasedH = false;
	int index=0;    // how many scores we have done
	int total_points = 0;
	int key_pressed_enter = 0;
	int key_pressed_space = 0;
	

	

public:
	void DrawDie(const olc::vi2d& vPos,
		const uint8_t nFace,
		const olc::vi2d& vSize = { 64, 64 },
		const olc::Pixel colFace = olc::DARK_RED,
		const olc::Pixel colSpot = olc::WHITE)
	{
		// Draw Background
		FillRect(vPos, vSize, colFace);

		int32_t nColL = int32_t(double(vSize.x) * 0.25);
		int32_t nColM = int32_t(double(vSize.x) * 0.5);
		int32_t nColR = int32_t(double(vSize.x) * 0.75);
		int32_t nRowT = int32_t(double(vSize.y) * 0.25);
		int32_t nRowM = int32_t(double(vSize.y) * 0.5);
		int32_t nRowB = int32_t(double(vSize.y) * 0.75);
		int32_t nRad = int32_t(double(nColL) * 0.4);

		if (nFace & 1)
		{
			FillCircle(vPos + olc::vi2d(nColM, nRowM), nRad, colSpot);
		}

		if (nFace > 1)
		{
			FillCircle(vPos + olc::vi2d(nColR, nRowB), nRad, colSpot);
			FillCircle(vPos + olc::vi2d(nColL, nRowT), nRad, colSpot);
		}

		if (nFace > 3)
		{
			FillCircle(vPos + olc::vi2d(nColR, nRowT), nRad, colSpot);
			FillCircle(vPos + olc::vi2d(nColL, nRowB), nRad, colSpot);
		}

		if (nFace == 6)
		{
			FillCircle(vPos + olc::vi2d(nColL, nRowM), nRad, colSpot);
			FillCircle(vPos + olc::vi2d(nColR, nRowM), nRad, colSpot);
		}

	}

	void DrawGrid(const olc::vi2d& vPos,
		const olc::vi2d& vSize = { 64, 64 },
		const olc::Pixel colFace = olc::BLACK)
	{
		DrawRect(vPos, { vSize.x + 1 , vSize.y + 1 }, olc::BLACK);
		DrawRect(vPos, vSize, olc::BLACK);
		DrawRect(vPos, { vSize.x + 2  , vSize.y + 2 }, olc::BLACK);
		DrawRect(vPos, { vSize.x + 3  , vSize.y + 3 }, olc::BLACK);
	}


	bool OnUserCreate() override
	{
		srand(time(NULL));

		vRolled = { 1, 2, 3, 4, 5 };
		std::generate(vRolled.begin(), vRolled.end(), []() { return rand() % 6 + 1; });
		vSorted = vRolled;
	

		for (int i = 0; i < 13; i++) {
			vScores_after.emplace_back(std::make_pair("", 0));
		}


		bWhat_to_score[index] = true;    //first option is selected
		return true;
	}

	bool OnUserUpdate(float fElapsedTime) override
	{

		auto Match = [&vSorted = vSorted](const std::vector<std::string>& vPatterns) -> bool
		{
			bool bResult = false;

			for (const auto& sPattern : vPatterns)
			{

				bool bMatch = true;
				uint8_t n = 0;

				for (size_t idx = 0; idx < 5; idx++)
				{
					if (sPattern[idx] == 'n')
					{
						if (n == 0)
						{
							n = vSorted[idx];
						}
						else
						{
							bMatch &= (vSorted[idx] == n);
						}
					}
					else if (sPattern[idx] == '?')
					{
						bMatch &= true;
					}
					else // is Face Value
					{
						bMatch &= ((sPattern[idx] - '0') == vSorted[idx]);
					}

				}

				bResult |= bMatch;
			}
			return bResult;
		};

		if (GetKey(olc::Key::SPACE).bReleased)
		{
			if(key_pressed_space < 2)
			{
				key_pressed_space++;
				int temp[5];
				for (int i = 0; i < 5; i++) 	if (bSelected_Dice[i]) temp[i] = vRolled[i];

				std::generate(vRolled.begin(), vRolled.end(), []() { return rand() % 6 + 1; });

				for (int i = 0; i < 5; i++)		if (bSelected_Dice[i])  vRolled[i] = temp[i];
			
				vSorted = vRolled;
				std::sort(vSorted.begin(), vSorted.end());
			}
		}

		if (GetKey(olc::Key::NP1).bReleased || (GetKey(olc::Key::K1).bReleased))
		{
			bSelected_Dice[0] = !bSelected_Dice[0];

		}
		if (GetKey(olc::Key::NP2).bReleased || (GetKey(olc::Key::K2).bReleased))
		{
			bSelected_Dice[1] = !bSelected_Dice[1];
		}
		if (GetKey(olc::Key::NP3).bReleased || (GetKey(olc::Key::K3).bReleased))
		{
			bSelected_Dice[2] = !bSelected_Dice[2];
		}
		if (GetKey(olc::Key::NP4).bReleased || (GetKey(olc::Key::K4).bReleased))
		{
			bSelected_Dice[3] = !bSelected_Dice[3];
		}
		if (GetKey(olc::Key::NP5).bReleased || (GetKey(olc::Key::K5).bReleased))
		{
			bSelected_Dice[4] = !bSelected_Dice[4];
		}

		if (GetKey(olc::Key::ENTER).bReleased)
		{
			if (key_pressed_enter < 13)
			{
				key_pressed_space = 0;
				key_pressed_enter++;

				for (int i = 0; i < 13; i++)
				{
					if (bWhat_to_score[i])
					{
						
						if (!bPoint_after[i])		
						{
							vScores_after[i] = vScores[i];
							bPoint_after[i] = true;
						}

						std::generate(vRolled.begin(), vRolled.end(), []() { return rand() % 6 + 1; });
						vSorted = vRolled;
						std::sort(vSorted.begin(), vSorted.end());

						for (int i = 0; i < 5; i++) bSelected_Dice[i] = false;

						total_points += vScores[i].second;

						bPoint[i] = true;
						
						//select next option which is not scored
						bWhat_to_score[index] = false;	

						int while_loop_times = 0;
						while (bPoint[(index + 1) % 13] && while_loop_times < 13)
						{
							index++;
							index = index % 13;
							while_loop_times++;
						}
						index++;
						index = index % 13;
						bWhat_to_score[index] = true;
						break;
					}
				}
			}
		}

		if (GetKey(olc::Key::DOWN).bReleased)
		{
			bWhat_to_score[index] = false;
			   while(bPoint[(index+1) % 13])
			   {
					index++;
					index = index % 13;
			   }
		    index++;
			index = index % 13;
			bWhat_to_score[index] = true;

		
		}

		if (GetKey(olc::Key::UP).bReleased)
		{
			bWhat_to_score[index] = false;
			if (index == 0) index = 13;

			while (bPoint[(index - 1)  % 13])
			{
				index--;
				if (index < 0) index = 12;
				index = index % 13;
				
			}
			index--;
			if (index < 0) index = 12;
			index =index % 13;
			bWhat_to_score[index] = true;
		}

		if (GetKey(olc::Key::H).bReleased)  
		{
			bReleasedH = !bReleasedH;
		}



		 vScores =
		{
			{"Total Ones      : ", std::count(vRolled.begin(), vRolled.end(), 1) * 1},
			{"Total Twos      : ", std::count(vRolled.begin(), vRolled.end(), 2) * 2},
			{"Total Threes    : ", std::count(vRolled.begin(), vRolled.end(), 3) * 3},
			{"Total Fours     : ", std::count(vRolled.begin(), vRolled.end(), 4) * 4},
			{"Total Fives     : ", std::count(vRolled.begin(), vRolled.end(), 5) * 5},
			{"Total Sixes     : ", std::count(vRolled.begin(), vRolled.end(), 6) * 6},
			{"Chance          : ", std::accumulate(vRolled.begin(), vRolled.end(), 0)},

			{"Three Of A Kind : ", (Match({"nnn??", "?nnn?", "??nnn"})) ? (3 * vRolled[2]) : 0},
			{"Four Of A Kind  : ", (Match({"nnnn?", "?nnnn"})) ? (4 * vRolled[2]) : 0},
			{"Full House      : ", ((Match({"nnn??"}) && Match({"???nn"})) || (Match({"??nnn"}) && Match({"nn???"}))) ? 25 : 0},
			{"Small Straight  : ", (Match({"1234?", "2345?", "?3456", "?2345"})) ? 30 : 0},
			{"Large Straight  : ", (Match({"12345", "23456"})) ? 40 : 0},
			{"Five Of A Kind  : ", (Match({"nnnnn"})) ? 50 : 0},
			{"TOTAL POINTS : ", total_points}
		};

		//Drawing
		Clear(olc::DARK_GREEN);
		olc::vi2d vOffset = { -60, 90 };
		for (int i = 0; i < 5; i++) if (bSelected_Dice[i]) DrawGrid({ vOffset.x + (80 * (i + 1)) , 10 });

		for (int i = 0; i < 5; i++)       DrawDie({ vOffset.x += 80, 10 }, vRolled[i]);
	
		for (int i = 0; i < 13; i++)
		{
			if(bPoint[i])	 DrawString(10, vOffset.y += 10, vScores_after[i].first + std::to_string(vScores_after[i].second), olc::GREEN);	
			else 
			{
				if(!bWhat_to_score[i])	DrawString(10, vOffset.y += 10,vScores[i].first + std::to_string(vScores[i].second));
				else					DrawString(10, vOffset.y += 10, vScores[i].first + std::to_string(vScores[i].second),olc::RED);
				
			}
		}
		DrawString(10, vOffset.y += 30, vScores[13].first + std::to_string(vScores[13].second));  //totalpoints
		DrawString(10,410, "H for Help");
		if(bReleasedH)   // show rules, controls
		{
			DrawString(300, 160, "Controls:");
			DrawString(300, 170, "1,2,3,4,5			 keep the dice");
			DrawString(300, 180, "Space				 re - roll up to 2 times ");
			DrawString(300, 190, "Enter				 score points");
		}

		if(key_pressed_enter > 12)	DrawString(50, vOffset.y += 50, "You scored " + std::to_string(vScores[13].second) + " points" , olc::GREEN , 2);  //show points after game end
		return true;
	}

};

int main()
{
	FiveDice demo;
	if (demo.Construct(640, 480, 2, 2))
		demo.Start();
	return 0;
}
