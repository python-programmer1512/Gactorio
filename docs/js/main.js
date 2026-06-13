// =============================================================================
// main.js — View 부트스트랩 (수업 자료의 src/main.cpp 에 1:1 대응)
// -----------------------------------------------------------------------------
// ImGui 시절 main.cpp:
//
//     Application app("Gactorio", 1280, 720);
//     gactorio::FactoryController controller;
//     app.addPanel(std::make_unique<FactoryPanel>(controller));
//     app.run();
//
// ★ C++ 연결의 시작점:
//   `Module.Controller` 는 ctrl::Controller 의 embind 프록시다(= 컴파일된 wasm 안의
//   C++ 객체를 JS에서 다루는 손잡이). 여기서 `new Module.Controller()` 로 그 객체를
//   하나 만들고, 모든 패널에 같은 컨트롤러를 주입한다. 어떤 패널도 이 경계를 우회해
//   Model(gactorio::*)을 직접 부르지 않는다.
//
// 이 파일이 수업 과제의 "UI와 백엔드를 잇는 단 하나의 진입 파일"(main.cpp) 역할을 한다.
// =============================================================================

// 쿼리스트링(?v=...)은 브라우저 캐시 무효화용 버전 태그. 코드와는 무관.
import { Application }     from './Application.js?v=20260613-compactline11';
import { SimControlPanel } from './panels/SimControlPanel.js?v=20260613-compactline11';
import { FactoryPanel }    from './panels/FactoryPanel.js?v=20260613-compactline11';
import { ProductsPanel }   from './panels/ProductsPanel.js?v=20260613-compactline11';
import { EventLogPanel }   from './panels/EventLogPanel.js?v=20260613-compactline11';
import { InventoryPanel }  from './panels/InventoryPanel.js?v=20260613-compactline11';

console.log('[gactorio] main.js loaded - build', '2026-06-13-compactline11');

// 부팅: 컨트롤러 1개 생성 → Application 에 주입 → 5개 패널 등록 → 루프 시작.
function boot() {
    console.log('[gactorio] wasm runtime ready, creating Controller');
    const controller = new Module.Controller();   // ★ C++ ctrl::Controller 인스턴스

    const app = new Application(controller);
    app.addPanel(new SimControlPanel(controller)); // 좌상단: 제어/통계/이벤트
    app.addPanel(new FactoryPanel(controller));    // 중앙: 생산라인/기계 카드
    app.addPanel(new ProductsPanel(controller));   // 제품 카탈로그 버튼
    app.addPanel(new EventLogPanel());             // 이벤트 로그(컨트롤러 불필요)
    app.addPanel(new InventoryPanel(controller));  // 재고/보충
    app.run();
}

// 모듈 스크립트는 defer 되므로, 실행 시점에 wasm 런타임이 준비됐을 수도/아닐 수도 있다.
// 두 경우(이미 준비됨 / 준비 콜백 대기)를 모두 처리해 경쟁 조건을 피한다.
if (typeof Module !== 'undefined') {
    if (Module.calledRun) boot();                  // 이미 런타임 준비됨
    else Module.onRuntimeInitialized = boot;        // 준비되면 boot 호출
} else {
    console.error('[gactorio] Module is undefined — gactorio.js failed to load');
}
