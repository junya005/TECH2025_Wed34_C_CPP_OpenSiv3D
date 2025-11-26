# include <Siv3D.hpp> // Siv3D v0.6.15

enum E_Fruit_Type
{
	Sakuranbo,
	Ichigo,
	Budou,
	Dekopon,
	Kaki,
	Ringo,
	Momo,
	Pin,
	Meron,
	Suica,
	MAX
};


class Fruit
{
public:

	bool IsDead = false;

	E_Fruit_Type Type = Sakuranbo;

	Fruit(float x, float y, E_Fruit_Type type)
	{
		Type = type;
		IsDead = false;

		// 銀河の中心からの距離と、角度をRandomに決定する
		_x = x;
		_y = y;

		float cross = (int)type * 1.3f;
		if (cross != 0.0f) {
			_radius = 5.0f *cross;
		}
		else
		{
			_radius = 5.0f;
		}

		switch (type)
		{
		case Sakuranbo:
			_color = ColorF( 0.9f,0.2f,0.1f );
			break;
		case Ichigo:
			_color = ColorF(0.5f, 0.15f, 0.15f);
			break;
		case Budou:
			_color = ColorF(0.25f, 0.6f, 0.095f);
			break;
		case Dekopon:
			_color = ColorF(0.65f, 0.1f, 0.65f);
			break;
		case Kaki:
			_color = ColorF(0.6f, 0.1f, 0.05f);
			break;
		case Ringo:
			_color = ColorF(0.9f, 0.05f, 0.05f);
			break;
		case Momo:
			_color = ColorF(0.95f, 0.75f, 0.75f);
			break;
		case Pin:
			_color = ColorF(0.95f, 0.7f, 0.15f);
			break;
		case Meron:
			_color = ColorF(0.95f, 0.7f, 0.15f);
			break;
		case Suica:
			_color = ColorF(0.15f, 0.45f, 0.125f);
			break;
		case MAX:
			break;
		default:
			break;
		}
	}

	void Logic(float deltaTime)
	{
		if (IsDead) return;

		// 加速度反映 動作反映
		_x += _accelX * deltaTime;
		_y += _accelY * deltaTime;

		// 壁当たり
		if(_x > 600 - _radius)
		{
			_x = 600 - _radius;

			if (_accelX > 0) _accelX *= -0.25f;
		}

		if (_x < 200 - _radius)
		{
			_x = 200 - _radius;

			if (_accelX > 0) _accelX *= -0.25f;
		}

		if(_y > 500)
		{
			_y = 500 - _radius;

			if (_accelY > 0) _accelY *= -0.25f;
		}

		// 振動禁止
		if (_accelX >= -0.01f && _accelX <= 0.01f)_accelX = 0.0f;
		if (_accelY >= -0.01f && _accelY <= 0.01f)_accelY = 0.0f;

		// 重力
		_accelY += deltaTime * CONST_GRAVITY_SCALE;

		if(_y >= 600.0f + _radius)
		{
			IsDead = true;
		}
	}

	void Draw()
	{
		if (IsDead) return;

		Circle{ _x, _y, _radius }.draw(_color);
	}

	void SearchAndFusionAndDestroy(Fruit& another)
	{

		if (_radius < 0) return;
		if (another._radius < 0) another.IsDead = true;

		Fruit* p1 = this;
		Fruit* p2 = &another;

		if (p1 == p2)
		{
			return;
		}

		float distX = another._x - _x;
		float distY = another._y - _y;
		float sqrDistance = distX * distX + distY * distY;
		float plusRadius = another._radius + _radius;

		// 二つの文明の活動領域がかぶった
		if (plusRadius * plusRadius >= sqrDistance)
		{
			float colTheta = Math::Atan2(distY,distX) + Math::Pi * 0.5f;
			// colTheta + 360 = this + ref
			float refTheta = colTheta + Math::Pi * 2.0f - Math::Atan2(_accelY, _accelX);

			float mag = Math::Sqrt(Math::Pow(_accelY, 2) + Math::Pow(_accelX, 2));

			float nomalizeX = Math::Cos(refTheta);
			float nomalizeY = Math::Sin(refTheta);

			_accelX = nomalizeX *mag;
			_accelY = nomalizeY *mag;

			float diff = Math::Sqrt(Math::Pow(plusRadius, 2) - sqrDistance);
			_x += nomalizeX * diff * 0.5f;
			_y += nomalizeY * diff * 0.5f;
		}
	}

	void SetRadius(float radius)
	{
		_radius = radius;
	}

	float GetRadius()
	{
		return _radius;
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
	float _x = 0.0f;
	float _y = 0.0f;

	ColorF _color = ColorF{ 0.5,0.5,0.5 };

	String _name = U"";

	float _accelX = 0.0f;
	float _accelY = 0.0f;
};


void Main()
{
	// 背景の色を設定する | Set the background color
	Scene::SetBackground(ColorF{ 0.6, 0.8, 0.7 });

	// 太文字のフォントを作成する | Create a bold font with MSDF method
	const Font font{ FontMethod::MSDF, 24, Typeface::Bold };

	const int FRUITS_MAX_NUM = 1024;

	Fruit* fruits[FRUITS_MAX_NUM];

	int cntFruit = 0;

	E_Fruit_Type nextType = (E_Fruit_Type)Random((int)Sakuranbo, (int)MAX-1);

	float mousePosX = 300.0f;
	float mousePosY = 300.0f;

	while (System::Update())
	{

		if (MouseL.down())
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

				Print << U"Pop!";
			}
		}

		for (int i = 0; i < cntFruit; i++)
		{
			for (int j = 0; j < cntFruit; j++)
			{
				if (i != j)
				{
					// 当たり判定など
					fruits[i]->SearchAndFusionAndDestroy(*fruits[j]);
				}
			}

			fruits[i]->Logic(Scene::DeltaTime());
			fruits[i]->Draw();
		}

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

		mousePosX = Cursor::Pos().x;
		mousePosY = Cursor::Pos().y;

		if (mousePosX > 590) mousePosX = 590;
		if (mousePosX < 210) mousePosX = 210;

		// マウスカーソル部分に〇を表示
		Circle(mousePosX, 100, 12).draw(Palette::Black);
		Circle(mousePosX, 100, 10).draw(Palette::White);

		Line(200, 500, 600, 500).draw(4, Palette::Orange);
		Line(200, 150, 200, 500).draw(4, Palette::Orange);
		Line(600, 150, 600, 500).draw(4, Palette::Orange);
	}
}
