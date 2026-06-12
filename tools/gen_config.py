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


def snake_to_pascal(name: str) -> str:
    """voltz_classic -> VoltzClassic"""
    return "".join(part[:1].upper() + part[1:] for part in name.split("_") if part)


def station_key_to_pascal(name: str) -> str:
    """MIXING -> Mixing"""
    return "".join(part[:1].upper() + part[1:].lower() for part in name.split("_") if part)


def main() -> int:
    config = json.loads(JSON_PATH.read_text(encoding="utf-8"))
    globals_obj = config.get("globals", {})
    product_items = config.get("items", [])

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

    lines.append("")
    lines.append("// Product route timings generated from data/factory_config.json.")
    for item in product_items:
        if not isinstance(item, dict):
            continue
        product_key = item.get("id")
        if not isinstance(product_key, str):
            continue
        product_ident = snake_to_pascal(product_key)

        totals = item.get("totals", {})
        if isinstance(totals, dict):
            total_time = totals.get("time")
            if isinstance(total_time, (int, float)):
                lines.append(f"inline constexpr double kProduct{product_ident}TotalTime = {float(total_time)};")

        recipe = item.get("recipe", {})
        if not isinstance(recipe, dict):
            continue
        for station_key, station_recipe in recipe.items():
            if not isinstance(station_key, str) or not isinstance(station_recipe, dict):
                continue
            station_time = station_recipe.get("time")
            if not isinstance(station_time, (int, float)):
                continue
            station_ident = station_key_to_pascal(station_key)
            lines.append(
                f"inline constexpr double kProduct{product_ident}{station_ident}Time = {float(station_time)};")

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
