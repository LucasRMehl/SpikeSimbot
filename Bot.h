#ifndef BOT_H
#define BOT_H

#include <string>
#include <vector>

class Bot
{
    public:
        void run();

    private:
        // internal state
        std::vector<int> handCards;
        std::vector<int> tableCards;
        int pot;
        std::string opponentStatus;
        int opponentBet;
        int currentBet;
        int myStack;
        int opponentStack;
        std::string playerHand;
        std::string opponentHand; // We'll never get this, but the spec allows it

        // Match Settings
        std::string botName;
        std::string opponentBotName;
        int timeBank;
        int timePerMove;
        int handsPerLevel;
        int startingStack;
        
        // Round Info
        int roundPlaying;
        int smallBlind;
        int bigBlind;
        std::string dealer;
        std::string table;
        int maxWinPot;
        int amountToCall;

        // Internal functions
        void executeAction();
        void parsePlayerMove();
        void parseOpponentMove();
        void parseMatch();
        void parseSettings();
        std::vector<int>& parseCards(std::string&, std::vector<int>&);
};

#endif