#include "song_manager.h"
#include <iostream>
#include <iomanip>
#include <string>

// Header banner for the console application
void printHeader() {
    std::cout << "\n======================================================================\n";
    std::cout << "     STACKSOUND: MUSIC RECENTLY-PLAYED MANAGER (C++ STACK)            \n";
    std::cout << "           Fundamental of Data Structures Mini-Project                \n";
    std::cout << "                 Demonstrating LIFO Principle                         \n";
    std::cout << "======================================================================\n";
}

// Display available menu options
void printMenu() {
    std::cout << "\n---------------------------- MAIN MENU ------------------------------\n";
    std::cout << " [1]  Browse Bollywood Song Library (12 Songs)\n";
    std::cout << " [2]  Play Song (PUSH previous song onto Back History Stack)\n";
    std::cout << " [3]  Previous Song (POP top song from Back History Stack)\n";
    std::cout << " [4]  Next Song (POP top song from Forward History Stack)\n";
    std::cout << " [5]  Peek Top of Back History Stack (View without removing)\n";
    std::cout << " [6]  Display Back History Stack (TOP to BOTTOM - LIFO Order)\n";
    std::cout << " [7]  Display Forward History Stack\n";
    std::cout << " [8]  Display Current Status & Stack Metrics\n";
    std::cout << " [9]  Clear Navigation History\n";
    std::cout << " [10] Run Prescribed Academic Demonstration Sequence\n";
    std::cout << " [0]  Exit Application\n";
    std::cout << "----------------------------------------------------------------------\n";
    std::cout << "Enter your choice [0-10]: ";
}

// Option 1: Display all 12 songs
void displayLibrary(const SongManager& manager) {
    const std::vector<Song>& lib = manager.getLibrary();
    std::cout << "\n----------------------- PREDEFINED BOLLYWOOD LIBRARY -----------------\n";
    std::cout << std::left << std::setw(6) << "ID"
              << std::setw(18) << "Title"
              << std::setw(30) << "Artist"
              << std::setw(24) << "Album"
              << std::setw(8) << "Time"
              << "Category\n";
    std::cout << "----------------------------------------------------------------------\n";
    for (size_t i = 0; i < lib.size(); ++i) {
        std::cout << std::left << std::setw(6) << lib[i].id
                  << std::setw(18) << lib[i].title
                  << std::setw(30) << lib[i].artist
                  << std::setw(24) << lib[i].album
                  << std::setw(8) << lib[i].duration
                  << lib[i].category << "\n";
    }
    std::cout << "----------------------------------------------------------------------\n";
}

// Option 10: Run the exact academic navigation test sequence requested by prompt
void runPrescribedDemo(SongManager& manager) {
    std::cout << "\n======================================================================\n";
    std::cout << "      RUNNING PRESCRIBED ACADEMIC DEMONSTRATION SEQUENCE              \n";
    std::cout << "======================================================================\n";
    OperationResult res;

    std::cout << "\n--- Step 1: Start with fresh history, Select 'Kesariya' ---\n";
    manager.playSong("song_1", res);
    manager.printTerminalLog(res);

    std::cout << "\n--- Step 2: Select 'Heeriye' (Kesariya gets PUSHED onto Back Stack) ---\n";
    manager.playSong("song_2", res);
    manager.printTerminalLog(res);

    std::cout << "\n--- Step 3: Select 'Chaleya' (Heeriye gets PUSHED onto Back Stack) ---\n";
    manager.playSong("song_7", res);
    manager.printTerminalLog(res);

    std::cout << "\n--- Step 4: Display Back History Stack (Should show Chaleya active, Heeriye at TOP, Kesariya at BOTTOM) ---\n";
    manager.getBackHistory().display(std::cout);

    std::cout << "\n--- Step 5: Navigate 'Previous Song' (POP Heeriye from Back Stack, PUSH Chaleya to Forward Stack) ---\n";
    manager.previousSong(res);
    manager.printTerminalLog(res);

    std::cout << "\n--- Step 6: Navigate 'Previous Song' again (POP Kesariya from Back Stack, PUSH Heeriye to Forward Stack) ---\n";
    manager.previousSong(res);
    manager.printTerminalLog(res);

    std::cout << "\n--- Step 7: Navigate 'Next Song' (POP Heeriye from Forward Stack, PUSH Kesariya to Back Stack) ---\n";
    manager.nextSong(res);
    manager.printTerminalLog(res);

    std::cout << "\n--- Step 8: Select a different new song 'Apna Bana Le' (Verify Forward Stack is CLEARED) ---\n";
    manager.playSong("song_3", res);
    manager.printTerminalLog(res);
    std::cout << "Forward Stack size after new song: " << manager.getForwardHistory().size() << " (Must be 0)\n";

    std::cout << "\n--- Step 9: Clear History (Preserves active selection 'Apna Bana Le') ---\n";
    manager.clearHistory(res);
    manager.printTerminalLog(res);

    std::cout << "\n--- Step 10: Attempt Previous Song on Empty Stack (Underflow verification) ---\n";
    manager.previousSong(res);
    manager.printTerminalLog(res);

    std::cout << "\n======================================================================\n";
    std::cout << "      PRESCRIBED DEMONSTRATION COMPLETED SUCCESSFULLY                 \n";
    std::cout << "======================================================================\n";
}

int main() {
    SongManager manager;
    printHeader();

    int choice = -1;
    while (choice != 0) {
        printMenu();
        if (!(std::cin >> choice)) {
            std::cin.clear();
            std::string dummy;
            std::cin >> dummy;
            std::cout << "\n[!] Invalid input. Please enter a number between 0 and 10.\n";
            continue;
        }

        OperationResult res;
        switch (choice) {
            case 1:
                displayLibrary(manager);
                break;

            case 2: {
                displayLibrary(manager);
                std::cout << "Enter Song ID to play (e.g., song_1, song_2, ... song_12): ";
                std::string id;
                std::cin >> id;
                manager.playSong(id, res);
                manager.printTerminalLog(res);
                break;
            }

            case 3:
                manager.previousSong(res);
                manager.printTerminalLog(res);
                break;

            case 4:
                manager.nextSong(res);
                manager.printTerminalLog(res);
                break;

            case 5: {
                Song topSong;
                manager.peekHistory(topSong, res);
                manager.printTerminalLog(res);
                break;
            }

            case 6:
                std::cout << "\n--- CURRENT BACK HISTORY STACK (LIFO) ---\n";
                manager.getBackHistory().display(std::cout);
                break;

            case 7:
                std::cout << "\n--- CURRENT FORWARD HISTORY STACK ---\n";
                manager.getForwardHistory().display(std::cout);
                break;

            case 8:
                std::cout << "\n--- CURRENT SYSTEM STATUS ---\n";
                std::cout << " Current Active Song:  " 
                          << (manager.hasSong() ? manager.getCurrentSong().title + " (" + manager.getCurrentSong().artist + ")" : "[None]") << "\n";
                std::cout << " Back History Size:    " << manager.getBackHistory().size() << " / " << manager.getBackHistory().capacity() << "\n";
                std::cout << " Forward History Size: " << manager.getForwardHistory().size() << " / " << manager.getForwardHistory().capacity() << "\n";
                break;

            case 9:
                manager.clearHistory(res);
                manager.printTerminalLog(res);
                break;

            case 10:
                runPrescribedDemo(manager);
                break;

            case 0:
                std::cout << "\nExiting StackSound. Thank you!\n";
                break;

            default:
                std::cout << "\n[!] Invalid selection. Please choose an option from 0 to 10.\n";
                break;
        }
    }

    return 0;
}
