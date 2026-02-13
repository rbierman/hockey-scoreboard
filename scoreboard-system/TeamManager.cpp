#include "TeamManager.h"
#include <filesystem>
#include <fstream>
#include <iostream>

namespace fs = std::filesystem;

TeamManager::TeamManager(const std::string& dataDir) : dataDir(dataDir) {
    ensureDataDirectoryExists();
    loadTeams();
}

void TeamManager::ensureDataDirectoryExists() {
    if (!fs::exists(dataDir)) {
        try {
            fs::create_directories(dataDir);
        } catch (const fs::filesystem_error& e) {
            std::cerr << "Error creating data directory: " << e.what() << std::endl;
        }
    }
}

std::string TeamManager::getTeamFilePath(const std::string& teamName) const {
    return (fs::path(dataDir) / (teamName + ".json")).string();
}

void TeamManager::loadTeams() {
    if (!fs::exists(dataDir)) return;

    for (const auto& entry : fs::directory_iterator(dataDir)) {
        if (entry.is_regular_file() && entry.path().extension() == ".json") {
            try {
                std::ifstream file(entry.path());
                nlohmann::json j;
                file >> j;
                Team team = j.get<Team>();
                teams[team.name] = team;
            } catch (const std::exception& e) {
                std::cerr << "Error loading team from " << entry.path() << ": " << e.what() << std::endl;
            }
        }
    }
}

void TeamManager::saveTeam(const std::string& teamName) {
    auto it = teams.find(teamName);
    if (it == teams.end()) return;

    try {
        std::ofstream file(getTeamFilePath(teamName));
        nlohmann::json j = it->second;
        file << j.dump(4);
    } catch (const std::exception& e) {
        std::cerr << "Error saving team " << teamName << ": " << e.what() << std::endl;
    }
}

void TeamManager::addOrUpdatePlayer(const std::string& teamName, const Player& player) {
    auto& team = teams[teamName];
    team.name = teamName;
    
    bool found = false;
    for (auto& p : team.players) {
        if (p.number == player.number) {
            p.name = player.name;
            found = true;
            break;
        }
    }
    
    if (!found) {
        team.players.push_back(player);
    }
    
    saveTeam(teamName);
}

void TeamManager::removePlayer(const std::string& teamName, int playerNumber) {
    auto it = teams.find(teamName);
    if (it != teams.end()) {
        auto& players = it->second.players;
        players.erase(std::remove_if(players.begin(), players.end(),
            [playerNumber](const Player& p) { return p.number == playerNumber; }),
            players.end());
        saveTeam(teamName);
    }
}

std::vector<std::string> TeamManager::getTeamNames() const {
    std::vector<std::string> names;
    for (const auto& [name, team] : teams) {
        names.push_back(name);
    }
    return names;
}

const Team* TeamManager::getTeam(const std::string& teamName) const {
    auto it = teams.find(teamName);
    if (it != teams.end()) {
        return &it->second;
    }
    return nullptr;
}

void TeamManager::deleteTeam(const std::string& teamName) {
    auto it = teams.find(teamName);
    if (it != teams.end()) {
        try {
            fs::remove(getTeamFilePath(teamName));
            teams.erase(it);
        } catch (const fs::filesystem_error& e) {
            std::cerr << "Error deleting team file: " << e.what() << std::endl;
        }
    }
}
