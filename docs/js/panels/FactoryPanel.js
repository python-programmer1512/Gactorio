// =============================================================================
// FactoryPanel — 생산라인을 "컨베이어 벨트 + 기계 카드" 형태로 그리는 패널 (중앙)
// -----------------------------------------------------------------------------
// 스냅샷의 모든 LineView 를 렌더한다. 카드/버튼은 매 렌더마다 새로 만들어지므로,
// 개별 버튼에 리스너를 달지 않고 #factory-content 에 위임(delegation) 리스너 하나만 둔다.
//
// 상호작용 중(드래그/슬라이더/휠 스크롤) 에는 #isLineInteracting 플래그를 세워
// innerHTML 재구성을 잠시 멈춘다 — 그래야 드래그 도중 DOM 이 갈아엎혀 끊기지 않는다.
//
// MVC: 버튼(라인추가/제거/수리) → Module.Controller 명령. 그리기 → snapshot 기반.
// (드래그/스크롤 같은 순수 표시 상호작용은 컨트롤러를 부르지 않고 View 내부에서만 처리.)
// =============================================================================

import { UIComponent } from '../UIComponent.js';
import { esc } from '../util.js';

export class FactoryPanel extends UIComponent {
    #ctrl;                          // Module.Controller (명령 대상)
    #selection;
    #isLineInteracting = false;     // 사용자가 라인과 상호작용 중인가(렌더 일시중지 플래그)
    #lineInteractionTimer = 0;      // 상호작용 종료를 늦추는 타이머 핸들
    #scrollLeftByLine = new Map();  // 라인별 가로 스크롤 위치 기억(재렌더 후 복원용)

    constructor(controller, selection = { machineId: null }) {
        super();
        this.#ctrl = controller;
        this.#selection = selection;
    }

    // bind(): 라인 추가 버튼 + 위임 리스너들(버튼 동작, 드래그/휠/슬라이더 스크롤) 연결.
    bind() {
        // 마우스로 컨베이어 라인을 좌우 드래그 스크롤(구형 브라우저/마우스 경로).
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

        // 포인터(마우스/터치/펜) 통합 드래그 스크롤. 가능하면 포인터 캡처로 부드럽게.
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

        // 라인 추가 버튼 → 컨트롤러 addLine 명령(새 4스테이션 라인 생성).
        document.getElementById('btn-add-line').addEventListener('click', () => {
            const id = this.#ctrl.addLine();
            console.log('[gactorio] addLine -> id', id);
        });

        // 위임 리스너: 카드/버튼이 매 렌더 새로 생기므로 부모에서 한 번만 처리.
        // data-act 속성으로 어떤 명령인지 구분해 컨트롤러로 보낸다.
        document.getElementById('factory-content').addEventListener('pointerdown', e => {
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
                return;
            }

            const slider = e.target.closest('.conveyor-slider');
            if (slider) {
                this.#holdLineInteraction();
                const onRelease = () => {
                    this.#releaseLineInteractionSoon();
                    window.removeEventListener('pointerup', onRelease);
                    window.removeEventListener('pointercancel', onRelease);
                    window.removeEventListener('mouseup', onRelease);
                };
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
            const scroller = slider.closest('.conveyor-line');
            if (!scroller) return;
            scroller.scrollLeft = Number(slider.value);
            this.#rememberScroll(scroller);
            this.#syncDragbar(scroller);
            this.#holdLineInteraction(350);
        });

        document.getElementById('factory-content').addEventListener('scroll', e => {
            const scroller = e.target.closest?.('.conveyor-line');
            if (!scroller) return;
            this.#rememberScroll(scroller);
            this.#syncDragbar(scroller);
            this.#holdLineInteraction(200);
        }, true);
    }

    // render(): 상호작용 중이면 DOM 재구성을 건너뛰고(드래그 끊김 방지) 스크롤바만 동기화.
    // 평상시엔 모든 라인 HTML 을 새로 만들어 채우고, 스크롤 위치를 복원한다.
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

    // 라인 1개의 HTML 생성: 헤더(이름/큐·가동수/Disappear 버튼) + 기계 카드들의 벨트.
    // Disappear(라인 제거) 버튼은 라인이 비어 있고(isRemovable) 라인이 2개 이상일 때만 활성.
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
                        <div class="meta">Queue/WIP: <b>${line.queueLength}</b> &middot; Active stations: <b>${activeStations}</b></div>
                    </div>
                    ${disappearBtn}
                </div>
                <div class="conveyor-line" data-line-id="${line.id}" style="--belt-width:${beltWidth}px">
                    <div class="conveyor-belt" aria-hidden="true"></div>
                    <div class="machine-flow">${machineCards}</div>
                    <input class="conveyor-slider" type="range" min="0" max="0" value="0" aria-label="Line scroll">
                </div>
            </div>`;
    }

    // 기계 1대의 카드 HTML: 상태 배지(색은 CSS의 state-*/machine-* 클래스), 진행 막대,
    // HP 막대, Repair(+5) 버튼, 그리고 고장(Broken)일 때만 Repair All 버튼.
    #machineCard(m, index) {
        const stateCls = `state-${m.state.toLowerCase()}`;
        const selectedCls = this.#selection.machineId === m.id ? ' selected' : '';
        const cardCls = `machine-card machine-${m.state.toLowerCase()}${selectedCls}`;
        const progress = Math.max(0, Math.min(1, m.progress));
        const progressPct = Math.round(progress * 100);
        const hpPct = Math.max(0, Math.min(100, m.health));
        const repairAllBtn = m.state === 'Broken'
            ? `<button class="small danger" data-act="repairAll" data-machine="${m.id}">Repair All</button>`
            : '';

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
                <div class="machine-actions">
                    <button class="small" data-act="repair" data-machine="${m.id}">Repair</button>
                    ${repairAllBtn}
                </div>
            </article>`;
    }

    // 아래는 모두 "컨베이어 가로 스크롤" 보조 기능(순수 View 동작, 컨트롤러와 무관).
    // 모든 라인의 스크롤바(슬라이더)를 현재 스크롤 위치에 맞춰 갱신.
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

    // 상호작용 시작 표시. durationMs>0 이면 그 시간 뒤 자동 해제(휠/슬라이더용),
    // 0 이면 명시적 해제(드래그 종료)까지 유지.
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

    // 드래그가 끝나면 약간의 지연 후 상호작용 해제(렌더 재개). 짧은 여유로 깜빡임 방지.
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

    // 한 라인의 슬라이더 max/value 를 스크롤 가능 폭에 맞춰 동기화. 스크롤 여백이 없으면 숨김.
    #syncDragbar(scroller) {
        const slider = scroller.querySelector('.conveyor-slider');
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
