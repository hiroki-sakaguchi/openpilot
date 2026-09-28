# この Fork の CI:既知の失敗と、その許容理由

`hiroki-sakaguchi/openpilot` は、comma の v0.11.1 のソース(`zeroelevenone` ブランチ)をもとにしています。
本家と同じ GitHub Actions(`.github/workflows/tests.yaml`)が動きますが、Fork では構造上どうしても通らないチェックがあります。
このファイルには、どの失敗を許容しているか、なぜ許容できるか、それでも毎回確認すべきことを書いておきます。

## まとめ

| チェック | 状態 | 扱い |
|---|---|---|
| process replay | 常に失敗する | 許容する。ただしログで Traceback の中身を必ず確認する。エラーがなくても、動かなかったプロセスがあるので安全の証明にはならない |
| comment(diff report) | process replay の結果次第で失敗することがある | 許容する |
| simulator driving | 常にスキップされる | 本家で無効化されている(`if: false`) |
| unit tests | 通るはず(約7分) | **失敗・タイムアウトは許容しない** |
| static analysis / build release / build macOS / Create UI Report / build docs | 通るはず | **失敗は許容しない** |

加えて、**CI では modeld(運転モデル)が一度も実行されない**という穴があります。modeld に手を入れたときは、下の「modeld の確認方法」で手元で確かめてください。

## process replay:常に失敗する

### 症状
`Run replay` のステップが、次のエラーで失敗します。
```
AssertionError: Remote file is empty or doesn't exist: https://raw.githubusercontent.com/commaai/ci-artifacts/refs/heads/process-replay/<segment>_controlsd_49bbba371...
```

### 原因
- process replay は、過去の走行ログを各プロセス(card、controlsd など)に流して、出力が comma の保管している正解の出力と一致するかを比べるテストです。
- 正解の出力は `commaai/ci-artifacts` の `process-replay` ブランチに保管されています。ここには comma の `master` の各コミットの分しかありません。
- この Fork のもとになった `zeroelevenone` は、リリース用の枝です。参照先のコミット(`49bbba371...`)の正解データは保管されておらず、ダウンロードの段階で失敗します。
- 正解データを置く場所は comma のリポジトリなので、Fork 側からは直せません。

### 変更とは関係ないことの証拠
変更を一切入れていない `hiroki-mici`(`zeroelevenone` そのまま)で `tests` を手動実行しても、同じエラーで失敗しました。
- 実行:https://github.com/hiroki-sakaguchi/openpilot/actions/runs/36387636222(2026-09-28)

### どのプロセスが実際に動いているか
`test_processes.py` は「走行ログ × プロセス」の組み合わせごとに、`run_test_process()` で**先に正解データを読み込み、そのあとでプロセスを動かします**。
- 正解データが**ある**組み合わせ:プロセスは実際に動く。落ちればログに Traceback が出る。
- 正解データが**ない**組み合わせ:ダウンロードの時点で例外になり、**プロセスは一度も動かない**。
- さらに、結果を集める側は最初の例外で止まるので、どの組み合わせが動いたかはログから分からない。

つまり、この Fork の process replay は「一部のプロセスが動く」だけで、どこまで確かめられたかは実行ごとに不明です。

### それでも毎回ログを確認する理由
動いた組み合わせで落ちれば、ログに Traceback が出ます。実例として、PR #1 の最初の実行では、card が Toyota で `TypeError` を出して落ちる不具合がここで見つかりました。そのままマージしていたら、実車で openpilot が動かなくなるところでした。

確認方法(`<RUN_ID>` は該当する実行の ID):
```bash
gh run view <RUN_ID> --repo hiroki-sakaguchi/openpilot --log | grep "^process replay" | grep -E "Traceback|Error" | sed 's/.*Z //' | sort | uniq -c
```
- それ以外の Traceback(`File ".../selfdrive/..."` で始まる、`TypeError` など)が出ていたら、**変更による不具合です。マージしてはいけません。**
- `Remote file is empty or doesn't exist` と、それに付随する `concurrent.futures` の Traceback **だけ**なら、CI としてはこれ以上分からない。**落ちないことの証明にはならない**ので、次のように扱う。
  - 変更が card・controlsd・plannerd・selfdrived・radard など、process replay の対象プロセスに関わる場合:単体テストに加えて、手元でそのプロセスを走行ログに流して確かめる(下の「対象プロセスを手元で動かす方法」)。
  - 対象プロセスに関わらない変更(UI だけ、modeld だけなど)の場合:process replay の結果は判断材料にならないので、それぞれの確認方法で確かめる。

### 対象プロセスを手元で動かす方法(未整備)
正解データなしで、決まったプロセスを走行ログに流して「落ちないこと」だけを確かめる手順は、**まだ確立できていません**。分かっていることを書いておきます。
- `test_processes.py --update-refs` は、正解データの代わりに元の走行ログを比べる相手にするので、全ての組み合わせでプロセスが動きます。ただし**対象を絞れません**(プロセスや車種を絞ると `Need to run full test when updating refs` で止まる)。全プロセス・全車種の走行ログを取得して動かすことになり、macOS では車速計算(MPC)が正しく動かないので Linux が必要です。この方法で最後まで動かしたことはまだありません。また、`selfdrive/test/process_replay/ref_commit` が書き換わるので、コミットに含めないでください。
- 対象を絞って動かす小さなスクリプト(`replay_process()` で動かし、落ちたら失敗にする)を試作しています。確認が終わったら別の PR で追加し、ここを書き換える予定です。
- 気をつける点:CI 用の走行ログの「TOYOTA」は **TOYOTA_PRIUS(2016〜20、TSS-P)** で、openpilot が加減速を制御しない車種です。レバーでの切り替えのように「openpilot が加減速を制御する Toyota」だけで動く処理は、この走行ログでは実行されません。その場合は「TOYOTA3」(Corolla TSS2)を使います。
- 走行ログに含まれない操作(たとえばクルコンレバーの操作)を通る処理は、走行ログを流しても実行されません。単体テストで確かめてください。

### process replay がカバーしないもの
`selfdrive/test/process_replay/test_processes.py` は `EXCLUDED_PROCS = {"modeld", "dmonitoringmodeld"}` としていて、**modeld と dmonitoringmodeld は対象外**です。

## comment(diff report):失敗することがある
`.github/workflows/diff_report.yaml` の `comment` ジョブは、process replay の完了を待ち、その結果から差分のレポートを作ります。process replay が上記の理由で失敗するので、このジョブも失敗することがあります。これは process replay の失敗の巻き添えなので許容します。

## simulator driving:常にスキップされる
本家の `tests.yaml` で `if: false  # FIXME: Started to timeout recently` として無効になっています。

## unit tests:失敗もタイムアウトも許容しない
- 通常は約7分で通ります。制限時間は20分です(本家は専用マシンで2分、Fork は GitHub の標準マシンで20分)。
- PR #1 の最初の実行では、98%まで進んだところで20分の制限時間切れになりました。このとき card が落ちる不具合が入っていて、その修正後の実行は約7分で通っています。そのため、**時間切れは「マシンが遅い」ではなく、何かが止まっている疑いとして扱ってください。**
- `hiroki-mici` での実行でも通っているので、ここでの失敗は変更によるものと考えます。

## CI で modeld が実行されないこと

### 状況
- process replay は modeld を対象から外しています(上記)。
- `selfdrive/test/process_replay/model_replay.py` は、`tests.yaml` から呼ばれていません。
- regen(`if: false`)と simulator driving(`if: false`)は無効です。

このため、`selfdrive/modeld/` の変更は、CI では単体テスト以外で一度も実行されません。

### modeld の確認方法(手元の mac)
modeld に手を入れたときは、comma が公開している CI 用の走行ログで modeld を実際に動かして確かめます。

1. 走行ログをダウンロードする(約160MB)。`urllib3` 経由だと細切れに取得して非常に遅いので、先に curl で落とす。
   ```bash
   B=https://commadataci.blob.core.windows.net/openpilotci/8494c69d3c710e81/000001d4--2648a9a404/4
   mkdir -p /tmp/op_route && (cd /tmp/op_route && for f in rlog.zst fcamera.hevc ecamera.hevc; do curl -sO $B/$f; done)
   ```
2. リポジトリのフォルダで `tools/fork/modeld_camera_offset_check.py` を実行する(`CameraOffset` を 0 と 15cm で1回ずつ modeld を動かし、出力件数と車線の中心の位置を表示する)。
   ```bash
   source .venv/bin/activate
   tools/fork/modeld_camera_offset_check.py /tmp/op_route
   ```
   - 中身は `process_replay.replay_process(get_process_config("modeld"), ...)` で、ログの切り出しは `model_replay.py` の `model_replay()` と同じ手順。camera offset 以外の変更を確かめるときも、これをもとに書き換えて使える。
   - mac では子プロセスを起動するときスクリプトを読み直すので、自分でスクリプトを書くときは本体を必ず `if __name__ == "__main__":` の中に書く。

## 手元の環境で出る、変更と無関係の失敗

| 環境 | 症状 | 扱い |
|---|---|---|
| macOS | 車速計算(MPC)を使うテスト(`test_following_distance.py` など)が `SQP_RTI: QP solver returned error status 3` を出し、車が動かない結果になって失敗する | 変更前のコードでも同じ。MPC のテストは Linux で実行する |
| Docker の Linux(ARM、Apple Silicon 上) | `selfdrive/test/longitudinal_maneuvers/test_longitudinal.py` の4件が失敗する | 変更前のコードでも同じ4件が失敗する。CI(x86)では通っている |
| Docker にソースを tar で渡すとき | `._*` という mac 固有の付属ファイルが混ざり、DBC の生成やフォントの処理が壊れる | コンテナ内で `find . -name "._*" -delete` を実行するか、`COPYFILE_DISABLE=1 tar ...` で作る |

## 確認の記録

| PR | 内容 | CI | 補足 |
|---|---|---|---|
| #1 | 停車時の車間の設定、クルコンレバーでのモード切り替え | process replay 以外はすべて成功 | 最初の実行で card が落ちる不具合を process replay のログで発見し、修正済み |
| #2 | カメラのずれの補正、車線内の位置の表示 | process replay 以外はすべて成功 | 変更は modeld と UI だけで、どちらも process replay の対象外。modeld は手元で確認(下記)、UI は単体テストと手元での描画で確認 |
| #3 | アクセルを抜くタイミングの設定(coast before stops) | process replay 以外はすべて成功。process replay のログは既知のダウンロード失敗だけ | plannerd は process replay の対象だが、この Fork の CI では動くとは限らない。代わりに、`LongitudinalPlanner` をシミュレーター(`Plant`)で動かすテストと、plannerd の設定値の読み込みを実際の Params で確認。plannerd 全体を走行ログに流す確認は、手順が未整備のため未実施 |
| #4 | 動画共有(share videos:スマホ向けのサムネイル付きプレーヤー) | process replay 以外はすべて成功。process replay のログは既知のダウンロード失敗だけ。追加したテストは CI でも実行済み(エンコーダーが必要なものも含む) | 新しいプロセス mediaserverd は process replay の対象外。単体テスト(一覧・サムネイル・MP4 変換・配信・停止条件・アクセス制限、テザリングの連動)と、手元の mac でサーバーを起動してブラウザで再生・画質切り替え・次の区切りへの自動再生まで確認。comma four の実機(テザリング、スマホからの接続、デバイスの PyAV)では未確認 |

### PR #2:modeld の手元での確認(2026-09-28、macOS)
`tools/fork/modeld_camera_offset_check.py` で確認した結果です。

| 補正値 | modelV2 の出力 | Traceback | 車線の中心(最後の20フレームの平均、右がプラス) |
|---|---|---|---|
| 0m | 60件 | なし | −0.142m |
| 15cm(左に取り付けた設定) | 60件 | なし | −0.276m |

- ずれは −0.134m で、期待値 −0.15m と方向が一致した。大きさの差 1.6cm は、補正を反映したモデルが少し違う判断をしている分と考えている。
- 補正値 0 のときは、`apply_camera_offset` が元の変形行列をそのまま返すので、変更前と同じ計算になる(`test_no_offset_is_unchanged` で確認)。
