#include <iostream>
#include <cstdlib>

#include <numeric>
#include <random>
#include <algorithm>
#include <iterator>
#include <string>

#include "SpecialKEval/SevenEval.h"

const int NUM_GAMES = 200000;

// Set up PRNG
std::random_device RD;
std::mt19937 PRNG(RD());

SevenEval GAME;

void print_hand(std::vector<int> *deck, int cards_in_play)
{
    // 0-3 A, 4-7 K, 8-11 Q, 12-15 J, 16-19 T, 20-23 9, 24-27 8
    // 28-31 7, 32-35 6, 36-39 5, 40-43 4, 44-47 3, 48-51 2
    std::string hand = "";
    for (int idx = 0; idx < cards_in_play; ++idx)
    {
        if (idx == 0)
            hand += "Hand: ";
        else if (idx == 2)
            hand += "\nFlop: ";
        else if (idx == 5)
            hand += "\nTurn: ";
        else if (idx == 6)
            hand += "\nRiver: ";
        // So, integer divide by 4 for value...
        int card_idx = (*deck)[idx];
        switch (card_idx / 4)
        {
            case 0:
                hand += "A";
                break;
            case 1:
                hand += "K";
                break;
            case 2:
                hand += "Q";
                break;
            case 3:
                hand += "J";
                break;
            case 4:
                hand += "T";
                break;
            default:
                hand += std::to_string(14-(card_idx/4));
                break;
        }
        // ...and mod 4 for suit
        switch (card_idx % 4)
        {
            case 0:
                hand += "s ";
                break;
            case 1:
                hand += "h ";
                break;
            case 2:
                hand += "d ";
                break;
            case 3:
                hand += "c ";
                break;
            default:
                std::cout << "Something went very, very wrong." << std::endl;
                break;
        }
    }

    std::cout << hand << std::endl;
}

float simulate(std::vector<int> *deck, int cards_in_play, int num_runs)
{
    auto deck_it = deck->begin();
    int wins = 0, losses = 0, ties = 0;
    int good_rank, evil_rank;
    for (int n = 0; n < num_runs; ++n)
    {
        // Advance iterator to avoid shuffling the cards in play
        deck_it = deck->begin();
        std::advance(deck_it, cards_in_play);
        std::shuffle(deck_it, deck->end(), PRNG);

        // get rank of my and opponent's seven cards
        good_rank = GAME.GetRank((*deck)[0],(*deck)[1],(*deck)[2],(*deck)[3],(*deck)[4],(*deck)[5],(*deck)[6]);
        evil_rank = GAME.GetRank((*deck)[7],(*deck)[8],(*deck)[2],(*deck)[3],(*deck)[4],(*deck)[5],(*deck)[6]);

        if (good_rank > evil_rank)
            ++wins;
        else if (evil_rank > good_rank)
            ++losses;
        else
            ++ties;
    }

    return (static_cast<float>(wins)/num_runs);
}

int main(int argc, char *argv[])
{
    // Fill deck with cards (SevenEval expects cards to be numbered 0-52)
    std::vector<int> deck(52);
    std::iota(deck.begin(), deck.end(), 0);

    // Shuffle deck once
    std::shuffle(deck.begin(), deck.end(), PRNG);

    // Set up sim
    int cards_in_play = 2;

    float win_pct = simulate(&deck, cards_in_play, NUM_GAMES);

    print_hand(&deck, cards_in_play);

    std::cout << "Win percentage: " << 100.0f * win_pct << "%" << std::endl;

	return 0;
}