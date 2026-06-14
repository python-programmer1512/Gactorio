#!/usr/bin/env python3
"""Summarize a draw.io class/sequence diagram: list class boxes and resolve
every edge to `source --[UML relationship]--> target`, inferring the
relationship from the arrow style. Read-only; used to verify diagrams vs code.
"""
import sys, re, html
import xml.etree.ElementTree as ET


def style_to_rel(style: str) -> str:
    s = style or ""
    def has(k):
        return k in s
    dashed = "dashed=1" in s
    # endArrow / startArrow + fill
    end = re.search(r"endArrow=([^;]+)", s)
    start = re.search(r"startArrow=([^;]+)", s)
    endfill = re.search(r"endFill=([01])", s)
    startfill = re.search(r"startFill=([01])", s)
    e = end.group(1) if end else ""
    st = start.group(1) if start else ""
    ef = endfill.group(1) if endfill else "1"
    sf = startfill.group(1) if startfill else "1"
    # diamonds = composition (filled) / aggregation (hollow), either end
    if "diamond" in e:
        return "COMPOSITION(◆)" if ef == "1" else "AGGREGATION(◇)"
    if "diamond" in st:
        return "COMPOSITION(◆)" if sf == "1" else "AGGREGATION(◇)"
    # block/triangle hollow = generalization (inheritance)
    if e == "block" and ef == "0":
        return "GENERALIZATION(▷)"
    if st == "block" and sf == "0":
        return "GENERALIZATION(▷ rev)"
    if e == "block":
        return "DEP/ASSOC(block-filled)"
    if dashed and (e == "open" or e == "openThin"):
        return "DEPENDENCY(- ->)"
    if e in ("open", "openThin"):
        return "ASSOCIATION(->)"
    if e == "none" and st == "none":
        return "LINE(no-arrow)"
    return f"OTHER(end={e},ef={ef},start={st},dashed={int(dashed)})"


def main(path):
    tree = ET.parse(path)
    root = tree.getroot()
    cells = root.iter("mxCell")
    names = {}
    vert_styles = {}
    edges = []
    parents = {}
    for c in cells:
        cid = c.get("id")
        val = c.get("value")
        if val is not None:
            val = html.unescape(re.sub(r"<[^>]+>", " ", val)).strip()
        style = c.get("style") or ""
        parents[cid] = c.get("parent")
        if c.get("edge") == "1":
            edges.append((cid, c.get("source"), c.get("target"), style, val))
        elif c.get("vertex") == "1":
            names[cid] = val
            vert_styles[cid] = style

    # Heuristic: "class boxes" are vertices whose parent is layer (1/0) — i.e.
    # not member rows nested inside another class box.
    top = []
    for cid, val in names.items():
        p = parents.get(cid)
        if val and p in ("1", "0", None):
            kind = ""
            stl = vert_styles.get(cid, "")
            if "swimlane" in stl or "childLayout" in stl:
                kind = " [container]"
            top.append((val, kind))

    print(f"### {path}")
    print(f"-- top-level boxes ({len(top)}):")
    for val, kind in top:
        print(f"   • {val}{kind}")
    def short(nm):
        if nm is None:
            return "?None"
        # class name only: text before the first ' - ' / ' + ' member marker
        return re.split(r"\s[-+]\s", nm)[0].strip()[:40] or nm

    print(f"-- edges ({len(edges)}):")
    for cid, s, t, style, lbl in edges:
        sn = short(names.get(s, f"?{s}"))
        tn = short(names.get(t, f"?{t}"))
        rel = style_to_rel(style)
        L = f"  '{lbl}'" if lbl else ""
        print(f"   {sn:28}--{rel:18}-->  {tn}{L}")
        if s is None or t is None or s not in names or t not in names:
            print(f"      !! DANGLING edge id={cid} (source={s}, target={t})")


if __name__ == "__main__":
    for p in sys.argv[1:]:
        main(p)
        print()
