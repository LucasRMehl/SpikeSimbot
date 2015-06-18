#include <iostream>
#include <cstdlib>

#include <numeric>
#include <random>
#include <algorithm>

#include "SpecialKEval/SevenEval.h"

const int NUM_GAMES = 100000;

int main(int argc, char *argv[])
{
    // Set up PRNG
    std::random_device rd;
    std::mt19937 prng(rd());

    // Fill deck with cards (SevenEval expects cards to be numbered 0-52)
    std::vector<int> deck = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9,
                             10,11,12,13,14,15,16,17,18,19,
                             20,21,22,23,24,25,26,27,28,29,
                             30,31,32,33,34,35,36,37,38,39,
                             40,41,42,43,44,45,46,47,48,49,
                             50,51};

	SevenEval game;


    int wins = 0, losses = 0, ties = 0;
    int good_rank, evil_rank;

    for (int n = 0; n < NUM_GAMES; ++n)
    {
        // Shuffle cards
        std::shuffle(deck.begin(), deck.end(), prng);

        // get rank of first seven
        good_rank = game.GetRank(deck[0],
                                 deck[1],
                                 deck[2],
                                 deck[3],
                                 deck[4],
                                 deck[5],
                                 deck[6]);
        evil_rank = game.GetRank(deck[7],
                                 deck[8],
                                 deck[2],
                                 deck[3],
                                 deck[4],
                                 deck[5],
                                 deck[6]);
        //std::cout << "My Rank: " << good_rank << std::endl;
        //std::cout << "Opponent Rank: " << evil_rank << std::endl;
        if (good_rank > evil_rank)
        {
            //std::cout << "I won!" << std::endl;
            ++wins;
        }
        else if (evil_rank > good_rank)
        {
            //std::cout << "I lost. :(" << std::endl;
            ++losses;
        }
        else
        {
            //std::cout << "We tied...somehow..." << std::endl;
            ++ties;
        }
    }

    std::cout << "Win percentage: " << 100.0f * ((float)wins/(float)NUM_GAMES) << "%" << std::endl;
    std::cout << "Loss percentage: " << 100.0f * ((float)losses/(float)NUM_GAMES) << "%" << std::endl;
    std::cout << "Tie percentage: " << 100.0f * ((float)ties/(float)NUM_GAMES) << "%" << std::endl;
/*
    // Check highest number rank
    std::cout << "0,1,2,3,4,5,6: " << game.GetRank(0,1,2,3,4,5,6) << std::endl;

    // Check spade flush rank 1
    std::cout << "0,4,8,12,16,20,24: " << game.GetRank(0,4,8,12,16,20,24) << std::endl;

    // Check spade flush rank 2
    std::cout << "4,8,12,16,20,24,28: " << game.GetRank(4,8,12,16,20,24,28) << std::endl;

    // Check heart(?) flush rank 1
    std::cout << "1,5,9,13,17,21,25: " << game.GetRank(1,5,9,13,17,21,25) << std::endl;
*/

	return 0;
}