# include <Siv3D.hpp> // Siv3D v0.6.15

// ノート
struct Note
{
	// ノートの時刻
	int32 time;

	// 押すべきキーのインデックス (0, 1, 2, 3)
	int32 key;

	// 消えたら false
	bool active = true;
};

// ノート情報を譜面ファイルからロードする関数
Array<Note> LoadNotes(const FilePath& path)
{
	TextReader reader{ path };

	if (not reader)
	{
		throw Error{ U"譜面 {} が見つかりません。"_fmt(path) };
	}

	Array<Note> notes;

	String line;

	// 1 行ずつ読み込む
	while (reader.readLine(line))
	{
		// 空白行はスキップ
		if (line.isEmpty())
		{
			continue;
		}

		// 読み込んだ行を半角スペースで分割
		const Array<String> params = line.split(U' ');

		// 分割した結果が 2 要素でない場合は不正な譜面
		if (params.size() != 2)
		{
			throw Error{ U"不正な譜面です。" };
		}

		// 分割した結果をそれぞれ int32 型に変換
		notes.emplace_back(Parse<int32>(params[0]), Parse<int32>(params[1]));
	}

	return notes;
}

// ノートの座標を計算する関数
Vec2 GetNotePos(const Note& note, int32 time)
{
	const double x = (250 + note.key * 100);
	const double y = (500 - (note.time - time) * 0.25);
	return{ x, y };
}

// ノートを押したときのエフェクト
struct NoteEffect : IEffect
{
	Vec2 m_start;

	int32 m_score;

	Font m_font;

	NoteEffect(const Vec2& start, int32 score, const Font& font)
		: m_start{ start }
		, m_score{ score }
		, m_font{ font } {
	}

	bool update(double t) override
	{
		Circle{ m_start, (30 + t * 80) }.drawFrame(15 * (0.5 - t));

		if (m_score == 2)
		{
			m_font(U"Excellent").drawAt(32, m_start.movedBy(0, (-20 - t * 160)), Palette::Orange);
		}
		else if (m_score == 1)
		{
			m_font(U"Good").drawAt(32, m_start.movedBy(0, (-20 - t * 160)), Palette::Skyblue);
		}

		return (t < 0.5);
	}
};

// ゲーム画面状態を定義
enum GamePhase
{
	TitlePhase,
	SelectPhase,
	MainPhase,
	ResultPhase,
};

// ゲーム結果を保存しておく構造体
struct GameResultState
{
private:
	int score = 0;
	int comboMax = 0;

public:
	GameResultState(int score, int comboMax)
	{
		this->score = score;
		this->comboMax = comboMax;
	};

	int GetScore()
	{
		return score;
	}

	int GetComboMax()
	{
		return comboMax;
	}
};

// タイトル画面のクラス
class TitleScreen
{
public:
	void Initialize()
	{

	}

	void Logic(GamePhase& gamePhase)
	{
		if(pressedStartButton)
		{
			pressedStartButton = false;

			// メインゲームへ移行
			gamePhase = GamePhase::MainPhase;
		}
	}

	void Draw(Font font)
	{
		//タイトルを描画
		font(U"RythmGame").draw(Vec2{ 240, 200 }, ColorF{ 0.96 });

		// スタートボタンを描画
		pressedStartButton = SimpleGUI::Button(U"Start", Vec2{ 350, 380 }, unspecified);
	}
private:
	bool pressedStartButton = false;
};

// メインゲーム画面のクラス
class MainGameScreen
{
public:
	void Initialize()
	{
		keyTransitions = Array<Transition>(Keys.size(), Transition{ 0.0s, 0.2s });

		// 譜面データの読み込み&初期化
		notes = LoadNotes(U"notes.txt");

		//スコアリセット
		score = 0;
		combo = 0;
		comboMax = 0;

		// 時間計測を開始
		stopwatch.reset();
		stopwatch.start();

		// 音楽再生
		inGameBgmGroup[0].setVolume(0.4f);
		inGameBgmGroup[0].play();

		// ファイルをオープンできなかったら
		if (not writer)
		{
			// 例外を投げて終了する
			throw Error{ U"Failed to open `notesWrite.txt`" };
		}

		for (int i = 0; i < 3; i++)
		{
			seGroup[i].setVolume(0.8f);
		}
	};

	void Logic(GamePhase& gamePhase, GameResultState& gameResult, Font font)
	{
		// 鳴らすSEのインデックス
		int seIndex = 2;

		// 経過時間（ミリ秒）
		time = stopwatch.ms();

		ClearPrint();

		Print << time;

		////////////////////////////////
		//
		//	状態更新
		//
		////////////////////////////////

		for (size_t i = 0; i < Keys.size(); ++i)
		{
			keyTransitions[i].update(Keys[i].down());
		}

		for (auto& note : notes)
		{
			// 消えているノートはスキップ
			if (not note.active)
			{
				continue;
			}

			// 現在のタイムとノートのタイムとの差（ミリ秒）
			const int32 diffMillisec = (time - note.time);

			// 差の絶対値が 250 ミリ秒未満なら
			if (Abs(diffMillisec) < 250)
			{
				// ノートに対応するキーが押されていたら
				if (Keys[note.key].down())
				{
					// ノートを消す
					note.active = false;

					// ノートの座標
					const Vec2 notePos = GetNotePos(note, time);

					if (Abs(diffMillisec) < 80)
					{
						// エフェクトを追加する
						effect.add<NoteEffect>(Vec2{ notePos.x, 500 }, 2, font);
						seIndex = 0;
						score += scorePerNote;
					}
					else
					{
						// エフェクトを追加する
						effect.add<NoteEffect>(Vec2{ notePos.x, 500 }, 1, font);
						seIndex = 2;

						// スコア加算
						score += scorePerNote / 2;
					}

					// コンボ数と最大コンボの更新
					combo++;
					if(combo > comboMax)
					{
						comboMax = combo;
					}
				}
			}

			// 250 ミリ秒以上の遅れはミス
			if (note.active && (250 <= diffMillisec))
			{
				// ノートを消す
				note.active = false;

				// SEを鳴らす
				seGroup[1].playOneShot();

				combo = 0;
			}
		}

		// キータッチSEを鳴らす
		if (Keys[0].down() || Keys[1].down() || Keys[2].down() || Keys[3].down())
		{
			seGroup[seIndex].playOneShot();
		}

		// キーを保存
		for (int i = 0; i < 4; i++)
		{
			if (Keys[i].down())
			{
				writer << time << U" " << i;
			}
		}

		// ゲーム終了
		int noteIndexFinal = notes.size() - 1;
		if(time > notes[noteIndexFinal].time + 2000)
		{
			// bgm停止
			inGameBgmGroup[0].stop();

			// リザルトを格納
			gameResult = GameResultState(score, comboMax);

			// 画面をリザルトへ
			gamePhase = GamePhase::ResultPhase;
		}
	}

	void Draw(Font font)
	{
		////////////////////////////////
		//
		//	描画
		//
		////////////////////////////////

		// 入力を描画する
		for (int32 i = 0; i < 4; ++i)
		{
			const double x = (250 + i * 100);
			RectF{ Arg::bottomCenter(x, 600), 80, 600 }
			.draw(Arg::top = ColorF{ 1.0, 0.0 }, Arg::bottom = ColorF{ 1.0, keyTransitions[i].easeOut() * 0.5 });
		}

		// 長方形を描画する
		Rect{ 0, 480, 800, 40 }.draw(ColorF{ 0.5 });

		// キー名を描画する
		for (int32 i = 0; i < 4; ++i)
		{
			const double x = (250 + i * 100);
			font(Keys[i].name()).drawAt(20, Vec2{ x, 500 }, ColorF{ 0.7 });
		}

		// スコアを描画する
		font(score).draw(40, Vec2{ 60,100 }, ColorF{ 0.9f });

		// コンボを描画する
		font(combo).draw(40, Vec2{ 60,140 }, ColorF{ 0.9f });

		// ノートを描画する
		for (const auto& note : notes)
		{
			// 消えているノートはスキップ
			if (not note.active)
			{
				continue;
			}

			// ノートの座標
			const Vec2 notePos = GetNotePos(note, time);

			// 画面内にあるノートのみ描画する
			if (-100.0 < notePos.y)
			{
				Circle{ notePos, 30 }.draw();
			}
		}

		// エフェクトの描画
		effect.update();
	}

	private:
		Array<Audio> inGameBgmGroup
		{
			Audio{ Audio::Stream, U"bgm/artcore.wav", Loop::No}
		};

		Array<Audio> seGroup
		{
			Audio{ Audio::Stream, U"bgm/tanbarin.wav", Loop::No},
			Audio{ Audio::Stream, U"bgm/glassBreak.wav", Loop::No},
			Audio{ Audio::Stream, U"bgm/typingStoroke.wav", Loop::No},
		};

		// ノート配列
		Array<Note> notes = LoadNotes(U"notes.txt");

		// 判定キー
		const Array<Input> Keys = { KeyA, KeyS, KeyD, KeyF };

		// キー入力エフェクトのトランジション
		Array<Transition> keyTransitions;

		// 時間測定用ストップウォッチ
		Stopwatch stopwatch{ StartImmediately::No };

		// エフェクト管理
		Effect effect;

		// 最終スコア
		const int FINAL_SCORE = 100000000;

		// スコア
		int score = 0;

		// 1ノーツ当たりのスコアを計算
		int scorePerNote = FINAL_SCORE / notes.size();

		// コンボ
		int combo = 0;

		// 最大コンボ
		int comboMax = 0;

		// 打ち込んだキーの時間とキー番号を覚えてテキストに書き加える
		TextWriter writer{ U"notesWrite.txt" };

		int32 time;
};

// リザルト画面のクラス
class ResultScreen
{
public:
	void Initialize(GameResultState gameResult)
	{
		score = gameResult.GetScore();
		comboMax = gameResult.GetComboMax();
	}

	void Logic(GamePhase& gamePhase)
	{
		if (pressedRetryButton)
		{
			pressedRetryButton = false;

			// タイトルへ移行
			gamePhase = GamePhase::TitlePhase;
		}
	}

	void Draw(Font font)
	{
		// ゲーム結果を描画
		font(U"Result").draw(Vec2{ 240, 50 }, ColorF{ 0.96 });
		font(U"Score:{}"_fmt(score)).draw(Vec2{ 240, 150 }, ColorF{ 0.96 });
		font(U"MaxCombo:{}"_fmt(comboMax)).draw(Vec2{ 240, 250 }, ColorF{ 0.96 });

		// スタートボタンを描画
		pressedRetryButton = SimpleGUI::Button(U"Title", Vec2{ 350, 380 }, unspecified);
	}
private:
	int score = 0;
	int comboMax = 0;
	bool pressedRetryButton = false;
	
};

void Main()
{
	// フォント
	const Font font{ FontMethod::MSDF, 48, Typeface::Heavy };

	GamePhase gamePhase = GamePhase::TitlePhase;
	GamePhase gamePhaseLastFrame = GamePhase::TitlePhase;

	GameResultState gameResult = GameResultState(0, 0);

	// タイトル画面を作成
	TitleScreen title = TitleScreen();
	title.Initialize();

	// メイン画面を作成
	MainGameScreen mainGame = MainGameScreen();

	// リザルト画面を作成
	ResultScreen result = ResultScreen();
	
	while (System::Update())
	{
		// 前フレームから画面状態が変わっていれば各画面の初期化処理
		if(gamePhase != gamePhaseLastFrame)
		{
			switch (gamePhase)
			{
			case GamePhase::TitlePhase:
				title.Initialize();
				break;
			case GamePhase::MainPhase:
				mainGame.Initialize();
				break;
			case GamePhase::ResultPhase:
				result.Initialize(gameResult);
				break;
			default:
				break;
			}
		}

		// 前フレームのゲーム画面状態を格納
		// 毎フレーム処理でゲーム画面状態の変更を行うため、この段階で格納
		gamePhaseLastFrame = gamePhase;

		// 各画面の毎フレーム処理
		switch (gamePhase)
		{
		case GamePhase::TitlePhase:
			title.Logic(gamePhase);
			title.Draw(font);
			break;
		case GamePhase::MainPhase:
			mainGame.Logic(gamePhase, gameResult, font);
			mainGame.Draw(font);
			break;
		case GamePhase::ResultPhase:
			result.Logic(gamePhase);
			result.Draw(font);
			break;
		default:
			break;
		}
	}
}
