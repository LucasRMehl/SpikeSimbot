#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <iterator>
#include <algorithm>
#include <map>
#include <random>

#include "Bot.h"
#include "SpecialKEval/SevenEval.h"

// This is how the underlying hand ranking code refers to the cards
std::map<std::string, int> CARD_IDX_MAP = {
    {"As", 0},{"Ks", 4},{"Qs", 8},{"Js",12},{"Ts",16},{"9s",20},{"8s",24},
    {"Ah", 1},{"Kh", 5},{"Qh", 9},{"Jh",13},{"Th",17},{"9h",21},{"8h",25},
    {"Ad", 2},{"Kd", 6},{"Qd",10},{"Jd",14},{"Td",18},{"9d",22},{"8d",26},
    {"Ac", 3},{"Kc", 7},{"Qc",11},{"Jc",15},{"Tc",19},{"9c",23},{"8c",27},
    {"7s",28},{"6s",32},{"5s",36},{"4s",40},{"3s",44},{"2s",48},
    {"7h",29},{"6h",33},{"5h",37},{"4h",41},{"3h",45},{"2h",49},
    {"7d",30},{"6d",34},{"5d",38},{"4d",42},{"3d",46},{"2d",50},
    {"7c",31},{"6c",35},{"5c",39},{"4c",43},{"3c",47},{"2c",51},
};

const std::vector<int> FULLDECK = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,
                                   18,19,20,21,22,23,24,25,26,27,28,29,30,31,
                                   32,33,34,35,36,37,38,39,40,41,42,43,44,45,
                                   46,47,48,49,50,51};

// Set up PRNG
std::mt19937 PRNG;  // (server didn't like std::random_device)

// Set up hand ranking lib
SevenEval EVAL;

const int NUM_GAMES = 5000;

void Bot::run()
{
    std::string inputType, botNameIn;
    int value;
    while (std::cin >> inputType)
    {
        // Respond to request for action first
        if (inputType == "Action")
        {
            std::cin >> botNameIn >> value;
            this->timeBank = value;
            this->executeAction();
        }
        // Parse player actions (several times per turn)
        else if (inputType == this->botName)
            this->parsePlayerMove();
        else if (inputType == this->opponentBotName)
            this->parseOpponentMove();
        // Parse the match information (once per round)
        else if (inputType == "Match")
            this->parseMatch();
        // Parse the match settings (only happens once)
        else if (inputType == "Settings")
            this->parseSettings();
    }
}

void Bot::executeAction()
{
    /* Do AI! */
    // Before the flop, just make a simple judgment
    // Top 15 hands: AA,KK,QQ,AKs,JJ,TT,AQs,AJs,AKo,KQs,ATs,KJs,AQo,99,JQs
    if (this->alreadyRaised)
    {
        std::cerr << "Already raised this turn, calling...\n";
        std::cout << "call 0\n";
    }
    int ntc = this->tableCards.size();
    if (ntc == 0)
    {
        std::cerr << "Before the flop simple bot.\n";
        int c1 = this->handCards[0];
        int c2 = this->handCards[1];
        if ((c1/4 == c2/4) && c1<4) // AA
        {
            std::cerr << "Ace pair! c1, c2 = " << c1 << "," << c2 << "\n";
            std::cout << "raise " << this->myStack << "\n";
        }
        else if ((c1/4 == c2/4) && c1<12) // KK or QQ
        {
            std::cerr << "High pair! c1, c2 = " << c1 << "," << c2 << "\n";
            std::cout << "raise " << (this->myStack/4) << "\n";
        }
        else if ((c1%4 == c2%4) && (c1<4 || c2<4) && (c1<20 || c2<20)) // Ax suited (where x is J or better)
        {
            std::cerr << "Ace high suited! c1, c2 = " << c1 << "," << c2 << "\n";
            std::cout << "raise " << (this->myStack/5) << "\n";
        }
        else if ((c1/4 == c2/4) && c1<24) // JJ or TT
        {
            std::cerr << "JJ or TT! c1, c2 = " << c1 << "," << c2 << "\n";
            std::cout << "raise " << (this->myStack/6) << "\n";
        }
        else if ((c1<4 && c2<12) || (c1<12 && c2<4)) // AK or AQ off suit
        {
            std::cerr << "Ace high off-suit! c1, c2 = " << c1 << "," << c2 << "\n";
            std::cout << "raise " << (this->myStack/7) << "\n";
        }
        else if ((c1/4 == c2/4) && c1<28) // 99
        {
            std::cerr << "99! c1, c2 = " << c1 << "," << c2 << "\n";
            std::cout << "raise " << (this->myStack/8) << "\n";
        }
        else if ((c1%4 == c2%4) && (c1<8 || c2<8) && (c1<20 || c2<20)) // Kx suited (where x is J or better)
        {
            std::cerr << "King high suited. c1, c2 = " << c1 << "," << c2 << "\n";
            std::cout << "raise " << (this->myStack/10) << "\n";
        }
        else if (c1/4 == c2/4) // any pair
        {
            std::cerr << "Low pair. c1, c2 = " << c1 << "," << c2 << "\n";
            std::cout << "call 0\n";
        }
        else if (this->amountToCall > 40)
        {
            if (c1>=28 && c2>=28)
            {
                std::cerr << "Crappy cards, I'm out. c1, c2 = " << c1 << "," << c2 << "\n";
                std::cout << "fold 0\n";
            }
        }
        else
        {
            std::cerr << "Eh, let's do this.\n";
            std::cout << "call 0\n";
        }
        return;
    }

    // Build vector of unknownCards (could probably keep track of these...)
    std::vector<int> unknownCards = FULLDECK;
    std::vector<int>::iterator deck_end;
    deck_end = std::remove(unknownCards.begin(), unknownCards.end(), this->handCards[0]);
    deck_end = std::remove(unknownCards.begin(), deck_end, this->handCards[1]);
    for (auto table_card_idx : this->tableCards)
        deck_end = std::remove(unknownCards.begin(), deck_end, table_card_idx);

    // Now simulate a metric crap-load of games
    int wins = 0, losses = 0, ties = 0;
    int good_rank, evil_rank;

    // 
    std::vector<int> evalDeck = this->tableCards;
    evalDeck.insert(evalDeck.end(), unknownCards.begin(), deck_end);

    std::cerr << "1. My hand: " << this->playerHand << "\n";
    std::cerr << "2. Cards on table: " << this->table << "\n";
    std::cerr << "3. Running simulations..." << std::endl;

    for (int n = 0; n < NUM_GAMES; ++n)
    {
        // Shuffle and assume the first 
        std::shuffle(evalDeck.begin() + ntc, evalDeck.end(), PRNG);
        // Get rank of my and opponent's seven cards
        good_rank = EVAL.GetRank(this->handCards[0],
                                 this->handCards[1],
                                 evalDeck[0],
                                 evalDeck[1],
                                 evalDeck[2],
                                 evalDeck[3],
                                 evalDeck[4]);
        evil_rank = EVAL.GetRank(evalDeck[0],
                                 evalDeck[1],
                                 evalDeck[2],
                                 evalDeck[3],
                                 evalDeck[4],
                                 evalDeck[5], // 5&6 represent opponent's cards
                                 evalDeck[6]);

        if (good_rank > evil_rank)
            ++wins;
        else if (evil_rank > good_rank)
            ++losses;
        else
            ++ties;
    }

    float win_pct = 100.0f * static_cast<float>(wins)/NUM_GAMES;
    std::cerr << "4. Win percentage = " << win_pct << "\n";

    // Now make some decisions.
    if (this->myStack + this->currentBet < 300)
    {
        if (win_pct > 90.0f)
        {
            std::cout << "raise " << this->myStack << "\n";
            std::cerr << "Going all in!\n";
        }
        else if (win_pct > 75.0f)
        {
            if ( (this->currentBet + this->amountToCall) < (this->myStack/2) )
            {
                std::cout << "raise " << (this->myStack/2 - this->currentBet) << "\n";
                std::cerr << "Raising!\n";
            }
            else
            {
                std::cout << "call 0" << "\n";
                std::cerr << "Call!\n";
            }
        }
        else
        {
            if (this->amountToCall < win_pct/2 || this->amountToCall < this->currentBet/5)
            {
                std::cout << "call 0" << "\n";
                std::cerr << "Call!\n";
            }
            else
            {
                std::cout << "fold 0" << "\n";
                std::cerr << "I'm out!\n";
            }
        }
        return;
    }
    else //if (this->myStack + this->currentBet < 1800)
    {
        if (win_pct > 95.0f)
        {
            std::cout << "raise " << this->myStack << "\n";
            std::cerr << "Going all in!\n";
        }
        else if (win_pct > 80.0f)
        {
            if ( (this->currentBet + this->amountToCall) < (this->myStack/4) )
            {
                std::cout << "raise " << (this->myStack/4 - this->currentBet) << "\n";
                std::cerr << "Raise!\n";
            }
            else
            {
                std::cout << "call 0" << "\n";
                std::cerr << "Call!\n";
            }
        }
        else if (win_pct > 60.0f)
        {
            if ( (this->currentBet + this->amountToCall) < (this->myStack/8) )
            {
                std::cout << "raise 40\n";
                std::cerr << "Raise!\n";
            }
            else
            {
                std::cout << "call 0" << "\n";
                std::cerr << "Call!\n";
            }
        }
        else
        {
            if (this->amountToCall < win_pct*2 || this->amountToCall < this->currentBet/4)
            {
                std::cout << "call 0" << "\n";
                std::cerr << "Call!\n";
            }
            else
            {
                std::cout << "fold 0" << "\n";
                std::cerr << "I'm out!\n";

            }
        }
        return;
    }
}

void Bot::parsePlayerMove()
{
    std::string move, hand;
    int value;
    std::cin >> move;
    if (move == "stack")
    {
        std::cin >> value;
        this->myStack = value;
    }
    else if (move == "post")
    {
        std::cin >> value;
        this->currentBet = value;
        this->myStack -= value;
    }
    else if (move == "hand")
    {
        std::cin >> hand;
        this->playerHand = hand;
        this->handCards = this->parseCards(hand, this->handCards);
    }
    else if (move == "fold")
    {
        // 
        std::cin >> value;
    }
    else if (move == "check")
    {
        //
        std::cin >> value;
    }
    else if (move == "call")
    {
        std::cin >> value;
        this->currentBet += amountToCall;
        this->myStack -= amountToCall;
        this->pot += amountToCall;
    }
    else if (move == "raise")
    {
        std::cin >> value;
        // First, match opponent's bet
        this->currentBet += amountToCall;
        this->myStack -= amountToCall;
        this->pot += amountToCall;
        // Then raise
        this->currentBet += value;
        this->myStack -= value;
        this->pot += value;
        // Don't let the bot raise over and over!
        this->alreadyRaised = true;
    }
    else if (move == "wins")
    {
        //
        std::cin >> value;
    }
}

void Bot::parseOpponentMove()
{
    std::string move;
    std::string hand;
    int value;

    std::cin >> move;
    if (move == "stack")
    {
        std::cin >> value;
        this->opponentStack = value;
    }
    else if (move == "post")
    {
        std::cin >> value;
        this->pot += value;
        this->opponentBet = value;
        this->opponentStack -= value;
    }
    else if (move == "hand")
    {
        std::cin >> hand;
        this->opponentHand = hand;
    }
    else if (move == "fold")
    {
        this->opponentStatus = "fold";
    }
    else if (move == "check")
    {
        this->opponentStatus = "check";
    }
    else if (move == "call")
    {
        std::cin >> value;
        this->opponentStatus = "call";
        this->opponentBet += value;
        this->opponentStack -= value;
        this->pot += value;
    }
    else if (move == "raise")
    {
        std::cin >> value;
        this->opponentStatus = "raise";
        this->opponentBet += value;
        this->opponentStack -= value;
        this->pot += value;
    }
    else if (move == "wins")
    {
        std::cin >> value;
        this->opponentStatus = "win";
        this->opponentStack += value;
    }
    else
    {
        std::cin >> table;
        std::cerr << "Unsupported player output: " << this->opponentBotName
                  << " " << move << " " << table << "\n";
    }
}

void Bot::parseMatch()
{
    std::string matchType;
    std::string cardsOnTable;
    int value;
    std::string botName;

    std::cin >> matchType;
    if (matchType == "round")
    {
        std::cin >> value;
        this->opponentStatus ="inPlay";
        this->roundPlaying = value;
        this->tableCards.clear();
        this->alreadyRaised = false;
        std::cerr << "Round " << value << "\n";
    }
    else if (matchType == "small_blind")
    {
        std::cin >> value;
        this->smallBlind = value;
    }
    else if (matchType == "big_blind")
    {
        std::cin >> value;
        this->bigBlind = value;
    }
    else if (matchType == "on_button")
    {
        std::cin >> botName;
        this->dealer = botName;
    }
    else if (matchType == "table")
    {
        std::cin >> cardsOnTable;
        this->table = cardsOnTable;
        this->tableCards = this->parseCards(cardsOnTable, this->tableCards);
        this->alreadyRaised = false;
    }
    else if (matchType == "max_win_pot")
    {
        std::cin >> value;
        this->maxWinPot = value;
    }
    else if (matchType == "amount_to_call")
    {
        std::cin >> value;
        this->amountToCall = value;
    }
    else
    {
        std::cin >> cardsOnTable;
        std::cerr << "Unsupported Match output: Match "
                  << matchType << " " << table << "\n";
    }       
}

void Bot::parseSettings()
{
    std::string settingType, botNameIn;
    int value;
    std::cin >> settingType;
    if (settingType == "timebank")
    {
        std::cin >> value;
        this->timeBank = value;
    }
    else if (settingType == "time_per_move")
    {
        std::cin >> value;
        this->timePerMove = value;
    }
    else if (settingType == "hands_per_level")
    {
        std::cin >> value;
        this->handsPerLevel = value;
    }
    else if (settingType == "your_bot")
    {
        std::cin >> botNameIn;
        this->botName = botNameIn;
        if (botNameIn=="player1")
            this->opponentBotName = "player2";
        else
            this->opponentBotName = "player1";
    }
    else if (settingType == "starting_stack")
    {
        std::cin >> value;
        this->startingStack = value;
    }
    else
    {
        std::cin >> botNameIn;
        std::cerr << "Unsupported Settings output: Settings "
                  << settingType << " " << botNameIn << "\n";
    }  
}

std::vector<int>& Bot::parseCards(std::string& cards, std::vector<int> &v_idxs)
{
    // Splits the string at the commas and builds the passed in vector
    v_idxs.clear();
    // ignore brackets
    std::string substr = cards.substr(1, cards.length()-2);
    std::istringstream ss(substr);
    std::string token;
    while(std::getline(ss, token, ',')) {
        std::cerr << "Card: " << token << ", Index: " << CARD_IDX_MAP[token] << "\n";
        v_idxs.push_back(CARD_IDX_MAP[token]);
    }
    return v_idxs;
}
