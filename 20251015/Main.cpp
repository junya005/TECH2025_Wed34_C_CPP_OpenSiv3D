# include <Siv3D.hpp> // Siv3D v0.6.15

class CirclePlayer
{
private:
	int x = 0;
	int y = 0;
	int r = 10;
	Color c = Palette::Black;

	float f_circle_move_x_speed = 600.0f;
	float f_circle_move_y_speed = 600.0f;

	Circle circle;
public:
	CirclePlayer()
	{
		circle = Circle(x,y,r);
	}

	CirclePlayer(int x, int y, int r, Color color = Palette::Magenta)
	{
		SetPosition(x, y);
		SetRadius(r);
		SetColor(color);
	}

	void SetPosition(int x, int y)
	{
		circle.x = x;
		circle.y = y;
	}

	void SetRadius(int r)
	{
		circle.r = r;
	}

	void SetColor(Color color)
	{
		c = color;
	}

	void Draw()
	{
		circle.draw(c);
	}

	void Logic(float deltaTime)
	{
		float screen_scale_x = Window::GetState().frameBufferSize.x;
		float screen_scale_y = Window::GetState().frameBufferSize.y;

		if (circle.x <= circle.r || circle.x >= screen_scale_x - circle.r)
		{
			f_circle_move_x_speed = -f_circle_move_x_speed;
		}

		if (circle.y <= circle.r || circle.y >= screen_scale_y - circle.r)
		{
			f_circle_move_y_speed = -f_circle_move_y_speed;
		}

		circle.x += f_circle_move_x_speed * deltaTime;
		circle.y += f_circle_move_y_speed * deltaTime;
	}
};

class CircleMouse
{
private:
	int x = Cursor::Pos().x;
	int y = Cursor::Pos().y;
	int r = 10;
	Color c = Palette::Black;
	Circle circle;
public:
	CircleMouse()
	{
		circle = Circle(x, y, r);
	}

	CircleMouse(int x, int y, int r, Color color = Palette::Magenta)
	{
		SetPosition(x, y);
		SetRadius(r);
		SetColor(color);
	}

	void SetPosition(int x, int y)
	{
		circle.x = x;
		circle.y = y;
	}

	void SetRadius(int r)
	{
		circle.r = r;
	}

	void SetColor(Color color)
	{
		c = color;
	}

	void Draw()
	{
		circle.draw(c);
	}

	void Logic(float deltaTime)
	{
		circle.x = Cursor::Pos().x;
		circle.y = Cursor::Pos().y;
	}
};

void Main()
{
	CirclePlayer cp_a = CirclePlayer(100, 200, 80);
	CircleMouse cm_a = CircleMouse(100,200,20, Palette::Aqua);

	int i_a = 0;	// C/C++の場合、初期化し忘れに注意
	float f_b = 3.0f;		// 具体的には、初期化しないまま使用すると、値が不定になる
	char c_d = 'S';

	String str_c = U"A";

	Array<int> arr_i_ids = { 999, 0, 1};

	Circle cir_player = Circle(100,200,80);

	Rect rect_enemy = Rect(100,200,30,50);

	// float f_circle_move_x_speed = 600.0f;
	// float f_circle_move_y_speed = 600.0f;

	// 1行無視しろ
	/*
		2～3行目も無視しろ
	*/

	Print << i_a << f_b << c_d << str_c << arr_i_ids[0] << Window::GetState().frameBufferSize;

	while(System::Update())
	{
		float deltaTime = Scene::DeltaTime();

		rect_enemy.draw(Palette::Aqua);

		cp_a.Draw();
		cp_a.Logic(deltaTime);

		cm_a.Draw();
		cm_a.Logic(deltaTime);

		



		// マウスカーソルに円を表示
		

		/*
		float moveSpeedPlayer = 5.0f;
		int moveSinePlayer = 1.0f;

		float moveSpeedEnemy = 5.0f;
		int moveSineEnemy = 1.0f;
		*/

		/*
		if (cir_player.x + cir_player.r >= Window::GetState().frameBufferSize.x
			|| cir_player.y + cir_player.r >= Window::GetState().frameBufferSize.y)
		{
			moveSinePlayer *= -1;

		}
		else if (cir_player.x - cir_player.r <= 0.0f || cir_player.y - cir_player.r <= 0.0f)
		{
			moveSinePlayer *= -1;
		}

		if (rect_enemy.x + rect_enemy.size.x >= Window::GetState().frameBufferSize.x
			|| rect_enemy.y + rect_enemy.size.y >= Window::GetState().frameBufferSize.y)
		{
			moveSineEnemy *= -1;
		}
		else if(rect_enemy.x <= 0.0f || rect_enemy.y <= 0.0f)
		{
			moveSineEnemy *= -1;
		}

		cir_player.x += moveSpeedPlayer * moveSinePlayer;
		rect_enemy.y += moveSpeedEnemy * moveSineEnemy;
		*/
	}
}
