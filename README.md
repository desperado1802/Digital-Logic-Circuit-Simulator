# Digital Logic Circuit Simulator

Welcome to the Digital Logic Circuit Simulator! This program allows you to define, simulate, and analyze digital logic circuits using basic logic gates such as AND, OR, and NOT.

## Features

- Define custom logic circuits with specified inputs and logic expressions.
- Run simulations of the defined circuits with specified input values.
- Generate and display truth tables for the defined circuits.

## Getting Started

### Prerequisites

- C++ compiler (e.g., `g++`)

### Installation

1. Clone the repository:
    ```sh
    git clone https://github.com/yourusername/digital-logic-circuit-simulator.git
    cd digital-logic-circuit-simulator
    ```

2. Compile the program:
    ```sh
    g++ -o logic_simulator main.cpp
    ```

### Usage

1. Run the compiled program:
    ```sh
    ./logic_simulator
    ```

2. Follow the prompts to enter commands.

### Commands

1. **DEFINE**: Define a new logic circuit.
    ```plaintext
    DEFINE <circuit_name>(<input1>, <input2>, ...): "<expression>"
    Example: DEFINE ic1(a, b): "a & b"
    ```

2. **RUN**: Run a simulation of a defined circuit with specified input values.
    ```plaintext
    RUN <circuit_name>(<value1>, <value2>, ...)
    Example: RUN ic1(1, 0)
    ```

3. **ALL**: Generate and display the truth table for a defined circuit.
    ```plaintext
    ALL <circuit_name>
    Example: ALL ic1
    ```

### Example

```sh
$ ./logic_simulator
Welcome to the Digital Logic Circuit Simulator!
Available commands:
1. DEFINE <circuit_name>(<input1>, <input2>, ...): "<expression>"
   Example: DEFINE ic1(a, b): "a & b"
2. RUN <circuit_name>(<value1>, <value2>, ...)
   Example: RUN ic1(1, 0)
3. ALL <circuit_name>
   Example: ALL ic1
Enter your commands:

DEFINE ic1(a, b): "a & b"
Defined circuit: ic1 with inputs: a, b and expression: a & b
Current circuits:
Name: ic1, Inputs: a b , Expression: a & b

RUN ic1(1, 0)
Running circuit: ic1
Input values: 1 0
Result: 0

ALL ic1
Truth table for ic1:
a b res
0 0 0
0 1 0
1 0 0
1 1 1
