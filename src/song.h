#ifndef SONG_H
#define SONG_H

#include <string>
#include <sstream>

// Represents a single song in the music library and history stack
struct Song {
    std::string id;
    std::string title;
    std::string artist;
    std::string album;
    std::string duration;
    std::string category;
    std::string artworkUrl;

    // Default constructor
    Song() : id(""), title(""), artist(""), album(""), duration("0:00"), category(""), artworkUrl("") {}

    // Parameterized constructor
    Song(std::string id, std::string title, std::string artist, 
         std::string album, std::string duration, std::string category, std::string artworkUrl)
        : id(id), title(title), artist(artist), album(album), 
          duration(duration), category(category), artworkUrl(artworkUrl) {}

    // Check if song is empty / uninitialized
    bool isEmpty() const {
        return id.empty();
    }

    // Helper to format song record as JSON string
    std::string toJson() const {
        std::ostringstream oss;
        oss << "{"
            << "\"id\":\"" << escapeJson(id) << "\","
            << "\"title\":\"" << escapeJson(title) << "\","
            << "\"artist\":\"" << escapeJson(artist) << "\","
            << "\"album\":\"" << escapeJson(album) << "\","
            << "\"duration\":\"" << escapeJson(duration) << "\","
            << "\"category\":\"" << escapeJson(category) << "\","
            << "\"artworkUrl\":\"" << escapeJson(artworkUrl) << "\""
            << "}";
        return oss.str();
    }

    // Simple JSON string escaper
    static std::string escapeJson(const std::string& input) {
        std::string output = "";
        for (char c : input) {
            if (c == '"') output += "\\\"";
            else if (c == '\\') output += "\\\\";
            else if (c == '\b') output += "\\b";
            else if (c == '\f') output += "\\f";
            else if (c == '\n') output += "\\n";
            else if (c == '\r') output += "\\r";
            else if (c == '\t') output += "\\t";
            else output += c;
        }
        return output;
    }
};

#endif // SONG_H
