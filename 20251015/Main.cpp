# include <Siv3D.hpp> // Siv3D v0.6.15

class CirclePlayer
{
private:
	int _x = 0;
	int _y = 0;
	int _r = 10;
	Color _c = Palette::Black;
	float _f_circle_move_x_speed = 600.0f;
	float _f_circle_move_y_speed = 600.0f;

public:
	CirclePlayer() // コンストラクター　クラス実体が生まれた瞬間呼ばれる
	{
	}

	CirclePlayer(float x, float y, float r, Color color)
	{
		SetPosition(x, y);
		SetRadius(r);
		SetColor(color);
	}

	void SetPosition(float x, float y)
	{
		_x = x;
		_y = y;
	}

	void SetRadius(float r)
	{
		_r = r;
	}

	void SetColor(Color color)
	{
		_c = color;
	}

	void Draw()
	{
		Circle(_x, _y, _r).draw(_c);
	}

	void Logic(float deltaTime)
	{
		// 画面サイズが変わっても対応できるように
		float screen_scale_x = Window::GetState().frameBufferSize.x;
		float screen_scale_y = Window::GetState().frameBufferSize.y;

		// 画面左端に接触
		if (_x <= _r)
		{
			_x = 2 * _r - _x;
			_f_circle_move_x_speed = -_f_circle_move_x_speed;
		}
		// 画面右端に接触
		if (_x >= screen_scale_x - _r)
		{
			_x = -_x - 2 * _r + screen_scale_x * 2;
			_f_circle_move_x_speed = -_f_circle_move_x_speed;
		}
		// 画面上部に接触
		if (_y <= _r)
		{
			_y = 2 * _r - _y;
			_f_circle_move_y_speed = -_f_circle_move_y_speed;
		}
		// 画面下部に接触
		if (_y >= screen_scale_y - _r)
		{
			_y = -_y - 2 * _r + screen_scale_y * 2;
			_f_circle_move_y_speed = -_f_circle_move_y_speed;
		}

		_x += _f_circle_move_x_speed * deltaTime;
		_y += _f_circle_move_y_speed * deltaTime;
	}

	void CheckHit(CirclePlayer c)
	{
		float diffX = (float)(c._x - _x);
		float diffY = (float)(c._y - _y);
		float sqrMagnitude = diffX * diffX + diffY * diffY;
		float sqrRadius = (float)(c._r * c._r + _r * _r);

		// 異なる円の中心が同じ
		if(sqrMagnitude<sqrRadius)
		{
			// HIT!
			// Print << U"HIT!";

			float ragRefSurface = Math::Atan2(diffY, diffX) - Math::Pi * 0.5f;
			float ragAccel = Math::Atan2(_f_circle_move_y_speed, _f_circle_move_x_speed);
			float ragRef = Math::Pi * 2.0f - (ragAccel - ragRefSurface)+ragRefSurface;

			float accelPower = Math::Sqrt(_f_circle_move_y_speed * _f_circle_move_y_speed + _f_circle_move_x_speed * _f_circle_move_x_speed);
			_f_circle_move_x_speed = accelPower * Math::Cos(ragRef);
			_f_circle_move_y_speed = accelPower * Math::Sin(ragRef);
		}
	}
};

Rect rect_enemy = Rect(100, 200, 30, 50);

CirclePlayer cp_a = CirclePlayer();
CirclePlayer cp_b = CirclePlayer(400, 150, 50, Palette::Green);
Circle cir_player = Circle(100, 200, 10);

// メソッドの定義
void Logic(float);
void Draw(float);
void Update(float);
void Initialize();
void InputMain(float);

void Main()
{
	Initialize();

	while(System::Update())
	{
		float deltaTime = Scene::DeltaTime();
		Update(deltaTime);
	}
}

/// @brief 全体のUpdateがここから始動する
/// @param deltaTime 
void Update(float deltaTime)
{
	Logic(deltaTime);
	Draw(deltaTime);
	InputMain(deltaTime);
}

/// @brief ロジカルに演算する場所
/// @param deltaTime 
void Logic(float deltaTime)
{
	rect_enemy.y++;
	
	cp_a.Logic(deltaTime);
	cp_b.Logic(deltaTime);

	cp_a.CheckHit(cp_b);
	cp_b.CheckHit(cp_a);

	cir_player.x = Cursor::Pos().x;
	cir_player.y = Cursor::Pos().y;
}

/// @brief 表示部分
/// @param deltaTime 
void Draw(float deltaTime)
{
	rect_enemy.draw(Palette::Aqua);

	cp_b.Draw(); //cir_player.draw(Palette::Red);
	cp_a.Draw(); //cir_player.draw(Palette::Red);

	cir_player.draw();
}

void Initialize()
{
	cp_a.SetPosition(100, 100);
	cp_a.SetRadius(50);
	cp_a.SetColor(Palette::Red);

	cp_b.SetPosition(400, 150);
	cp_b.SetRadius(50);
	cp_b.SetColor(Palette::Green);

	int i_a = 0;	// C/C++の場合、初期化し忘れに注意
	float f_b = 3.0f;		// 具体的には、初期化しないまま使用すると、値が不定になる
	char c_d = 'S';

	String str_c = U"A";

	Array<int> arr_i_ids = { 999, 0, 1 };

	// 1行無視しろ
	/*
		2～3行目も無視しろ
	*/

	Print << i_a << f_b << c_d << str_c << arr_i_ids[0] << Window::GetState().frameBufferSize;

}

/// @brief 入力はここにまとめる
/// @param deltaTime 
void InputMain(float deltaTime)
{
	// R + Shiftキーをたたいたら、ゲームが最初に戻る
	if(KeyR.down() && KeyLShift.pressed())
	{
		Initialize();
	}
}
