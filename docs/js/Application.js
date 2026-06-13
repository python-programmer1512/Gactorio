// =============================================================================
// Application — 메인 루프 소유자 (View 의 심장)
// -----------------------------------------------------------------------------
// ImGui 시절 src/views/Application.{h,cpp} 의 대응물:
//   * GLFW 윈도우 + vsync 루프  →  requestAnimationFrame 루프
//   * "프레임마다 ui.renderAll()" →  10Hz 로 throttle 한 DOM 재구성
//     (60Hz 로 innerHTML 을 매번 갈아끼우면 클릭 도중 버튼이 사라진다. FactoryPanel 의
//      pointerdown 처리와 짝을 이루는 방어책.)
//
// ★ C++ 연결: View 는 Model 을 절대 만지지 않는다. 모든 명령/조회는 controller 프록시
//   (Module.Controller ⇒ ctrl::Controller ⇒ gactorio::*)를 통해서만 오간다.
//   - 매 프레임 this.#ctrl.tick(dt)  → C++ 시뮬레이션 1틱 진행
//   - 10Hz 로 this.#ctrl.snapshot() → C++ 가 만든 FactoryView 를 받아 화면 갱신
//
// has-a: Application ◆ AppUI(#ui), → Module.Controller(#ctrl, 호출 대상)
// =============================================================================

import { AppUI } from './AppUI.js';
import { toPlainSnapshot } from './util.js';

export class Application {
    static RENDER_INTERVAL_MS = 100;   // DOM 갱신 주기: 10Hz

    #ctrl;                  // Module.Controller (embind 프록시 = Controller 경계)
    #ui = new AppUI();      // 패널 합성기
    #lastTime = 0;          // 직전 프레임 시각(틱 delta 계산용)
    #lastRenderMs = 0;      // 직전 렌더 시각(throttle 용)
    #tickCount = 0;

    constructor(controller) {
        this.#ctrl = controller;
    }

    addPanel(panel) {
        this.#ui.addPanel(panel);
    }

    // 루프 시작: 시작 시각 기록 후 첫 프레임 예약.
    run() {
        this.#lastTime = performance.now();
        requestAnimationFrame(this.#frame);
    }

    // 매 애니메이션 프레임 콜백(화살표 함수라 this 바인딩 유지).
    #frame = (now) => {
        const dt = (now - this.#lastTime) / 1000;   // 초 단위 delta
        this.#lastTime = now;

        // 시뮬레이션은 매 vsync 마다 진행. C++ SimClock 이 pause 를 내부 처리하므로
        // 일시정지 중 tick 호출은 무해(0초 진행).
        this.#ctrl.tick(dt);
        this.#tickCount += 1;

        // 렌더는 10Hz 로 제한: 스냅샷을 plain 데이터로 변환해 패널들에 전달.
        if (now - this.#lastRenderMs >= Application.RENDER_INTERVAL_MS) {
            const snapshot = toPlainSnapshot(this.#ctrl.snapshot());
            snapshot.tickCount = this.#tickCount;
            this.#ui.renderAll(snapshot);
            this.#lastRenderMs = now;
        }

        requestAnimationFrame(this.#frame);   // 다음 프레임 예약
    };
}
