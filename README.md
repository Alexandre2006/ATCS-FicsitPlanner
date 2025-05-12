# Ficsit Planner

A Satisfactory mod that makes it easy to plan and calculate optimized factories!

## Features
- Uses game context to determine what recipes, items, and resources are available
- Optimization based off of power consumption or complexity
  - Overrides for custom recipes (if wanted)
- Convenient CLI via in-game chat
- WORK IN PROGRESS GUI

## Usage

### CLI Commands
| Command | Parameters | Description |
| --- | --- | --- |
| `create` | `"<Item Name>" <Quantity>` | Creates a factory that produces `<Item Name>` at a rate of `<Quantity>` per minute. |
| `delete` | `<FactoryID>` | Deletes the factory with ID `<FactoryID>` |
| `get` | `<FactoryID>` | Get the details of the factory with ID `<FactoryID>` |
| `list` | None | Get a list of all the factories created and their IDs |
| `update` | `<FactoryID> <NodeID> <RecipeIndex>` | Update the factory with ID `<FactoryID>` to use the `<RecipeIndex>` at node `<NodeID>` |
| `optimize` | `<FactoryID> "power"/"complexity"` | Optimize the factory with ID `<FactoryID>` to minimize complexity or power consumption.

### GUI
Use the keybind `K` to open the window while in game. This can be changed from the game settings.

## Demo
https://github.com/user-attachments/assets/dbd3e22f-227c-409c-b23e-114dbd6586b0

## Algorithm Info

| Algorithm | Time Complexity | Space Complexity | Explanation |
| --- | --- | --- | --- |
| Power / Complexity Calculation | $O(n)$ | $O(1)$ | Sums all of the power / complexity values in the tree recursively. |
| Factory Creation / Optimization | $O(B^n)$ | $O(B^n)$ | Goes through all possible combinations recursively. No optimizations, as optimizations lose data that is potentially useful, especially when allowing the user to customize the factory after it is designed. $B$ represents the average number of branches per item, and $n$ represents the average depth for any item crafted. |
| Item Search (GUI) | $O(n*m)$ | $O(n)$ | Basic search algorithm. Checks which items begin with the entered search term. $n$ is the number of items, $m$ is the number of characters looked up so far.
