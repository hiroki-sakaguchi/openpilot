# openpilot モデルファイル問題解決記録

## 問題の概要

openpilotのrelease3ブランチ（v0.9.8）をクローンした際、`.lfsconfig`が廃止されたことでモデルファイルの取得方法が不明になった。さらに、`make build-comma`を実行するとモデルファイルが消えてしまう問題が発生していた。

## 調査開始時の状況

### 初期状態
- ブランチ: `new-personality`（openpilot release3ベース）
- `.lfsconfig`と`.gitattributes`が削除済み
- `make build-comma`実行時にモデル関係のエラーが発生

### git status
```
M selfdrive/modeld/models/driving_policy.onnx
M selfdrive/modeld/models/driving_vision.onnx
```

## 調査フェーズ1: Git LFS設定の調査

### 公式release3ブランチの調査
1. **upstream/release3ブランチをフェッチして確認**
   - `.gitattributes`: 存在しない
   - `.lfsconfig`: 存在しない
   - 結論: 公式でもGit LFS設定は完全に廃止されている

2. **モデルファイル形式の変更を発見**
   - 公式release3: ONNXファイル → Pickleファイル（`.pkl`）に移行
   - TinyGradでコンパイル済みのモデルを使用
   - `dmonitoring_model.onnx`のみONNX形式で残存

### 公式のモデル取得方法
```bash
git clone https://github.com/commaai/openpilot.git -b release3
# モデルファイルは自動的に含まれる（LFS不要）
```

## 調査フェーズ2: build-commaエラーの原因特定

### Makefileの分析
```makefile
build-comma:
	docker build -t op-base-aarch64 -f Dockerfile.openpilot_base .
	docker run --rm -it \
		-v $(PWD):/workspace \
		op-base-aarch64 \
		bash -c "cd /workspace && scons --clean && scons -j$$(sysctl -n hw.ncpu)"
```

### 問題の特定
1. **`scons --clean`でファイルが削除される**
   - `commonmodel_pyx.so`
   - `dmonitoring_model_tinygrad.pkl`
   - `driving_policy_metadata.pkl`
   - `driving_policy_tinygrad.pkl`
   - `driving_vision_metadata.pkl`
   - `driving_vision_tinygrad.pkl`

2. **SConscriptでの動的生成設定**
   - `selfdrive/modeld/SConscript`の35-53行目でPickleファイルがビルドターゲットとして定義
   - `scons --clean`で削除される
   - 再生成時にONNXファイルが存在しないためエラー

## 解決フェーズ: 静的ファイル化の実装

### 解決策の選択
**オプション1**: ビルドターゲットから除外（採用）
- 公式release3の構成に合わせる
- Pickleファイルを静的ファイルとして扱う
- シンプルで保守しやすい

**オプション2**: ONNX生成プロセス修正（不採用）
- 複雑性が増す
- 公式構成から逸脱

### 実装手順

#### 1. SConscriptの修正
**ファイル**: `selfdrive/modeld/SConscript`

**削除したコード** (35-53行目):
```python
# Get model metadata
for model_name in ['driving_vision', 'driving_policy']:
  fn = File(f"models/{model_name}").abspath
  script_files = [File(Dir("#selfdrive/modeld").File("get_model_metadata.py").abspath)]
  cmd = f'python3 {Dir("#selfdrive/modeld").abspath}/get_model_metadata.py {fn}.onnx'
  lenv.Command(fn + "_metadata.pkl", [fn + ".onnx"] + tinygrad_files + script_files, cmd)

# Compile tinygrad model
pythonpath_string = 'PYTHONPATH="${PYTHONPATH}:' + env.Dir("#tinygrad_repo").abspath + '"'
if arch == 'larch64':
  device_string = 'QCOM=1'
elif arch == 'Darwin':
  device_string = 'CLANG=1 IMAGE=0'
else:
  device_string = 'LLVM=1 LLVMOPT=1 BEAM=0 IMAGE=0'

for model_name in ['driving_vision', 'driving_policy', 'dmonitoring_model']:
  fn = File(f"models/{model_name}").abspath
  cmd = f'{pythonpath_string} {device_string} python3 {Dir("#tinygrad_repo").abspath}/examples/openpilot/compile3.py {fn}.onnx {fn}_tinygrad.pkl'
  lenv.Command(fn + "_tinygrad.pkl", [fn + ".onnx"] + tinygrad_files, cmd)
```

**追加したコメント**:
```python
# Model files are now static files (not generated during build)
# Removed dynamic generation to match official release3 configuration
```

#### 2. 公式ファイルの復元
```bash
# 公式release3から必要ファイルを取得
git show upstream/release3:selfdrive/modeld/models/commonmodel_pyx.so > selfdrive/modeld/models/commonmodel_pyx.so
git show upstream/release3:selfdrive/modeld/models/driving_policy_metadata.pkl > selfdrive/modeld/models/driving_policy_metadata.pkl
git show upstream/release3:selfdrive/modeld/models/driving_policy_tinygrad.pkl > selfdrive/modeld/models/driving_policy_tinygrad.pkl
git show upstream/release3:selfdrive/modeld/models/driving_vision_metadata.pkl > selfdrive/modeld/models/driving_vision_metadata.pkl
git show upstream/release3:selfdrive/modeld/models/driving_vision_tinygrad.pkl > selfdrive/modeld/models/driving_vision_tinygrad.pkl
git show upstream/release3:selfdrive/modeld/models/dmonitoring_model_tinygrad.pkl > selfdrive/modeld/models/dmonitoring_model_tinygrad.pkl

# 実行権限を設定
chmod +x selfdrive/modeld/models/commonmodel_pyx.so
```

#### 3. 検証テスト
```bash
make build-comma
```

**結果**:
- ビルドが正常に完了
- モデルファイルが保持される
- エラーが解消

## 最終的なファイル構成

### モデルディレクトリの内容
```
selfdrive/modeld/models/
├── README.md
├── __init__.py
├── commonmodel.cc
├── commonmodel.h
├── commonmodel.pxd
├── commonmodel_pyx.cpp
├── commonmodel_pyx.o
├── commonmodel_pyx.pxd
├── commonmodel_pyx.pyx
├── commonmodel_pyx.so              # 復元（実行ファイル）
├── dmonitoring_model.current
├── dmonitoring_model.onnx
├── dmonitoring_model_tinygrad.pkl  # 復元
├── driving_policy_metadata.pkl     # 復元
├── driving_policy_tinygrad.pkl     # 復元
├── driving_vision_metadata.pkl     # 復元
└── driving_vision_tinygrad.pkl     # 復元
```

### 削除されたファイル
- `driving_policy.onnx` (不要になった)
- `driving_vision.onnx` (不要になった)

## コミット情報

**コミットメッセージ**:
```
Fix model build issues by making pickle files static

- Remove dynamic model file generation from SConscript
- Switch to static pickle files matching official release3 configuration
- Fixes make build-comma failing due to missing model dependencies
```

**変更されたファイル**:
- `selfdrive/modeld/SConscript` (修正)
- `Makefile` (修正)
- `selfdrive/modeld/models/driving_policy.onnx` (削除)
- `selfdrive/modeld/models/driving_vision.onnx` (削除)
- 複数のPickleファイル (追加)

## 学んだ教訓

1. **公式ブランチとの同期の重要性**
   - モデルファイル管理方法が変更されていた
   - Git LFSから静的ファイルへの移行

2. **ビルドシステムの理解**
   - SConsのターゲット定義が問題の根本原因
   - `scons --clean`の動作を理解することの重要性

3. **段階的な問題解決**
   - まず問題の根本原因を特定
   - 公式構成を調査
   - 最もシンプルな解決策を選択

## 今後の注意点

- 公式のopenpilotアップデート時は、モデルファイル管理方法の変更に注意
- SConscriptの変更時は、ビルドターゲットへの影響を考慮
- Docker環境でのビルド時は、ファイルの永続化を確認

---

**作成日**: 2025年6月15日
**対象バージョン**: openpilot release3 (v0.9.8)
**解決者**: Claude Code

openpilot release3 (v0.9.8) 公式ではgit lfs方式が廃止になっておりモデルを含む全てのファイルをgithubのみで管理しており通常のgit lfsコマンドは使わずgit pullコマンドで全てのファイルがダウンロードされる。