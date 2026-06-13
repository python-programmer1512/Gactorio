# Gactorio OOPS Presentation Script

Team members: Go Won-gyu, Cho Yong-bin  
Last revised: 26.06.13

---

# 1. Application UI Demonstration - 1 min

First, we will open the Gactorio executable file and show the actual UI.

At the top of the screen, you can see the title `Gactorio` and a description saying that it is an energy drink factory simulator. This project is structured so that the C++ Backend is built into WebAssembly and controlled through an HTML, CSS, and JavaScript View.

In the `Simulation Control` area on the left, you can check the current simulation time and control the simulation status and speed using the `Pause`, `Reset`, and `Speed` slider. If we increase the speed here, time passes faster. If we press pause, the production progress stops.

Below that, in the `Memento` area, there are `Save Checkpoint` and `Undo` buttons. After saving the current factory state as a checkpoint, even if product production or machine states change, pressing undo restores the previous state. The history number shows how many checkpoints are currently saved.

In the center `Factory` area, production lines and machine cards are displayed. Each line has machines such as Mixing, Quality, Bottling, and Packaging. When a product task is added, we can see the machine status and progress change. We can also add a new production line using the `+ Add Line` button.

On the right side, in the `Products` area, we can select products such as Voltz Classic, Hyper Bolt, and Aurora Zero and add them to the production queue. In the `Inventory` area, we can check raw materials and finished product stock, and we can also restock raw materials.

Finally, the `Statistics` and `Event Log` areas display events and statistics collected through the Observer pattern, such as production started, process completed, product completed, machine broken, and repair completed. In other words, this UI is not just a simple screen. It is the result of the Backend’s Factory, Controller, DTO, and Event structures actually being connected and working together.

---

# 2. Implemented Scenarios - 1 min

We implemented three representative scenarios.

The first is the product production scenario. When the user selects a product in the Products panel, the View does not directly modify the Model. Instead, it sends an enqueue command to the Controller. The Controller forwards the request to the Factory, and the Factory checks and consumes the required raw materials from the Inventory. Then it creates a ProductionTask and adds it to the ProductionLine queue. After that, as ticks proceed, the ProductionLine assigns tasks to idle Machines, and each Machine increases the process progress. When all ProcessSteps are finished, the finished product quantity is reflected in the Inventory, and the Event Log and Statistics are also updated.

The second is the machine breakdown and repair scenario. During production, if a Machine’s health decreases or a break command is called, the Machine enters the Broken state. In the Broken state, production does not proceed even if update is called. After that, when a repair or repairAll command is received, the Machine changes to the Maintenance state. Once the repair time has passed, it returns to an available state. This flow is implemented using the State Pattern, so the behavior of each state — Idle, Working, Broken, and Maintenance — is separated.

The third is the checkpoint and undo scenario. When the user presses Save Checkpoint, the FactoryController saves the current state of the Factory as a FactoryMemento and stores it in the SimulationHistory stack. After that, even if a line is added or a product is produced, pressing Undo retrieves the most recent FactoryMemento and allows the Factory to restore its time, inventory, production line queues, and machine states. This feature was implemented using the Memento Pattern.

---

# 3. Class Diagrams and Class Structures - 13 min

Now, we will explain the overall class structure while looking at the class diagram.

## 3.1 Overall Class Diagram

First, this is the overall class diagram.

The Backend is largely divided into four areas: `Controller`, `DTO`, `Common`, and `Model`. The most important point in the overall flow is that external requests enter through the `FactoryController`, the `FactoryController` manipulates the Model, and then returns the result to the outside in the form of DTO Snapshots.

`FactoryController` is the entry point of the Backend. It is the class that users directly access when they run the simulation, add products, or use checkpoint and undo features. Internally, it owns a `CarbonationFactory` through a `unique_ptr`, and it also has a `SimulationHistory` for the undo feature.

The DTO area consists of read-only state objects delivered to the outside. `FactorySnapshot` represents the entire factory state, and it contains `InventorySnapshot`, `ProductionLineSnapshot`, `MachineSnapshot`, `EventSnapshot`, and `StatisticsSnapshot`. The key point of this structure is that Model objects are not directly exposed to the outside. Instead, the current state is copied and delivered as values.

The Common area contains `SimClock`, common enums, and ID types. `SimClock` handles simulation time, speed multiplier, and pause state. `Types.hpp` defines common types such as `ProductId`, `MachineId`, `ItemType`, `MachineStatus`, and `EventType`.

The Model area contains the actual simulation logic. At the center is `Factory`, and `Factory` manages `Inventory`, `ProductionLine`, `EventBus`, `EventLogObserver`, `StatisticsObserver`, and `SimClock`. `ProductionLine` has the task queue and Machine list and performs the actual production process. `Machine` processes each production step.

Now, I will first explain the has-a relationships. `FactoryController` has a `CarbonationFactory` and a `SimulationHistory`. `Factory` has a `SimClock`, an `Inventory`, a list of `ProductionLine` objects, an `EventBus`, and Observer objects. `ProductionLine` has machines and a task queue. `FactorySnapshot` has other snapshot DTOs. These relationships are represented as composition or aggregation, showing where each object’s lifecycle and responsibility belong.

On the other hand, is-a relationships are inheritance relationships. `CarbonationFactory` is a `Factory`. `MixingStation`, `QualityStation`, `BottlingStation`, and `PackagingStation` are `Machine` objects. `IdleState`, `WorkingState`, `BrokenState`, and `MaintenanceState` are `MachineState` objects. `EventLogObserver` and `StatisticsObserver` are `Observer` objects. Thanks to these inheritance relationships, Factory or EventBus can operate based on abstract roles rather than concrete classes.

There are also dependency relationships. `FactoryController` depends on the Model layer, but the View does not directly depend on the Model. The View depends only on the View-friendly DTOs such as `ctrl::Controller` and `FactoryView`. Inside the Model, `Machine` depends on an `EventBus` pointer to publish events, and Panels depend on `Module.Controller` to send commands. These dependencies are restricted so that they flow only in the necessary direction.

## 3.2 Core Simulation Class Diagram

Next is the Core Simulation Class Diagram.

The key point of this diagram is how production lines, machines, tasks, products, and inventory cooperate around the `Factory`. `Factory` acts as the aggregate root. In other words, it is the central object that manages time, inventory, production lines, and the event system inside the factory.

`Factory` updates time using `SimClock` and manages raw material and finished product quantities using `Inventory`. Multiple `ProductionLine` objects are the units where actual production happens, and each line has a waiting `ProductionTask` queue and a list of `Machine` objects that process the actual steps.

`ProductionTask` represents the production progress of one product. It references one `Product` and manages which process step is currently being performed using `currentStepIndex`. Since each product has a different process order, it checks the currently required machine role by following the `ProcessStep` list owned by the Product.

`Machine` is an abstract base class that performs the actual process. Concrete machines inherit from `Machine` and perform tasks according to the roles they can handle. For example, a machine with the Mixing role handles the mixing process, while a machine with the Bottling role handles the bottling process.

The execution flow starts from `FactoryController::tick(deltaTime)`. The Controller does not directly handle production logic. Instead, it calls `Factory::update()`. `Factory` first updates the `SimClock`, then assigns available tasks to Machines for each ProductionLine. After that, Machine updates are performed. When a process is completed, the task moves to the next step or is completed as a finished product. The completed product is reflected through `Inventory::addProduct()`, and the necessary events are published through the `EventBus`.

This structure is easy to modify because the overall production flow and detailed roles are separated. When adding a new product, we only need to add a ProductDefinition to the ProductCatalog. When adding a new machine, we can create a new class that inherits from Machine. This allows us to extend detailed objects while keeping the overall orchestration of the Factory unchanged.

## 3.3 Product Domain Class Diagram

Next is the Product Domain Class Diagram.

This diagram shows how a product is defined as data and how that data is used in production tasks and inventory management. The central classes are `ProductCatalog`, `ProductDefinition`, `Product`, `Inventory`, and `ProductionTask`.

`ProductCatalog` is a registry that stores product definitions. Each product’s ID, name, type, required materials, and process route are stored in the form of a `ProductDefinition`. A `ProductDefinition` can be seen as a blueprint for making one product.

`ProductDefinition` has two key pieces of data. The first is `ItemRequirement`. It represents which raw materials and how many of them are needed to make one product. The second is `ProcessStep`. It represents the order of processes required to produce the product, and each step contains the required `MachineRole` and base processing time.

`ProductCatalog` creates actual `Product` objects based on these definitions. `ProductionTask` references a Product and manages the current process progress. `Inventory` does not store actual Product objects. Instead, it stores raw material quantities based on `ItemType` and finished product quantities based on `ProductId`.

Here, the has-a relationship is that `Product` has item requirements and process steps. `ProductionTask` has a Product reference and a current step index. `Inventory` has maps of item quantities and product quantities.

For is-a relationships, `VoltzClassic`, `HyperBolt`, and `AuroraZero` are concrete products of `Product`. On the raw material side, `Ingredient`, `Water`, `EmptyBottle`, `Label`, and `Package` belong to the `Item` hierarchy.

This design supports the Open-Closed Principle. When adding a new product, we can add a product definition instead of modifying the production line or Machine code. Since the production system only reads the Product’s requirements and route, it is not tightly coupled to specific product names.

## 3.4 Machine State Diagram

Next is the Machine State Diagram.

The key point of this diagram is that `Machine` does not operate using only a single simple state value. Instead, it uses the State Pattern to separate state-specific behavior into separate classes.

A Machine has four states: `Idle`, `Working`, `Broken`, and `Maintenance`. Idle means the machine is waiting for work, and Working means production is in progress. Broken means the machine is broken, so production does not proceed even when update is called. Maintenance means the machine is under repair, and once the repair is completed, it returns to an available state.

`Machine` has a `MachineState` object representing its current state. When update is called, the actual behavior is delegated to this State object. For example, if the current state is `WorkingState`, the production progress logic is executed. If the current state is `MaintenanceState`, the repair progress logic is executed.

Here, the `MachineStatus` enum is a value used for external display and storage. When showing the machine state in a DTO Snapshot or saving and restoring it through Memento, an enum value is needed. However, the actual behavior is handled by the `MachineState` classes.

This design follows the Single Responsibility Principle and the Open-Closed Principle. `Machine` manages common machine data and state transitions, while each State class handles the update logic for each state. If a new state is needed, we can extend the system by adding a new MachineState implementation instead of adding a huge if-statement inside Machine.

## 3.5 Observer / Event Pattern Diagram

Next is the Observer / Event Pattern Diagram.

The key point of this structure is that events generated during the production process are delivered by the `EventBus`, and multiple Observers receive those events and perform their own roles.

The main event publishers are `ProductionLine` and `Machine`. Events are generated when a task is added to the queue, when a task starts, when a process is completed, when a product is completed, when a machine breaks down, and when repair is completed.

The important point is that ProductionLine or Machine does not directly modify logs or statistics. Instead, they create an `Event` object and publish it through `EventBus::publish()`. The `Event` contains information such as the time of occurrence, event type, related machine ID, and message.

`EventBus` has a list of registered `Observer` objects. When an event is published, it calls each Observer’s `onEvent()` method. Representative Observers are `EventLogObserver` and `StatisticsObserver`. `EventLogObserver` stores events in an internal list, and `StatisticsObserver` updates statistics such as the number of completed products and machine breakdowns based on the event type.

The dependency in this design is very clear. The production logic depends only on EventBus and does not depend on the specific processing method of logs or statistics. If we need a new notification feature or monitoring feature, we can add a new Observer. There is no need to significantly modify the existing Machine or ProductionLine code.

This part is also related to the Dependency Inversion Principle. Event handlers are connected through the `Observer` interface, and EventBus does not depend on the internal implementation of concrete Observers.

## 3.6 Memento Pattern Diagram

Next is the Memento Pattern Diagram.

The key point of this diagram is that roles are separated so that the simulation state can be saved as a checkpoint and restored to a previous state when needed.

There are three important roles in the Memento Pattern. The first is the Originator, and in this project, `Factory` plays that role. Factory can save the current simulation state as a `FactoryMemento`, and it can also receive a FactoryMemento and restore its own state.

The second is the Memento. This role is handled by `FactoryMemento`, `LineMemento`, and `MachineMemento`. `FactoryMemento` stores the entire factory state, including the states of production lines and machines. `LineMemento` stores production line queue information, and `MachineMemento` stores information such as the status and health of each machine.

Here, the Memento objects are not structs with public fields. Instead, they are classes with private fields and getters. This means Factory and the restoration logic can read the required values, but external code cannot directly modify the internal values of the snapshot. This strengthens encapsulation.

The third role is the Caretaker, and `SimulationHistory` plays this role. `SimulationHistory` stores FactoryMementos in a stack structure and supports checkpoint saving and undo. However, it does not directly modify internal Factory objects. The Originator, Factory, is responsible for deciding how to restore the actual state.

The entry point for using this feature from the outside is `FactoryController`. It provides methods such as `saveCheckpoint()`, `undo()`, `canUndo()`, and `historySize()`, so the View or external layers can use state saving and restoration without knowing the internal structure of the Memento.

This structure follows the Single Responsibility Principle. Factory is responsible for saving and restoring its own state, while SimulationHistory is responsible only for managing the stack of saved snapshots. Also, because DTO Snapshot and Memento are separated, screen display data and internal restoration data are not mixed together.

## 3.7 FE JS View Class Diagram and MVC Boundary

Following the Backend UML, let us also briefly look at the JavaScript View structure that handles the actual screen.

The MVC mapping of this project is as follows. The Model is the `gactorio::*` C++ backend classes, and the Controller is `ctrl::Controller`. In JavaScript, this Controller is exposed as an object called `Module.Controller` through Emscripten embind. The View consists of `Application`, `AppUI`, `UIComponent`, and each Panel class.

The important point is that the View does not directly know the Model. In the JavaScript files, Model classes such as `Factory`, `Machine`, and `ProductionLine` are not handled directly. The View only sends commands to `Module.Controller` and reads the plain data received through `snapshot()` to render the screen.

`Application` handles the main loop. Every frame, it calls the controller’s `tick()` method, and at regular intervals, it receives a `snapshot()` and passes it to `AppUI`. `AppUI` stores multiple `UIComponent` panels and renders all panels through `renderAll(snapshot)`.

Each screen area is divided into a separate panel class. `SimControlPanel` handles pause, reset, speed, checkpoint, and undo. `FactoryPanel` renders production lines and machine cards. `ProductsPanel` handles the product catalog and enqueue buttons. `InventoryPanel` handles raw item restocking and the finished product table. `EventLogPanel` shows the event log collected through the Observer pattern.

Here, the is-a relationship is that each Panel inherits from `UIComponent`. The has-a relationship is that `Application` has an `AppUI`, and `AppUI` has UIComponent panels. The dependency structure is that Panels depend on `Module.Controller` to send commands and depend on snapshot data for rendering.

This View structure is also easy to extend. If a new panel is needed, we can create a class that inherits from `UIComponent` and add it through `app.addPanel()` in `main.js`. There is no need to significantly modify the existing AppUI or other panels.

## 3.8 How the Code Is Easy to Extend and Modify

Now, let us summarize why this code is easy to extend and modify.

First, the boundaries between Controller, DTO, Model, and View are separated. The View does not directly modify internal Model classes. It only uses Controller commands and snapshots. Therefore, even if the UI changes, the impact on the production logic is small. Conversely, even if the internal Backend structure changes, the impact on the View is reduced as long as the DTO contract is maintained.

Second, product definitions and the production flow are separated. When adding a new product, we can add its definition and process route to the ProductCatalog, while keeping the core algorithms of ProductionLine and Machine unchanged.

Third, state, event, and restoration features are separated through design patterns. Machine states are handled by the State Pattern, logs and statistics are handled by the Observer Pattern, and checkpoint and undo are handled by the Memento Pattern. Therefore, when modifying one feature, the scope of impact on other features is reduced.

Fourth, has-a relationships and is-a relationships are clear. Objects that should be owned are connected through composition, while roles that should be extended are represented through inheritance and interfaces. This makes object lifecycles and extension points easier to understand.

## 3.9 SOLID Principles

Finally, let us look at the design from the perspective of the SOLID principles.

First is the Single Responsibility Principle. `FactoryController` is responsible for handling external commands and providing snapshots, while `Factory` is responsible for factory state and simulation flow. `Inventory` is responsible only for inventory management, `SimClock` only for time management, `EventLogObserver` only for storing logs, and `StatisticsObserver` only for updating statistics.

Next is the Open-Closed Principle. New products can be extended by adding ProductCatalog definitions, and new machines can be added by inheriting from Machine. New event handling features can be added by adding Observers, and new View panels can be added by inheriting from UIComponent.

Next is the Liskov Substitution Principle. `MixingStation`, `QualityStation`, `BottlingStation`, and `PackagingStation` can all be treated as `Machine`. Each MachineState can also be substituted through the `MachineState` interface. Observer implementations can also be treated uniformly as `Observer`.

Next is the Interface Segregation Principle. The View does not need to know the entire Model API. It only uses the View-oriented commands of `ctrl::Controller` and the `FactoryView` snapshot. Observers only need to implement the necessary interface, which is `onEvent()`. Each layer uses only the small interface it needs.

Finally, the Dependency Inversion Principle. Event handling depends on the `Observer` abstraction rather than concrete log or statistics classes. The View also does not directly depend on concrete C++ Model classes. Instead, it depends on the Controller and DTOs. Thanks to this, the upper-level flow is less sensitive to changes in concrete implementations.

In summary, Gactorio manages simulation state around the Factory and separates the Model from external layers using the Controller and DTOs. It also uses the State, Observer, and Memento Patterns to separate responsibilities for state management, event handling, and state restoration. Therefore, even when requirements increase, the system can be extended and modified without significantly disrupting the existing code.

This concludes our presentation. Thank you.