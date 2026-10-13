#ifndef SONG_MANAGER_H
#define SONG_MANAGER_H

#include "song.h"
#include "stack.h"
#include <vector>
#include <string>

// Operation outcome report for terminal logging and frontend response
struct OperationResult {
    bool success;
    std::string action;
    std::string operation;      // "PUSH", "POP", "PEEK", "CLEAR", "NOOP"
    std::string message;
    std::string affectedSongTitle;
    std::string currentSongTitle;
    int backHistorySize;
    int forwardHistorySize;

    OperationResult() 
        : success(true), action(""), operation("NONE"), message(""), 
          affectedSongTitle("None"), currentSongTitle("None"),
          backHistorySize(0), forwardHistorySize(0) {}

    std::string toJson() const {
        std::ostringstream oss;
        oss << "{"
            << "\"success\":" << (success ? "true" : "false") << ","
            << "\"action\":\"" << Song::escapeJson(action) << "\","
            << "\"operation\":\"" << Song::escapeJson(operation) << "\","
            << "\"message\":\"" << Song::escapeJson(message) << "\","
            << "\"affectedSongTitle\":\"" << Song::escapeJson(affectedSongTitle) << "\","
            << "\"currentSongTitle\":\"" << Song::escapeJson(currentSongTitle) << "\","
            << "\"backHistorySize\":" << backHistorySize << ","
            << "\"forwardHistorySize\":" << forwardHistorySize
            << "}";
        return oss.str();
    }
};

/**
 * Manages the predefined Bollywood song library, the authoritative
 * Back History Stack (LIFO), Forward History Stack (LIFO), and current song state.
 */
class SongManager {
private:
    std::vector<Song> library;
    SongStack backHistory;     // Stack 1: Recently Played / Back navigation
    SongStack forwardHistory;  // Stack 2: Forward navigation after back
    Song currentSong;
    bool hasCurrentSong;
    OperationResult lastResult;

    void initializeLibrary();
    const Song* findSongById(const std::string& id) const;

public:
    SongManager();

    // Core actions
    bool playSong(const std::string& songId, OperationResult& result);
    bool previousSong(OperationResult& result);
    bool nextSong(OperationResult& result);
    bool peekHistory(Song& outSong, OperationResult& result);
    bool clearHistory(OperationResult& result);

    // Getters and inspectors
    bool hasSong() const { return hasCurrentSong; }
    const Song& getCurrentSong() const { return currentSong; }
    const SongStack& getBackHistory() const { return backHistory; }
    const SongStack& getForwardHistory() const { return forwardHistory; }
    const std::vector<Song>& getLibrary() const { return library; }
    const OperationResult& getLastResult() const { return lastResult; }

    // JSON serialization
    std::string getStateJson() const;
    std::string getLibraryJson() const;

    // Terminal log formatter
    void printTerminalLog(const OperationResult& res) const;
};

#endif // SONG_MANAGER_H
