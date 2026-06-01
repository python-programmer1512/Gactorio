// =============================================================================
// app.js — the View layer.
//
// All communication with the simulation goes through `controller`, a JS proxy
// for the C++ ctrl::Controller exposed by gactorio.js (Emscripten glue).
// =============================================================================
'use strict';

console.log('[gactorio] app.js loaded — build', '2026-06-01-e');

let controller = null;
let lastTime   = 0;
let paused     = false;

// Boot when the wasm runtime is ready. Guard for the case where it's already
// initialized by the time this script runs (race-safe).
function boot() {
    console.log('[gactorio] wasm runtime ready, creating Controller');
    controller = new Module.Controller();
    lastTime   = performance.now();
    bindUI();
    requestAnimationFrame(frame);
}

if (typeof Module !== 'undefined') {
    if (Module.calledRun) boot();
    else Module.onRuntimeInitialized = boot;
} else {
    console.error('[gactorio] Module is undefined — gactorio.js failed to load');
}

// ---------------------------------------------------------------------------
// Main loop
// ---------------------------------------------------------------------------
function frame(now) {
    const dt  = (now - lastTime) / 1000;
    lastTime  = now;
    controller.tick(dt);
    render(controller.snapshot());
    requestAnimationFrame(frame);
}

// ---------------------------------------------------------------------------
// Rendering
// ---------------------------------------------------------------------------
function render(snap) {
    document.getElementById('sim-time').textContent = snap.simulationTime.toFixed(2);
    renderStats(snap.stats);
    renderFactory(snap.lines);
    renderEvents(snap.events);
    renderInventory(snap.inventory);
}

function renderStats(s) {
    document.getElementById('stats-grid').innerHTML = `
        <div>Tasks started     <b>${s.tasksStarted}</b></div>
        <div>Steps completed   <b>${s.stepsCompleted}</b></div>
        <div>Products done     <b>${s.productsDone}</b></div>
        <div>Machines broken   <b>${s.machinesBroken}</b></div>
        <div>Machines repaired <b>${s.machinesRepaired}</b></div>
        <div>State changes     <b>${s.stateChanges}</b></div>`;
}

function renderFactory(lines) {
    let html = '';
    for (let i = 0; i < lines.size(); i++) {
        const line = lines.get(i);

        let machineRows = '';
        for (let j = 0; j < line.machines.size(); j++) {
            const m = line.machines.get(j);
            const stateCls = `state-${m.state.toLowerCase()}`;
            const isBroken = m.state === 'Broken';
            // Repair is always available (incremental +HP). Repair All is a
            // big-button full-restore that only makes sense when the machine
            // is broken — show it conditionally.
            const repairAllBtn = isBroken
                ? `<button class="small danger" data-act="repairAll" data-machine="${m.id}">Repair All</button>`
                : '';
            machineRows += `
                <tr>
                    <td>${esc(m.name)}</td>
                    <td>${esc(m.type)}</td>
                    <td class="${stateCls}">${esc(m.state)}</td>
                    <td>${m.health.toFixed(0)}</td>
                    <td><progress max="1" value="${m.progress.toFixed(3)}"></progress></td>
                    <td>
                        <button class="small" data-act="repair" data-machine="${m.id}">Repair</button>
                        ${repairAllBtn}
                    </td>
                </tr>`;
        }

        const active = line.currentTaskName
            ? `${esc(line.currentTaskName)} · ${(line.currentTaskProgress * 100).toFixed(0)}%`
            : '<span style="color:#666">(idle)</span>';

        html += `
            <div class="line">
                <h3>${esc(line.name)}</h3>
                <div class="meta">Queue: <b>${line.queueLength}</b> · Active: ${active}</div>
                <progress max="1" value="${line.currentTaskProgress.toFixed(3)}"></progress>
                <div class="enqueue">
                    <button class="small" data-act="enqueue" data-line="${line.id}" data-kind="VoltzClassic">+ Voltz Classic</button>
                    <button class="small" data-act="enqueue" data-line="${line.id}" data-kind="HyperBolt">+ Hyper Bolt</button>
                    <button class="small" data-act="enqueue" data-line="${line.id}" data-kind="AuroraZero">+ Aurora Zero</button>
                </div>
                <table>
                    <thead><tr><th>Name</th><th>Type</th><th>State</th><th>HP</th><th>Progress</th><th></th></tr></thead>
                    <tbody>${machineRows}</tbody>
                </table>
            </div>`;
    }
    document.getElementById('factory-content').innerHTML = html;
}

function renderEvents(events) {
    let html = '';
    const n = events.size();
    for (let i = n - 1; i >= 0; i--) {
        const e = events.get(i);
        html += `<div class="event">
            <span class="time">${e.time.toFixed(2)}s</span>
            <span class="type">${esc(e.typeName)}</span>
            <span class="msg">${esc(e.message)}</span>
        </div>`;
    }
    document.getElementById('event-log').innerHTML = html ||
        '<div style="color:#666">(no events yet)</div>';
}

function renderInventory(items) {
    let rows = '';
    for (let i = 0; i < items.size(); i++) {
        const it = items.get(i);
        rows += `<tr><td>${esc(it.id)}</td><td>${it.quantity}</td></tr>`;
    }
    document.getElementById('inventory-content').innerHTML = `
        <table>
            <thead><tr><th>Item</th><th>Qty</th></tr></thead>
            <tbody>${rows || '<tr><td colspan="2" style="color:#666">(empty)</td></tr>'}</tbody>
        </table>`;
}

// ---------------------------------------------------------------------------
// Event wiring — top-level controls + delegated factory buttons.
// Uses closest() so clicks on text inside a button still resolve to the button.
// ---------------------------------------------------------------------------
function bindUI() {
    document.getElementById('btn-pause').addEventListener('click', e => {
        paused = !paused;
        if (paused) { controller.pause();  e.target.textContent = 'Resume'; }
        else        { controller.resume(); e.target.textContent = 'Pause';  }
    });
    document.getElementById('btn-reset').addEventListener('click', () => {
        controller.reset();
    });
    document.getElementById('speed').addEventListener('input', e => {
        const v = parseFloat(e.target.value);
        controller.setSpeed(v);
        document.getElementById('speed-label').textContent = v.toFixed(1) + '×';
    });

    // Per-line / per-machine buttons (rebuilt every frame → use delegation).
    document.body.addEventListener('click', e => {
        const btn = e.target.closest('button[data-act]');
        if (!btn) return;
        const act = btn.dataset.act;

        try {
            if (act === 'enqueue') {
                const lineId   = parseInt(btn.dataset.line, 10);
                const kindName = btn.dataset.kind;
                const kindEnum = Module.ProductKind[kindName];
                console.log('[gactorio] enqueue', { lineId, kindName, kindEnum });
                if (kindEnum === undefined) {
                    console.error('[gactorio] ProductKind not found:', kindName,
                                  'available:', Object.keys(Module.ProductKind || {}));
                    return;
                }
                const ok = controller.enqueue(lineId, kindEnum);
                console.log('[gactorio] enqueue result =', ok);
            } else if (act === 'repair') {
                const id = parseInt(btn.dataset.machine, 10);
                console.log('[gactorio] repair (+5 HP)', id);
                const ok = controller.repair(id);
                console.log('[gactorio] repair result =', ok);
            } else if (act === 'repairAll') {
                const id = parseInt(btn.dataset.machine, 10);
                console.log('[gactorio] repairAll', id);
                const ok = controller.repairAll(id);
                console.log('[gactorio] repairAll result =', ok);
            }
        } catch (err) {
            console.error('[gactorio] action threw:', err);
        }
    });
}

// HTML escape — keep arbitrary event messages from breaking markup.
function esc(s) {
    return String(s)
        .replaceAll('&', '&amp;')
        .replaceAll('<', '&lt;')
        .replaceAll('>', '&gt;')
        .replaceAll('"', '&quot;');
}
