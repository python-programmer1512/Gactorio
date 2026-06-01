#!/usr/bin/env python3
"""Generate include/common/Config.h from data/factory_config.json.

The "globals" object in the JSON becomes a set of inline constexpr doubles
in namespace gactorio::config. C++ code can include the header and use the
constants directly — keeping all runtime hyperparameters traceable back to
the JSON source of truth.

Run manually after editing factory_config.json:

    python tools/gen_config.py

build_web.ps1 / build_web.sh invoke this before each wasm build.
"""
from __future__ import annotations

import json
import re
import sys
from pathlib import Path


ROOT       = Path(__file__).resolve().parent.parent
JSON_PATH  = ROOT / "data" / "factory_config.json"
OUT_PATH   = ROOT / "include" / "common" / "Config.h"


def camel_to_pascal(name: str) -> str:
    """damageChancePerSecond -> DamageChancePerSecond"""
    return name[:1].upper() + name[1:]


def main() -> int:
    config = json.loads(JSON_PATH.read_text(encoding="utf-8"))
    globals_obj = config.get("globals", {})

    lines = [
        "#pragma once",
        "",
        "// =============================================================================",
        "// AUTO-GENERATED from data/factory_config.json — DO NOT EDIT BY HAND.",
        "// Regenerate with:   python tools/gen_config.py",
        "// =============================================================================",
        "",
        "namespace gactorio::config {",
        "",
    ]

    for key, value in globals_obj.items():
        if key.startswith("_"):
            continue
        if not isinstance(value, (int, float)):
            continue
        ident = "k" + camel_to_pascal(key)
        # always render as double
        lines.append(f"inline constexpr double {ident:<26} = {float(value)};")

    lines += [
        "",
        "} // namespace gactorio::config",
        "",
    ]

    OUT_PATH.parent.mkdir(parents=True, exist_ok=True)
    OUT_PATH.write_text("\n".join(lines), encoding="utf-8")
    print(f"wrote {OUT_PATH.relative_to(ROOT)}  ({len([l for l in lines if 'constexpr' in l])} constants)")
    return 0


if __name__ == "__main__":
    sys.exit(main())
