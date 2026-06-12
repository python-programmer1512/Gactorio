// =============================================================================
// FactoryPanel - production lines rendered as conveyor-belt shop-floor views.
//
// Renders every LineView from the snapshot. Dynamic buttons are rebuilt with
// each render, so actions use one delegated pointerdown listener scoped to
// #factory-content.
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
            console.log('[gactorio] addLine -> id', id);
        });

        document.getElementById('factory-content').addEventListener('pointerdown', e => {
            const btn = e.target.closest('button[data-act]');
            if (!btn || btn.disabled) return;

            try {
                switch (btn.dataset.act) {
                case 'removeLine': {
                    const id = parseInt(btn.dataset.line, 10);
                    console.log('[gactorio] removeLine', id, '->', this.#ctrl.removeLine(id));
                    break;
                }
                case 'repair': {
                    const id = parseInt(btn.dataset.machine, 10);
                    console.log('[gactorio] repair (+5 HP)', id, '->', this.#ctrl.repair(id));
                    break;
                }
                case 'repairAll': {
                    const id = parseInt(btn.dataset.machine, 10);
                    console.log('[gactorio] repairAll', id, '->', this.#ctrl.repairAll(id));
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
            html += this.#lineHtml(line, snap.lines.length);
        }
        document.getElementById('factory-content').innerHTML =
            html || '<div class="empty-factory">No production lines.</div>';
    }

    #lineHtml(line, totalLines) {
        const activeStations = line.machines.filter(m => m.state === 'Working' || m.state === 'Maintenance').length;

        const canDisappear = line.isRemovable && totalLines > 1;
        const disappearTitle = totalLines <= 1
            ? 'At least one line must remain'
            : line.isRemovable
                ? 'Remove this production line'
                : 'Line is busy';
        const disappearBtn = `
            <button class="small danger" data-act="removeLine" data-line="${line.id}"
                    title="${esc(disappearTitle)}" ${canDisappear ? '' : 'disabled'}>Disappear</button>`;
        const beltWidth = (line.machines.length * 205) + (Math.max(0, line.machines.length - 1) * 44);
        const machineCards = line.machines.map((m, index) => `
            ${this.#machineCard(m, index)}
            ${index < line.machines.length - 1 ? '<div class="belt-link" aria-hidden="true"><span></span></div>' : ''}
        `).join('');

        return `
            <div class="line">
                <div class="line-header">
                    <div>
                        <h3>${esc(line.name)}</h3>
                        <div class="meta">Queue/WIP: <b>${line.queueLength}</b> &middot; Active stations: <b>${activeStations}</b></div>
                    </div>
                    ${disappearBtn}
                </div>
                <div class="conveyor-line" style="--belt-width:${beltWidth}px">
                    <div class="conveyor-belt" aria-hidden="true"></div>
                    <div class="machine-flow">${machineCards}</div>
                </div>
            </div>`;
    }

    #machineCard(m, index) {
        const stateCls = `state-${m.state.toLowerCase()}`;
        const cardCls = `machine-card machine-${m.state.toLowerCase()}`;
        const progress = Math.max(0, Math.min(1, m.progress));
        const progressPct = Math.round(progress * 100);
        const hpPct = Math.max(0, Math.min(100, m.health));
        const repairAllBtn = m.state === 'Broken'
            ? `<button class="small danger" data-act="repairAll" data-machine="${m.id}">Repair All</button>`
            : '';

        return `
            <article class="${cardCls}">
                <div class="machine-card-top">
                    <span class="station-index">${String(index + 1).padStart(2, '0')}</span>
                    <span class="machine-badge">${esc(m.type)}</span>
                    <b class="${stateCls}">${esc(m.state)}</b>
                </div>
                <h4>${esc(m.name)}</h4>
                <div class="machine-route">${esc(m.type)} process station</div>
                <div class="meter-row">
                    <span>Progress</span>
                    <span>${progressPct}%</span>
                </div>
                <progress max="1" value="${progress.toFixed(3)}"></progress>
                <div class="meter-row">
                    <span>HP</span>
                    <span>${m.health.toFixed(0)}</span>
                </div>
                <div class="hp-meter"><span style="width:${hpPct.toFixed(0)}%"></span></div>
                <div class="machine-actions">
                    <button class="small" data-act="repair" data-machine="${m.id}">Repair</button>
                    ${repairAllBtn}
                </div>
            </article>`;
    }
}
