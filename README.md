# StackSound: Music Recently-Played Manager Using Stack
> **Subject:** Fundamental of Data Structures (B.Tech SEM 3)  
> **Primary Data Structure:** Stack (LIFO: Last-In, First-Out)  
> **Programming Language:** C++ (Dev-C++ / MinGW64 Compatible)  
> **Repository:** [https://github.com/pritiudata0810-commits/Music-Recently-Played-Manager.git](https://github.com/pritiudata0810-commits/Music-Recently-Played-Manager.git)

---

## 🎵 Project Overview
**StackSound** is an interactive, Apple Music-inspired desktop music history simulator designed as a college mini-project for **Fundamental of Data Structures**. It demonstrates the real-world application of the **Stack** data structure in managing audio playback history and bidirectional navigation (Previous / Next).

The project features a **pure manual array-based C++ Stack** (no `std::stack`), dual-stack history management, an independent Dev-C++ 5.11 console application, an integrated Windows Sockets HTTP backend with live formatted terminal logs, and a sleek Apple-inspired desktop user interface with real-time stack animations.

---

## 📂 Project Directory Structure

```text
Music-Recently-Played-Manager/
├── src/
│   ├── song.h                    # Song data model & JSON serializer
│   ├── stack.h                   # Array-based SongStack declaration (LIFO)
│   ├── stack.cpp                 # Manual push, pop, peek, isFull, isEmpty implementation
│   ├── song_manager.h            # Dual-stack manager (Back & Forward) header
│   ├── song_manager.cpp          # Predefined Bollywood library & terminal logger
│   ├── main_standalone.cpp       # Standalone interactive Dev-C++ console menu
│   ├── devcpp_single_file.cpp    # One-click Dev-C++ F9 compile helper
│   └── backend_server.cpp        # Winsock HTTP server (Port 8000 / REST API)
├── frontend/
│   ├── index.html                # Apple-inspired desktop UI & Stack visualizer
│   ├── style.css                 # Dark charcoal & warm amber aesthetics
│   └── app.js                    # Authoritative state synchronization & UI logic
├── assets/
│   ├── song_1.svg to song_12.svg # 12 custom vector album covers for Bollywood songs
│   └── default.svg               # Fallback player artwork
├── tests/
│   ├── test_stack.cpp            # 60 automated C++ unit and navigation tests
│   └── test_stack.exe            # Compiled test runner
├── docs/
│   └── CCE4_PRESENTATION_GUIDE.md # Complete CCE-4 academic presentation & viva guide
├── build_devcpp.bat              # Batch build script for Dev-C++ / MinGW64
├── start_backend.bat             # One-click launcher for backend & desktop UI
├── run_tests.bat                 # One-click test runner
└── README.md                     # Project documentation
```

---

## 🚀 How to Run

### Method 1: Running in Dev-C++ (Standalone Console Application)
1. Open **Dev-C++** on your Windows computer.
2. Go to **File -> Open** and navigate to:
   `E:\Priti\B.Tech SEM 3\FDS\Music-Recently-Played-Manager\src\devcpp_single_file.cpp`
3. Press **F9** (or click **Execute -> Compile & Run**).
4. Dev-C++ compiles and launches the interactive console menu:
   - Browse the 12 Bollywood songs
   - Push and play songs
   - Pop previous songs
   - Peek at the top of the history stack
   - Display stack contents in LIFO order
   - Run the automated academic test sequence

---

### Method 2: Running the Integrated Desktop Application
To run the full application with the C++ backend and Apple-inspired graphical interface:

1. Double-click `start_backend.bat` in the root project folder.
2. A dedicated terminal window titled **"StackSound C++ Backend Terminal (Real LIFO Operations)"** will open.
3. Your default web browser will automatically open:
   `http://127.0.0.1:8000`
4. As you click songs or controls in the browser:
   - The UI communicates directly with the running C++ executable.
   - The C++ backend performs real Stack operations and updates state.
   - The dedicated terminal displays the formatted operations in real time.
   - The Stack Visualizer panel in the browser animates and updates `TOP ->`.

To stop the backend, press **Ctrl+C** or close the terminal window.

---

### Method 3: Running Automated Unit Tests
Double-click `run_tests.bat` (or run in terminal):
```cmd
run_tests.bat
```
This executes **60 automated test cases** verifying:
- Empty stack detection & underflow prevention
- Push, Pop, and Peek semantics
- LIFO order verification
- Stack overflow boundary at capacity (25 elements)
- Dual-stack navigation sequence
- Forward stack clearing on new branch
- Clear history with current song preservation

---

## 🎓 Academic Demonstration Sequence for Viva Voce

Follow this step-by-step sequence in front of your examiner:

1. **Launch the Application:** Run `start_backend.bat`. Point out the C++ terminal window and the browser UI side-by-side.
2. **Initial State:** Show that both the Back History Stack and Forward History Stack are empty (`Size: 0 / 25`).
3. **First Song (Kesariya):** Click **Kesariya**.
   - Show terminal log: `INITIAL SELECTION` (No previous song was pushed).
4. **Second Song (Heeriye):** Click **Heeriye**.
   - Show terminal log: `PUSH` operation!
   - Kesariya was pushed onto the Back History Stack.
   - Show Stack Visualizer: `[TOP] -> Kesariya`.
5. **Third Song (Chaleya):** Click **Chaleya**.
   - Show terminal log: `PUSH` operation!
   - Heeriye was pushed to TOP.
   - Show Stack Visualizer: `[TOP] -> Heeriye`, followed by `Kesariya` below it (demonstrating **LIFO**).
6. **Previous Song (Backward Navigation):** Click the **Previous Song** button in the bottom player bar.
   - Show terminal log: `POP` from Back History!
   - Heeriye is popped and becomes the current song.
   - Chaleya is pushed onto the Forward History Stack.
   - Show Stack Visualizer: Kesariya is now at the TOP.
7. **Branching (Clearing Forward Stack):** Play **Apna Bana Le**.
   - Show terminal log: Forward History is cleared (`0`) because a new navigation path was initiated.
8. **Clear History:** Click **Clear History**.
   - Show that history stacks reset to `0`, while the currently selected song (`Apna Bana Le`) remains safely active.
9. **Boundary Check (Underflow):** Click **Previous Song** on the empty history.
   - Show the error message: `STACK UNDERFLOW: Back History is empty.`

---

## 📊 Complexity Analysis

| Operation | Time Complexity | Space Complexity |
| :--- | :---: | :---: |
| `push()` | $\mathcal{O}(1)$ | $\mathcal{O}(1)$ |
| `pop()` | $\mathcal{O}(1)$ | $\mathcal{O}(1)$ |
| `peek()` | $\mathcal{O}(1)$ | $\mathcal{O}(1)$ |
| `isEmpty()` | $\mathcal{O}(1)$ | $\mathcal{O}(1)$ |
| `isFull()` | $\mathcal{O}(1)$ | $\mathcal{O}(1)$ |
| `clear()` | $\mathcal{O}(1)$ | $\mathcal{O}(1)$ |
| `size()` | $\mathcal{O}(1)$ | $\mathcal{O}(1)$ |
| `display()` | $\mathcal{O}(N)$ | $\mathcal{O}(1)$ |

---

## 👥 Academic Information
- **Student:** Priti Udata
- **Degree:** B.Tech Computer Engineering (SEM 3)
- **Subject:** Fundamental of Data Structures (FDS)
- **Presentation Reference:** `docs/CCE4_PRESENTATION_GUIDE.md`
