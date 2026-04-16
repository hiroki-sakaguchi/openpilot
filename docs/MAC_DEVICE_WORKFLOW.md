# Mac and Device Workflow

This workflow is for local development on a Mac with final validation on a comma device.

## Why this split exists

- Mac builds are for fast feedback.
- Device builds are the source of truth for runtime behavior.
- Mac build artifacts must not be treated as device prebuilt artifacts.

## Recommended flow

1. Edit code on the Mac.
2. Set up the local Python environment once:

   ```bash
   tools/op.sh setup
   ```

   This repo keeps uv-managed tooling under `.uv/` and the Python environment under `.venv/`.

3. Run a full Mac build when you want fast compile feedback:

   ```bash
   source .venv/bin/activate
   scons -u -j"$(sysctl -n hw.ncpu)"
   ```

4. If the build dirties tracked generated files, restore them:

   ```bash
   bash scripts/restore_build_artifacts.sh --dry-run --clean-untracked
   bash scripts/restore_build_artifacts.sh --clean-untracked
   ```

5. Move the source changes to the device.
6. Build again on the device.
7. Test on the device and in the car.

## Rules

- Do not commit Mac build artifacts as if they were device prebuilt artifacts.
- Mac builds are useful for compile-time validation, not for producing deployable binaries.
- If you want a real prebuilt branch for `installer.comma.ai/...`, create the artifacts on a device-compatible environment and commit those artifacts there.

## When to use prebuilt

Use prebuilt only when all of the following are true:

- You want the device to skip startup build steps.
- You are ready to commit generated artifacts.
- Those artifacts were built on the device or a compatible target environment.

## Commands

Local setup:

```bash
tools/op.sh setup
```

Local full build:

```bash
source .venv/bin/activate
scons -u -j"$(sysctl -n hw.ncpu)"
```

Restore Mac build artifacts:

```bash
bash scripts/restore_build_artifacts.sh --clean-untracked
```

Device build:

```bash
tools/op.sh build
```
