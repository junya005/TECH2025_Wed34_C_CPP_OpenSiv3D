# include <Siv3D.hpp> // Siv3D v0.6.15

/*
暗黒森林理論がテーマのゲームを制作
*/

char matherTangue[5] = { U'a', U'e', U'i', U'o', U'u' };
char childTangue[26] = { U'A', U'B', U'C', U'D', U'E',
						 U'F', U'G', U'H', U'I', U'J',
						 U'K', U'L', U'M', U'N', U'O',
						 U'P', U'Q', U'R', U'S', U'T',
						 U'U', U'V', U'W', U'X', U'Y',
						 U'Z'
						};



class Civilization
{
public:
	bool IsDead = false;

	Civilization()
	{
		IsDead = false;

		// コンストラクタ
		_attack = Random(0.0f, 1.0f);
		_insight = Random(0.0f, 1.0f);
		_invisible = Random(0.0f, 1.0f);

		// 銀河の中心からの距離と、角度をRandomに決定する
		float distance = Random(0.0f, 330.0f);
		float theta = Random(0.0, Math::Pi * 2.0);

		_x = Math::Cos(theta) * distance;
		_y = Math::Sin(theta) * distance;

		// 何音節の名前か
		int wardNum = Random(1, 8);

		for (int i = 0; i < wardNum; i++)
		{
			int a = Random(0, 26);
			int b = Random(0, 5);
			_name = _name + childTangue[a];
			_name = _name + matherTangue[b];
		}

	}

	void Logic(float deltaTime, double year)
	{
		if (IsDead)return;
		_radius += Random(-deltaTime, +deltaTime) * 10.0f;

		if(_radius < 0)
		{
			IsDead = true;
			Print << U"宇宙歴" << year << U"年[" << _name << U"]は滅びた";
		}
	}

	void Draw()
	{
		if (IsDead)return;
		Circle(_x + 1280/2, _y + 720/2, _radius).draw(ColorF(_attack, _insight, _invisible));
	}

	void DrawName(const Font& font)
	{
		if (IsDead)return;
		int size = _radius * 2;
		if (size > 10) size = 10;
		font(_name).drawAt(size, Vec2{ _x + 1280 / 2, _y + 720 / 2 + 5 }, ColorF{ 1, 1, 1 });
	}

	void SearchAndDestroy(Civilization& another, double& year)
	{
		Civilization* p1 = this;
		Civilization* p2 = &another;

		if (p1 == p2)
		{
			Print << U"宇宙歴" << year << U"年に[" << _name << U"]の内戦が終了した";
			return;
		}
		if (IsDead || another.IsDead)return;

		// 自分の隠蔽度が相手の眼光よりも高ければ、何もしない
		if (another._invisible <= _insight) return;

		float distX = another._x - _x;
		float distY = another._y - _y;
		float sqrDistance = distX * distX + distY * distY;
		float plusRadius = another._radius + _radius;

		// 2つの文明の活動領域がかぶった
		if(plusRadius * plusRadius >= sqrDistance)
		{
			// 絶対先制攻撃のパーセンテージ
			float percentage100 = 0;
			if (_radius <= another._radius) percentage100 + 0.25f;
			if (another._attack <= _attack)percentage100 + 0.25f;
			if (sqrDistance > 25) percentage100 += (float)(sqrDistance - 25) * 0.1f;

			percentage100 += _attack;

			if(percentage100>=Random(0,100))
			{
				another._radius += _radius / another._radius * 0.5f;

				IsDead = true;

				Print << U"宇宙歴" << year << U"年[" << _name << U"]\nが[" << another._name << U"]に滅ぼされた";
			}
		}
	}

	float GetRadius()
	{
		return _radius;
	}

	String GetName()
	{
		return _name;
	}

	ColorF GetColor()
	{
		return ColorF(_attack, _insight, _invisible);
	}

private:
	// 攻撃性向
	float _attack = 0.5f;
	// 察知能力
	float _insight = 0.5f;
	// 隠蔽性向
	float _invisible = 0.5f;
	// 描画される円の半径
	float _radius = 1.0f;
	// 描画される円の中心座標
	float _x = 0.0f, _y = 0.0f;

	
	String _name = U"";
};

void Main()
{
	Window::Resize(1280,720);

	// 太文字のフォントを作成する | Create a bold font with MSDF method
	const Font font{ FontMethod::MSDF, 12, Typeface::Bold };

	// 背景の色を設定する | Set the background color
	Scene::SetBackground(ColorF{ 0.0f, 0.0f, 0.0f });

	double year = 0.0f;

	// ランダム数のシード値を現在時間のマイクロ数にする
	Reseed(Time::GetMicrosecSinceEpoch());

	Civilization civ = Civilization();

	float popCivCount = 0.0f;

	Civilization civs[10000];
	Civilization* drawCivs[10000];
	int popedCount = 0;

	while (System::Update())
	{
		popCivCount += Scene::DeltaTime()*10.0f;

		for(; popCivCount > 1.0f && popedCount < 10000;popCivCount -= 1.0f)
		{
			Civilization c = Civilization();

			civs[popedCount] = c;
			drawCivs[popedCount] = &civs[popedCount];

			popedCount++;
		}

		// ポインター配列を実態の半径の大きい順に並べる	ただし、死んでいる文明は劣後する
		Civilization* temp;
		for (int i = 0; i < popedCount; i++)
		{
			for (int j = 0; j < popedCount; j++)
			{
				if (drawCivs[i]->IsDead || drawCivs[i]->GetRadius() > drawCivs[j]->GetRadius())
				{
					temp = drawCivs[i];
					drawCivs[i] = drawCivs[j];
					drawCivs[j] = temp;
				}
			}
		}

		for (int i = 0; i < popedCount; i++)
		{
			if (civs[i].IsDead)
			{
				for (int j = i; j < popedCount - 1; j++)
				{
					civs[j] = civs[j + 1];
				}
				popedCount--;
			}
		}

		Rect(0, 0, 1280, 800).draw({ ColorF{0.2f, 0, 0},ColorF{0, 0.2f, 0}, ColorF{0.0f, 0.0f, 0.2f} });

		// テキストを描く | Draw text
		font(U"宇宙歴:{}"_fmt((long)(year+=Scene::DeltaTime()*1000.0f))).draw(16, Vec2{ 800, 10 }, ColorF{ 1.0f, 1.0f, 1.0f });

		

		for (int i = 0; i < popedCount; i++)
		{
			for (int j = 0; j < popedCount; j++)
			{
				if (i != j)civs[i].SearchAndDestroy(civs[j], year);
			}

			civs[i].Logic(Scene::DeltaTime(), year);
			drawCivs[i]->Draw();
			civs[i].DrawName(font);
		}

		// UI部分を表示する
		for (int i = 0; i < 10 && i < popedCount; i++)
		{
			Circle(1000 - 20, 50 + 50 * i, 16).draw(drawCivs[i]->GetColor());
			font(U"{}.［{}]{}"_fmt(i+1, (int)drawCivs[i]->GetRadius(), drawCivs[i]->GetName())).draw(16, Vec2{1000, 50 + 50 * i}, drawCivs[i]->GetColor());
		}


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
