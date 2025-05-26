# Striker (C)

Striker is a Blackjack simulation and analysis tool written in C. It supports multiple player strategies, deck configurations, and is built for high-performance simulations across multiple threads.

> This project is part of a multi-language software engineering demonstration. See the full collection at [rees.me](http://rees.me).

## Features

- Supports multiple Blackjack strategies:
  - Mimic dealer
  - Basic strategy
  - Linear regression
  - Polynomial regression
  - Neural network-based strategy
  - High-low counting
  - Wong counting
- Deck configurations:
  - Single-deck
  - Double-deck
  - Six-deck shoe
- Highly parallel simulations using multi-threading
- Configurable simulation parameters via command-line arguments
- Outputs JSON reports (ideal for downstream visualization or data analysis)

## Usage

### Build

```bash
make all
```

### Run

```bash
./bin/striker-c [options]
```

### Options

| Option                              | Description                                           |
|-------------------------------------|-------------------------------------------------------|
| `--help`                            | Show this help message                                |
| `--version`                         | Display the program version                           |
| `-h`, `--number-of-hands` `<n>`     | Number of hands to simulate                           |
| `-t`, `--number-of-threads` `<n>`   | Number of threads to use                              |
| `-M`, `--mimic`                     | Use the mimic dealer player strategy                  |
| `-B`, `--basic`                     | Use the basic player strategy                         |
| `-L`, `--linear`                    | Use the linear regression player strategy             |
| `-P`, `--polynomial`                | Use the polynomial regression player strategy         |
| `-N`, `--neural`                    | Use the neural network player strategy                |
| `-H`, `--high-low`                 | Use the high-low count player strategy                |
| `-W`, `--wong`                      | Use the Wong count player strategy                    |
| `-1`, `--single-deck`              | Use a single deck of cards and rules                  |
| `-2`, `--double-deck`              | Use a double deck of cards and rules                  |
| `-6`, `--six-shoe`                 | Use a six-deck shoe of cards and rules                |

### Example

```bash
./bin/striker-c -B -2 -h 100000 -t 8
```

This runs a simulation using the **basic strategy** on a **double-deck** game for **100,000 hands** using **8 threads**.

## Project Structure

```
.
├── src/
│   ├── arguments/       # Command-line argument parsing
│   ├── cards/           # Card, deck, and hand logic
│   ├── constants/       # Game and simulation constants
│   ├── player/          # Player strategies
│   ├── report/          # Report generation and JSON output
│   ├── simulator/       # Core simulation logic
│   └── main.c           # Entry point
├── Makefile
└── README.md
```

## Requirements

- GCC (or compatible C compiler)
- GNU Make
- Libraries:
  - `cjson`
  - `libcurl`
  - `libmongoc`
  - `libbson`
  - `pthread`

## License

This project is open source and available under the [MIT License](LICENSE).

## Author

**Wade Rees**  
📫 [wade@rees.me](mailto:wade@rees.me)  
🔗 [LinkedIn](https://www.linkedin.com/in/wade-rees-978a02)  
🌐 [http://rees.me](http://rees.me)

