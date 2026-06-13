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
        document.getElementById('inspector-content').addEventListener('pointerdown', e => {
            const btn = e.target.closest('button[data-act]');
            if (!btn) return;
            e.preventDefault();

            const id = parseInt(btn.dataset.machine, 10);
            switch (btn.dataset.act) {
            case 'forceBreak':
                this.#ctrl.breakMachine(id);
                break;
            case 'instantRepair':
                this.#ctrl.instantRepair(id);
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
        const root = document.getElementById('inspector-content');
        const currentView = root.querySelector('.inspector-view');

        if (currentView?.dataset.machineId === String(machine.id)) {
            currentView.querySelector('.js-state').textContent = machine.state;
            currentView.querySelector('.js-line').textContent = line.name;
            currentView.querySelector('.js-type').textContent = machine.type;
            currentView.querySelector('.js-scenario').textContent = line.scenarioName || line.scenarioId || 'Normal Flow';
            currentView.querySelector('.js-queue').textContent = `${line.queueLength} / ${capacityText}`;
            currentView.querySelector('.js-output').textContent = String(snap.stats.productsDone);
            currentView.querySelector('.js-dropped').textContent = String(Number(line.droppedTaskCount || 0));
            currentView.querySelector('.js-process').textContent = `${Math.round(progress * 100)}%`;
            currentView.querySelector('.js-health').textContent = machine.health.toFixed(0);
            currentView.querySelector('.js-healthbar').style.width = `${hp.toFixed(0)}%`;
            currentView.querySelector('.js-progress').textContent = `${Math.round(progress * 100)}%`;
            currentView.querySelector('progress').value = progress.toFixed(3);
            return;
        }

        root.innerHTML = `
            <div class="inspector-view" data-machine-id="${machine.id}">
            <div class="inspector-title">
                <b>${esc(machine.name)}</b>
                <span class="js-state">${esc(machine.state)}</span>
            </div>
            <dl class="inspector-fields">
                <dt>Line</dt><dd class="js-line">${esc(line.name)}</dd>
                <dt>Type</dt><dd class="js-type">${esc(machine.type)}</dd>
                <dt>Scenario</dt><dd class="js-scenario">${esc(line.scenarioName || line.scenarioId || 'Normal Flow')}</dd>
                <dt>Queue/WIP</dt><dd class="js-queue">${line.queueLength} / ${capacityText}</dd>
                <dt>Output</dt><dd class="js-output">${snap.stats.productsDone}</dd>
                <dt>Dropped</dt><dd class="js-dropped">${Number(line.droppedTaskCount || 0)}</dd>
                <dt>Process</dt><dd class="js-process">${Math.round(progress * 100)}%</dd>
            </dl>
            <div class="meter-row"><span>Health</span><span class="js-health">${machine.health.toFixed(0)}</span></div>
            <div class="hp-meter"><span class="js-healthbar" style="width:${hp.toFixed(0)}%"></span></div>
            <div class="meter-row"><span>Progress</span><span class="js-progress">${Math.round(progress * 100)}%</span></div>
            <progress max="1" value="${progress.toFixed(3)}"></progress>
            <div class="inspector-actions">
                <button class="small danger" data-act="forceBreak" data-machine="${machine.id}">Force Break</button>
                <button class="small" data-act="instantRepair" data-machine="${machine.id}">Instant Repair</button>
            </div>
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
