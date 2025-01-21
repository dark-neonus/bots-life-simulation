# Bots Life Simulation

## Overview
**Bots Life Simulation** is a dynamic environment where you can create and customize your own bot behaviors, design unique simulations, and observe how bots interact with their surroundings and each other. The project is highly modular and allows extensive customization, making it ideal for both experimentation and entertainment.

## Features

### Create Your Own Bot Brain
- Develop custom behavior logic for bots by implementing your own decision-making algorithms.
- Assign unique roles or behaviors to bots and test how they perform in various scenarios.

### Simulation Settings
- Configure simulation parameters such as sizes or number of chunks.
- Adjust spawn rates, number and density for bots, food and other objects in the environment.
- Define properties for terrain and resources, influencing bot behaviors dynamically.

### Interactive Environment
- Observe bots as they:
  - Move through the environment.
  - Interact with food, other bots, and terrain.
  - Engage in combat or cooperative behaviors.
- Dynamic object lifecycle management, including creation, decay, and deletion.

### Debugging Tools
- Visualize object positions and states within the simulation.
- Examine and adjust parameters of objects in simulations or global statistic.

## Getting Started

### Prerequisites
- A modern C++ compiler supporting C\++23 or later. (but you can try C\++17 or C\++20)
- A development environment of your choice (e.g., Visual Studio, CLion, etc.).

### Installation

1. Clone the repository:
   ```bash
   git clone https://github.com/dark-neonus/bots-life-simulation.git
   cd bots-life-simulation
   ```

### Unix (Linux, Macos)

2. Build the project:
   ```bash
   mkdir build && cd build
   cmake .. && make
   ```
3. Run the simulation:
   ```bash
   ./bots-life-simulation
   ```

## Windows 

2. Build the project:
   ```bash
   mkdir build
   cd build
   ```

   ```
   cmake --build . --config Debug
   ```
   **or** 
   ```
   cmake --build . --config Release
   ```
3. Run the simulation:
   ```
   start ./Debug/simulation_try_1.exe
   ```

### Usage
- Configure simulation parameters in the configuration file.
- Write custom bot logic by extending the `src/brains/examples/Base.h` class.
- Use the GUI or log outputs to monitor and analyze simulation behavior.

### Future Plans
- *__COMPLETE THE PROJECT (in the hopes)__*
- Optimize simulation for testing a big number of bots with complex logic.
- Enhance visualization tools for better simulation monitoring.

## Contributing
Contributions are welcome! Please follow these steps:
1. Fork the repository.
2. Create a new branch for your feature/bug fix.
3. Submit a pull request with a detailed description of your changes.

## License
This project is licensed under the MIT License. See the `LICENSE` file for details.

---
Start exploring the fascinating world of bot interactions with **Bots Life Simulation**.
Unleash your creativity and push the boundaries of automatic bots behavior!
