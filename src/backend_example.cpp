#include "controller/FactoryController.hpp"

#include <iostream>

// =============================================================================
// backend_example.cpp — GUI 없이 백엔드만 돌려보는 네이티브 점검용 main()
// -----------------------------------------------------------------------------
// emscripten/JS 없이도 Model+Controller 가 정상 동작하는지 확인하는 작은 실행 파일.
// (View 와 완전히 분리돼 있다는 증거이기도 함 — 여기엔 ImGui/DOM/embind 가 전혀 없다.)
// =============================================================================

int main() {
    gactorio::FactoryController controller;   // 기본 음료 공장 구성됨
    controller.tick(1.0);                     // 1초 진행
    controller.tick(2.0);                     // 2초 더 진행

    const auto snapshot = controller.snapshot();   // 현재 상태 스냅샷

    std::cout << "Gactorio backend example\n";
    std::cout << "Simulation time: " << snapshot.timeSeconds() << "s\n";
    std::cout << "Production lines: " << snapshot.productionLines().size() << "\n";
    std::cout << "Events: " << snapshot.events().size() << "\n";

    return 0;
}
