現在の実装（selfdrive/controls/controlsd.py）では、Auto Experimental Mode（AEM）を ON にすると 100 Hz（10 ms 周期）で次のフローが回ります。
前提となる入力
車速 v_ego_kmh ＝ CS.vEgo * CV.MS_TO_KPH
アクセルペダル踏み込み状態 gas_pressed ＝ CS.gasPressed
Gas-Gating 状態 gas_gating_active ＝ !long_plan.allowThrottle
（ロングプランナーがスロットルを許可していない＝「ガスゲーティング発動中」）
ON 条件（すべて満たす）
AEM トグルが ON (AutoExperimentalMode == true)
車速 ≤ 55 km/h
gas_gating_active が TRUE
アクセルペダルを踏んでいない (gas_pressed == false)
現在 Experimental Mode が OFF
→ 条件成立時、即座に ExperimentalMode パラメータを true に設定し Experimental Mode を自動 ON。
OFF 条件（いずれか）
Experimental Mode が ON かつ
車速 > 55 km/h（56 km/h 以上）
または アクセルペダル踏み込み (gas_pressed == true)
→ 条件成立時、ExperimentalMode を false に設定し自動 OFF。
ループ挙動（時系列イメージ）
① ユーザが Settings › Toggles で
・Experimental Mode = ON
・Auto Experimental Mode = ON
にすると、その時点では Experimental Mode は まだ ON のまま（手動で ON したため）。
② 走り出して車速が 56 km/h を超えると OFF 条件にヒットし、Experimental Mode が自動で OFF。
③ 信号待ちなどで速度が 55 km/h 以下になり、かつ Gas-Gating が発動すると ON 条件を満たすので自動で再び ON。
④ この ON 状態でアクセルを軽く踏むと即座に OFF。
⑤ アクセルを戻し、再び Gas-Gating＋≤ 55 km/h を満たせば再度 ON。
…以後、条件に応じて何度でも ON/OFF を繰り返す。
ユーザ操作との関係
AEM が ON でも、ユーザが Settings 画面で Experimental Mode を OFF にすると AEM も同時に OFF へ変更（UI ロジックで強制的に落とす）→ 手動優先。
AEM が OFF の間は一切の自動切替えは行われず、従来通りユーザのトグル操作のみで切替え。
まとめ
Auto Experimental Mode ON 直後から、上記の「55 km/h と Gas-Gating & アクセル」の条件監視が始まり、満たした瞬間に Experimental Mode を自動で ON/OFF します。どちらかの条件が外れれば即座に OFF、再び満たせばいつでも再 ON される――これが実装された挙動です