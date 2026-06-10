// =============================================================================
// FactoryPanel — production lines with machine tables. (Middle column.)
//
// Renders every LineView from the snapshot. Dynamic buttons (Repair,
// Repair All, Disappear) are rebuilt with each render, so they are wired
// through a delegated 'pointerdown' listener scoped to #factory-content:
// pointerdown fires on press, before the next innerHTML swap can destroy
// the button, where a plain 'click' would be silently dropped.
// =============================================================================

import { UIComponent } from '../UIComponent.js';
import { esc } from '../util.js';

export class FactoryPanel extends UIComponent {
    #ctrl;

    constructor(controller) {
        super();
        this.#ctrl = controller;
    }

    bind() {
        document.getElementById('btn-add-line').addEventListener('click', () => {
            const id = this.#ctrl.addLine();
            console.log('[gactorio] addLine → id', id);
        });

        document.getElementById('factory-content').addEventListener('pointerdown', e => {
            const btn = e.target.closest('button[data-act]');
            if (!btn || btn.disabled) return;

            try {
                switch (btn.dataset.act) {
                case 'removeLine': {
                    const id = parseInt(btn.dataset.line, 10);
                    console.log('[gactorio] removeLine', id, '→', this.#ctrl.removeLine(id));
                    break;
                }
                case 'repair': {
                    const id = parseInt(btn.dataset.machine, 10);
                    console.log('[gactorio] repair (+5 HP)', id, '→', this.#ctrl.repair(id));
                    break;
                }
                case 'repairAll': {
                    const id = parseInt(btn.dataset.machine, 10);
                    console.log('[gactorio] repairAll', id, '→', this.#ctrl.repairAll(id));
                    break;
                }
                }
            } catch (err) {
                console.error('[gactorio] factory action threw:', err);
            }
        });
    }

    render(snap) {
        let html = '';
        for (const line of snap.lines) {
            html += this.#lineHtml(line);
        }
        document.getElementById('factory-content').innerHTML =
            html || '<div style="color:#666">(no production lines — press “+ Add Line”)</div>';
    }

    #lineHtml(line) {
        const active = line.currentTaskName
            ? `${esc(line.currentTaskName)} · ${(line.currentTaskProgress * 100).toFixed(0)}%`
            : '<span style="color:#666">(idle)</span>';

        // Disappear (remove) is offered only while the line is fully idle.
        const disappearBtn = line.isRemovable
            ? `<button class="small danger" data-act="removeLine" data-line="${line.id}">Disappear</button>`
            : '';

        return `
            <div class="line">
                <div class="line-header">
                    <h3>${esc(line.name)}</h3>
                    ${disappearBtn}
                </div>
                <div class="meta">Queue: <b>${line.queueLength}</b> · Active: ${active}</div>
                <progress max="1" value="${line.currentTaskProgress.toFixed(3)}"></progress>
                <table>
                    <thead><tr><th>Name</th><th>Type</th><th>State</th><th>HP</th><th>Progress</th><th></th></tr></thead>
                    <tbody>${line.machines.map(m => this.#machineRow(m)).join('')}</tbody>
                </table>
            </div>`;
    }

    #machineRow(m) {
        const stateCls = `state-${m.state.toLowerCase()}`;
        // Repair is the always-on incremental +HP; Repair All (full restore
        // with delay) appears only once the machine is actually broken.
        const repairAllBtn = m.state === 'Broken'
            ? `<button class="small danger" data-act="repairAll" data-machine="${m.id}">Repair All</button>`
            : '';
        return `
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
}
