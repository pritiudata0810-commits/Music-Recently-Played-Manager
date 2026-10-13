#include "song_manager.h"
#include <iostream>
#include <iomanip>

SongManager::SongManager() : hasCurrentSong(false) {
    initializeLibrary();
    lastResult.action = "INITIALIZE";
    lastResult.operation = "INIT";
    lastResult.message = "System initialized with 12 Bollywood songs. Stacks empty.";
    lastResult.success = true;
}

void SongManager::initializeLibrary() {
    library.clear();
    library.push_back(Song("song_1", "Kesariya", "Arijit Singh", 
                           "Brahmāstra", "4:28", "Romantic", "assets/song_1.svg"));
    library.push_back(Song("song_2", "Heeriye", "Jasleen Royal, Arijit Singh", 
                           "Heeriye Single", "3:15", "Feel-Good", "assets/song_2.svg"));
    library.push_back(Song("song_3", "Apna Bana Le", "Arijit Singh, Sachin-Jigar", 
                           "Bhediya", "4:21", "Soulful", "assets/song_3.svg"));
    library.push_back(Song("song_4", "Sajni", "Arijit Singh, Ram Sampath", 
                           "Laapataa Ladies", "2:50", "Vintage Melody", "assets/song_4.svg"));
    library.push_back(Song("song_5", "O Maahi", "Arijit Singh, Pritam", 
                           "Dunki", "3:53", "Travel Romance", "assets/song_5.svg"));
    library.push_back(Song("song_6", "Tere Vaaste", "Varun Jain, Sachin-Jigar, Shadab Faridi", 
                           "Zara Hatke Zara Bachke", "3:09", "Upbeat Romance", "assets/song_6.svg"));
    library.push_back(Song("song_7", "Chaleya", "Arijit Singh, Shilpa Rao", 
                           "Jawan", "3:20", "Dance Pop", "assets/song_7.svg"));
    library.push_back(Song("song_8", "Tum Se", "Raghav Chaitanya, Sachin-Jigar", 
                           "TBMAUJ", "4:24", "Modern Romance", "assets/song_8.svg"));
    library.push_back(Song("song_9", "Zaalima", "Arijit Singh, Harshdeep Kaur", 
                           "Raees", "4:59", "Classic Romance", "assets/song_9.svg"));
    library.push_back(Song("song_10", "Ilahi", "Arijit Singh, Pritam", 
                           "Yeh Jawaani Hai Deewani", "3:49", "Wanderlust", "assets/song_10.svg"));
    library.push_back(Song("song_11", "Aaj Ki Raat", "Madhubanti Bagchi, Divya Kumar", 
                           "Stree 2", "3:48", "High Energy", "assets/song_11.svg"));
    library.push_back(Song("song_12", "Tauba Tauba", "Karan Aujla", 
                           "Bad Newz", "3:27", "Punjabi Groove", "assets/song_12.svg"));
}

const Song* SongManager::findSongById(const std::string& id) const {
    for (size_t i = 0; i < library.size(); ++i) {
        if (library[i].id == id) {
            return &library[i];
        }
    }
    return NULL;
}

// Play a selected song:
// 1. If song is already currently selected: no-op, refresh status.
// 2. If existing current song: PUSH currentSong onto BackHistoryStack, clear ForwardHistoryStack.
// 3. Set newly selected song as currentSong.
bool SongManager::playSong(const std::string& songId, OperationResult& result) {
    const Song* targetSong = findSongById(songId);
    result.action = "PLAY NEW SONG";

    if (targetSong == NULL) {
        result.success = false;
        result.operation = "ERROR";
        result.message = "Song ID not found in library: " + songId;
        result.affectedSongTitle = "None";
        result.currentSongTitle = hasCurrentSong ? currentSong.title : "None";
        result.backHistorySize = backHistory.size();
        result.forwardHistorySize = forwardHistory.size();
        lastResult = result;
        return false;
    }

    // Documented policy: playing the identical song that is already active
    if (hasCurrentSong && currentSong.id == targetSong->id) {
        result.success = true;
        result.operation = "NO-OP (SAME SONG)";
        result.message = "'" + targetSong->title + "' is already currently selected. No history change.";
        result.affectedSongTitle = targetSong->title;
        result.currentSongTitle = currentSong.title;
        result.backHistorySize = backHistory.size();
        result.forwardHistorySize = forwardHistory.size();
        lastResult = result;
        return true;
    }

    if (hasCurrentSong) {
        // Push previously active song onto Back History
        if (backHistory.isFull()) {
            result.success = false;
            result.operation = "STACK OVERFLOW";
            result.message = "Back history stack reached maximum capacity (" + 
                             std::to_string(backHistory.capacity()) + "). Please clear history.";
            result.affectedSongTitle = currentSong.title;
            result.currentSongTitle = currentSong.title;
            result.backHistorySize = backHistory.size();
            result.forwardHistorySize = forwardHistory.size();
            lastResult = result;
            return false;
        }

        backHistory.push(currentSong);
        std::string pushedTitle = currentSong.title;
        currentSong = *targetSong;
        hasCurrentSong = true;

        // Starting a new path clears forward history (standard browser/media navigation rule)
        forwardHistory.clear();

        result.success = true;
        result.operation = "PUSH";
        result.message = "Pushed previous song '" + pushedTitle + "' onto Back History Stack. Started new navigation track.";
        result.affectedSongTitle = pushedTitle;
        result.currentSongTitle = currentSong.title;
        result.backHistorySize = backHistory.size();
        result.forwardHistorySize = forwardHistory.size();
    } else {
        // Initial song selection: no previous song exists
        currentSong = *targetSong;
        hasCurrentSong = true;
        forwardHistory.clear();

        result.success = true;
        result.operation = "INITIAL SELECTION";
        result.message = "Selected initial song '" + currentSong.title + "'. No previous song pushed.";
        result.affectedSongTitle = currentSong.title;
        result.currentSongTitle = currentSong.title;
        result.backHistorySize = backHistory.size();
        result.forwardHistorySize = forwardHistory.size();
    }

    lastResult = result;
    return true;
}

// Navigate to Previous Song:
// 1. Check if BackHistoryStack is empty. If empty, report underflow error.
// 2. Push currentSong onto ForwardHistoryStack.
// 3. Pop top song from BackHistoryStack and make it currentSong.
bool SongManager::previousSong(OperationResult& result) {
    result.action = "PREVIOUS SONG";

    if (backHistory.isEmpty()) {
        result.success = false;
        result.operation = "STACK UNDERFLOW";
        result.message = "Back History Stack is empty. Cannot navigate to previous song.";
        result.affectedSongTitle = "None";
        result.currentSongTitle = hasCurrentSong ? currentSong.title : "None";
        result.backHistorySize = backHistory.size();
        result.forwardHistorySize = forwardHistory.size();
        lastResult = result;
        return false;
    }

    if (hasCurrentSong) {
        forwardHistory.push(currentSong);
    }

    Song poppedSong;
    backHistory.pop(poppedSong);
    currentSong = poppedSong;
    hasCurrentSong = true;

    result.success = true;
    result.operation = "POP (BACK HISTORY) + PUSH (FORWARD)";
    result.message = "Popped '" + poppedSong.title + "' from Back History Stack into Current Song. Pushed previous song to Forward History.";
    result.affectedSongTitle = poppedSong.title;
    result.currentSongTitle = currentSong.title;
    result.backHistorySize = backHistory.size();
    result.forwardHistorySize = forwardHistory.size();

    lastResult = result;
    return true;
}

// Navigate to Next Song:
// 1. Check if ForwardHistoryStack is empty. If empty, report error.
// 2. Push currentSong onto BackHistoryStack.
// 3. Pop top song from ForwardHistoryStack and make it currentSong.
bool SongManager::nextSong(OperationResult& result) {
    result.action = "NEXT SONG";

    if (forwardHistory.isEmpty()) {
        result.success = false;
        result.operation = "STACK UNDERFLOW";
        result.message = "Forward History Stack is empty. No forward songs available in navigation path.";
        result.affectedSongTitle = "None";
        result.currentSongTitle = hasCurrentSong ? currentSong.title : "None";
        result.backHistorySize = backHistory.size();
        result.forwardHistorySize = forwardHistory.size();
        lastResult = result;
        return false;
    }

    if (hasCurrentSong) {
        backHistory.push(currentSong);
    }

    Song poppedSong;
    forwardHistory.pop(poppedSong);
    currentSong = poppedSong;
    hasCurrentSong = true;

    result.success = true;
    result.operation = "POP (FORWARD HISTORY) + PUSH (BACK)";
    result.message = "Popped '" + poppedSong.title + "' from Forward History into Current Song.";
    result.affectedSongTitle = poppedSong.title;
    result.currentSongTitle = currentSong.title;
    result.backHistorySize = backHistory.size();
    result.forwardHistorySize = forwardHistory.size();

    lastResult = result;
    return true;
}

// PEEK: Examines top of BackHistoryStack without removing it
bool SongManager::peekHistory(Song& outSong, OperationResult& result) {
    result.action = "PEEK HISTORY";

    if (backHistory.isEmpty()) {
        result.success = false;
        result.operation = "PEEK (EMPTY)";
        result.message = "Back History Stack is empty. Nothing to peek.";
        result.affectedSongTitle = "None";
        result.currentSongTitle = hasCurrentSong ? currentSong.title : "None";
        result.backHistorySize = backHistory.size();
        result.forwardHistorySize = forwardHistory.size();
        lastResult = result;
        return false;
    }

    backHistory.peek(outSong);
    result.success = true;
    result.operation = "PEEK";
    result.message = "Top song of Back History is '" + outSong.title + "' by " + outSong.artist + ". Stack remains unchanged.";
    result.affectedSongTitle = outSong.title;
    result.currentSongTitle = hasCurrentSong ? currentSong.title : "None";
    result.backHistorySize = backHistory.size();
    result.forwardHistorySize = forwardHistory.size();

    lastResult = result;
    return true;
}

// CLEAR HISTORY: Clears back and forward stacks, keeps current song selected
bool SongManager::clearHistory(OperationResult& result) {
    result.action = "CLEAR HISTORY";
    int oldBack = backHistory.size();
    int oldForward = forwardHistory.size();

    backHistory.clear();
    forwardHistory.clear();

    result.success = true;
    result.operation = "CLEAR";
    result.message = "Cleared " + std::to_string(oldBack) + " back history entries and " + 
                     std::to_string(oldForward) + " forward entries. Current song '" + 
                     (hasCurrentSong ? currentSong.title : "None") + "' preserved.";
    result.affectedSongTitle = "All History Elements";
    result.currentSongTitle = hasCurrentSong ? currentSong.title : "None";
    result.backHistorySize = 0;
    result.forwardHistorySize = 0;

    lastResult = result;
    return true;
}

// Serializes the full system state to JSON
std::string SongManager::getStateJson() const {
    std::ostringstream oss;
    oss << "{"
        << "\"hasCurrentSong\":" << (hasCurrentSong ? "true" : "false") << ","
        << "\"currentSong\":" << (hasCurrentSong ? currentSong.toJson() : "null") << ","
        << "\"backHistorySize\":" << backHistory.size() << ","
        << "\"forwardHistorySize\":" << forwardHistory.size() << ","
        << "\"capacity\":" << backHistory.capacity() << ","
        << "\"lastResult\":" << lastResult.toJson() << ",";

    // Back History array in LIFO order
    std::vector<Song> backList;
    backHistory.getElementsLIFO(backList);
    oss << "\"backHistory\":[";
    for (size_t i = 0; i < backList.size(); ++i) {
        if (i > 0) oss << ",";
        oss << backList[i].toJson();
    }
    oss << "],";

    // Forward History array
    std::vector<Song> forwardList;
    forwardHistory.getElementsLIFO(forwardList);
    oss << "\"forwardHistory\":[";
    for (size_t i = 0; i < forwardList.size(); ++i) {
        if (i > 0) oss << ",";
        oss << forwardList[i].toJson();
    }
    oss << "]";

    oss << "}";
    return oss.str();
}

// Serializes the predefined song library to JSON
std::string SongManager::getLibraryJson() const {
    std::ostringstream oss;
    oss << "[";
    for (size_t i = 0; i < library.size(); ++i) {
        if (i > 0) oss << ",";
        oss << library[i].toJson();
    }
    oss << "]";
    return oss.str();
}

// Prints formatted terminal log strictly adhering to Section 9 requirement
void SongManager::printTerminalLog(const OperationResult& res) const {
    std::cout << "\n======================================================================\n";
    std::cout << "                 STACKSOUND - C++ STACK BACKEND                       \n";
    std::cout << "======================================================================\n";
    std::cout << " Action:               " << res.action << "\n";
    std::cout << " Operation:            " << res.operation << "\n";
    std::cout << " Status:               " << (res.success ? "SUCCESS" : "FAILED / UNDERFLOW") << "\n";
    std::cout << " Details:              " << res.message << "\n";
    std::cout << " Affected Item:        " << res.affectedSongTitle << "\n";
    std::cout << " Current Song:         " << (hasCurrentSong ? (currentSong.title + " (" + currentSong.artist + ")") : "[None]") << "\n";
    std::cout << " Back History Size:    " << backHistory.size() << " / " << backHistory.capacity() << " (LIFO Stack)\n";
    std::cout << " Forward History Size: " << forwardHistory.size() << " / " << forwardHistory.capacity() << "\n";
    std::cout << " Back History Top:     ";
    Song topSong;
    if (backHistory.peek(topSong)) {
        std::cout << topSong.title << " (" << topSong.artist << ")\n";
    } else {
        std::cout << "[EMPTY STACK]\n";
    }
    std::cout << "======================================================================\n" << std::flush;
}
