import { UIComponent } from '../UIComponent.js';
import { esc } from '../util.js';

export class InspectorPanel extends UIComponent {
    #ctrl;
    #selection;

    constructor(controller, selection) {
        super();
        this.#ctrl = controller;
        this.#selection = selection;
    }

    bind() {
        document.getElementById('inspector-content').addEventListener('click', e => {
            const btn = e.target.closest('button[data-act]');
            if (!btn) return;

            const id = parseInt(btn.dataset.machine, 10);
            switch (btn.dataset.act) {
            case 'forceBreak':
                this.#ctrl.breakMachine(id);
                break;
            case 'instantRepair':
                this.#ctrl.instantRepair(id);
                break;
            case 'repair':
                this.#ctrl.repair(id);
                break;
            case 'repairAll':
                this.#ctrl.repairAll(id);
                break;
            }
        });
    }

    render(snap) {
        const selected = this.#findSelectedMachine(snap);
        if (!selected) {
            document.getElementById('inspector-content').innerHTML =
                '<div class="muted">Select a machine on the factory floor.</div>';
            return;
        }

        const { line, machine } = selected;
        const progress = Math.max(0, Math.min(1, machine.progress));
        const hp = Math.max(0, Math.min(100, machine.health));
        const queueCapacity = Number(line.queueCapacity || 0);
        const capacityText = queueCapacity === 0 ? 'Unlimited' : String(queueCapacity);
        const repairAllButton = machine.state === 'Broken'
            ? `<button class="small danger" data-act="repairAll" data-machine="${machine.id}">Repair All</button>`
            : '';

        document.getElementById('inspector-content').innerHTML = `
            <div class="inspector-title">
                <b>${esc(machine.name)}</b>
                <span>${esc(machine.state)}</span>
            </div>
            <dl class="inspector-fields">
                <dt>Line</dt><dd>${esc(line.name)}</dd>
                <dt>Type</dt><dd>${esc(machine.type)}</dd>
                <dt>Scenario</dt><dd>${esc(line.scenarioName || line.scenarioId || 'Normal Flow')}</dd>
                <dt>Queue/WIP</dt><dd>${line.queueLength} / ${capacityText}</dd>
                <dt>Output</dt><dd>${snap.stats.productsDone}</dd>
                <dt>Dropped</dt><dd>${Number(line.droppedTaskCount || 0)}</dd>
                <dt>Process</dt><dd>${Math.round(progress * 100)}%</dd>
            </dl>
            <div class="meter-row"><span>Health</span><span>${machine.health.toFixed(0)}</span></div>
            <div class="hp-meter"><span style="width:${hp.toFixed(0)}%"></span></div>
            <div class="meter-row"><span>Progress</span><span>${Math.round(progress * 100)}%</span></div>
            <progress max="1" value="${progress.toFixed(3)}"></progress>
            <div class="inspector-actions">
                <button class="small danger" data-act="forceBreak" data-machine="${machine.id}">Force Break</button>
                <button class="small" data-act="instantRepair" data-machine="${machine.id}">Instant Repair</button>
                <button class="small" data-act="repair" data-machine="${machine.id}">+5 HP</button>
                ${repairAllButton}
            </div>`;
    }

    #findSelectedMachine(snap) {
        const selectedId = this.#selection.machineId;
        for (const line of snap.lines) {
            for (const machine of line.machines) {
                if (machine.id === selectedId) {
                    return { line, machine };
                }
            }
        }

        const firstLine = snap.lines[0];
        const firstMachine = firstLine?.machines?.[0];
        if (firstMachine) {
            this.#selection.machineId = firstMachine.id;
            return { line: firstLine, machine: firstMachine };
        }
        return null;
    }
}
