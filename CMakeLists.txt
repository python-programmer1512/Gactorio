cmake_minimum_required(VERSION 3.20)
project(Gactorio LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(GACTORIO_BUILD_GUI   "Build the ImGui GUI executable"  ON)
option(GACTORIO_BUILD_TESTS "Build BE smoke tests"             ON)

# =============================================================================
# Backend library — pure simulation (no UI dependencies).
# =============================================================================
add_library(gactorio_backend
    src/common/Types.cpp
    src/common/SimClock.cpp
    src/model/Item.cpp
    src/model/Product.cpp
    src/model/ProductionTask.cpp
    src/model/Recipe.cpp
    src/model/Inventory.cpp
    src/model/MachineState.cpp
    src/model/MachineStates.cpp
    src/model/Machine.cpp
    src/model/ProductionLine.cpp
    src/model/Factory.cpp
    src/model/CarbonationFactory.cpp
    src/model/events/EventBus.cpp
    src/model/events/EventLogObserver.cpp
    src/model/events/StatisticsObserver.cpp
    src/dto/InventorySnapshot.cpp
    src/dto/MachineSnapshot.cpp
    src/dto/ProductionLineSnapshot.cpp
    src/dto/EventSnapshot.cpp
    src/dto/StatisticsSnapshot.cpp
    src/dto/FactorySnapshot.cpp
    src/controller/FactoryController.cpp
)
target_include_directories(gactorio_backend PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Console example (BE only, no GUI deps)
add_executable(gactorio_backend_example src/backend_example.cpp)
target_link_libraries(gactorio_backend_example PRIVATE gactorio_backend)

# =============================================================================
# GUI Application — BE + GLFW + OpenGL + ImGui.
# =============================================================================
if (GACTORIO_BUILD_GUI)
    find_package(glfw3 REQUIRED)
    find_package(OpenGL REQUIRED)

    include(FetchContent)
    FetchContent_Declare(
        imgui
        GIT_REPOSITORY https://github.com/ocornut/imgui.git
        GIT_TAG        v1.91.5
    )
    FetchContent_MakeAvailable(imgui)

    add_library(imgui STATIC
        ${imgui_SOURCE_DIR}/imgui.cpp
        ${imgui_SOURCE_DIR}/imgui_demo.cpp
        ${imgui_SOURCE_DIR}/imgui_draw.cpp
        ${imgui_SOURCE_DIR}/imgui_tables.cpp
        ${imgui_SOURCE_DIR}/imgui_widgets.cpp
        ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
        ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
    )
    target_include_directories(imgui PUBLIC
        ${imgui_SOURCE_DIR}
        ${imgui_SOURCE_DIR}/backends
    )
    target_link_libraries(imgui PUBLIC glfw OpenGL::GL)

    add_executable(gactorio
        src/main.cpp
        src/views/Application.cpp
        src/views/AppUI.cpp
        src/views/FactoryPanel.cpp
    )
    target_include_directories(gactorio PRIVATE src/views)
    target_link_libraries(gactorio PRIVATE
        gactorio_backend
        imgui
        glfw
        OpenGL::GL
    )
endif()

# =============================================================================
# BE smoke tests
# =============================================================================
if (GACTORIO_BUILD_TESTS)
    enable_testing()

    set(SMOKE_TESTS
        product_smoke_test:tests/model/ProductSmokeTest.cpp
        inventory_smoke_test:tests/model/InventorySmokeTest.cpp
        production_task_smoke_test:tests/model/ProductionTaskSmokeTest.cpp
        sim_clock_smoke_test:tests/model/SimClockSmokeTest.cpp
        machine_smoke_test:tests/model/MachineSmokeTest.cpp
        concrete_machine_smoke_test:tests/model/ConcreteMachineSmokeTest.cpp
        machine_state_smoke_test:tests/model/MachineStateSmokeTest.cpp
        observer_smoke_test:tests/model/ObserverSmokeTest.cpp
        production_line_smoke_test:tests/model/ProductionLineSmokeTest.cpp
        factory_flow_smoke_test:tests/model/FactoryFlowSmokeTest.cpp
        factory_controller_smoke_test:tests/controller/FactoryControllerSmokeTest.cpp
    )
    foreach(entry IN LISTS SMOKE_TESTS)
        string(REPLACE ":" ";" pair "${entry}")
        list(GET pair 0 name)
        list(GET pair 1 src)
        add_executable(${name} ${src})
        target_link_libraries(${name} PRIVATE gactorio_backend)
        add_test(NAME ${name} COMMAND ${name})
    endforeach()
endif()
