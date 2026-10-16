# SPDX-License-Identifier: BSD-3-Clause
"""Put ``tools/`` on ``sys.path`` so tests can import the ``nes`` package."""

import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parents[1] / "tools"))
