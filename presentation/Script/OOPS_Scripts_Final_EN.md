# Gactorio Presentation Script — Integrated Version Based on Demo Flow

Team members: Wonkyu Ko, Yongbin Cho
Presentation Topic: Gactorio C++ Backend / WebAssembly View / UML Structure

---

## 0. One-Sentence Summary — Opening

Hello. We will now present the Gactorio project.

Gactorio is a C++ OOP project that simulates an energy drink production factory.
The core backend was written in C++, built into WebAssembly, and connected to a View implemented with HTML, CSS, and JavaScript.

Today’s presentation will proceed in the following order.

First, we will demonstrate the actual application UI. Next, we will explain the implemented production scenarios. After that, we will use UML diagrams to explain the MVC structure, class relationships, design patterns, extensibility, and SOLID principles.

---

# Step 1 — Application Execution & UI Demonstration

First, let me show you the actual running Gactorio screen.

This screen is mainly divided into the simulation control area, factory production line area, product request area, inventory area, and event log area.

In the Simulation Control area at the top left, we can pause or resume the simulation. The Reset button initializes the entire state, and the speed slider controls the simulation speed. There are also Save Checkpoint and Undo buttons. These features use the Memento Pattern to save the current factory state and restore a previous state.

The Factory area in the center shows the actual production lines. Each line contains several machines displayed as cards. Each card shows the machine name, current status, progress, and HP. A machine can have states such as Idle, Working, Broken, and Maintenance, and its color and progress bar change depending on the current state.

In the Products area on the right, we can request production of products such as Voltz Classic, Hyper Bolt, and Aurora Zero. When a product button is clicked, the production command is delivered to the backend through the Controller, and the production task is added to the queue.

The Inventory area shows both raw materials and finished product stock. When production is completed, the finished product count increases, and the required raw materials are consumed during the production process.

The Event Log area displays events that occur during production. For example, task start, step completion, product completion, machine breakdown, and repair completion events are recorded in chronological order. This log is not written directly by the production logic. Instead, it is collected through the Observer Pattern.

Now, let’s produce one product.

When I click a product button, a production task is added to the queue, and the production line finds an idle machine that matches the current process step. The machine state changes from Idle to Working, and the progress bar begins to fill. When one process step is completed, the task moves on to the next step. When the final step is completed, the finished product is added to the Inventory.

The important point here is that the JavaScript View does not directly modify the C++ Model. The View only sends commands through a single boundary called Module.Controller and displays the data received from snapshots.

In other words, UI buttons do not directly access Factory or Machine objects. They work by sending commands to the Controller.

---

# Step 2 — Explanation of Implemented Scenarios

Next, I will explain the main scenarios we implemented.

## Scenario 1. Normal Flow

The first scenario is Normal Flow.

This is the basic production flow. When a product production request is made, a ProductionTask is created and added to the queue of a ProductionLine.

The production process is divided into multiple process steps. For example, there are steps such as Mixing, Quality, Bottling, and Packaging. Each step is assigned to a Machine that can perform the corresponding role.

The ProductionLine checks the tasks waiting in the queue and searches for an idle machine that can handle the current step. If a suitable machine exists, the task is assigned to that machine, and the machine changes to the Working state.

As the machine’s update function is repeatedly called, the progress increases. When the step is completed, the ProductionTask moves to the next step. When all steps are completed, production is finished, and the finished product count is reflected in the Inventory.

The key idea in this structure is polymorphism. The update loop of Factory or ProductionLine does not directly check the concrete machine type. It does not use if-statements to distinguish whether a machine is a MixingStation or a BottlingStation. Instead, it calls update through the abstract Machine interface.

In other words, the simulation loop operates polymorphically through Machine pointers.

## Scenario 2. Random Breakdowns

The second scenario is Random Breakdowns.

In this scenario, machines can take damage with a certain probability during production. When HP reaches 0, the machine enters the Broken state, and production stops.

In the Broken state, the task does not progress. When the user gives a Repair or Repair All command, the machine changes to the Maintenance state. After a certain amount of time, the repair is completed, HP is restored, and the machine returns to an idle or workable state.

This part uses the State Pattern. A Machine does not simply store a MachineStatus enum value. Instead, it owns state objects such as IdleState, WorkingState, BrokenState, and MaintenanceState.

Each state object is responsible for its own behavior, so the Machine class does not need a large number of state-based if-statements. For example, WorkingState handles progress increase and possible breakdowns, BrokenState stops production, and MaintenanceState handles the repair process.

Thanks to this structure, even if we add a new state later, we can extend the system by adding a new MachineState subclass without heavily modifying the existing Machine update structure.

## Additional Scenarios. Bottleneck / Overflow

In addition, we also included a structure for selecting scenarios for each production line.

The Bottleneck scenario can slow down a specific process step to create a bottleneck. In this case, tasks continue to enter from earlier steps, but processing becomes slower at a specific stage, causing the queue to build up.

The Overflow scenario limits the queue capacity. When production requests exceed the allowed number of tasks, the dropped task count increases.

So, instead of supporting only normal production, the system allows us to switch scenarios for each production line and observe situations such as bottlenecks, breakdowns, and overflow.

---

# Step 3 — Class Diagram Explanation

Now, I will explain the overall structure based on the UML diagrams.

The explanation will follow this order: MVC structure, C++ and JavaScript connection, is-a relationships, has-a relationships, dependency relationships, design patterns, extensibility, and SOLID principles.

---

## 3-1. Overall MVC Structure and C++ / JavaScript Connection

First, let’s look at the overall structure.

This project separates Model, Controller, and View according to the MVC principle.

The Model consists of the classes in the gactorio namespace of the C++ backend. Factory, ProductionLine, Machine, Product, Inventory, EventBus, and the Memento hierarchy belong to the Model. These classes handle the actual simulation state and rules.

The Controller is the boundary that receives external requests. Inside C++, there is FactoryController, and for WebAssembly integration, there is a web-facing facade called ctrl::Controller. In JavaScript, this object is exposed as Module.Controller.

The View consists of JavaScript classes under docs/js. Application, AppUI, UIComponent, and the various Panel classes are responsible for the screen.

C++ and JavaScript are connected through Emscripten embind. In src/web/bindings.cpp, the C++ Controller and View-related structures are exposed to JavaScript. As a result, JavaScript can create a C++ Controller object using new Module.Controller() and call methods such as tick, snapshot, enqueue, repair, and undo.

The command flow goes from the View down to the Controller.

When the user clicks a button, a JavaScript Panel calls a Module.Controller method. This call is delivered to ctrl::Controller, then internally to gactorio::FactoryController, and finally modifies the Model.

On the other hand, the query flow goes upward through snapshots.

Application periodically calls controller.snapshot(). The backend copies the current Factory state into View-only data structures such as FactoryView, LineView, and MachineView, and returns them. JavaScript converts this data into plain objects and re-renders the screen.

The important point here is that the View does not directly know the Model objects. JavaScript files do not directly handle C++ Model objects such as Factory, Machine, ProductionLine, or Inventory. They only send commands to Module.Controller and read snapshot data.

Therefore, the coupling between Model and View is low. Even if the internal backend structure changes, the View can remain mostly unchanged as long as the Controller and snapshot boundary are maintained.

---

## 3-2. JavaScript View Class Structure

Next, let’s look at the JavaScript View structure.

At the top level, there is the Application class. Application runs the main loop using the browser’s requestAnimationFrame. Every frame, it calls controller.tick(dt) to advance the simulation time. At regular intervals, it receives a snapshot and updates the screen.

Under Application, there is AppUI. AppUI acts as a compositor that manages multiple panels. It stores a list of UIComponents, and when renderAll(snapshot) is called, the render method of every registered panel is executed.

UIComponent is the common base class for all panels. Each panel inherits from UIComponent and renders the screen area it is responsible for.

The main panels are as follows.

SimControlPanel handles simulation controls such as pause, resume, reset, speed, checkpoint, and undo.

FactoryPanel displays production lines and machine cards. It also handles the scenario dropdown for each line.

InspectorPanel displays detailed information about the selected machine and performs operations such as repair or force break.

ProductsPanel shows the product list and provides production request buttons.

InventoryPanel displays raw material and finished product inventory and sends restock commands.

EventLogPanel shows the event logs collected through the Observer structure.

RuntimeConfigPanel handles loading runtime JSON configuration.

The advantage of this structure is that each screen area can be extended independently. If a new UI section is needed, we can create a new panel that inherits from UIComponent and register it in AppUI. There is no need to heavily modify the existing panels.

---

## 3-3. is-a Relationships — Inheritance Structure

Now, let’s look at the is-a relationships in the UML, which represent inheritance.

The first hierarchy is the Machine hierarchy.

Machine is an abstract base class, and concrete machines inherit from it. The default machines include MixingStation, QualityStation, BottlingStation, and PackagingStation. ConfiguredStation can also be used for config-based creation.

Machine provides common interfaces such as typeName, role, processType, and update, while concrete machines behave according to their own roles.

The second hierarchy is the MachineState hierarchy.

MachineState is the abstract base class for machine states. IdleState, WorkingState, BrokenState, and MaintenanceState inherit from it. Each state handles state-specific behavior through methods such as enter, update, exit, and name.

The third hierarchy is the Product hierarchy.

Product is the abstract base class for products, and concrete products such as VoltzClassic, HyperBolt, and AuroraZero inherit from it. A product has a name, ID, process route, and recipe information.

The fourth hierarchy is the Observer hierarchy.

Observer is the common interface for receiving events. EventLogObserver and StatisticsObserver inherit from it. EventLogObserver stores event logs, while StatisticsObserver calculates statistics such as completed products, breakdown counts, and repair counts.

The fifth hierarchy is the UIComponent hierarchy on the View side.

SimControlPanel, FactoryPanel, ProductsPanel, InventoryPanel, EventLogPanel, InspectorPanel, and RuntimeConfigPanel inherit from UIComponent.

In this way, both the backend and frontend use abstract base classes and concrete derived classes to maintain common interfaces.

---

## 3-4. has-a Relationships — Ownership and Containment Structure

Next, let’s look at the has-a relationships.

Factory is the aggregate root of the entire factory. Factory owns SimClock, Inventory, a list of ProductionLines, EventBus, EventLogObserver, and StatisticsObserver.

ProductionLine is the unit where actual production takes place. It has a ProductionTask queue and owns multiple Machines through unique_ptr.

Machine owns a MachineState through unique_ptr. This is the core of the State Pattern.

ProductionTask represents one product production task. It stores which Product is being produced, which step is currently in progress, and whether the input has been consumed.

Inventory manages the quantity of raw materials and finished products. It does not store actual Product objects. Instead, it manages quantities based on item IDs and product IDs.

The Memento structure also has has-a relationships. FactoryMemento contains a list of LineMementos, and LineMemento contains MachineMemento and ProductionTaskMemento information. SimulationHistory stores FactoryMementos in a stack.

The important point here is that ownership relationships are clearly expressed. Strong ownership is represented with unique_ptr or value members. Tasks shared between the queue and machines are represented with shared_ptr. Simple references or caches use raw pointers, but their non-owning meaning is made clear.

---

## 3-5. Dependency Relationships — Direction of Dependency

Next, let’s look at dependency relationships.

A dependency relationship means that a class does not own another class, but creates, calls, or uses it.

FactoryController manipulates Factory and creates DTO Snapshots. External layers give commands to the Model only through FactoryController.

ProductCatalog creates Products based on ProductDefinitions. New products can be extended through catalog definitions.

Factory creates and restores FactoryMementos for checkpoint and undo. However, SimulationHistory does not interpret the contents of a Memento. It only stores them.

Machine and ProductionLine depend on EventBus. When events such as task start, step completion, product completion, or machine breakdown occur, they are delivered to Observers through EventBus.

On the View side, panels depend on Module.Controller. Panels send button inputs as Controller commands and read snapshot data to update the DOM.

The overall dependency direction goes from View to Controller, and from Controller to Model. The Model does not know the View and does not depend on JavaScript or the DOM. This is the most important point in MVC separation.

---

## 3-6. Tick / Update Sequence

Next, I will explain how a single tick proceeds.

When controller.tick(deltaTime) is called from outside, the Controller calls update(deltaTime) on the internal Factory.

When Factory::update starts, SimClock is updated first. SimClock converts the real deltaTime into simulation time, and if the simulation is paused, it prevents time from advancing.

Next, Factory tries to assign tasks for each ProductionLine. If there is a task waiting in the queue and an idle machine that can process the current step, the task is assigned to that machine.

The machine that receives the task enters the Working state and proceeds with the process through update. WorkingState increases the progress and, when the process step is complete, moves the ProductionTask to the next step.

When all steps are finished, product production is completed. Factory adds the finished product to Inventory and publishes a product completion event through EventBus.

After that, Factory tries task assignment again. Since a machine that just completed a task may now be idle, the system can immediately assign the next task without waiting until the next tick.

In summary, the tick flow is: time update, task assignment, machine update, process completion handling, inventory update, event publishing, and reassignment.

---

## 3-7. State Pattern

Now, let’s explain the design patterns.

The first one is the State Pattern.

A Machine can have states such as Idle, Working, Broken, and Maintenance. If these states were handled only with enum values, the Machine class would need more and more state-based if-statements.

So, in this project, we created an abstract class called MachineState and separated state-specific behavior into subclasses.

IdleState represents a state in which the machine can receive a task. When a task is assigned, it changes to WorkingState.

WorkingState represents the state in which production is actually progressing. It increases progress and checks whether the process has been completed. It can also handle the possibility of breakdowns.

BrokenState represents a machine breakdown. In this state, production stops.

MaintenanceState represents a machine under repair. After a certain amount of time, HP is restored and the machine returns to a workable state.

Thanks to this design, Machine can delegate update behavior to the current state object. If a new state is needed, we can extend the system by adding a new MachineState subclass rather than modifying the entire Machine class.

---

## 3-8. Observer / Event Pattern

The second pattern is the Observer Pattern.

Many events occur during production. Events are generated when a task is added to the queue, when a task starts, when a step is completed, when a product is completed, when a machine breaks down, or when a machine is repaired.

If the production logic directly updated logs and statistics, Machine and ProductionLine would have too many responsibilities.

So, we used EventBus and Observer.

When an event occurs, Machine or ProductionLine calls EventBus::publish. EventBus then delivers the event to the registered Observers.

EventLogObserver receives events and stores them as logs. StatisticsObserver receives events and updates statistics such as completed products, started tasks, completed steps, breakdown counts, and repair counts.

The advantage of this structure is that production logic and additional features are separated. If we later need a new statistics feature or notification feature, we can simply add a new Observer. The existing Machine or ProductionLine logic does not need to be heavily changed.

---

## 3-9. Memento Pattern

The third pattern is the Memento Pattern.

This project supports Save Checkpoint and Undo. To implement this, we created a Memento structure that can save and restore the Factory state.

The Originator is Factory. Factory can save the current simulation state as a FactoryMemento and restore itself from a saved FactoryMemento.

FactoryMemento, LineMemento, MachineMemento, and ProductionTaskMemento play the role of Memento. They store information such as simulation time, inventory, production line state, queue state, machine state, and tasks in progress.

The Caretaker is SimulationHistory. SimulationHistory stores FactoryMementos in a stack. However, it does not directly interpret the contents of a Memento. The actual restoration is handled by Factory, the Originator.

From outside, this feature is used through FactoryController methods such as saveCheckpoint, undo, canUndo, and historySize.

Here, DTO Snapshot and Memento are separated. DTO is data used to show the current state to the View, while Memento is data used to restore the internal state for undo. Therefore, we do not use the display snapshot directly for undo. Instead, we use a separate FactoryMemento hierarchy.

---

## 3-10. Config-Based Extension Structure

Next, let’s look at the config-based extension structure.

The current backend supports not only a hard-coded default factory, but also JSON config-based factory creation.

FactoryConfig is the configuration data read from JSON. It contains definitions for items, products, stations, recipes, production lines, initial inventory, startup tasks, and scenarios.

FactoryConfigLoader reads a JSON file or string and converts it into FactoryConfig.

DefinitionRegistry indexes each definition by ID and verifies whether stations and recipes are correctly connected to each other.

FactoryBuilder converts the validated config into actual runtime objects. It creates production lines, creates machines through StationFactory, and sets up initial inventory and startup tasks.

Thanks to this structure, when adding a new product or production line, we do not need to modify the entire C++ update loop. We can extend the factory configuration through data and the builder path.

---

## 3-11. Extensibility and Maintainability

Now, let’s summarize why this design is easy to extend.

To add a new machine type, we can create a class that inherits from Machine, implement the required virtual functions, and register it in the creation path. Since the Factory update loop only uses the Machine interface, there is little need to modify the loop.

To add a new product, we can add product information to ProductCatalog or the config definition. Since the product route and recipe are separated as data, we do not need to modify the production loop with product-specific if-statements.

To add a new machine state, we can create a new state class that inherits from MachineState and connect the transition point. We do not need to add a large switch statement inside Machine.

To add new statistics or logging features, we can implement a new Observer and register it to EventBus. Event publishers can continue to simply call publish on EventBus.

To add a new UI section, we can create a panel that inherits from UIComponent and register it in AppUI. There is no need to heavily modify existing panels.

In summary, this project is designed to extend features through abstract classes and registration structures rather than directly inserting new logic into the core loops.

---

## 3-12. Summary of SOLID Principles

Finally, let’s summarize the design from the perspective of SOLID principles.

First, the Single Responsibility Principle. Factory manages the overall factory state, ProductionLine handles task assignment, and Machine handles process execution. Inventory only manages inventory, EventBus only delivers events, and SimulationHistory only stores Mementos.

Second, the Open-Closed Principle. Machines, products, states, Observers, and UI panels can be extended by adding new classes without heavily modifying the existing core structure.

Third, the Liskov Substitution Principle. Factory and ProductionLine operate through the abstract Machine type, not concrete machines. EventBus also delivers events through the Observer interface, not concrete Observer classes.

Fourth, the Interface Segregation Principle. Observer is a small interface centered on onEvent, MachineState contains only the methods needed for state transitions, and UIComponent is separated around bind and render.

Fifth, the Dependency Inversion Principle. The View does not directly depend on the concrete Model. Instead, it depends on the Module.Controller boundary. Inside the backend, ProductionLine also depends on the abstract Machine class, not concrete machine classes.

As a result, this design aims for maintainability and extensibility by combining encapsulation, inheritance, polymorphism, abstraction, and the State, Observer, and Memento Patterns.

---

# Step 4 — Q&A Preparation

Finally, we prepared answers for expected questions.

## Q1. Why did you use Web UI instead of Dear ImGui?

We judged that the core purpose of the assignment was MVC separation and backend structure rather than the UI technology itself.
So we built the C++ Model and Controller into WebAssembly and implemented the View using HTML, CSS, and JavaScript.

The important point is that the View and Model are not directly connected. The JavaScript View communicates only through a single boundary called Module.Controller. Therefore, although the UI technology changed to DOM, the MVC principle was still maintained.

## Q2. Does the simulation loop contain concrete type branching?

The core update loop does not directly check concrete machine types.
Factory and ProductionLine assign tasks and call update through the abstract Machine interface.

In other words, the code does not directly use if-statements to distinguish whether a machine is a MixingStation or a BottlingStation. Instead, it uses common interfaces such as role and processType.

## Q3. What is the difference between DTO Snapshot and Memento?

DTO Snapshot is data used to show the current state to the View.
For example, FactorySnapshot, MachineSnapshot, and InventorySnapshot copy and deliver the values needed for screen display.

On the other hand, Memento is internal state-saving data used for undo. It stores information required for restoration, such as tasks in progress, machine states, inventory, and time.

So, snapshot is for display, and memento is for restoration.

## Q4. Why did you use the Observer Pattern?

If the production logic directly modified logs and statistics, Machine and ProductionLine would take on too many responsibilities.

So, events are published through EventBus, and EventLogObserver and StatisticsObserver separately handle log storage and statistics updates.

Thanks to this structure, even if we add new statistics or notification features later, we do not need to heavily change the production logic.

## Q5. How is memory managed between C++ and JavaScript?

Vectors or object handles returned by embind are not automatically cleaned up by the JavaScript garbage collector.

So, in util.js, C++ vectors are copied into JavaScript arrays, and then delete is called to release the handles.
This logic is gathered in util.js instead of being scattered across panels, allowing each panel to focus on rendering the screen.

## Q6. What is the most important OOP design point in this project?

The most important points are the separation between Model and View, and the polymorphic simulation loop.

The View does not directly access the Model and communicates only through the Controller.
Also, Factory and ProductionLine do not directly check concrete machine types. They call update through the Machine interface.

These two points best demonstrate the MVC structure and OOP polymorphism.

---

# Closing

To summarize:

The Gactorio backend manages production lines, machines, products, inventory, time, events, statistics, and undo functionality around the Factory class.

We separated the View and Model through the Controller and DTO Snapshot, and the JavaScript View communicates with the C++ backend only through Module.Controller.

Machine behavior is separated by state using the State Pattern, and event logs and statistics are separated from the production logic using EventBus and Observer. We also implemented checkpoint and undo using the Memento Pattern.

Finally, the config-based structure allows products, machines, production lines, and scenarios to be extended in a data-driven way.

This concludes our presentation on the Gactorio project. Thank you.
