---
name: openpilot-mac-device-workflow
description: Use when working on openpilot with a Mac for editing and fast local builds, while treating the comma device as the authoritative build and runtime environment. Covers local .uv/.venv setup, Mac full-build validation, restoring tracked build artifacts, device builds, and when prebuilt artifacts are appropriate.
---

# Openpilot Mac and Device Workflow

Use this skill when the task is about day-to-day openpilot development on a Mac with final validation on a comma device.

## Workflow

1. Edit code on the Mac.
2. Set up the repo-local Python environment:

   ```bash
   tools/op.sh setup
   ```

3. Use Mac builds only for fast feedback:

   ```bash
   source .venv/bin/activate
   scons -u -j"$(sysctl -n hw.ncpu)"
   ```

4. After a Mac full build, restore tracked generated artifacts if needed:

   ```bash
   bash scripts/restore_build_artifacts.sh --dry-run --clean-untracked
   bash scripts/restore_build_artifacts.sh --clean-untracked
   ```

5. Build again on the comma device before relying on behavior there:

   ```bash
   tools/op.sh build
   ```

## Rules

- Treat Mac builds as compile-time validation only.
- Treat device builds as the runtime source of truth.
- Do not use Mac-generated binaries as device prebuilt artifacts.
- Only commit prebuilt artifacts when they were built on a device-compatible environment.

## References

- Repo workflow note: `docs/MAC_DEVICE_WORKFLOW.md`
- Restore helper: `scripts/restore_build_artifacts.sh`
