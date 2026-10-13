#ifndef STACK_H
#define STACK_H

#include "song.h"
#include <iostream>
#include <vector>

// Fixed capacity for array-based Stack implementation (academic standard)
const int MAX_STACK_CAPACITY = 25;

/**
 * Array-based manual Stack implementation for Song records.
 * Demonstrates the Last-In, First-Out (LIFO) principle.
 * Does NOT use std::stack.
 */
class SongStack {
private:
    Song elements[MAX_STACK_CAPACITY];
    int topIndex; // Points to current top element; -1 when empty

public:
    // Constructor initializes an empty stack
    SongStack();

    // Fundamental Stack operations
    bool push(const Song& song);
    bool pop(Song& outSong);
    bool peek(Song& outSong) const;

    // State inspection
    bool isEmpty() const;
    bool isFull() const;
    int size() const;
    int capacity() const;

    // Reset stack
    void clear();

    // Display stack to output stream (Top to Bottom)
    void display(std::ostream& os = std::cout) const;

    // Retrieve copy of elements in LIFO order (Index 0 is TOP)
    void getElementsLIFO(std::vector<Song>& outList) const;
};

#endif // STACK_H
