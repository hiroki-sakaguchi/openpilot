#!/usr/bin/env bash
set -e

# Increase the pip timeout to handle TimeoutError
export PIP_DEFAULT_TIMEOUT=200

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null && pwd )"
ROOT="$DIR"/../
cd "$ROOT"

UV_ROOT="$ROOT/.uv"
UV_BIN_DIR="$UV_ROOT/bin"
UV_BIN="$UV_BIN_DIR/uv"
export UV_CACHE_DIR="$UV_ROOT/cache"
export UV_PYTHON_INSTALL_DIR="$UV_ROOT/python"
export UV_PYTHON_BIN_DIR="$UV_ROOT/python-bin"
export UV_MANAGED_PYTHON=1

if [[ ! -x "$UV_BIN" ]]; then
  echo "installing uv locally..."
  mkdir -p "$UV_BIN_DIR"
  curl -LsSf https://astral.sh/uv/install.sh | env UV_UNMANAGED_INSTALL="$UV_BIN_DIR" sh
fi

PATH="$UV_BIN_DIR:$PATH"

echo "installing python packages..."
if [[ ! -x "$UV_PYTHON_BIN_DIR/python3.12" ]]; then
  "$UV_BIN" python install 3.12
fi
# Default setup targets core development, build, lint, and tests.
# Optional tools/docs extras can be installed later when needed.
"$UV_BIN" sync --python 3.12 --frozen --extra dev --extra testing
source .venv/bin/activate

echo "PYTHONPATH=${PWD}" > "$ROOT"/.env
if [[ "$(uname)" == 'Darwin' ]]; then
  echo "# msgq doesn't work on mac" >> "$ROOT"/.env
  echo "export ZMQ=1" >> "$ROOT"/.env
  echo "export OBJC_DISABLE_INITIALIZE_FORK_SAFETY=YES" >> "$ROOT"/.env
fi
