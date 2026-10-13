#include "stack.h"

// Constructor: Initializes stack to empty state
SongStack::SongStack() : topIndex(-1) {}

// PUSH: Inserts a song at the top of the stack
// Returns true on success, false on Stack Overflow
bool SongStack::push(const Song& song) {
    if (isFull()) {
        return false; // Overflow error
    }
    topIndex++;
    elements[topIndex] = song;
    return true;
}

// POP: Removes and returns the top song from the stack
// Returns true on success, false on Stack Underflow
bool SongStack::pop(Song& outSong) {
    if (isEmpty()) {
        return false; // Underflow error
    }
    outSong = elements[topIndex];
    topIndex--;
    return true;
}

// PEEK: Examines the top song without removing it
// Returns true on success, false if stack is empty
bool SongStack::peek(Song& outSong) const {
    if (isEmpty()) {
        return false;
    }
    outSong = elements[topIndex];
    return true;
}

// Check if the stack contains no elements
bool SongStack::isEmpty() const {
    return topIndex == -1;
}

// Check if the stack has reached its maximum capacity
bool SongStack::isFull() const {
    return topIndex == MAX_STACK_CAPACITY - 1;
}

// Returns the current number of elements stored in the stack
int SongStack::size() const {
    return topIndex + 1;
}

// Returns the fixed capacity of the stack
int SongStack::capacity() const {
    return MAX_STACK_CAPACITY;
}

// Clears all elements by resetting the top index to -1
void SongStack::clear() {
    topIndex = -1;
}

// Displays stack contents from TOP to BOTTOM (LIFO order)
void SongStack::display(std::ostream& os) const {
    if (isEmpty()) {
        os << "   [Stack is currently empty]\n";
        return;
    }
    os << "   ----------------------------------------\n";
    for (int i = topIndex; i >= 0; --i) {
        if (i == topIndex) {
            os << "   [TOP]    -> " << elements[i].title << " (" << elements[i].artist << ")\n";
        } else if (i == 0) {
            os << "   [BOTTOM] -> " << elements[i].title << " (" << elements[i].artist << ")\n";
        } else {
            os << "   [ " << i << " ]     -> " << elements[i].title << " (" << elements[i].artist << ")\n";
        }
    }
    os << "   ----------------------------------------\n";
    os << "   Total elements: " << size() << " / " << MAX_STACK_CAPACITY << "\n";
}

// Populates outList with copies of songs from TOP down to BOTTOM
void SongStack::getElementsLIFO(std::vector<Song>& outList) const {
    outList.clear();
    for (int i = topIndex; i >= 0; --i) {
        outList.push_back(elements[i]);
    }
}
