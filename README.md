# PACMAN_LandTiger

# Pac-Man Game for LandTiger Board

## Project Overview
This project involves implementing the classic Pac-Man game using the LandTiger Board and KEIL µVision emulator. The game mimics the original Pac-Man mechanics with a few modifications to fit the platform's capabilities.

## Getting Started
Follow these steps to set up and run the project:

1. **Installation**:
    - Download and install the KEIL µVision software.
    - Clone this repository to your local machine.

2. **Building the Project**:
    - Open the project in KEIL µVision.
    - Ensure that the compilation target is set to `SW_Debug`.
    - Compile the project.

3. **Running the Project**:
    - If you have a physical LandTiger Board, load the compiled project directly onto the board.
    - For emulation, run the project using KEIL µVision's emulator.

## Features
- **Labyrinth and Pills**:
    - The display of the LandTiger board shows the labyrinth for Pac-Man.
    - The labyrinth contains 240 Standard Pills and 6 Power Pills placed randomly.
    - Pac-Man can move through the joystick, and eats the pills to score points.
    - The central "teleport" locations allow Pac-Man to move from one side of the screen to the other.

- **Game Mechanics**:
    - Each Standard Pill increases the score by 10 points, and each Power Pill increases it by 50 points.
    - Every 1000 points, Pac-Man earns a new life.
    - The game starts in "PAUSE" mode, and can be paused/resumed using the `INT0` button.
    - A Countdown timer starts from 60 seconds, and if it expires before all pills are eaten, a "Game Over!" screen is shown.
    - If all pills are eaten before the timer expires, a "Victory!" screen is displayed.

- **Extra Features**:
    - An AI-controlled ghost (Blinky) pursues Pac-Man, causing him to lose a life in case of contact.
        - **Chase Mode**: The ghost actively tries to catch Pac-Man.
        - **Frightened Mode**: Activated when Pac-Man eats a "Power Pills". The ghost turns blue for 10 seconds, runs away from Pac-Man, and can be eaten by Pac-Man for extra 100 points.
    - Sound effects and background music are played using the speaker.
    - The current Score, Remaining Lives, and Countdown timer are printed through the CAN bus, configured in external loopback mode.


Project for ExtraPoint of "Architettura dei Sistemi di Elaborazione" at POLITO.
