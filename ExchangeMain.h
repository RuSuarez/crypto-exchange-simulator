#pragma once

#include <vector>
#include "OrderBookEntry.h"
#include "OrderBook.h"
#include "Wallet.h"


class ExchangeMain
{
    public:
        ExchangeMain();
        /** Call this to start the sim */
        void init();
    private: 
        void printIntro();
        void blankLine();
        void printMenu();
        void printHelp();
        void printMarketStats();
        void printAvailableProducts();
        void enterAsk();
        void enterBid();
        void printWallet();
        void gotoNextTimeframe();
        // int getUserOption();
        void quiz();
        void parsingUserOption();
        void processUserOption(int userOption);

        std::string currentTime;

    OrderBook orderBook{"orders.csv"};
        Wallet wallet;

};
