# include <Siv3D.hpp> // Siv3D v0.6.14

struct Player
{
	Circle circle{ 400, 530, 28 };

	Texture textureAlive{ U"👶"_emoji };
	Texture textureDead{ U"😭"_emoji };

	// プレイヤーの状態を更新する関数
	void update(double deltaTime)
	{
		const double sprintSpeed = (deltaTime * 400.0);
		const double walkSpeed = (deltaTime * 200.0);

		if (KeyLShift.pressed()){
			// [A] キーが押されたら左に移動
			if (KeyA.pressed())
			{
				circle.x -= sprintSpeed;
			}
	
			// [D] キーが押されたら右に移動
			if (KeyD.pressed())
			{
				circle.x += sprintSpeed;
			}

		}
		else {
			if (KeyA.pressed())
			{
				circle.x -= walkSpeed;
			}
	
			// [D] キーが押されたら右に移動
			if (KeyD.pressed())
			{
				circle.x += walkSpeed;
			}
		}

		// プレイヤーが画面外に出ないようにする
		circle.x = Clamp(circle.x, 28.0, 772.0);
	}

	void setDefaultValue() {
		circle.x = 400;
	};

	// プレイヤーを描く関数
	void draw(const bool alive) const
	{
		if (alive){
			textureAlive.scaled(0.5).drawAt(circle.center);
		}
		else {
			textureDead.scaled(0.5).drawAt(circle.center);
		}
	}
};

// アイテムクラス
struct Item
{
	Circle circle;

	// アイテムの種類（0: モアイ, 1: おばけ, 2:めてお）
	int32 type;

	void update(double deltaTime)
	{
		
		// アイテムを下に移動させる
		if (type==0) {
				circle.y += (deltaTime * 250.0);
		}
		else if (type==1){
				circle.y += (deltaTime * 200.0);
		}
		else {
			circle.y += (deltaTime * 350.0);
		}
		
	}

	// アイテムを描く関数
	void draw(const Array<Texture>& itemTextures) const
	{
		// アイテムの種類に応じたテクスチャを描く
		itemTextures[type].scaled(0.5).rotated(circle.y * 0.3_deg).drawAt(circle.center);
	}


};

struct Difficulty {
	// アイテムが出現する周期（秒）
	double spawnInterval = 1;

	// 蓄積時間（秒）
	double accumulatedTime = 0;

	// スコア
	int32 health = 1;

	// 残り時間（秒）
	double remainingTime = 1;

	// アイテムの出現回数
	int32 spawnTimes = 0;

	void setValue(const int32 diff) {
		accumulatedTime = 0.0;
		spawnTimes = 0;

		if (diff == 0) {
			spawnInterval = 0.08;
			health = 500;
			remainingTime = 20.0;
		}
		else if (diff == 1){
			spawnInterval = 0.055;
			health = 1000;
			remainingTime = 30.0;
		}
		else {
			spawnInterval = 0.05;
			health = 2000;
			remainingTime = 40.0;
		}
	}
};

struct SaveData
{
	Array<int32> highscores = {0, 0, 0};

	template <class Archive>
	void SIV3D_SERIALIZE(Archive& archive)
	{
		archive(highscores);
	}

};

void UpdateItems(Array<Item>& items, double deltaTime, const Player& player, int32& score, const Audio& audio)
{
	// すべてのアイテムの状態を更新する
	for (auto& item : items)
	{
		item.update(deltaTime);
	}

	// 各アイテムについて
	for (auto it = items.begin(); it != items.end();)
	{
		// プレイヤーとアイテムが交差したら
		if (player.circle.intersects(it->circle))
		{
			// 体力を減算する（モアイ: 10点, おばけ: 50点, めてお: 100点）
			if (it->type == 0) {
				score -= 10;
			}
			else if (it->type == 1) {
				score -= 50;
			}
			else {
				score -= 100;
			}

			// アイテムを削除する
			it = items.erase(it);

			audio.playOneShot();
		}
		else
		{
			++it;
		}
	}

	// 地面に落下したアイテムを削除する
	items.remove_if([](const Item& item) { return (628 < item.circle.y); });
}

// 背景画面を描く関数
void DrawBackground(const bool alive, const int32 diff)
{
	if (alive) {
		if (diff==0) {
			// 空を描く
			Rect{ 0, 0, 800, 550 }.draw(Arg::top(0.3, 0.6, 1.0), Arg::bottom(0.6, 0.9, 1.0));
			// 地面を描く
			Rect{ 0, 550, 800, 50 }.draw(Arg::top( 0.3, 0.6, 0.3 ), Arg::bottom( 0.28, 0.5, 0.28 ));
		}
		else if (diff == 1){
			Rect{ 0, 0, 800, 550 }.draw(Arg::top(0.98, 0.38, 0.0), Arg::bottom(0.95, 0.78, 0.59));
			Rect{ 0, 550, 800, 50 }.draw(Arg::top( 0.21, 0.36, 0.21 ), Arg::bottom( 0.22, 0.44, 0.22 ));
		}
		else if(diff == 2){
			Rect{ 0, 0, 800, 550 }.draw(Arg::top(0.13, 0.15, 0.46), Arg::bottom(0.28, 0.30, 0.61));
			Rect{ 0, 550, 800, 50 }.draw(Arg::top( 0.21, 0.36, 0.21 ), Arg::bottom( 0.22, 0.44, 0.22 ));
		}

	}
	else {
		Rect{ 0, 0, 800, 550 }.draw(Arg::top(0.7, 0.4, 0.0), Arg::bottom(0.4, 0.1, 0.0));
		Rect{ 0, 550, 800, 50 }.draw(ColorF{ 0.7, 0.4, 0.7 });
	}
}

// アイテムを描く関数
void DrawItems(const Array<Item>& items, const Array<Texture>& itemTextures)
{
	for (const auto& item : items)
	{
		item.draw(itemTextures);
	}
}

// UI を描く関数
void DrawUI(int32 health, double remainingTime, const Font& font)
{
	// スコアを描く
	font(U"HP: {}"_fmt(health)).draw(45, Vec2{ 20, 20 });

	// 残り時間を描く
	font(U"TIME: {:.0f}"_fmt(remainingTime)).draw(45, Arg::topRight(780, 20));

	font(U"R[クイックリトライ]").drawAt(15, Vec2{ 650, 570 }, ColorF{ 0.1 });
	font(U"操作方法：A[←] D[→]  Lshift[ダッシュ]").drawAt(15, Vec2{ 400, 570 }, ColorF{ 0.1 });
}

void DrawVicUI(const int32 score, const int32 highscore, const bool refreshed, const Font& font, const int32 diff)
{
	font(U"SCORE: {}"_fmt(score)).draw(45, Vec2{ 20, 20 });
	font(U"HIGH SCORE: {}"_fmt(highscore)).draw(45, Arg::topRight(780, 20));
	if (diff == 2) {
		font(U"VICTORY").drawAt(120, Vec2{ 400, 270 }, ColorF{ 0.7 });
		if (refreshed) {
		font(U"NEW RECORD!").drawAt(40, Vec2{ 400, 400 }, ColorF{ 0.7 });
		}
	}
	else {
		font(U"VICTORY").drawAt(120, Vec2{ 400, 270 }, ColorF{ 0.3 });
		if (refreshed) {
		font(U"NEW RECORD!").drawAt(40, Vec2{ 400, 400 }, ColorF{ 0.3 });
		}
	}
	
	
	font(U"Spaceを押してハイスコアを目指す").drawAt(15, Vec2{ 400, 570 }, ColorF{ 0.1 });
	font(U"Tを押してタイトル").drawAt(15, Vec2{ 120, 570 }, ColorF{ 0.9 });

	if (SimpleGUI::Button(U"ハイスコアをツイート", Vec2{ 540, 450 }))
	{
		
		if (diff == 0) {
			// ハッシュタグや URL を含めると広まりやすいです。
			String text = U"避けゲーで {} 点取ったよ！\n#Test #Siv3D\nhttps://github.com/Siv3D/OpenSiv3D"_fmt(ThousandsSeparate(highscore));
			// ツイート投稿画面を開く
			Twitter::OpenTweetWindow(text);
		}
		else if (diff == 1){
			String text = U"避けゲー(HARDモード)で {} 点取ったよ！\n#Test #Siv3D\nhttps://github.com/Siv3D/OpenSiv3D"_fmt(ThousandsSeparate(highscore));
			// ツイート投稿画面を開く
			Twitter::OpenTweetWindow(text);
		}
		else if (diff == 2){
			String text = U"避けゲー(VERY HARD)で {} 点取ったよ！\n#Test #Siv3D\nhttps://github.com/Siv3D/OpenSiv3D"_fmt(ThousandsSeparate(highscore));
			// ツイート投稿画面を開く
			Twitter::OpenTweetWindow(text);
		}

	}
}

void DrawTitleUI(const Font& font)
{
	font(U"避けゲー").drawAt(100, Vec2{ 400, 260 }, ColorF{ 0.2 });
	font(U"赤ちゃんを外敵から守ろう！").drawAt(25, Vec2{ 400, 420 }, ColorF{ 0.3 });
	font(U"Spaceを押してスタート").drawAt(15, Vec2{ 400, 570 }, ColorF{ 0.1 });
	font(U"Ver.1.22").drawAt(15, Vec2{ 700, 520 }, ColorF{ 0.3 });
	font(U"ハイスコア[Q]").draw(30, Vec2{ 10, 10 }, ColorF{ 0.1 });
}

void DrawScoreUI(const Font& font, const Array<int32> highscore) {

	font(U"HIGH SCORE").drawAt(50, Vec2{ 400, 80 }, ColorF{ 0.1 });
	font(U"戻る[Q]").draw(30, Vec2{ 10, 10 }, ColorF{ 0.1 });
	font(U"Normal : {}"_fmt(highscore[0])).draw(40, Vec2{100, 120}, ColorF{0.1});
	if (highscore[0] > 0) {
		font(U"Hard : {}"_fmt(highscore[1])).draw(40, Vec2{100, 180}, ColorF{0.1});
		if (highscore[1] > 0) {
			font(U"Very Hard : {}"_fmt(highscore[2])).draw(40, Vec2{100, 240}, ColorF{0.1});
		}
	}
	font(U"セーブデータ削除[Delete]+[0]").drawAt(15, Vec2{ 400, 570 }, ColorF{ 0.1 });
	

}

void DrawGOUI(const Font& font)
{
	font(U"GAME OVER").drawAt(80, Vec2{ 400, 270 }, ColorF{ 0.9 });
	font(U"Spaceを押してリトライ").drawAt(15, Vec2{ 400, 570 }, ColorF{ 0.1 });
}

void Main()
{
	System::SetTerminationTriggers(UserAction::CloseButtonClicked);

	const Font font{ FontMethod::MSDF, 48, Typeface::Bold };
	const Audio audio_playing{ Resource(U"audio/maou_game_town24.ogg") };
	const Audio audio_go{ Resource(U"audio/maou_game_piano07.ogg") };
	const Audio audio_vic{ Resource(U"audio/MusMus-JGL-012.mp3") };
	const Audio audio_oh{ Resource(U"audio/musmus_cancel_set/cncl06.mp3") };
	audio_go.setVolume(0.9);

	// アイテムのテクスチャ配列
	const Array<Texture> itemTextures =
	{
		Texture{ U"🗿"_emoji },
		Texture{ U"👻"_emoji },
		Texture{ U"☄️"_emoji}
	};

	Player player;
	Difficulty difficulty;

	// アイテムの配列
	Array<Item> items;
	items << Item{ Circle{ 200, 200, 30 }, 0 };
	items << Item{ Circle{ 400, 150, 30 }, 1 };


	int32 sceneId = 0;
	int32 diffId = 0;
	Array<int32> highscore;

	bool scoreRefreshed = false;

	// セーブデータが見つかればそれを読み込む
    {
        // バイナリファイルをオープン
        Deserializer<BinaryReader> reader{ U"save.bin" };

        if (reader) // もしオープンに成功したら
        {
            SaveData saveData;

            reader(saveData);

			highscore = saveData.highscores;
        }
		else
		{
			highscore = {0, 0, 0};
		}
    }


	while (System::Update())
	{
		if (KeyEscape.down()) {
			sceneId = 0;

			audio_playing.stopAllShots();
			audio_go.stopAllShots();
			audio_vic.stopAllShots();
			audio_vic.stopAllShots();
			audio_oh.stopAllShots();

			difficulty.setValue(diffId);		
			items.clear();
		}

		const double deltaTime = Scene::DeltaTime();

		switch (sceneId){
			case 0:

				// デバッグモード (完成したら絶対消せ)
				if (false) {
					highscore[0] = 10;
					highscore[1] = 10;
				}


				// 背景を描く
				DrawBackground(true, 0);
				DrawTitleUI(font);
			
				if (KeySpace.down()) {
					diffId = 0;
					difficulty.setValue(diffId);
					sceneId = 1;
					player.setDefaultValue();
					audio_playing.playOneShot();
				}

				if (highscore[0] > 0) {
					font(U"Pを押してHardモード").drawAt(15, Vec2{ 620, 565 }, ColorF{ 0.9 });
					if (KeyP.down()) {
						diffId = 1;
						difficulty.setValue(diffId);
						sceneId = 1;
						player.setDefaultValue();
						audio_playing.playOneShot();
					}

					if (highscore[1] > 0) {
						font(U"*を押してVery Hard").drawAt(15, Vec2{ 620, 585 }, ColorF{ 0.9 });
						if (KeyColon_JIS.down() || KeyNumMultiply.down()) {
						diffId = 2;
						difficulty.setValue(diffId);
						sceneId = 1;
						player.setDefaultValue();
						audio_playing.playOneShot();
						}

					}
				}

				if (KeyQ.down()) {
					sceneId = 4;
				}
				break;

			case 1:

				// ゲームが進行している場合
				if (0.0 < difficulty.remainingTime && 0 < difficulty.health)
				{

					// 残り時間を減らす
					difficulty.remainingTime = Max((difficulty.remainingTime - deltaTime), 0.0);

					// 蓄積時間を増やす
					difficulty.accumulatedTime += deltaTime;

					// 蓄積時間が周期を超えたら
					if (difficulty.spawnInterval < difficulty.accumulatedTime)
					{
						// 新しいアイテムを追加する
						if (diffId == 0) {
							if (0 <= difficulty.spawnTimes % 10 && difficulty.spawnTimes % 10 < 8) {
								items << Item{ Circle{ Random(30.0, 770.0), -30, 30 }, 0 };
							}
							else if (8 <= difficulty.spawnTimes % 10 && difficulty.spawnTimes % 10 < 10) {
								items << Item{ Circle{ Random(30.0, 770.0), -30, 30 }, 1 };
							}
						}
						else if (diffId == 1) {
							if (0 <= difficulty.spawnTimes % 10 && difficulty.spawnTimes % 10 < 5) {
								items << Item{ Circle{ Random(30.0, 770.0), -30, 30 }, 0 };
							}
							else if (5 <= difficulty.spawnTimes % 10 && difficulty.spawnTimes % 10 < 10) {
								items << Item{ Circle{ Random(30.0, 770.0), -30, 30 }, 1 };
							}
						}
						else if (diffId == 2){
							if (0 <= difficulty.spawnTimes % 10 && difficulty.spawnTimes % 10 < 4) {
								items << Item{ Circle{ Random(30.0, 770.0), -30, 30 }, 0 };
							}
							else if (4 <= difficulty.spawnTimes % 10 && difficulty.spawnTimes % 10 < 8) {
								items << Item{ Circle{ Random(30.0, 770.0), -30, 30 }, 1 };
							}
							else {
								items << Item{ Circle{ Random(30.0, 770.0), -30, 30 }, 2 };
							}
						}

						// 出現回数を増やす
						difficulty.spawnTimes++;

						// 蓄積時間を周期分減らす
						difficulty.accumulatedTime -= difficulty.spawnInterval;
					}

					// プレイヤーの状態を更新する
					player.update(deltaTime);

					// すべてのアイテムの状態を更新する
					UpdateItems(items, deltaTime, player, difficulty.health, audio_oh);
				}
				else
				{
					player.setDefaultValue();
					audio_playing.stopAllShots();
					audio_oh.stopAllShots();
					items.clear();
				}

				if (KeyR.down()) {
					player.setDefaultValue();
					audio_playing.stopAllShots();
					audio_oh.stopAllShots();

					difficulty.setValue(diffId);
					audio_playing.playOneShot();

					items.clear();
				}

				/////////////////////////////////
				//
				//	描画
				//
				/////////////////////////////////

				// 背景を描く
				DrawBackground(true, diffId);

				// プレイヤーを描く
				if (0 < difficulty.health) {
					player.draw(true);
					
					if (0.0 >= difficulty.remainingTime) {
						audio_playing.stopAllShots();
						audio_oh.stopAllShots();

						sceneId = 3;

						if (diffId == 0) {
							audio_vic.playOneShot();
						}
						else {
							audio_vic.playOneShot();
						}
					}
				}
				else {
					audio_playing.stopAllShots();
					audio_oh.stopAllShots();
					audio_go.playOneShot();
					sceneId = 2;
				}

				// すべてのアイテムを描く
				DrawItems(items, itemTextures);
	
				// UI を描く
				DrawUI(difficulty.health, difficulty.remainingTime, font);
				break;

			case 2:
				// 背景を描く
				DrawBackground(false, diffId);
				player.draw(false);

				// UI を描く
				DrawGOUI(font);

				if (KeySpace.down())
				{
					audio_go.stopAllShots();
					difficulty.setValue(diffId);
					audio_playing.playOneShot();
					player.setDefaultValue();

					sceneId = 1;
					items.clear();
				}
				break;

			case 3:
				DrawBackground(true, diffId);
				player.draw(true);
				

				if (difficulty.health > highscore[diffId] && not scoreRefreshed) {
					highscore[diffId] = difficulty.health;
					scoreRefreshed = true;
				}

				DrawVicUI(difficulty.health, highscore[diffId], scoreRefreshed, font, diffId);

				if (KeySpace.down())
				{
					sceneId = 1;
					audio_vic.stopAllShots();
					audio_vic.stopAllShots();
					difficulty.setValue(diffId);
					items.clear();
					player.setDefaultValue();
					audio_playing.playOneShot();

					scoreRefreshed = false;
				}
				if (KeyT.down())
				{
					sceneId = 0;
					audio_vic.stopAllShots();
					audio_vic.stopAllShots();
					difficulty.setValue(diffId);
					items.clear();

					scoreRefreshed = false;
				}
				break;

			case 4:
				diffId = 0;
				DrawBackground(true, diffId);
				DrawScoreUI(font, highscore);
				if (KeyQ.down()) {
					sceneId = 0;
				}

				if (KeyDelete.pressed() && Key0.pressed()) {
					sceneId = 0;
					highscore = {0, 0, 0};
				}


				break;
				
			default:
				Print << U"error";
				break;


		}
	}

	{
	//バイナリデータの書き込み
	Serializer<BinaryWriter> writer{ U"save.bin" };
	writer(SaveData{highscore});
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
