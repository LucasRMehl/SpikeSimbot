#ifndef BOT_H
#define BOT_H



class Bot
{
    public:
        Bot();

        void run();
        void sim();

        void setBotName(string name);
        string getBotName();
        void setOpponentBotName(string name);
        string getOpponentBotName( );

    private:

        vector<Card> table;
        vector<Card> hand;
        vector<Card> oppHand;

        string botName;
        string opponentBotName;
        int timeBank;
        int timePerMove;
        int handsPerLevel;
        int startingStack;
        string opponentStatus;
        int opponentBet;
        int currentBet;
        int pot;
        Parser parser;

        int roundPlaying;
        int smallBlind;
        int bigBlind;
        string onButton;
        int maxWinPot;
        int amountToCall;
        int myStack;
        int opponentStack;
        string phase;

};


#endif