// =============================================================================
// app.js — the View layer.
//
// All communication with the simulation goes through `controller`, a JS proxy
// for the C++ ctrl::Controller exposed by web/gactorio.js (Emscripten glue).
//
// Commands : controller.tick(dt), controller.pause(), .enqueue(...), ...
// Queries  : controller.snapshot()  → returns a plain JS object (FactoryView)
//
// No other C++ symbol is visible here — the entire Model is hidden behind
// the Controller API.
// =============================================================================

'use strict';

let controller = null;
let lastTime   = 0;
let paused     = false;

// Wait for the wasm runtime to be ready, then build the controller and start.
Module.onRuntimeInitialized = () => {
    controller = new Module.Controller();
    lastTime   = performance.now();
    bindUI();
    requestAnimationFrame(frame);
};

// ---------------------------------------------------------------------------
// Main loop — runs every animation frame (~60 fps).
// ---------------------------------------------------------------------------
function frame(now) {
    const dt  = (now - lastTime) / 1000;
    lastTime  = now;

    controller.tick(dt);                       // advance simulation
    render(controller.snapshot());             // re-render whole UI

    requestAnimationFrame(frame);
}

// ---------------------------------------------------------------------------
// Rendering — pure read of the snapshot into the DOM.
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
            machineRows += `
                <tr>
                    <td>${esc(m.name)}</td>
                    <td>${esc(m.type)}</td>
                    <td class="${stateCls}">${esc(m.state)}</td>
                    <td>${m.health.toFixed(0)}</td>
                    <td><progress max="1" value="${m.progress.toFixed(3)}"></progress></td>
                    <td>
                        <button class="small danger" data-act="break"  data-machine="${m.id}">Break</button>
                        <button class="small"        data-act="repair" data-machine="${m.id}">Repair</button>
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
    for (let i = n - 1; i >= 0; i--) {        // most recent first
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
// One-time event wiring (top-level controls + delegated factory buttons)
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

    // Event delegation: per-line/per-machine buttons are rebuilt every frame.
    document.body.addEventListener('click', e => {
        const t = e.target;
        if (!t.dataset.act) return;
        if (t.dataset.act === 'enqueue') {
            controller.enqueue(+t.dataset.line, Module.ProductKind[t.dataset.kind]);
        } else if (t.dataset.act === 'break') {
            controller.breakMachine(+t.dataset.machine);
        } else if (t.dataset.act === 'repair') {
            controller.repair(+t.dataset.machine);
        }
    });
}

// ---------------------------------------------------------------------------
// Minimal HTML escape so event messages can't break the markup.
// ---------------------------------------------------------------------------
function esc(s) {
    return String(s)
        .replaceAll('&', '&amp;')
        .replaceAll('<', '&lt;')
        .replaceAll('>', '&gt;')
        .replaceAll('"', '&quot;');
}
