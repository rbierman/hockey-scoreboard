#pragma once

#include <string>
#include <ixwebsocket/IXWebSocketServer.h>
#include <ixwebsocket/IXWebSocket.h>
#include <nlohmann/json.hpp>
#include "../ScoreboardState.h"
#include "../TeamManager.h"
#include "Base64Coder.h"

class ScoreboardController;

class WebSocketManager {
public:
    WebSocketManager(int port, ScoreboardController& controller, TeamManager& teamManager, const Base64Coder& base64Coder);
    ~WebSocketManager();

    void start();
    void stop();

    void broadcastState(const ScoreboardState& state);

private:
    int port;
    ScoreboardController& controller;
    TeamManager& teamManager;
    const Base64Coder& base64Coder;
    ix::WebSocketServer server;

    void handleMessage(std::shared_ptr<ix::ConnectionState> connectionState, ix::WebSocket & webSocket, const ix::WebSocketMessagePtr & msg);
    void handleCommand(const std::string& payload);
    nlohmann::json stateToJson(const ScoreboardState& state);
    nlohmann::json teamsToJson();
};
