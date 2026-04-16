#!/usr/bin/env bash
set -euo pipefail

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null && pwd )"
ROOT="$(cd "$DIR/.." && pwd)"
cd "$ROOT"

DRY_RUN=0
CLEAN_UNTRACKED=0
for arg in "$@"; do
  case "$arg" in
    --dry-run)
      DRY_RUN=1
      ;;
    --clean-untracked)
      CLEAN_UNTRACKED=1
      ;;
    *)
      echo "usage: $0 [--dry-run] [--clean-untracked]"
      exit 1
      ;;
  esac
done

artifacts=(
  ":(glob)cereal/gen/cpp/*.capnp.c++"
  ":(glob)cereal/gen/cpp/*.capnp.h"
  "cereal/messaging/bridge"
  "common/params_pyx.cpp"
  "common/params_pyx.so"
  "common/transformations/transformations.cpp"
  "common/transformations/transformations.so"
  "msgq_repo/msgq/ipc_pyx.cpp"
  "msgq_repo/msgq/ipc_pyx.so"
  "msgq_repo/msgq/visionipc/visionipc_pyx.cpp"
  "msgq_repo/msgq/visionipc/visionipc_pyx.so"
  "opendbc_repo/opendbc/can/packer_pyx.cpp"
  "opendbc_repo/opendbc/can/packer_pyx.so"
  "opendbc_repo/opendbc/can/parser_pyx.cpp"
  "opendbc_repo/opendbc/can/parser_pyx.so"
  ":(glob)panda/board/obj/**"
  ":(glob)panda/board/jungle/obj/**"
  "rednose_repo/rednose/helpers/ekf_sym_pyx.cpp"
  "rednose_repo/rednose/helpers/ekf_sym_pyx.so"
  "selfdrive/assets/assets.cc"
  ":(glob)selfdrive/controls/lib/*_mpc_lib/c_generated_code/**"
  ":(glob)selfdrive/locationd/models/generated/*"
  "selfdrive/modeld/models/commonmodel_pyx.cpp"
  "selfdrive/modeld/models/commonmodel_pyx.so"
  "selfdrive/modeld/models/dmonitoring_model_tinygrad.pkl"
  "selfdrive/pandad/pandad"
  "selfdrive/pandad/pandad_api_impl.cpp"
  "selfdrive/pandad/pandad_api_impl.so"
  "selfdrive/ui/ui"
  ":(glob)selfdrive/ui/translations/main_*.qm"
  "system/loggerd/bootlog"
  "system/loggerd/encoderd"
  "system/loggerd/loggerd"
  "system/proclogd/proclogd"
  "system/ubloxd/ubloxd"
)

ignored_untracked_artifacts=(
  ".sconsign.dblite"
  "build/"
  "cereal/"
  "common/"
  "msgq_repo/"
  "opendbc_repo/opendbc/can/"
  "panda/board/"
  "rednose_repo/rednose/"
  "rednose_repo/site_scons/"
  "selfdrive/"
  "system/"
  "third_party/"
  "tinygrad_repo/"
)

if [[ $DRY_RUN -eq 1 ]]; then
  git status --short -- "${artifacts[@]}"
  if [[ $CLEAN_UNTRACKED -eq 1 ]]; then
    git clean -ndX -- "${ignored_untracked_artifacts[@]}"
  fi
  exit 0
fi

git restore --worktree --source=HEAD -- "${artifacts[@]}"

if [[ $CLEAN_UNTRACKED -eq 1 ]]; then
  git clean -fdX -- "${ignored_untracked_artifacts[@]}"
fi

echo "Restored tracked build artifacts."
