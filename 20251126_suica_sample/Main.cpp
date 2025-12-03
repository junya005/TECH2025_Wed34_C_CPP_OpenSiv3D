# include <Siv3D.hpp> // Siv3D v0.6.15

enum E_Fruit_Type
{
	Sakuranbo,
	Ichigo,
	Budou,
	Dekopon,
	Kaki,
	Ringo,
	Nashi,
	Momo,
	Pin,
	Meron,
	Suica,
	MAX
};

class Fruit
{
public:

	bool _coledLeft = false;
	bool _coledRight = false;
	bool _coledButtom = false;

	bool IsDead = false;

	E_Fruit_Type Type = Sakuranbo;

	Fruit(float x, float y, E_Fruit_Type type)
	{
		Type = type;
		IsDead = false;

		// 銀河の中心からの距離と、角度をRandomに決定する
		_x = x;
		_y = y;

		SwitchType(type);
	}

	void SwitchType(E_Fruit_Type type)
	{
		float cross = Math::Pow(1.3f, (int)type);
		if (cross != 0.0f) {
			_radius = 10.0f * cross;
		}
		else
		{
			_radius = 10.0f;
		}

		switch (type)
		{
		case Sakuranbo:
			_color = ColorF(0.9f, 0.2f, 0.1f);
			break;
		case Ichigo:
			_color = ColorF(0.95f, 0.1f, 0.05f);
			break;
		case Budou:
			_color = ColorF(0.65f, 0.1f, 0.65f);
			break;
		case Dekopon:
			_color = ColorF(0.85f, 0.8f, 0.0f);
			break;
		case Kaki:
			_color = ColorF(0.95f, 0.4f, 0.05f);
			break;
		case Ringo:
			_color = ColorF(0.91f, 0.25f, 0.25f);
			break;
		case Nashi:
			_color = ColorF(0.75f, 0.75f, 0.05f);
			break;
		case Momo:
			_color = ColorF(0.95f, 0.75f, 0.75f);
			break;
		case Pin:
			_color = ColorF(0.95f, 0.7f, 0.15f);
			break;
		case Meron:
			_color = ColorF(0.25f, 0.95f, 0.35f);
			break;
		case Suica:
			_color = ColorF(0.05f, 0.45f, 0.125f);
			break;
		case MAX:
			break;
		default:
			break;
		}
	}

	void Logic(float deltaTime, int& gameOverCount)
	{
		_coledWithWall = false;
		_coledRight = false;
		_coledLeft = false;
		_coledButtom = false;

		if (IsDead) return;

		_x += _accelX * deltaTime;
		_y += _accelY * deltaTime;

		//　壁当たり
		if (_x > 600 - _radius && 150 < _y) {

			if(_x >= 600)
			{
				_x = 600 + _radius;

				if (_accelX < 0) _accelX *= -0.025f;
				_coledLeft = true;
			}
			else
			{
				_x = 600 - _radius;

				if (_accelX > 0) _accelX *= -0.025f;
				_coledRight = true;
			}

		}

		if (_x < 200 + _radius && 150 < _y) {

			if(_x <= 200)
			{
				_x = 200 - _radius;

				if (_accelX > 0) _accelX *= -0.025f;
				_coledRight = true;
			}
			else
			{
				_x = 200 + _radius;

				if (_accelX < 0) _accelX *= -0.025f;
				_coledLeft = true;
			}
		}

		if (_y > 500 - _radius && _x > 200 && _x < 600) {
			_y = 500 - _radius;

			if (_accelY > 0) _accelY *= -0.025f;
			_coledButtom = true;

		}

		_coledWithWall = _coledLeft || _coledRight || _coledButtom;

		// 振動禁止
		if (_accelX >= -0.01f && _accelX <= 0.01f) _accelX = 0.0f;
		if (_accelY >= -0.01f && _accelY <= 0.01f) _accelY = 0.0f;

		// 加速度反映　動作反映
		_accelY += deltaTime * CONST_GRAVITY_SCALE;

		_accelX *= 0.975f;
		_accelY *= 0.975f;

		if (_coledLeft && _accelX < 0)_accelX *= 0.9f;
		if (_coledRight && _accelX > 0)_accelX *= 0.9f;
		if (_coledButtom && _accelY > 0)_accelY *= 0.9f;

		// GameOverフラグ
		if (_y >= 600 + _radius)
		{
			IsDead = true;
			gameOverCount ++ ;
		}
	}

	void Draw()
	{
		if (IsDead) return;

		Circle{ _x, _y, _radius }.draw(_color);
	}

	void SearchAndFusionAndDestroy(Fruit* another)
	{
		if (IsDead)return;
		if (_radius < 0) return;

		float distX = another->_x - _x;
		float distY = another->_y - _y;
		float sqrDistance = distX * distX + distY * distY;
		float plusRadius = another->_radius + _radius;

		// 二つの文明の活動領域がかぶった
		if (plusRadius * plusRadius >= sqrDistance)
		{
			if(Type == another->Type)
			{
				if(Type == Suica)
				{
					IsDead = true;
					another->IsDead = true;
				}
				else
				{
					Type = (E_Fruit_Type)((int)Type + 1);
					another->IsDead = true;
					_x += distX * 0.5f;
					_y += distY * 0.5f;

					SwitchType(Type);
				}
			}

			float accelTheta = Math::Atan2(distY, distX);

			// 壁に固定されていない場合は、めり込まないようにする
			if (_coledLeft == false || _coledButtom == false || _coledRight == false)
			{
				float diff = Math::Sqrt(Math::Pow(plusRadius, 2) - sqrDistance);

			
				float shiftX = Math::Cos(accelTheta) * -diff * 0.5f;
				float shiftY = Math::Sin(accelTheta) * -diff * 0.5f;

				if (_coledLeft && shiftX < 0) shiftX = 0.0f;
				if (_coledLeft && shiftX > 0) shiftX = 0.0f;
				if (_coledButtom && shiftY > 0) shiftY = 0.0f;

				_x += shiftX;
				_y += shiftY;
			}

			float colTheta = Math::Atan2(distY,distX) + Math::Pi * 0.5f;
			// colTheta + 360 = this + ref
			float refTheta = colTheta * 2.0f - Math::Atan2(_accelY, _accelX);

			float mag = (GetPotent() + another->GetPotent())*0.5f/_weight;

			float nomalizeX = Math::Cos(refTheta);
			float nomalizeY = Math::Sin(refTheta);

			if ((_coledLeft || another->_coledLeft) && nomalizeX < 0.0f)nomalizeX = 0.0f;
			if ((_coledRight || another->_coledRight) && nomalizeX > 0.0f)nomalizeX = 0.0f;
			if ((_coledButtom || another->_coledButtom) && nomalizeY > 0.0f)nomalizeY = 0.0f;

			_accelX = nomalizeX * mag * 0.25f;
			_accelY = nomalizeY * mag * 0.25f;
		}
	}

	float GetPotent()
	{
		if (_coledWithWall) return 9999999.0f;
		return _weight * Math::Sqrt(Math::Pow(_accelY, 2) + Math::Pow(_accelX, 2));
	}

	void SetRadius(float radius)
	{
		_radius = radius;
		_weight = 4.0f / 3.0f * Math::Pi * Math::Pow(_radius, 3);
	}

	float GetRadius()
	{
		return _radius;
	}

	float GetWeight()
	{
		return _weight;
	}

	void SetName(String name)
	{
		_name = name;
	}

	String GetName()
	{
		return _name;
	}

	void SetColor(ColorF color)
	{
		_color = color;
	}

	ColorF GetColor()
	{
		return _color;
	}

private:
	const float CONST_GRAVITY_SCALE = 9.8f * 300;

	float _radius = 10.0f;
	float _weight = 4.0f / 3.0f * Math::Pi * Math::Pow(_radius,3);
	float _x = 0.0f;
	float _y = 0.0f;

	ColorF _color = ColorF{ 0.5,0.5,0.5 };

	String _name = U"";

	float _accelX = 0.0f;
	float _accelY = 0.0f;

	bool _coledWithWall = _coledLeft||_coledRight||_coledButtom;
};

bool GetGameOver(int& gameOverCount)
{
	return gameOverCount > 0;
}

void Main()
{
	// 背景の色を設定する | Set the background color
	Scene::SetBackground(ColorF{ 0.6, 0.8, 0.7 });

	// 太文字のフォントを作成する | Create a bold font with MSDF method
	const Font font{ FontMethod::MSDF, 24, Typeface::Bold};

	const int FRUITS_MAX_NUM = 1024;

	Fruit* fruits[FRUITS_MAX_NUM];

	int cntFruit = 0;

	E_Fruit_Type nextType = (E_Fruit_Type)Random((int)Sakuranbo, (int)MAX-1);

	float mousePosX = 300.0f;
	float mousePosY = 300.0f;

	int gameOverCount = 0;

	bool RetryButtonDown = false;

	const ColorF buttonOnColor = ColorF(0.1);
	const ColorF buttonOffColor = ColorF(0.9);

	while (System::Update())
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

				nextType = (E_Fruit_Type)Random((int)Sakuranbo, (int)MAX-1);

				// Print << U"Pop!";
			}
		}

		for (int i = 0; i < cntFruit; i++)
		{
			if(gameOver==false)
			{
				for (int j = 0; j < cntFruit; j++)
				{
					if (i != j)
					{
						// 当たり判定など
						fruits[i]->SearchAndFusionAndDestroy(fruits[j]);
					}
				}

				fruits[i]->Logic(Scene::DeltaTime(), gameOverCount);
			}
			
			fruits[i]->Draw();
		}

		if (gameOver == false)
		{
			for (int i = 0; i < cntFruit; i++)
			{
				if (fruits[i]->IsDead)
				{
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

			// マウスカーソル部分に〇を表示
			Circle(mousePosX, 100, 12).draw(Palette::Black);
			Circle(mousePosX, 100, 10).draw(Palette::White);
		}

		mousePosX = Cursor::Pos().x;
		mousePosY = Cursor::Pos().y;

		Line(200, 500, 600, 500).draw(4, Palette::Orange);
		Line(200, 150, 200, 500).draw(4, Palette::Orange);
		Line(600, 150, 600, 500).draw(4, Palette::Orange);

		if(gameOver)
		{
			Rect rect = Rect{ 0, 0, 800, 600 }.draw(ColorF{ 0,0,0,0.85f });

			font(U"GAME OVER").draw(60, rect.center()+Vec2(-40*9*0.5f, -60), ColorF(0.8f));

			ColorF aC;
			ColorF bC;

			if(RetryButtonDown)
			{
				aC = buttonOnColor;
				bC = buttonOffColor;
			}
			else
			{
				aC = buttonOffColor;
				bC = buttonOnColor;
			}

			Rect rect_button = Rect{ 360, 370, 120, 60 }.draw(buttonOffColor);

			font(U"Retry").draw(30, rect_button.center() + Vec2(-20 * 5 * 0.5f, -30), buttonOnColor);

			if ((mousePosX > 360 && mousePosX < 480 && mousePosY > 370 && mousePosY < 430)==false)
			{
				RetryButtonDown = true;
			}

			if (MouseL.down())
			{
				if(mousePosX>360 && mousePosX < 480 && mousePosY > 370 && mousePosY < 430)
				{
					RetryButtonDown = true;
				}
			}

			if(MouseL.up()&& RetryButtonDown)
			{
				// 一旦ゲームオーバーを解く
				gameOverCount = 0;
			}
		}
	}
}
