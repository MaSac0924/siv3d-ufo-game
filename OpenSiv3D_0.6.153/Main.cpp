#include <Siv3D.hpp>

//
//	おもな変更・追加内容
//
//	1. プレイヤーのHPを追加した
//	2. 敵と衝突したら即ゲームオーバーするようにした
//	3. 敵のHPのバーを表示するようにした,なおかつダメージを受けると減るようにした
//	
//	
//	...
//

// 敵クラス
struct Enemy
{
	// 円
	Circle circle;

	// HP（0 になると破壊）
	double hp;
};

// 敵が画面外に出たかどうかを判定する関数
bool CheckRemove(const Enemy& enemy)
{
	return (800 < enemy.circle.y);
}

// 敵の HP が 0 以下かどうかを判定する関数
bool CheckHP(const Enemy& enemy)
{
	return (enemy.hp <= 0.0);
}

void Main()
{
	// 背景色を設定する
	Scene::SetBackground(ColorF{ 0.5, 0.3, 1.0 });

	// 絵文字テクスチャ
	const Texture emojiEnemy{ U"🛸"_emoji };
	const Texture emojiPlayer{ U"🤖"_emoji };

	// フォント
	const Font fontBold{ FontMethod::MSDF, 48, Typeface::Bold };

	// 敵が登場する時間間隔（秒）
	const double interval = 0.8;

	// 敵の配列
	Array<Enemy> enemies;

	// プレイヤー
	Circle player{ 400, 530, 30 };

	// プレイヤーのHPを追加したーーーーーーーーーーーーー
	double playerHP = 1.0;

	// 蓄積時間（秒）
	double accumulatedTime = 0.0;

	while (System::Update())
	{
		//ClearPrint();
		//Print << U"敵の数: " << enemies.size();

		const double deltaTime = Scene::DeltaTime();

		// // 敵と衝突したら即ゲームオーバーするようにした----
		if (playerHP <= 0.0)
		{
			fontBold(U"Game Over!").drawAt(Scene::Center(), Palette::Red);
			continue; 
		}

		////////////////////////////////////////////////////////////////
		//
		//	プレイヤーの移動
		//
		////////////////////////////////////////////////////////////////

		if (KeyLeft.pressed()) // [←] キーが押されている
		{
			// プレイヤーを左に移動させる
			player.x -= (400 * deltaTime);
		}
		else if (KeyRight.pressed()) // [→] キーが押されている
		{
			// プレイヤーを右に移動させる
			player.x += (400 * deltaTime);
		}

		// プレイヤーが画面外に行かないようにする
		if (player.x < 40)
		{
			player.x = 40;
		}
		else if (760 < player.x)
		{
			player.x = 760;
		}

		////////////////////////////////////////////////////////////////
		//
		//	敵の出現と移動
		//
		////////////////////////////////////////////////////////////////

		// 蓄積時間を増やす
		accumulatedTime += deltaTime;

		// 蓄積時間が一定の時間を超えたら
		if (interval < accumulatedTime)
		{
			// 敵を追加する
			enemies << Enemy{ { Random(40, 760), -50, 20 }, 0.5 };

			// 蓄積時間を減らす
			accumulatedTime -= interval;
		}

		// すべての敵を下に移動させる
		for (auto& enemy : enemies)
		{
			// 敵を下に移動させる
			enemy.circle.y += (200 * deltaTime);
		}

		////////////////////////////////////////////////////////////////
		//
		//	プレイヤーの攻撃（ビーム）
		//
		////////////////////////////////////////////////////////////////

		// ビームを表現する長方形
		const RectF beamRect{ (player.x - 5), 0, 10, 500 };

		// 各敵について
		for (auto& enemy : enemies)
		{
			// ビームがあたっていたら
			if (beamRect.intersects(enemy.circle))
			{
				// HP を削る
				enemy.hp -= deltaTime;

				if (enemy.hp <= 0.0)
				{
					// 倒したときの処理をここに書ける
				}
			}
		}

		//一度でも敵に衝突したら即ゲームオーバーする機能を追加した----------------------------
		for (const auto& enemy : enemies)
		{
			if (enemy.circle.intersects(player))
			{
				playerHP = 0.0; 
				break;
			}
		}

		////////////////////////////////////////////////////////////////
		//
		//	敵の削除
		//
		////////////////////////////////////////////////////////////////

		// 画面外の敵を削除する
		// ヒント: この前後で enemies.size() を比べると、倒せずに画面外に出た敵の数がわかる
		enemies.remove_if(CheckRemove);

		// HP が 0.0 以下の敵を削除する
		enemies.remove_if(CheckHP);

		////////////////////////////////////////////////////////////////
		//
		//	描画
		//
		////////////////////////////////////////////////////////////////


		// 敵を描画する
		for (const auto& enemy : enemies)
		{
			// 敵のHPバーの表示するようにしたーーーーーーーーーーーーー
			constexpr double enemySize = 40.0;
			constexpr double maxBarWidth = 40.0;
			constexpr double maxHP = 0.5;
			
			Vec2 hpBarPos = enemy.circle.center - Vec2{ maxBarWidth / 2, enemySize / 2 + 10 };
			RectF(hpBarPos, maxBarWidth, 5).draw(ColorF{ 0.3 });
			RectF(hpBarPos, (enemy.hp / maxHP) * maxBarWidth, 5).draw(ColorF{ 1.0, 0.2, 0.2 });

			// 敵の描画
			if (enemy.circle.intersects(beamRect)) // ビームがあたっていたら
			{
				emojiEnemy.scaled(0.5 + Random(0.0, 0.15)).drawAt(enemy.circle.center);
			}
			else
			{
				emojiEnemy.scaled(0.5).drawAt(enemy.circle.center);
			}
		}


		// プレイヤーを描画する
		emojiPlayer.scaled(0.5).drawAt(player.center);

		// ビームを描画する
		beamRect.draw(ColorF{ 1.0, 0.8, 0.6 });
	}
}
