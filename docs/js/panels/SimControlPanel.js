// =============================================================================
// SimControlPanel — 시뮬레이션 시계/일시정지/리셋/배속 + Memento(저장·되돌리기) +
//                    통계 그리드 (좌측 컬럼 상단)
// -----------------------------------------------------------------------------
// ImGui 시절 C++ SimControlPanel 의 대응물: 컨트롤러 참조를 들고, 읽기 전용 상태를
// 그리며, 버튼 입력을 "명령"으로 컨트롤러에 전달한다. 절대 Model 을 직접 만지지 않는다.
//
// MVC: 입력(버튼/슬라이더) → Module.Controller 명령 호출,  출력(시간/통계) → snapshot 렌더.
// =============================================================================

import { UIComponent } from '../UIComponent.js';

export class SimControlPanel extends UIComponent {
    #ctrl;                 // Module.Controller (명령 대상)
    #running = true;       // 현재 재생 중인지(일시정지 토글 표시용)

    constructor(controller) {
        super();
        this.#ctrl = controller;
    }

    // bind(): DOM 버튼/슬라이더에 이벤트 핸들러를 한 번 연결.
    bind() {
        // Pause/Resume 토글: 상태에 따라 컨트롤러 pause/resume 호출 + 버튼 라벨 갱신.
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

        // Reset: 공장을 초기 상태로 재생성.
        document.getElementById('btn-reset').addEventListener('click', () => {
            this.#ctrl.reset();
        });

        // Memento - 체크포인트 저장(현재 상태를 스택에 push).
        document.getElementById('btn-save').addEventListener('click', () => {
            this.#ctrl.saveCheckpoint();
            console.log('[gactorio] checkpoint saved, history =', this.#ctrl.historySize());
        });

        // Memento - 되돌리기(최근 체크포인트로 복원).
        document.getElementById('btn-undo').addEventListener('click', () => {
            const ok = this.#ctrl.undo();
            console.log('[gactorio] undo →', ok, 'history =', this.#ctrl.historySize());
        });

        // 배속 슬라이더: 값 변경 시 컨트롤러 setSpeed + 라벨 갱신.
        document.getElementById('speed').addEventListener('input', e => {
            const v = parseFloat(e.target.value);
            this.#ctrl.setSpeed(v);
            document.getElementById('speed-label').textContent = v.toFixed(1) + '×';
        });
    }

    // render(): 스냅샷의 시간/통계를 화면에 반영.
    render(snap) {
        document.getElementById('sim-time').textContent = snap.simulationTime.toFixed(2);

        const s = snap.stats;
        document.getElementById('stats-grid').innerHTML = `
            <div>Tasks started     <b>${s.tasksStarted}</b></div>
            <div>Steps completed   <b>${s.stepsCompleted}</b></div>
            <div>Products done     <b>${s.productsDone}</b></div>
            <div>Machines broken   <b>${s.machinesBroken}</b></div>
            <div>Machines repaired <b>${s.machinesRepaired}</b></div>
            <div>State changes     <b>${s.stateChanges}</b></div>`;

        // Memento 상태(히스토리 개수/Undo 가능 여부)는 스냅샷이 아니라 컨트롤러에 직접
        // 묻는다 — 히스토리는 Factory 상태가 아니라 Caretaker 에 있기 때문.
        document.getElementById('history-size').textContent = this.#ctrl.historySize();
        document.getElementById('btn-undo').disabled = !this.#ctrl.canUndo();
    }
}
