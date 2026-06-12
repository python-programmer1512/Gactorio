# JP Gaejip Presentation Script – Short Version

Team members: Wonkyu Ko, Yongbin Cho
Last revised: 2026.06.13

---

# 1. Project and Backend Scope

Hello. We will now present the Backend UML structure of the Gactorio Project.

Gactorio is a C++ project that simulates a beverage production factory.
The backend manages production lines, machines, products, inventory, simulation time, event logs, statistics, and the undo feature.

---

# 2. Overall Class Diagram

First, let’s look at the overall class structure.

The backend is divided into four areas: Controller, DTO, Common, and Model.
External requests come into the FactoryController. The FactoryController then manipulates the Model and returns the result in the form of DTO Snapshots.

FactoryController is the entry point of the backend.
It is the class used when the user runs the simulation, adds products, or uses checkpoint and undo features.
Internally, it owns the Factory and also manages SimulationHistory for the undo feature.

The DTO area contains read-only state objects that are delivered to the outside.
FactorySnapshot represents the overall factory state and includes inventory, production lines, machines, events, and statistics.
DTOs do not directly expose Model objects. Instead, they copy the current state as values and pass it outward.

The Common area contains SimClock and shared types.
SimClock manages time, speed multiplier, and pause state.
Types.hpp contains common definitions such as ProductId, MachineId, ItemType, MachineStatus, and EventType.

The Model area is responsible for the actual simulation logic.
At the center is Factory, which manages Inventory, ProductionLine, EventBus, Observer, and SimClock.
ProductionLine owns the task queue and machines, and performs actual production.
Machine handles each process step.

The Memento area stores the internal factory state for checkpoint and undo.
FactoryMemento stores the state, and SimulationHistory keeps these mementos in a stack structure.

---

# 3. Core Simulation Class Diagram

Now, let’s look at the core structure of the actual simulation.

Factory acts as the aggregate root and manages time, inventory, production lines, and the event system in one place.

Factory manages SimClock, Inventory, multiple ProductionLines, and EventBus.
SimClock is responsible for simulation time and speed.
Inventory manages the quantities of raw materials and finished products.
ProductionLine is the unit where actual production takes place, and EventBus delivers events that occur during production.

ProductionLine has a queue of waiting ProductionTasks and a list of Machines that perform the actual processes.
It manages which product tasks are waiting and assigns them to machines that can process them.

ProductionTask represents the production progress of a single product.
It references a specific Product and manages which process step is currently being performed using currentStepIndex.
Since each product has a different required process sequence, the task checks the current required process by following the ProcessStep list owned by the Product.

Machine is an abstract class that performs the actual processing.
Concrete machines inherit from Machine and perform tasks according to the roles they can handle.
When a task is assigned, the Machine proceeds with production based on its current state.
When the process is completed, it either moves the task to the next step or reports that the product has been completed.

Here, the behavior of each Machine state is separated into MachineState objects.
Machine has states such as Idle, Working, Broken, and Maintenance.
The actual update behavior for each state is delegated to each State object.

---

# 4. Tick/Update Sequence Diagram

Now, let’s look at the order in which the simulation operates while it is running.

This diagram shows how a single update proceeds when FactoryController::tick(deltaTime) is called.

First, the tick() method of FactoryController is called from the outside.
FactoryController does not directly handle the production logic. Instead, it calls update() on the internal Factory.

When Factory::update() starts, it first updates SimClock.
SimClock reflects the given deltaTime according to simulation time.
If the simulation is paused or stopped, it prevents time from progressing.

Next, Factory calls assignAvailableTask() for each ProductionLine.
If there is a waiting ProductionTask in the task queue and an idle Machine that can handle the required process, the task is assigned to that machine.
The assigned Machine then changes to the Working state.

After that, Factory iterates through the Machine list and calls update() on each Machine.
Each Machine behaves according to its current state, and state-specific behavior is delegated to the MachineState object.
If the Machine is in the Working state, its production progress increases.
If it is in the Maintenance state, repair progresses.
If it is in the Broken state, production remains stopped.

When a process is completed, ProductionTask moves to the next step.
When all processes are finished, product production is completed, and Factory calls Inventory::addProduct() to increase the finished product inventory.
If necessary, product completion or machine state change events are also published through EventBus.

Finally, Factory calls assignAvailableTask() once again.
This is because a machine that just finished a task may now be in the Idle state.
This structure allows the next task to be assigned immediately instead of waiting until the next tick.

---

# 5. Product Domain Class Diagram

Now, let’s look at the product domain structure.

At the center of this diagram are ProductCatalog, ProductDefinition, Product, Inventory, and ProductionTask.

ProductCatalog acts as a catalog that stores product definitions.
Each product’s ID, name, type, required materials, and process route are stored as a ProductDefinition.
In other words, it can be seen as the blueprint for creating a product.

ProductDefinition contains two key pieces of information.
The first is ItemRequirement.
This represents which materials and how many of them are required to create one product.
The second is ProcessStep.
This represents the order of processes required to produce the product.
Each step contains the required machine role and the base processing time.

ProductCatalog creates actual Product objects based on ProductDefinition.
Production lines and machines do not need to know the concrete implementation of each product.
They only use the ID, name, required materials, and process route provided by Product.

ProductionTask is a product task that is actually being produced.
It references one Product and manages which process step is currently in progress through currentStepIndex.
Using this information, it can determine which role of Machine the current task should be assigned to.

Inventory does not store actual Product objects.
Instead, it stores raw materials by ItemType and finished products by ProductId as quantities.
When a production request comes in, Factory checks the Product’s required materials and consumes those materials from Inventory.
When the product is completed, the finished product count is increased based on ProductId.

---

# 6. Machine State Diagram

Now, let’s look at the state management structure of Machine.

Machine uses the State Pattern to separate behavior for each state into separate classes.

Machine has four states: Idle, Working, Broken, and Maintenance.

Machine owns a MachineState object that represents its current state.
When update is called, the actual behavior is delegated to this object.

On the other hand, the MachineStatus enum is used as a state value for external display and storage.
MachineStatus is needed when showing the machine state in a DTO Snapshot or when saving and restoring the state through Memento.

Because state-specific behavior is separated into different classes,
the Machine class does not need to handle all state logic directly.
This also helps maintain a clear structure when adding new states or behaviors.

---

# 7. Observer / Event Pattern Diagram

Now, let’s look at the Observer / Event structure.

Events that occur during production are delivered by EventBus,
and multiple Observers receive those events and perform their own roles.

The main sources of events are ProductionLine and Machine.
ProductionLine can generate an event when a task enters the queue.
Machine can generate events such as task started, process completed, product completed, machine broken, or repair completed.

At this point, ProductionLine or Machine does not directly modify logs or statistics.
Instead, they create an Event object and publish it through EventBus::publish().
The Event contains information such as the occurrence time, event type, related machine ID, and message.

EventBus keeps a list of registered Observers.
When an event is published, it calls onEvent() on each Observer.

Representative Observers are EventLogObserver and StatisticsObserver.
EventLogObserver stores received events in an internal list and acts as the event log.
StatisticsObserver updates statistics such as the number of completed products or machine failures based on the event type.

Factory manages EventBus and the Observers.
It registers EventLogObserver and StatisticsObserver to EventBus,
so that events generated during production are delivered to both Observers.

Because production logic is separated from logging and statistics,
notification or monitoring features can be extended by simply adding new Observers.

---

# 8. Memento Pattern Diagram

Now, let’s look at the Memento Pattern structure.

The roles are separated so that the simulation state can be saved as a checkpoint and restored to a previous state when needed.

The important roles in the Memento Pattern are Originator, Memento, and Caretaker.

In this project, the Originator is Factory.
Factory can save the current simulation state as a FactoryMemento,
and it can restore its own state by receiving a FactoryMemento.

The Memento role is handled by FactoryMemento, LineMemento, and MachineMemento.
FactoryMemento stores the overall factory state.
LineMemento stores information about the production line’s task queue.
MachineMemento stores information such as the state and health of each machine.

The Caretaker role is handled by SimulationHistory.
SimulationHistory stores FactoryMementos in a stack structure and supports checkpoint saving and undo.
It does not directly modify the internal structure of Factory.

The entry point for using this feature from the outside is FactoryController.
FactoryController provides checkpoint and undo features through methods such as saveCheckpoint(), undo(), canUndo(), and historySize().

The saving flow works as follows.
When FactoryController::saveCheckpoint() is called,
Factory gathers the information needed for restoration, such as current time, inventory, production line queues, and machine states, and creates a FactoryMemento.
SimulationHistory then stores it in a stack.

The undo flow works in the opposite direction.
When FactoryController::undo() is called, SimulationHistory takes out the most recent checkpoint and passes it to Factory::restoreFromMemento().
Factory restores its internal state based on the saved time, inventory, production line state, and machine state.

DTO Snapshot and Memento are separated.
DTO Snapshot is a value object used to show the current state on the screen or through an API.
Memento is a storage object used to restore the internal state.
Through this separation, external layers can return the simulation to a previous point without directly manipulating the inside of the Model.

---

# 9. Design Strengths and Conclusion

This project manages the simulation state around Factory,
and separates the external layer from the Model through FactoryController and DTOs.

In addition, Machine state management is implemented with the State Pattern,
event logs and statistics are implemented with the Observer Pattern,
and checkpoint and undo features are implemented with the Memento Pattern.

As a result, the responsibilities of production logic, state management, event handling, and restoration are separated,
making the structure easier to maintain and extend.

This concludes our presentation. Thank you.
