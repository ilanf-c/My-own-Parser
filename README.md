```markdown
# LR(0), LR(1), and LALR(1) Parser Construction

This project provides an implementation for constructing **LR(0)**, **LR(1)**, and **LALR(1)** parsers, which are widely used in compiler design for syntax analysis. The implementation supports constructing DFA (Deterministic Finite Automata) representations for each type of parser and computes the necessary **First** and **Follow** sets for the grammar.

## Overview

The project defines the following components:

1. **Grammar Representation**:
   - Grammar is represented as a **multimap of rules**, where each non-terminal (key) maps to a set of right-hand side productions (values). This structure allows the parser to handle context-free grammars with ease.

2. **First and Follow Sets**:
   - **First sets**: A set of terminals that can begin the derivation of a non-terminal.
   - **Follow sets**: A set of terminals that can follow a non-terminal in any derivation.

3. **LR(0) Parser**:
   - A simple parser that uses a DFA to analyze input based on a set of production rules. It is limited in the grammars it can handle but is efficient for certain classes of grammars.

4. **LR(1) Parser**:
   - An extended version of the LR(0) parser that includes **lookahead** symbols to handle a broader range of grammars.

5. **LALR(1) Parser**:
   - A more compact form of the LR(1) parser, obtained by merging equivalent states in the LR(1) DFA. LALR(1) parsers are often used in practical compilers for efficiency.

## Features

- **First and Follow Set Calculation**: Automatically computes the First and Follow sets for non-terminal symbols in the grammar.
- **DFA Construction**: Generates **LR(0)**, **LR(1)**, and **LALR(1)** DFA tables for efficient parsing.
- **State Transitions**: Computes the transition functions for each DFA.
- **Grammar Parsing**: Parses a simple example grammar and constructs the corresponding DFA.

## File Structure

- `main.cpp`: The main program where the parser construction logic is implemented.
- `settings.json`: VSCode settings for the project configuration.
- `tasks.json`: VSCode tasks for building and running the project.
- `README.md`: This readme file.

## Dependencies

- C++11 or later.

## How to Use

### 1. Clone the repository

```bash
git clone https://github.com/yourusername/lr-parser.git
cd lr-parser
```

### 2. Open the project in VSCode

Open the project folder in Visual Studio Code.

### 3. Build the project using VSCode tasks

In VSCode, press `Ctrl + Shift + B` to build the project using the predefined tasks. This will compile the `main.cpp` file using the `g++` compiler.

Alternatively, you can run the following command in the terminal:

```bash
g++ -std=c++11 main.cpp -o parser
```

### 4. Run the project using VSCode tasks

You can run the compiled program directly from VSCode by setting up a custom run task. Press `Ctrl + Shift + P` and select "Tasks: Run Task", then select the task for running the program (`Run Parser`).

Alternatively, you can run the program manually using the terminal:

```bash
./parser
```

This will generate the LR(1) and LALR(1) parser DFA for the example grammar `A -> ( A ) | a` and print the results.

### Example Grammar

The example grammar used in this implementation is:

```
A -> ( A )
A -> a
```

This grammar represents simple recursive structures such as balanced parentheses.

### Output

The program generates the DFA transitions for LR(1) and LALR(1) parsing, displaying the parsing states and transition relations.

## How it Works

- **LR(0)**: The parser constructs the initial DFA based on the grammar, where each state corresponds to a set of items (productions) with a position marker.
- **LR(1)**: Adds a lookahead symbol to the LR(0) items, expanding the parsing table to handle more complex grammars.
- **LALR(1)**: Merges states in the LR(1) DFA that have identical cores, resulting in a more compact DFA with similar parsing power.

## VSCode Configuration

### settings.json

Make sure your `settings.json` file is properly configured to handle the C++ project and set the compiler paths if necessary. Here's an example of a basic configuration for your project:

```json
{
    "C_Cpp.default.compilerPath": "/usr/bin/g++",
    "C_Cpp.default.intelliSenseMode": "gcc-x64",
    "C_Cpp.default.includePath": [
        "/usr/include/c++/9"
    ],
    "files.associations": {
        "*.cpp": "cpp",
        "*.h": "cpp"
    }
}
```

### tasks.json

Your `tasks.json` file can be configured to automate the build and run steps in VSCode. Below is an example configuration:

```json
{
    "version": "2.0.0",
    "tasks": [
        {
            "label": "build parser",
            "type": "shell",
            "command": "g++",
            "args": [
                "-std=c++11",
                "main.cpp",
                "-o",
                "parser"
            ],
            "group": {
                "kind": "build",
                "isDefault": true
            }
        },
        {
            "label": "run parser",
            "type": "shell",
            "command": "./parser",
            "group": {
                "kind": "test",
                "isDefault": true
            },
            "dependsOn": "build parser"
        }
    ]
}
```

With this setup, you can build and run the project directly from VSCode using the `Ctrl + Shift + B` shortcut.

## Contributions

Contributions are welcome! If you find any bugs or want to suggest improvements, feel free to open an issue or submit a pull request.

## Acknowledgements

- This project is based on the principles of compiler theory, specifically the construction of deterministic parsers using finite automata.
- The algorithms implemented here are a simplified version of standard techniques used in compiler design.
```
