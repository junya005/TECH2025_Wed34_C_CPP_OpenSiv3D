# include <Siv3D.hpp> // Siv3D v0.6.15

enum EElementType
{
	white,
	red,
	mazenta,
	perple,
	orange,
	blue,
	pink,
	yellow,
	cian,
	green,
	end_element
};

class Element
{
public:
	EElementType elementType = white;

	Element()
	{
		int elementMax = (int)EElementType::end_element;
		int elementRandom = Random(0, elementMax-1);

		SetUpElement((EElementType)elementRandom);
	}

	void SetUpElement(EElementType type)
	{
		int elementNum = (int)type;

		_radius = (elementNum + 1) * 10.0f;

		_color = GetColorByEElementType(type);
	}

	void Logic()
	{


		// 重力
		_y++;
	}

	void Draw()
	{
		Circle(_x, _y, _radius).draw(_color);
	}


private:

	float _x = 800/2;
	float _y = 20;
	float _radius = 1.0f;
	ColorF _color = ColorF{ 1,1,1 };

	bool isTatchWall = false;

	ColorF GetColorByEElementType(EElementType type)
	{
		if (type == end_element)return ColorF{ 0,0,0 };

		switch (type)
		{
		case EElementType::white:
			return _color = ColorF{ 1,1,1 };
		case EElementType::red:
			return _color = ColorF{ 1,0,0};
		case EElementType::mazenta:
			return _color = ColorF{ 1,1,0 };
		case EElementType::perple:
			return _color = ColorF{ 0.7,0.5,0 };
		case EElementType::orange:
			return _color = ColorF{ 1,0,0.5 };
		case EElementType::blue:
			return _color = ColorF{ 0,0,1 };
		case EElementType::pink:
			return _color = ColorF{ 0.2,1,0 };
		case EElementType::yellow:
			return _color = ColorF{ 1,0,1 };
		case EElementType::cian:
			return _color = ColorF{ 0,0.2,1 };
		case EElementType::green:
			return _color = ColorF{ 0,1,0 };
		}

		return ColorF{ 0,0,0 };
	}
};

class Wall
{
public:
	Wall(float x, float y, float sizeX, float sizeY)
	{
		_x = x;
		_y = y;
		_sizeX = sizeX;
		_sizeY = sizeY;
	}

	void Logic()
	{

	}

	void Draw()
	{
		Rect(_x, _y, _sizeX, _sizeY).draw(ColorF{1,1,1});
	}

private:
	float _x = 0.0f;
	float _y = 0.0f;
	float _sizeX = 200.0f;
	float _sizeY = 200.0f;
};

void Main()
{
	// 背景の色を設定する | Set the background color
	Scene::SetBackground(ColorF{ 0, 0, 0 });

	// 太文字のフォントを作成する | Create a bold font with MSDF method
	const Font font{ FontMethod::MSDF, 48, Typeface::Bold };

	Element e = Element();

	Wall buttomWall = Wall(100, 500, 600, 20);
	Wall leftWall = Wall(100, 100, 20, 400);
	Wall rightWall = Wall(680, 100, 20, 400);

	while (System::Update())
	{
		buttomWall.Draw();
		leftWall.Draw();
		rightWall.Draw();

		e.Logic();
		e.Draw();
	}
}

//
// - Debug ビルド: プログラムの最適化を減らす代わりに、エラーやクラッシュ時に詳細な情報を得られます。
//
// - Release ビルド: 最大限の最適化でビルドします。
//
// - [デバッグ] メニュー → [デバッグの開始] でプログラムを実行すると、[出力] ウィンドウに詳細なログが表示され、エラーの原因を探せます。
//
// - Visual Studio を更新した直後は、プログラムのリビルド（[ビルド]メニュー → [ソリューションのリビルド]）が必要な場合があります。
//
