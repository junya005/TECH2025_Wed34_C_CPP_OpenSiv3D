#pragma once
#include "Fruits.h"

// 初期化用データ
class GameManager
{
private:
	const ColorF BackgroundColor{ 0.6, 0.8, 0.7 };

	// 太文字のフォントを作成する | Create a bold font with MSDF method
	const Font font{ FontMethod::MSDF, 24, Typeface::Bold };

	const int FRUITS_MAX_NUM = 1024;

	Fruit* fruits[1024];

	int cntFruit = 0;

	E_Fruit_Type nextType = (E_Fruit_Type)Random((int)Sakuranbo, (int)MAX - 1);

	float mousePosX = 300.0f;
	float mousePosY = 300.0f;

	int gameOverCount = 0;

	bool RetryButtonDown = false;

	const ColorF buttonOnColor = ColorF(0.1);
	const ColorF buttonOffColor = ColorF(0.85);

	Fruit* nextFruitView;

	Fruit* fruitsIcons[(int)E_Fruit_Type::MAX];
	int fruitsDestroyNumbers[(int)E_Fruit_Type::MAX];

	int _gamePoint = 0;

	bool GetGameOver(int& gameOverCount)
	{
		return gameOverCount >= 3;
	}

public:
	void Initialize()
	{
		// 背景の色を設定する | Set the background color
		Scene::SetBackground(BackgroundColor);

		nextType = (E_Fruit_Type)Random((int)Sakuranbo, (int)MAX - 1);

		Fruit* fruit = new Fruit(mousePosX, 100, nextType, true);
		nextFruitView = fruit;


		for (int i = 0; i < MAX; i++)
		{
			fruitsIcons[i] = new Fruit(680, 470-i*40, (E_Fruit_Type)i,true);
		}
	}

	void SetDefault()
	{
		cntFruit = 0;
		nextType = (E_Fruit_Type)Random((int)Sakuranbo, (int)MAX - 1);
		nextFruitView->SwitchType(nextType);
		gameOverCount = 0;
		_gamePoint = 0;

		for(int i = 0; i < (int)E_Fruit_Type::MAX; i++)
		{
			fruitsDestroyNumbers[i] = 0;
		}
	}

	void Update()
	{
		bool gameOver = GetGameOver(gameOverCount);


		if (MouseL.down() && gameOver == false)
		{
			if (cntFruit >= FRUITS_MAX_NUM)
			{
				Print << U"Error!! Full Array Index";
			}
			else
			{

				Fruit* fruit = new Fruit(mousePosX, 100, nextType);
				fruit->Type = nextType;
				fruits[cntFruit++] = fruit;

				nextType = (E_Fruit_Type)Random((int)Sakuranbo, (int)MAX - 1);
				nextFruitView->SwitchType(nextType);
				// Print << U"Pop!";
			}
		}

		for (int i = 0; i < cntFruit; i++)
		{
			if (gameOver == false)
			{
				fruits[i]->Logic(Scene::DeltaTime(), gameOverCount);

				for (int j = 0; j < cntFruit; j++)
				{
					if (i != j)
					{
						// 当たり判定など
						fruits[i]->SearchAndFusionAndDestroy(fruits[j]);
					}
				}


			}

			fruits[i]->Draw();
		}

		if (gameOver == false)
		{
			for (int i = 0; i < cntFruit; i++)
			{
				if (fruits[i]->IsDead)
				{
					_gamePoint += ((int)fruits[i]->Type * (int)fruits[i]->Type)+1;
					fruitsDestroyNumbers[(int)fruits[i]->Type]++;

					if(fruits[i]->Type == Suica)gameOverCount--;
					for (int j = i; j < cntFruit - 1; j++)
					{
						// delete &fruits[j];
						fruits[j] = fruits[j + 1];
					}
					cntFruit--;
				}
			}



			if (mousePosX > 590) mousePosX = 590;
			if (mousePosX < 210) mousePosX = 210;

			// 次に落ちるだろう、フルーツの状態を表示する
			nextFruitView->Draw(mousePosX, 100, 0.25f);

			// マウスカーソル部分に〇を表示
			Circle(mousePosX, 100, 12).draw(Palette::Black);
			Circle(mousePosX, 100, 10).draw(Palette::White);
		}

		mousePosX = Cursor::Pos().x;
		mousePosY = Cursor::Pos().y;

		Line(200, 500, 600, 500).draw(4, Palette::Orange);
		Line(200, 150, 200, 500).draw(4, Palette::Orange);
		Line(600, 150, 600, 500).draw(4, Palette::Orange);

		for (int i = 0; i < MAX; i++)
		{
			if(i == Suica)
			{
				fruitsIcons[i]->Draw(font, fruitsDestroyNumbers[i]);
			}
			else
			{
				fruitsIcons[i]->Draw(font, fruitsDestroyNumbers[i] * 2);
			}
		}

		font(Format(U"Score:",_gamePoint)).draw(30, 100);

		for(int i = 0; i < 3-gameOverCount; i++)
		{
			font(U"♥").draw(60,30+50*i,10, Palette::Darkred);
		}

		if (gameOver)
		{
			Rect rect = Rect{ 0,0,800,600 }.draw(ColorF{ 0,0,0,0.85f });

			font(U"GAME OVER").draw(60, rect.center() - Vec2(40 * 9 / 2, 60), ColorF(0.8f));
			font(Format(U"Score:", _gamePoint)).draw(50, rect.center() - Vec2(40 * 9 / 2, 160), ColorF(0.8f));

			ColorF aC;
			ColorF bC;

			if (RetryButtonDown)
			{
				aC = buttonOnColor;
				bC = buttonOffColor;
			}
			else
			{
				bC = buttonOnColor;
				aC = buttonOffColor;
			}

			Rect buttonRect = Rect{ 360,370,120,60 }.draw(aC);

			font(U"Retry").draw(30, buttonRect.center() - Vec2(18 * 5 / 2, 20), bC);

			if ((mousePosX > 360 && mousePosX < 480 && mousePosY > 370 && mousePosY < 430) == false)
			{
				RetryButtonDown = false;
			}

			if (MouseL.down())
			{
				if (mousePosX > 360 && mousePosX < 480 && mousePosY > 370 && mousePosY < 430)
				{
					RetryButtonDown = true;
				}
			}

			if (MouseL.up() && RetryButtonDown)
			{
				SetDefault();

				// 一旦ゲームオーバーを解く
				gameOverCount = 0;
			}
		}
	}
};

