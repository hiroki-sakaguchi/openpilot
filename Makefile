download-models:
	@echo "Downloading model files from upstream..."
	curl -L "https://github.com/commaai/openpilot/raw/master/selfdrive/modeld/models/driving_policy.onnx" -o selfdrive/modeld/models/driving_policy.onnx
	curl -L "https://github.com/commaai/openpilot/raw/master/selfdrive/modeld/models/driving_vision.onnx" -o selfdrive/modeld/models/driving_vision.onnx
	@echo "Model files downloaded successfully"

clean:
	bash -c "source .venv/bin/activate && scons --clean"

build:
	bash -c "source .venv/bin/activate && scons -u -j$$(nproc)"

setup:
	brew install tmux
	sh tools/mac_setup.sh
	$(MAKE) download-models
	bash -c "source .venv/bin/activate && scons --clean && git lfs pull && scons -u -j$$(nproc)"

lint:
	bash -c "source .venv/bin/activate && scripts/lint/lint.sh"

test:
	bash -c "source .venv/bin/activate && python -m pytest"

check:
	bash -c "source .venv/bin/activate && uv sync --frozen --all-extras"

build-test:
	bash -c "source .venv/bin/activate && scons --clean && scons --no-cache --random -j$$(nproc)"

check-deps:
	bash -c "source .venv/bin/activate && uv sync --frozen --all-extras --check"

safety-test:
	cd opendbc_repo/opendbc/safety/tests && ./test.sh

rebuild-ui:
	bash -c "source .venv/bin/activate && scons -j$$(nproc) selfdrive/ui/"

start:
	bash -c "source .venv/bin/activate && ./launch_openpilot.sh"

build-comma:
	@echo "Building openpilot using official docker_build.sh script..."
	selfdrive/test/docker_build.sh base
	selfdrive/test/docker_build.sh prebuilt

release:
	$(MAKE) clean