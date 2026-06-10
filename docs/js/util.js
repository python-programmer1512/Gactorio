// =============================================================================
// util.js — shared helpers for the View layer.
// =============================================================================

// Escape untrusted text before inserting into innerHTML.
export function esc(s) {
    return String(s)
        .replaceAll('&', '&amp;')
        .replaceAll('<', '&lt;')
        .replaceAll('>', '&gt;')
        .replaceAll('"', '&quot;');
}

// Convert an embind vector handle to a plain JS array, then free the
// underlying C++ allocation. embind handles are NOT garbage-collected —
// the old monolithic app.js skipped delete() and leaked a few heap blocks
// on every render (10×/second).
export function vecToArray(vec) {
    const out = [];
    for (let i = 0; i < vec.size(); i++) out.push(vec.get(i));
    vec.delete();
    return out;
}

// Flatten Module.Controller.snapshot() into pure data. After this call no
// embind handle survives, so panels can treat the snapshot as a plain
// JS object and nothing needs manual cleanup downstream.
export function toPlainSnapshot(snap) {
    return {
        simulationTime: snap.simulationTime,
        stats:          snap.stats,
        lines: vecToArray(snap.lines).map(line => ({
            ...line,
            machines: vecToArray(line.machines),
        })),
        events:    vecToArray(snap.events),
        inventory: vecToArray(snap.inventory),
    };
}
