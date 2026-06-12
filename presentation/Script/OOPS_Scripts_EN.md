# JP Gactorio Presentation Script

Members: Wonkyu Go, Yongbin Cho
Last modified: 2026.06.13

# 1. Project and Backend Scope

Hello. We will now present the Backend UML structure of the Gactorio Project.

Gactorio is a C++ project that simulates a beverage production factory.
The Backend of this project manages production lines, machines, products, inventory, simulation time, event logs, statistics, and the undo feature.

The scope of UML analysis in this presentation is the Backend structure.
Specifically, it includes the Model layer, such as Factory, ProductionLine, Machine, Product, and Inventory; the FactoryController, which handles external requests; DTO Snapshots, which are delivered to the screen or API; and structures related to Event, Observer, and Memento.

First, we will look at the overall Backend structure through the complete class diagram.
Then, we will explain the core simulation structure and the tick/update flow.
After that, we will examine the product domain and the main design patterns in order.

In particular, there are three important design points in this project:
the State Pattern for managing Machine states,
the Observer Pattern for event logs and statistics,
and the Memento Pattern for checkpoint and undo features.

In other words, this presentation will explain, mainly through UML, what classes make up the Gactorio Backend and how these classes cooperate during the simulation process.

# 2. Overall Class Diagram

In this section, we will first look at the overall class structure of the Gactorio Backend.

This diagram divides the Backend into four main areas: Controller, DTO, Common, and Model.
The most important point in the overall flow is that external requests enter through the FactoryController, the FactoryController manipulates the Model, and then returns the result to the outside in the form of DTO Snapshots.

First, the FactoryController in the Controller area serves as the entry point of the Backend.
It is the class that users directly access when they progress the simulation, add products, or use checkpoint and undo features.
The FactoryController internally owns the Factory and also manages SimulationHistory for the undo feature.

The DTO area consists of read-only state objects delivered to the outside.
FactorySnapshot represents the overall factory state, and it contains inventory, production line, machine, event, and statistics information.
The important point is that DTOs do not directly expose Model objects. Instead, they copy the current state as values and deliver them.

The Common area contains simulation time and common types.
SimClock manages simulation time, speed multiplier, and pause state.
Types.hpp contains common definitions such as product IDs, machine IDs, item types, machine states, and event types.

The largest area, the Model area, is responsible for the actual simulation logic.
At the center is the Factory, which manages Inventory, ProductionLine, EventBus, Observer, and SimClock.
ProductionLine has task queues and machines and performs actual production, while Machine handles each process step.

On the right side, the Memento-related structures are placed.
The FactoryMemento hierarchy stores the internal state of the factory for checkpoint and undo, and SimulationHistory keeps these Mementos in a stack structure.

To summarize, this overall class diagram shows that the Gactorio Backend receives external requests through the FactoryController boundary, manages simulation state around the Factory, and separately uses DTO, Event, and Memento structures.

# 3. Core Simulation Class Diagram

In this section, we will look at the Core Simulation structure, which is the center of the actual simulation.

The key point of this diagram is to show how production lines, machines, tasks, products, and inventory are connected around the Factory.
In the Gactorio Backend, Factory serves as the aggregate root of the simulation.
In other words, it is the central object that manages the factory’s internal time, inventory, production lines, and event system in one place.

First, Factory manages SimClock, Inventory, multiple ProductionLines, and EventBus.
SimClock is responsible for simulation time, speed multiplier, and pause state, while Inventory manages the quantities of raw materials and finished products.
ProductionLine is the unit where actual production occurs, and EventBus delivers events that occur during production.

ProductionLine has two main components.
One is the ProductionTask queue, which stores pending tasks.
The other is the list of Machines that process the actual production steps.
In other words, a production line manages which product tasks are waiting and assigns those tasks to machines that can process them.

ProductionTask represents the production progress of a single product.
Each task refers to a specific Product and manages the current process step using currentStepIndex.
Since each product has a different required process sequence, ProductionTask checks the currently required process by following the list of ProcessSteps owned by the Product.

Machine is an abstract base class that processes the actual production steps.
Concrete machines inherit from Machine and perform tasks according to the roles they can handle.
When a task is assigned, the Machine progresses production according to its current state.
When the process is complete, it either moves the task to the next step or reports product completion.

An important point here is that state-specific behavior of Machine is separated into MachineState objects.
Machine has states such as Idle, Working, Broken, and Maintenance, and the actual update behavior for each state is delegated to each State object.
This part will be explained in more detail in the State Pattern section later.

The overall production flow can be summarized as follows.
Factory manages production lines and inventory, and ProductionLine assigns tasks to machines.
Machine processes each step according to its state and role, and completed products are reflected back into Inventory.

The point to emphasize in this diagram is ownership.
Factory owns ProductionLines, and ProductionLine owns Machines.
On the other hand, tasks can be shared between the queue and machines, so they are connected using shared pointers.

To summarize, the Core Simulation structure is the key diagram that shows how actual production logic in the Gactorio Backend is carried out through cooperation among objects.

# 4. Tick/Update Sequence Diagram

In this section, we will examine the order in which the Core Simulation structure introduced earlier operates during actual execution.

This diagram shows how one simulation update proceeds when FactoryController::tick(deltaTime) is called.
In other words, while the previous Class Diagram showed “which objects are connected,” this Sequence Diagram shows “in what order those objects are called.”

First, the outside calls tick() on FactoryController.
FactoryController does not directly handle production logic. Instead, it calls update() on its internal Factory.
In this way, the Controller serves as an entry point that delivers external commands to the Model.

When Factory::update() starts, it first updates the SimClock.
SimClock reflects the given deltaTime based on simulation time and manages how much the current time has progressed.
If the simulation is paused or stopped, this step prevents time from progressing.

Next, Factory calls assignAvailableTask() for each ProductionLine.
In this step, if there is a pending ProductionTask in the task queue and an idle Machine that can process the task’s current process step, the task is assigned.
The assigned Machine transitions to the Working state and performs actual production in the later update step.

After that, Factory iterates over the Machine list and calls update() on each Machine.
Each Machine behaves according to its current state, and the state-specific processing is delegated to a MachineState object.
For example, in the Working state, production progress increases; in the Maintenance state, repair progresses; and in the Broken state, production remains stopped.

As production progresses, ProductionTask checks the current ProcessStep.
When a process step is complete, it moves on to the next step.
When all process steps are complete, product production is finished, and the result is delivered to the Factory through the ProductionLine.

After machine updates are complete, Factory collects the completed products.
Then, it calls Inventory::addProduct() to increase the finished product inventory.
If necessary, product completion events or machine state change events are also published through EventBus during this process.

Finally, Factory calls assignAvailableTask() once again.
The reason for this is that a machine may have just finished a task and become Idle.
This structure allows the system to immediately assign the next available task without waiting until the next tick.

The overall flow can be summarized as follows.
FactoryController receives the tick command, Factory updates time, and ProductionLine assigns tasks.
Then, Machines progress production according to their states, and completed products are reflected in Inventory.
Finally, if any machine has newly become available, another task is assigned.

Therefore, one tick flow proceeds in the following order:

time update → task assignment → machine update → process progress → completed product reflected in inventory → reassignment

This Sequence Diagram is a key diagram that shows how the static class structure of the Gactorio Backend cooperates inside the actual simulation loop.

# 5. Product Domain Class Diagram

In this section, we will look at the product domain structure.

So far, we have seen how production lines and machines operate when tick is called.
In this section, we will explain how the products being produced are defined and how those definitions are connected to the actual production flow.

At the center of this diagram are ProductCatalog, ProductDefinition, Product, Inventory, and ProductionTask.

First, ProductCatalog serves as a catalog that stores product definitions.
Each product’s ID, name, type, required materials, and process route are stored in the form of ProductDefinition.
In other words, ProductDefinition can be understood as the blueprint for producing a product.

There are two important pieces of information in ProductDefinition.
The first is ItemRequirement.
ItemRequirement represents which materials and how many of them are required to produce one unit of a product.

The second is ProcessStep.
ProcessStep represents the process sequence through which a product is produced.
Each step contains the required machine role and the standard processing time.
Therefore, the process route defines which machines a product must pass through and in what order.

ProductCatalog creates actual Product objects based on these ProductDefinitions.
Production lines or machines do not need to directly know the concrete product implementation.
They only use the ID, name, required materials, and process route provided by Product.

ProductionTask represents an actual product task that is currently being produced.
Each ProductionTask refers to one Product and manages how far the production has progressed using currentStepIndex.
It also checks the current ProcessStep so that it can determine which role of Machine the task should be assigned to.

Inventory manages stock.
However, it does not store actual Product objects.
Instead, it stores only quantities: raw materials by ItemType and finished products by ProductId.
When a production request comes in, Factory checks the Product’s required materials and consumes them from Inventory.
When the product is completed, the finished product quantity is increased based on ProductId.

The overall flow can be summarized as follows.
ProductCatalog manages product definitions, and ProductDefinition contains required materials and process routes.
Factory consumes materials from Inventory based on this information, and ProductionTask follows the Product’s ProcessSteps to proceed with production.
Finally, the completed product is reflected in Inventory.

Therefore, the Product Domain structure is a diagram that shows what data defines a product and how that data is used in production tasks and inventory management.

# 6. Machine State Diagram

In this section, we will look at the Machine state management structure.

The key point of this diagram is that Machine does not operate using only a simple state value.
Instead, it uses the State Pattern to separate behavior for each state into separate classes.

Machine has four main states:
Idle, Working, Broken, and Maintenance.

First, Idle is the state in which the machine is waiting.
When a task that the machine can process is assigned, it transitions to the Working state.

Working is the state in which actual production is carried out.
In this state, the progress of the currently assigned ProductionTask increases.
When the process is complete, the task either moves to the next step or product completion is handled.

Broken is the state in which the machine has broken down.
In this state, production does not progress even if update is called.
To use the machine again, it must move into the repair process through repair().

Maintenance is the state in which the machine is being repaired.
In this state, repair time progresses, and when repair is completed, the machine’s health is restored.
After that, depending on the situation, the machine can return to the Idle state or continue its task.

The important design point here is that state-specific behavior is not handled with a large if-statement inside Machine.
Machine has a MachineState object representing the current state, and when update is called, the actual behavior is delegated to this State object.

For example, if the current state is WorkingState, production progress logic is executed.
If it is MaintenanceState, repair progress logic is executed.
If it is BrokenState, production does not proceed.
If it is IdleState, the machine remains ready to receive a new task.

In this way, MachineState is responsible for behavior specific to each state.
On the other hand, the MachineStatus enum is used as a state value for external display and saving.
For example, the MachineStatus value is needed when displaying machine state in DTO Snapshot or when saving and restoring state through Memento.

Therefore, in this structure, MachineState objects are responsible for actual behavior, while the MachineStatus enum is used for lookup, display, and saving.

The state transitions can be summarized as follows.
A machine basically starts in the Idle state.
When a task is assigned, it becomes Working.
When production is complete, it returns to Idle.
If a breakdown occurs during production, it becomes Broken.
When repair is called, it moves to Maintenance.
When repair is completed, it returns to a usable state.

To summarize, the Machine State structure separates machine behavior by state into separate classes.
This allows the Machine class to avoid directly handling all state logic, and it also keeps the structure clearer when adding new states or state-specific behavior.

# 7. Observer / Event Pattern Diagram

In this section, we will look at the Observer / Event structure of the Gactorio Backend.

The key point of this diagram is that EventBus delivers events generated during production, and multiple Observers receive those events and perform their own roles.

The main sources of events are ProductionLine and Machine.
ProductionLine can generate an event when a task enters the queue.
Machine can generate events such as task start, process completion, product completion, machine breakdown, and repair completion.

At this point, ProductionLine or Machine does not directly store events in a log or modify statistics.
Instead, they create Event objects and publish them through EventBus::publish().
An Event object contains information such as occurrence time, event type, related machine ID, and message.

EventBus serves as a dispatcher that delivers events.
EventBus maintains a list of registered Observers, and when an event is published, it calls onEvent() on each Observer.

The representative classes that implement the Observer interface are EventLogObserver and StatisticsObserver.

EventLogObserver stores received events in an internal list.
This object serves as an event log that records what happened during the simulation.

StatisticsObserver updates statistics based on event types rather than simply storing events.
For example, when a product completion event occurs, it increases the number of completed products.
When a machine breakdown event occurs, it increases the breakdown count.

In this structure, Factory manages EventBus and the Observers.
Factory registers EventLogObserver and StatisticsObserver with EventBus so that events generated during production are delivered to both Observers.

The important point is that production logic is separated from logging and statistics.
Machine and ProductionLine only need to report that “an event occurred.”
Whether that event is stored as a log or aggregated as statistics is handled by the Observers.

Also, DTOs and Observers are not directly connected.
When the state needs to be shown externally, FactoryController reads the results from the Observers and converts them into DTOs such as EventSnapshot or StatisticsSnapshot.

The overall flow can be summarized as follows.
An event occurs in ProductionLine or Machine.
EventBus delivers this event to the Observers.
EventLogObserver stores the event record, and StatisticsObserver updates statistics.
Finally, FactoryController provides this information externally in the form of DTO Snapshots.

To summarize, the Observer / Event structure separates the production logic that generates events from the log and statistics functions that process those events.
Thanks to this structure, even if a notification feature or debugging monitoring feature is added later, it can be extended by adding a new Observer without significantly changing the existing production logic.

# 8. Memento Pattern Diagram

In this section, we will look at the Memento Pattern structure of the Gactorio Backend.

The key point of this diagram is that the roles are separated so that simulation state can be saved as checkpoints and restored to a previous state when needed.

There are three important roles in the Memento Pattern.

The first is the Originator.
In this project, Factory plays the role of Originator.
Factory can save the current simulation state as a FactoryMemento, and it can also receive a FactoryMemento to restore its own state.

The second is the Memento.
This role is handled by FactoryMemento, LineMemento, and MachineMemento.
FactoryMemento stores the overall factory state, and it contains production line states and machine states.
LineMemento stores task queue information for a production line, while MachineMemento stores the state and health of each individual machine.

The third is the Caretaker.
This role is handled by SimulationHistory.
SimulationHistory stores FactoryMementos in a stack structure and supports checkpoint saving and undo.
In other words, SimulationHistory stores states, but it does not directly modify the internal structure of Factory.

The entry point for using this feature from the outside is FactoryController.
FactoryController provides checkpoint and undo features through methods such as saveCheckpoint(), undo(), canUndo(), and historySize().
Therefore, external layers can use state saving and restoration through FactoryController without knowing the internal structure of Memento.

The saving flow is as follows.
First, FactoryController::saveCheckpoint() is called.
Then, FactoryController asks Factory to create a FactoryMemento from the current state.
Factory gathers the information necessary for restoration, such as current time, inventory, production line queues, and machine states, and creates a FactoryMemento.
After that, SimulationHistory saves this Memento in a stack.

The undo flow proceeds in the opposite direction.
When FactoryController::undo() is called, the most recent checkpoint is popped from SimulationHistory.
Then, this FactoryMemento is passed to Factory::restoreFromMemento().
Factory restores its internal state based on the saved time, inventory, production line state, and machine state.

An important design point here is that DTO Snapshots and Mementos are separated.
DTO Snapshots are value objects used to show the current state on the screen or through an API.
On the other hand, Mementos are storage objects used to restore internal state.
Therefore, DTOs such as FactorySnapshot are not used for undo; only the FactoryMemento hierarchy is used for restoration.

Another important point is that SimulationHistory does not directly handle Factory’s internal objects.
SimulationHistory only stores and retrieves saved Mementos.
How the state is saved and restored is handled by Factory, the Originator.
Thanks to this, the responsibility for state management remains inside Factory, while history management logic and restoration logic are separated.

To summarize, this Memento structure is organized so that Factory acts as the Originator that saves and restores its own state, while SimulationHistory acts as the Caretaker that stores those states.
By providing checkpoint and undo features externally through FactoryController, the external layer can return the simulation state to a previous point without directly manipulating the Model internals.

# 9. Design Advantages and Conclusion

Finally, we will summarize the Gactorio Backend structure.

This project manages simulation state around the Factory and separates the external layer from the Model through FactoryController and DTOs.

In addition, Machine state management is implemented using the State Pattern,
event logs and statistics are implemented using the Observer Pattern,
and checkpoint and undo features are implemented using the Memento Pattern.

As a result, the responsibilities of production logic, state management, event handling, and restoration are separated, making the structure easier to maintain and extend.

This concludes our presentation.
Thank you.
