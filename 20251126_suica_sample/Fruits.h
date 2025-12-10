#pragma once

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
	bool _coledBottom = false;

	bool IsDead = false;

	E_Fruit_Type Type = Sakuranbo;

	Fruit(float x, float y, E_Fruit_Type type, bool isOnlyView = false)
	{
		_isOnlyViewObject = isOnlyView;
		Type = type;
		IsDead = false;

		// 銀河の中心からの距離と、角度をRandomに決定する
		_x = x;
		_y = y;

		SwitchType(type);
	}

	void SwitchType(E_Fruit_Type type)
	{
		float sizeMulti = 1.3f;

		// 5乗以降はPow関数を使用(速いので)
		if (5 < (int)type)
		{
			float cross = Math::Pow(sizeMulti, (int)type);
			if (cross != 0.0f) {
				_radius = 10.0f * cross;
			}
		}

		switch (type)
		{
		case Sakuranbo:
			_radius = 10.0f;
			_color = ColorF(0.9f, 0.2f, 0.1f);
			break;
		case Ichigo:
			_radius = 10.0f * 1.3f;
			_color = ColorF(0.95f, 0.1f, 0.05f);
			break;
		case Budou:
			_radius = 10.0f * 1.3f * 1.3f;
			_color = ColorF(0.65f, 0.1f, 0.65f);
			break;
		case Dekopon:
			_radius = 10.0f * 1.3f * 1.3f * 1.3f;
			_color = ColorF(0.85f, 0.8f, 0.0f);
			break;
		case Kaki:
			_radius = 10.0f * 1.3f * 1.3f * 1.3f * 1.3f;
			_color = ColorF(0.95f, 0.4f, 0.05f);
			break;
		case Ringo:
			_radius = 10.0f * 1.3f * 1.3f * 1.3f * 1.3f * 1.3f;
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
		if (_isOnlyViewObject)return;
		if (IsDead) return;

		if (((_accelX > 0 && _coledRight) == false)
			&& ((_accelX < 0 && _coledLeft) == false))
		{
			_x += _accelX * deltaTime;
		}
		if ((_accelY > 0 && _coledBottom) == false)
		{
			_y += _accelY * deltaTime;
		}

		_coledWithWall = false;
		_coledRight = false;
		_coledLeft = false;
		_coledBottom = false;

		//　壁当たり
		if (_x > 600 - _radius && 150 < _y) {

			if (_x >= 600)
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

			if (_x <= 200)
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
			_coledBottom = true;

		}

		_coledWithWall = _coledLeft || _coledRight || _coledBottom;

		// 振動禁止
		if (_accelX >= -0.01f && _accelX <= 0.01f) _accelX = 0.0f;
		if (_accelY >= -0.01f && _accelY <= 0.01f) _accelY = 0.0f;

		// 加速度反映　動作反映
		_accelY += deltaTime * CONST_GRAVITY_SCALE;

		_accelX *= 0.9175f;
		_accelY *= 0.9175f;

		// GameOverフラグ
		if (_y >= 600 + _radius)
		{
			IsDead = true;
			gameOverCount++;
		}
	}

	void Draw()
	{
		if (IsDead) return;

		Circle{ _x, _y, _radius }.draw(_color);
	}

	void Draw(float x, float y, float alpha)
	{
		Circle{ x, y, _radius }.draw(ColorF{ _color.r, _color.g, _color.b, alpha });
	}

	void Draw(const Font& font, int destroyNumber)
	{
		Circle{ _x,_y,_radius*0.075f+10 }.draw(_color);
		font(Format(U"x", destroyNumber)).draw(_x+5, _y -5);
	}

	void SearchAndFusionAndDestroy(Fruit* another)
	{
		if (_isOnlyViewObject)return;
		if (IsDead)return;
		if (_radius < 0) return;

		float distX = another->_x - _x;
		float distY = another->_y - _y;
		float sqrDistance = distX * distX + distY * distY;
		float plusRadius = another->_radius + _radius;

		// 二つの文明の活動領域がかぶった
		if (plusRadius * plusRadius >= sqrDistance)
		{
			if (Type == another->Type)
			{
				if (Type == Suica)
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
			if (_coledLeft == false || _coledRight == false || _coledBottom == false)
			{
				float diff = Math::Sqrt(plusRadius * plusRadius - sqrDistance);


				float shiftX = Math::Cos(accelTheta) * -diff * 0.5f;
				float shiftY = Math::Sin(accelTheta) * -diff * 0.5f;

				if (_coledLeft && shiftX < 0) shiftX = 0.0f;
				if (_coledLeft && shiftX > 0) shiftX = 0.0f;
				if (_coledBottom && shiftY > 0) shiftY = 0.0f;

				_x += shiftX;
				_y += shiftY;
			}

			float colTheta = accelTheta + Math::Pi * 0.5f;
			// colTheta + 360 = this + ref
			float refTheta = colTheta * 2.0f - Math::Atan2(_accelY, _accelX);

			float mag = (GetPotent() + another->GetPotent()) * 0.5f / _weight;

			float nomalizeX = Math::Cos(refTheta);
			float nomalizeY = Math::Sin(refTheta);

			if ((_coledLeft || another->_coledLeft) && nomalizeX < 0.0f)nomalizeX = 0.0f;
			if ((_coledRight || another->_coledRight) && nomalizeX > 0.0f)nomalizeX = 0.0f;
			if ((_coledBottom || another->_coledBottom) && nomalizeY > 0.0f)nomalizeY = 0.0f;

			_accelX = nomalizeX * mag * 0.025f;
			_accelY = nomalizeY * mag * 0.025f;
		}
	}

	float GetPotent()
	{
		if (_coledWithWall) return 9999999.0f;
		return _weight * Math::Sqrt(_accelY * _accelY + _accelX * _accelX);
	}

	void SetRadius(float radius)
	{
		_radius = radius;
		_weight = 4.0f / 3.0f * Math::Pi * _radius * _radius * _radius;
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
	float _weight = 4.0f / 3.0f * Math::Pi * _radius * _radius * _radius;
	float _x = 0.0f;
	float _y = 0.0f;

	ColorF _color = ColorF{ 0.5,0.5,0.5 };

	String _name = U"";

	float _accelX = 0.0f;
	float _accelY = 0.0f;

	bool _coledWithWall = _coledLeft || _coledRight || _coledBottom;
	bool _isOnlyViewObject = false;
};


