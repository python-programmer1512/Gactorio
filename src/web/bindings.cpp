// =============================================================================
// bindings.cpp — Emscripten(embind) 바인딩: ctrl::Controller 를 JS 에 노출
// =============================================================================
// ★ C++ ↔ JS 연결의 "접착(glue)" 파일 ★
//
// 이 파일은 emcc(Emscripten 컴파일러)로 빌드된다(build_web.ps1 / build_web.sh).
// 빌드 산출물은 web/(→ docs/)에 두 파일:
//   - gactorio.js    : JS 글루. .wasm 을 로드하고 Module.Controller, Module.ProductKind,
//                      Module.FactoryView 등 아래에서 등록한 심볼을 JS 전역(Module)에 만든다.
//   - gactorio.wasm  : 위에서 컴파일된 C++ 바이너리(Model+Controller).
//
// 동작 원리:
//   * EMSCRIPTEN_BINDINGS(...) 블록은 "어떤 C++ 타입/메서드를 JS 에 노출할지" 선언한다.
//   * value_object<T>(...) : C++ 구조체(DTO)를 JS의 plain object 로 자동 변환(필드 매핑).
//                            snapshot() 이 반환하는 FactoryView 가 JS 로 넘어갈 때 이
//                            매핑을 따라 깊은 값-복사(deep copy)된다 → 포인터 누출 없음.
//   * register_vector<T>("...") : std::vector<T> 를 JS 에서 size()/get(i) 로 순회 가능한
//                            핸들로 노출(util.js 의 vecToArray 가 배열로 변환 후 delete).
//   * class_<Controller>(...) : ctrl::Controller 를 JS 클래스로 노출. .function(...) 로
//                            메서드를 하나씩 매핑한다.
//
// JS 사용 예 (docs/js/main.js, panels):
//   const ctrl = new Module.Controller();        // ctrl::Controller 인스턴스 생성
//   ctrl.tick(0.016);                             // 명령 → Model
//   const snap = ctrl.snapshot();                 // 조회 → FactoryView(값 복사)
//   ctrl.enqueueAutoProduct(101);                 // 명령(제품 ID로 enqueue)
//   Module.ProductKind.VoltzClassic               // 노출된 enum 값
//
// 핵심: 여기서 노출하는 건 오직 ctrl::* 경계뿐이다. gactorio::* Model 심볼은 하나도
// 노출하지 않는다 → View(JS)는 Model 을 절대 직접 보지 못한다(MVC 경계 강제).
// =============================================================================

#include <emscripten/bind.h>

#include "controller/Controller.h"

using namespace emscripten;
using namespace ctrl;

EMSCRIPTEN_BINDINGS(gactorio_module) {
    // ---- Enum: JS 에서 Module.ProductKind.VoltzClassic 형태로 사용 ----------
    enum_<ProductKind>("ProductKind")
        .value("Unknown",      ProductKind::Unknown)
        .value("VoltzClassic", ProductKind::VoltzClassic)
        .value("HyperBolt",    ProductKind::HyperBolt)
        .value("AuroraZero",   ProductKind::AuroraZero);

    // ---- Leaf 구조체(DTO): value_object 로 JS plain object 에 1:1 필드 매핑 ----
    // 각 .field 는 (JS 속성명, C++ 멤버 포인터)를 연결한다.
    value_object<MachineView>("MachineView")
        .field("id",       &MachineView::id)
        .field("name",     &MachineView::name)
        .field("type",     &MachineView::type)
        .field("state",    &MachineView::state)
        .field("progress", &MachineView::progress)
        .field("health",   &MachineView::health);
    register_vector<MachineView>("VectorMachineView");   // vector<MachineView> 노출

    value_object<LineView>("LineView")
        .field("id",                  &LineView::id)
        .field("name",                &LineView::name)
        .field("queueLength",         &LineView::queueLength)
        .field("currentTaskName",     &LineView::currentTaskName)
        .field("currentTaskProgress", &LineView::currentTaskProgress)
        .field("isRemovable",         &LineView::isRemovable)
        .field("machines",            &LineView::machines);   // 중첩 vector<MachineView>
    register_vector<LineView>("VectorLineView");

    value_object<EventView>("EventView")
        .field("time",     &EventView::time)
        .field("typeName", &EventView::typeName)
        .field("message",  &EventView::message);
    register_vector<EventView>("VectorEventView");

    value_object<InventoryEntry>("InventoryEntry")
        .field("id",        &InventoryEntry::id)
        .field("name",      &InventoryEntry::name)
        .field("quantity",  &InventoryEntry::quantity)
        .field("isProduct", &InventoryEntry::isProduct);
    register_vector<InventoryEntry>("VectorInventoryEntry");

    value_object<ProductOption>("ProductOption")
        .field("id",              &ProductOption::id)
        .field("key",             &ProductOption::key)
        .field("name",            &ProductOption::name)
        .field("tier",            &ProductOption::tier)
        .field("durationSeconds", &ProductOption::durationSeconds)
        .field("requirements",    &ProductOption::requirements);
    register_vector<ProductOption>("VectorProductOption");

    value_object<Statistics>("Statistics")
        .field("tasksStarted",     &Statistics::tasksStarted)
        .field("stepsCompleted",   &Statistics::stepsCompleted)
        .field("productsDone",     &Statistics::productsDone)
        .field("machinesBroken",   &Statistics::machinesBroken)
        .field("machinesRepaired", &Statistics::machinesRepaired)
        .field("stateChanges",     &Statistics::stateChanges);

    // FactoryView: 한 프레임의 화면 데이터 전체. lines/events/inventory 는 위에서
    // 등록한 vector 들이라 그대로 중첩 매핑된다.
    value_object<FactoryView>("FactoryView")
        .field("simulationTime", &FactoryView::simulationTime)
        .field("stats",          &FactoryView::stats)
        .field("lines",          &FactoryView::lines)
        .field("events",         &FactoryView::events)
        .field("inventory",      &FactoryView::inventory);

    // ---- Controller: JS 의 Module.Controller 클래스로 노출 ------------------
    // snapshot() 은 캐시된 FactoryView 의 *복사본*을 JS 로 넘긴다(value_object 가 deep
    // copy 수행). C++ 쪽은 틱마다 캐시를 재사용하므로 비용이 작다.
    // 아래 .function 들이 곧 JS 에서 호출 가능한 메서드 목록이다(= View가 쓰는 명령/조회).
    class_<Controller>("Controller")
        .constructor<>()
        .function("tick",         &Controller::tick)
        .function("pause",        &Controller::pause)
        .function("resume",       &Controller::resume)
        .function("reset",        &Controller::reset)
        .function("setSpeed",     &Controller::setSpeed)
        .function("enqueue",      &Controller::enqueue)
        .function("enqueueProduct", &Controller::enqueueProduct)
        .function("enqueueAuto",  &Controller::enqueueAuto)
        .function("enqueueAutoProduct", &Controller::enqueueAutoProduct)
        .function("addLine",      &Controller::addLine)
        .function("removeLine",   &Controller::removeLine)
        .function("breakMachine", &Controller::breakMachine)
        .function("repair",         &Controller::repair)
        .function("restockItem",    &Controller::restockItem)
        .function("repairAll",      &Controller::repairAll)
        .function("saveCheckpoint", &Controller::saveCheckpoint)
        .function("undo",           &Controller::undo)
        .function("canUndo",        &Controller::canUndo)
        .function("historySize",    &Controller::historySize)
        .function("products",       &Controller::products)
        .function("snapshot",       &Controller::snapshot);
}
