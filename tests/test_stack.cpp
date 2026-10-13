#include "../src/stack.h"
#include "../src/song_manager.h"
#include <iostream>
#include <cassert>
#include <string>

static int totalTests = 0;
static int passedTests = 0;

void recordTest(const std::string& testName, bool condition) {
    totalTests++;
    std::cout << "[TEST " << totalTests << "] " << testName << " ... ";
    if (condition) {
        passedTests++;
        std::cout << "PASSED\n";
    } else {
        std::cout << "FAILED! <--- Assertion Error\n";
    }
}

int main() {
    std::cout << "======================================================================\n";
    std::cout << "        STACKSOUND: C++ UNIT & NAVIGATION TEST SUITE                  \n";
    std::cout << "======================================================================\n\n";

    // -------------------------------------------------------------------------
    // GROUP 1: Raw SongStack Class Operations
    // -------------------------------------------------------------------------
    std::cout << "--- 1. Testing Raw SongStack Operations ---\n";
    SongStack stack;

    // 1.1 Empty stack detection
    recordTest("Empty stack detection on init (isEmpty == true)", stack.isEmpty() == true);
    recordTest("Empty stack size is 0", stack.size() == 0);
    recordTest("Empty stack is not full", stack.isFull() == false);

    // 1.2 Empty stack underflow handling
    Song dummy;
    recordTest("Pop on empty stack returns false (underflow protection)", stack.pop(dummy) == false);
    recordTest("Peek on empty stack returns false", stack.peek(dummy) == false);

    // 1.3 Push one song
    Song s1("s1", "Song One", "Artist 1", "Album 1", "3:00", "Romantic", "art1.svg");
    recordTest("Push single song returns true", stack.push(s1) == true);
    recordTest("Stack size is 1 after 1 push", stack.size() == 1);
    recordTest("Stack is no longer empty", stack.isEmpty() == false);

    // 1.4 Peek without removing
    Song peeked;
    recordTest("Peek returns true", stack.peek(peeked) == true);
    recordTest("Peek returns correct song title", peeked.title == "Song One");
    recordTest("Peek does NOT decrement stack size", stack.size() == 1);

    // 1.5 Push multiple songs & LIFO order verification
    Song s2("s2", "Song Two", "Artist 2", "Album 2", "3:30", "Pop", "art2.svg");
    Song s3("s3", "Song Three", "Artist 3", "Album 3", "4:00", "Dance", "art3.svg");
    stack.push(s2);
    stack.push(s3);
    recordTest("Stack size is 3 after pushing 2 more songs", stack.size() == 3);

    Song topPeek;
    stack.peek(topPeek);
    recordTest("Top song is the most recently pushed (Song Three)", topPeek.title == "Song Three");

    // 1.6 Pop and verify LIFO order
    Song pop1, pop2, pop3;
    recordTest("Pop #1 succeeds", stack.pop(pop1) == true);
    recordTest("Pop #1 returned Song Three (LIFO)", pop1.title == "Song Three");
    recordTest("Pop #2 returned Song Two (LIFO)", stack.pop(pop2) && pop2.title == "Song Two");
    recordTest("Pop #3 returned Song One (LIFO)", stack.pop(pop3) && pop3.title == "Song One");
    recordTest("Stack is empty after popping all elements", stack.isEmpty() == true && stack.size() == 0);

    // 1.7 Stack overflow testing (fill to MAX_STACK_CAPACITY)
    std::cout << "\n--- 2. Testing Stack Overflow Boundary ---\n";
    for (int i = 0; i < MAX_STACK_CAPACITY; ++i) {
        Song temp("id_" + std::to_string(i), "Title " + std::to_string(i), "Artist", "Album", "3:00", "Cat", "art.svg");
        stack.push(temp);
    }
    recordTest("Stack reaches full capacity (25 elements)", stack.size() == MAX_STACK_CAPACITY);
    recordTest("isFull() returns true", stack.isFull() == true);
    Song extra("overflow", "Overflow Song", "Artist", "Album", "3:00", "Cat", "art.svg");
    recordTest("Push onto full stack returns false (overflow protection)", stack.push(extra) == false);

    // 1.8 Clear stack
    stack.clear();
    recordTest("clear() resets stack to empty", stack.isEmpty() == true && stack.size() == 0);

    // -------------------------------------------------------------------------
    // GROUP 2: Prescribed SongManager Navigation Sequence
    // -------------------------------------------------------------------------
    std::cout << "\n--- 3. Testing SongManager Prescribed Navigation Sequence ---\n";
    SongManager manager;
    OperationResult res;

    // Step A: Start with no current song
    recordTest("Initial state has no current song", manager.hasSong() == false);
    recordTest("Initial Back History size is 0", manager.getBackHistory().size() == 0);
    recordTest("Initial Forward History size is 0", manager.getForwardHistory().size() == 0);

    // Step B: Select Kesariya (song_1)
    recordTest("Select Kesariya returns success", manager.playSong("song_1", res) == true);
    recordTest("Current song is Kesariya", manager.getCurrentSong().title == "Kesariya");
    recordTest("Back History size remains 0 (first song, no previous pushed)", manager.getBackHistory().size() == 0);

    // Step C: Select Heeriye (song_2)
    recordTest("Select Heeriye returns success", manager.playSong("song_2", res) == true);
    recordTest("Current song is Heeriye", manager.getCurrentSong().title == "Heeriye");
    recordTest("Back History size is now 1", manager.getBackHistory().size() == 1);
    Song backTop;
    manager.getBackHistory().peek(backTop);
    recordTest("Back History TOP is Kesariya", backTop.title == "Kesariya");

    // Step D: Select Chaleya (song_7)
    recordTest("Select Chaleya returns success", manager.playSong("song_7", res) == true);
    recordTest("Current song is Chaleya", manager.getCurrentSong().title == "Chaleya");
    recordTest("Back History size is now 2", manager.getBackHistory().size() == 2);
    manager.getBackHistory().peek(backTop);
    recordTest("Back History TOP is Heeriye (LIFO)", backTop.title == "Heeriye");

    // Step E: Navigate Previous Song (#1)
    recordTest("Navigate Previous Song succeeds", manager.previousSong(res) == true);
    recordTest("Current song reverted to Heeriye", manager.getCurrentSong().title == "Heeriye");
    recordTest("Back History size reduced to 1", manager.getBackHistory().size() == 1);
    recordTest("Forward History size increased to 1", manager.getForwardHistory().size() == 1);
    Song fwdTop;
    manager.getForwardHistory().peek(fwdTop);
    recordTest("Forward History TOP is Chaleya", fwdTop.title == "Chaleya");

    // Step F: Navigate Previous Song again (#2)
    recordTest("Navigate Previous Song again succeeds", manager.previousSong(res) == true);
    recordTest("Current song reverted to Kesariya", manager.getCurrentSong().title == "Kesariya");
    recordTest("Back History size is now 0 (empty)", manager.getBackHistory().size() == 0);
    recordTest("Forward History size is now 2", manager.getForwardHistory().size() == 2);

    // Step G: Navigate Next Song
    recordTest("Navigate Next Song succeeds", manager.nextSong(res) == true);
    recordTest("Current song moved forward to Heeriye", manager.getCurrentSong().title == "Heeriye");
    recordTest("Back History size is now 1 (Kesariya pushed back)", manager.getBackHistory().size() == 1);
    recordTest("Forward History size reduced to 1", manager.getForwardHistory().size() == 1);

    // Step H: Select a different new song (Apna Bana Le) -> Confirm Forward History is cleared
    recordTest("Select new song 'Apna Bana Le'", manager.playSong("song_3", res) == true);
    recordTest("Current song is now Apna Bana Le", manager.getCurrentSong().title == "Apna Bana Le");
    recordTest("Forward History is completely CLEARED on new branch", manager.getForwardHistory().size() == 0);
    recordTest("Back History size is 2 (Heeriye on top)", manager.getBackHistory().size() == 2);

    // Step I: Clear History
    recordTest("clearHistory() succeeds", manager.clearHistory(res) == true);
    recordTest("Back History size is 0 after clear", manager.getBackHistory().size() == 0);
    recordTest("Forward History size is 0 after clear", manager.getForwardHistory().size() == 0);
    recordTest("Current song 'Apna Bana Le' PRESERVED after clear", manager.getCurrentSong().title == "Apna Bana Le");

    // Step J: Attempt Previous Song on Empty History (Underflow check)
    recordTest("Previous on empty history fails safely", manager.previousSong(res) == false);
    recordTest("Current song remains 'Apna Bana Le' without corruption", manager.getCurrentSong().title == "Apna Bana Le");

    // Step K: Invalid Song ID check
    recordTest("Playing non-existent song ID fails gracefully", manager.playSong("invalid_id_999", res) == false);

    // -------------------------------------------------------------------------
    // SUMMARY
    // -------------------------------------------------------------------------
    std::cout << "\n======================================================================\n";
    std::cout << " TOTAL TESTS:  " << totalTests << "\n";
    std::cout << " PASSED TESTS: " << passedTests << "\n";
    std::cout << " FAILED TESTS: " << (totalTests - passedTests) << "\n";
    if (passedTests == totalTests) {
        std::cout << " STATUS:       ALL TESTS PASSED WITH 100% SUCCESS!\n";
    } else {
        std::cout << " STATUS:       SOME TESTS FAILED!\n";
    }
    std::cout << "======================================================================\n";

    return (passedTests == totalTests) ? 0 : 1;
}
