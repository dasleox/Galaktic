#!/bin/bash
set -euo pipefail

EXECUTABLE="bin/GalakSandbox"   # use relative path if in current folder
ARGS=("$@")

# Make it executable automatically
chmod +x "$EXECUTABLE"

# Run it
"$EXECUTABLE" "${ARGS[@]}"
rc=$?

exit $rc
