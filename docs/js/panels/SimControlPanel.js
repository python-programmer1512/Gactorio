// =============================================================================
// SimControlPanel — simulation clock, pause/reset, speed, Memento controls,
// and the aggregate statistics grid. (Left column, top half.)
//
// Mirrors the C++ SimControlPanel from the ImGui era: holds a controller
// reference, renders read-only state, forwards button presses as commands.
// =============================================================================

import { UIComponent } from '../UIComponent.js';

export class SimControlPanel extends UIComponent {
    #ctrl;
    #running = true;
    #lastSnap = null;

    constructor(controller) {
        super();
        this.#ctrl = controller;
    }

    bind() {
        document.getElementById('btn-pause').addEventListener('click', e => {
            this.#running = !this.#running;
            if (this.#running) {
                this.#ctrl.resume();
                e.target.textContent = 'Pause';
            } else {
                this.#ctrl.pause();
                e.target.textContent = 'Resume';
            }
        });

        document.getElementById('btn-reset').addEventListener('click', () => {
            this.#ctrl.reset();
        });

        document.getElementById('btn-save').addEventListener('click', () => {
            this.#ctrl.saveCheckpoint();
            console.log('[gactorio] checkpoint saved, history =', this.#ctrl.historySize());
        });

        document.getElementById('btn-undo').addEventListener('click', () => {
            const ok = this.#ctrl.undo();
            console.log('[gactorio] undo →', ok, 'history =', this.#ctrl.historySize());
        });

        document.getElementById('speed').addEventListener('input', e => {
            const v = parseFloat(e.target.value);
            this.#ctrl.setSpeed(v);
            document.getElementById('speed-label').textContent = v.toFixed(1) + '×';
        });

        document.getElementById('global-scenario').addEventListener('change', e => {
            if (!this.#lastSnap) return;
            for (const line of this.#lastSnap.lines) {
                this.#ctrl.setLineScenario(line.id, e.target.value);
            }
        });
    }

    render(snap) {
        this.#lastSnap = snap;
        document.getElementById('sim-time').textContent = snap.simulationTime.toFixed(2);
        const firstLine = snap.lines[0];
        if (firstLine) {
            document.getElementById('global-scenario').value = firstLine.scenarioId || 'normal-flow';
        }

        const s = snap.stats;
        const wip = snap.lines.reduce((sum, line) => sum + line.queueLength, 0);
        const lost = snap.lines.reduce((sum, line) => sum + Number(line.droppedTaskCount || 0), 0);
        document.getElementById('stats-grid').innerHTML = `
            <div>Tasks started     <b>${s.tasksStarted}</b></div>
            <div>Steps completed   <b>${s.stepsCompleted}</b></div>
            <div>Products done     <b>${s.productsDone}</b></div>
            <div>WIP products      <b>${wip}</b></div>
            <div>Machines broken   <b>${s.machinesBroken}</b></div>
            <div>Machines repaired <b>${s.machinesRepaired}</b></div>
            <div>Lost products     <b>${lost}</b></div>
            <div>State changes     <b>${s.stateChanges}</b></div>`;

        // Memento status — queried from the controller, not the snapshot,
        // because history lives in the Caretaker (not the Factory state).
        document.getElementById('history-size').textContent = this.#ctrl.historySize();
        document.getElementById('btn-undo').disabled = !this.#ctrl.canUndo();
    }
}
