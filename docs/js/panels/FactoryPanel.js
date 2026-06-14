// =============================================================================
// FactoryPanel - production lines rendered as conveyor-belt shop-floor views.
//
// Renders every LineView from the snapshot. Dynamic buttons are rebuilt with
// each render, so actions use one delegated pointerdown listener scoped to
// #factory-content.
// =============================================================================

import { UIComponent } from '../UIComponent.js';
import { esc } from '../util.js';

const SCENARIO_OPTIONS = [
    { id: 'normal-flow', label: 'Normal Flow' },
    { id: 'random-breakdowns', label: 'Random Breakdowns' },
];

export class FactoryPanel extends UIComponent {
    #ctrl;
    #selection;
    #isLineInteracting = false;
    #lineInteractionTimer = 0;
    #scrollLeftByLine = new Map();

    constructor(controller, selection = { machineId: null }) {
        super();
        this.#ctrl = controller;
        this.#selection = selection;
    }

    bind() {
        const beginMouseDrag = (scroller, startX) => {
            const startScrollLeft = scroller.scrollLeft;
            this.#holdLineInteraction();
            scroller.classList.add('dragging');

            const onMove = event => {
                scroller.scrollLeft = startScrollLeft - (event.clientX - startX);
                this.#rememberScroll(scroller);
                this.#syncDragbar(scroller);
            };
            const onUp = () => {
                this.#releaseLineInteractionSoon();
                scroller.classList.remove('dragging');
                window.removeEventListener('mousemove', onMove);
                window.removeEventListener('mouseup', onUp);
            };

            window.addEventListener('mousemove', onMove);
            window.addEventListener('mouseup', onUp);
        };

        const beginPointerDrag = (scroller, event) => {
            const startX = event.clientX;
            const startScrollLeft = scroller.scrollLeft;
            this.#holdLineInteraction();
            scroller.classList.add('dragging');
            try {
                scroller.setPointerCapture(event.pointerId);
            } catch (_err) {
                // Pointer capture is an enhancement; window listeners below
                // still keep drag scrolling usable if capture is unavailable.
            }

            const onMove = moveEvent => {
                scroller.scrollLeft = startScrollLeft - (moveEvent.clientX - startX);
                this.#rememberScroll(scroller);
                this.#syncDragbar(scroller);
            };
            const onUp = () => {
                this.#releaseLineInteractionSoon();
                scroller.classList.remove('dragging');
                scroller.removeEventListener('pointermove', onMove);
                scroller.removeEventListener('pointerup', onUp);
                scroller.removeEventListener('pointercancel', onUp);
                window.removeEventListener('pointermove', onMove);
                window.removeEventListener('pointerup', onUp);
                window.removeEventListener('pointercancel', onUp);
            };

            scroller.addEventListener('pointermove', onMove);
            scroller.addEventListener('pointerup', onUp);
            scroller.addEventListener('pointercancel', onUp);
            window.addEventListener('pointermove', onMove);
            window.addEventListener('pointerup', onUp);
            window.addEventListener('pointercancel', onUp);
        };

        document.getElementById('btn-add-line').addEventListener('click', () => {
            const id = this.#ctrl.addLine();
            console.log('[gactorio] addLine -> id', id);
        });

        document.getElementById('factory-content').addEventListener('pointerdown', e => {
            if (e.target.closest('.scenario-select')) {
                this.#holdLineInteraction();
                return;
            }

            const btn = e.target.closest('button[data-act]');
            if (btn && btn.disabled) return;

            if (btn) {
                try {
                    switch (btn.dataset.act) {
                    case 'removeLine': {
                        const id = parseInt(btn.dataset.line, 10);
                        console.log('[gactorio] removeLine', id, '->', this.#ctrl.removeLine(id));
                        break;
                    }
                    }
                } catch (err) {
                    console.error('[gactorio] factory action threw:', err);
                }
                return;
            }

            const slider = e.target.closest('.conveyor-slider');
            if (slider) {
                e.preventDefault();
                this.#holdLineInteraction();
                const updateSlider = event => {
                    const line = slider.closest('.line');
                    const scroller = line?.querySelector('.conveyor-line');
                    if (!scroller) return;

                    const rect = slider.getBoundingClientRect();
                    const ratio = rect.width <= 0
                        ? 0
                        : Math.max(0, Math.min(1, (event.clientX - rect.left) / rect.width));
                    const max = Number(slider.max || 0);
                    const nextValue = Math.round(ratio * max);
                    slider.value = String(nextValue);
                    scroller.scrollLeft = nextValue;
                    this.#rememberScroll(scroller);
                    this.#syncDragbar(scroller);
                };
                const onRelease = () => {
                    this.#releaseLineInteractionSoon();
                    window.removeEventListener('pointermove', updateSlider);
                    window.removeEventListener('pointerup', onRelease);
                    window.removeEventListener('pointercancel', onRelease);
                    window.removeEventListener('mouseup', onRelease);
                };
                updateSlider(e);
                window.addEventListener('pointermove', updateSlider);
                window.addEventListener('pointerup', onRelease);
                window.addEventListener('pointercancel', onRelease);
                window.addEventListener('mouseup', onRelease);
                return;
            }

            const card = e.target.closest('.machine-card[data-machine-id]');
            if (card) {
                this.#selection.machineId = parseInt(card.dataset.machineId, 10);
                return;
            }

            const scroller = e.target.closest('.conveyor-line');
            if (!scroller || e.button !== 0) return;
            e.preventDefault();
            beginPointerDrag(scroller, e);
            return;
        });

        document.getElementById('factory-content').addEventListener('mousedown', e => {
            if (e.target.closest('button')) return;
            if (e.target.closest('.conveyor-slider')) return;
            const scroller = e.target.closest('.conveyor-line');
            if (!scroller || e.button !== 0) return;

            e.preventDefault();
            beginMouseDrag(scroller, e.clientX);
        });

        document.getElementById('factory-content').addEventListener('wheel', e => {
            const scroller = e.target.closest('.conveyor-line');
            if (!scroller) return;
            if (Math.abs(e.deltaY) <= Math.abs(e.deltaX)) return;

            scroller.scrollLeft += e.deltaY;
            this.#rememberScroll(scroller);
            this.#syncDragbar(scroller);
            this.#holdLineInteraction(350);
            e.preventDefault();
        }, { passive: false });

        document.getElementById('factory-content').addEventListener('input', e => {
            const slider = e.target.closest('.conveyor-slider');
            if (!slider) return;
            const line = slider.closest('.line');
            const scroller = line?.querySelector('.conveyor-line');
            if (!scroller) return;
            scroller.scrollLeft = Number(slider.value);
            this.#rememberScroll(scroller);
            this.#syncDragbar(scroller);
            this.#holdLineInteraction(350);
        });

        document.getElementById('factory-content').addEventListener('change', e => {
            const select = e.target.closest('.scenario-select');
            if (!select) return;

            const lineId = parseInt(select.dataset.line, 10);
            const scenarioId = select.value;
            try {
                const ok = this.#ctrl.setLineScenario(lineId, scenarioId);
                if (!ok) {
                    console.warn('[gactorio] setLineScenario failed', lineId, scenarioId);
                }
            } catch (err) {
                console.warn('[gactorio] setLineScenario threw:', err);
            }
            this.#releaseLineInteractionSoon();
        });

        document.getElementById('factory-content').addEventListener('focusin', e => {
            if (e.target.closest('.scenario-select')) {
                this.#holdLineInteraction();
            }
        });

        document.getElementById('factory-content').addEventListener('focusout', e => {
            if (e.target.closest('.scenario-select')) {
                this.#releaseLineInteractionSoon();
            }
        });

        document.getElementById('factory-content').addEventListener('scroll', e => {
            const scroller = e.target.closest?.('.conveyor-line');
            if (!scroller) return;
            this.#rememberScroll(scroller);
            this.#syncDragbar(scroller);
            this.#holdLineInteraction(200);
        }, true);
    }

    render(snap) {
        if (this.#isLineInteracting) {
            this.#syncDragbars();
            return;
        }

        let html = '';
        for (const line of snap.lines) {
            html += this.#lineHtml(line, snap.lines.length);
        }
        document.getElementById('factory-content').innerHTML =
            html || '<div class="empty-factory">No production lines.</div>';
        this.#restoreScrollPositions();
        this.#syncDragbars();
    }

    #lineHtml(line, totalLines) {
        const activeStations = line.machines.filter(m => m.state === 'Working' || m.state === 'Maintenance').length;
        const scenarioId = line.scenarioId || 'normal-flow';
        const scenarioName = line.scenarioName || 'Normal Flow';
        const queueCapacity = Number(line.queueCapacity || 0);
        const droppedTaskCount = Number(line.droppedTaskCount || 0);
        const capacityText = queueCapacity === 0 ? 'Unlimited' : String(queueCapacity);
        const scenarioOptions = SCENARIO_OPTIONS.map(option => `
            <option value="${esc(option.id)}" ${option.id === scenarioId ? 'selected' : ''}>${esc(option.label)}</option>
        `).join('');

        const canDisappear = line.isRemovable && totalLines > 1;
        const disappearTitle = totalLines <= 1
            ? 'At least one line must remain'
            : line.isRemovable
                ? 'Remove this production line'
                : 'Line is busy';
        const disappearBtn = `
            <button class="small danger" data-act="removeLine" data-line="${line.id}"
                    title="${esc(disappearTitle)}" ${canDisappear ? '' : 'disabled'}>Disappear</button>`;
        const cardWidth = 168;
        const linkWidth = 24;
        const beltWidth = (line.machines.length * cardWidth) + (Math.max(0, line.machines.length - 1) * linkWidth);
        const machineCards = line.machines.map((m, index) => `
            ${this.#machineCard(m, index)}
            ${index < line.machines.length - 1 ? '<div class="belt-link" aria-hidden="true"></div>' : ''}
        `).join('');

        return `
            <div class="line">
                <div class="line-header">
                    <div>
                        <h3>${esc(line.name)}</h3>
                        <div class="scenario-control">
                            <label>
                                Scenario
                                <select class="scenario-select" data-line="${line.id}" aria-label="${esc(line.name)} scenario">
                                    ${scenarioOptions}
                                </select>
                            </label>
                            <span>${esc(scenarioName)}</span>
                        </div>
                        <div class="meta">
                            Queue/WIP: <b>${line.queueLength}</b> &middot;
                            Active stations: <b>${activeStations}</b> &middot;
                            Capacity: <b>${esc(capacityText)}</b> &middot;
                            Dropped: <b>${droppedTaskCount}</b>
                        </div>
                    </div>
                    ${disappearBtn}
                </div>
                <div class="conveyor-shell">
                    <div class="conveyor-line" data-line-id="${line.id}" style="--belt-width:${beltWidth}px">
                        <div class="conveyor-belt" aria-hidden="true"></div>
                        <div class="machine-flow">${machineCards}</div>
                    </div>
                    <input class="conveyor-slider" type="range" min="0" max="0" value="0" aria-label="${esc(line.name)} horizontal scroll">
                </div>
            </div>`;
    }

    #machineCard(m, index) {
        const stateCls = `state-${m.state.toLowerCase()}`;
        const selectedCls = this.#selection.machineId === m.id ? ' selected' : '';
        const cardCls = `machine-card machine-${m.state.toLowerCase()}${selectedCls}`;
        const progress = Math.max(0, Math.min(1, m.progress));
        const progressPct = Math.round(progress * 100);
        const hpPct = Math.max(0, Math.min(100, m.health));

        return `
            <article class="${cardCls}" data-machine-id="${m.id}">
                <div class="machine-card-top">
                    <span class="station-index">${String(index + 1).padStart(2, '0')}</span>
                    <b class="${stateCls}">${esc(m.state)}</b>
                </div>
                <h4>${esc(m.name)}</h4>
                <div class="machine-badge">${esc(m.type)}</div>
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
            </article>`;
    }

    #syncDragbars() {
        for (const scroller of document.querySelectorAll('.conveyor-line')) {
            this.#syncDragbar(scroller);
        }
    }

    #rememberScroll(scroller) {
        const lineId = scroller.dataset.lineId;
        if (!lineId) return;
        this.#scrollLeftByLine.set(lineId, scroller.scrollLeft);
    }

    #holdLineInteraction(durationMs = 0) {
        this.#isLineInteracting = true;
        if (this.#lineInteractionTimer !== 0) {
            clearTimeout(this.#lineInteractionTimer);
            this.#lineInteractionTimer = 0;
        }
        if (durationMs > 0) {
            this.#lineInteractionTimer = setTimeout(() => {
                this.#isLineInteracting = false;
                this.#lineInteractionTimer = 0;
            }, durationMs);
        }
    }

    #releaseLineInteractionSoon() {
        if (this.#lineInteractionTimer !== 0) {
            clearTimeout(this.#lineInteractionTimer);
        }
        this.#lineInteractionTimer = setTimeout(() => {
            this.#isLineInteracting = false;
            this.#lineInteractionTimer = 0;
        }, 150);
    }

    #restoreScrollPositions() {
        for (const scroller of document.querySelectorAll('.conveyor-line')) {
            const lineId = scroller.dataset.lineId;
            if (!lineId || !this.#scrollLeftByLine.has(lineId)) continue;
            scroller.scrollLeft = this.#scrollLeftByLine.get(lineId);
        }
    }

    #syncDragbar(scroller) {
        const slider = scroller.closest('.conveyor-shell')?.querySelector('.conveyor-slider');
        if (!slider) return;

        const maxScrollLeft = scroller.scrollWidth - scroller.clientWidth;
        if (maxScrollLeft <= 0) {
            slider.classList.add('is-hidden');
            return;
        }

        slider.classList.remove('is-hidden');
        slider.max = String(Math.round(maxScrollLeft));
        slider.value = String(Math.round(scroller.scrollLeft));
    }
}
