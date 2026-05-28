cmake_minimum_required(VERSION 3.16)

project(Gactorio LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(gactorio_backend
    src/common/Types.cpp
    src/common/SimClock.cpp
    src/model/Item.cpp
    src/model/Product.cpp
    src/model/ProductCatalog.cpp
    src/model/DefaultProducts.cpp
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
    src/controller/SimulationHistory.cpp
    src/controller/FactoryController.cpp
)

target_include_directories(gactorio_backend
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

add_executable(gactorio_backend_example
    src/main.cpp
)

target_link_libraries(gactorio_backend_example
    PRIVATE
        gactorio_backend
)

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
    factory_inventory_enqueue_smoke_test:tests/model/FactoryInventoryEnqueueSmokeTest.cpp
    product_catalog_integration_smoke_test:tests/model/ProductCatalogIntegrationSmokeTest.cpp
    simulation_history_smoke_test:tests/controller/SimulationHistorySmokeTest.cpp
    memento_integration_smoke_test:tests/controller/MementoIntegrationSmokeTest.cpp
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
