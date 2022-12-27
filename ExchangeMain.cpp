#include "ExchangeMain.h"
#include <iostream>
#include <vector>
#include "OrderBookEntry.h"
#include "CSVReader.h"
#include <string>

ExchangeMain::ExchangeMain()
{

}

void ExchangeMain::init()
{
    std::string input;
    currentTime = orderBook.getEarliestTime();

    wallet.insertCurrency("BTC", 10);

    printIntro();
    while(true)
    {
        parsingUserOption();
        // processUserOption(input);
    }
}

void ExchangeMain::printIntro()
{
    std::cout << "====================================================== " << std::endl;
    std::cout << "Please enter a command, or help for a list of commands" << std::endl;
    std::cout << "====================================================== " << std::endl;
    blankLine();
}

// function that simply creates a blank line
void ExchangeMain::blankLine()
{
    std::cout << " " << std::endl;
}


void ExchangeMain::printMenu()
{
    // Printing help menu
    std::cout << "advisorbot> menu below:" << std::endl;
    blankLine();
    std::cout << "1: help <cmd> -> output help for the specified command " << std::endl;
    std::cout << "2: prod -> list available products " << std::endl;
    std::cout << "3: min product bid/ask -> find minimum bid or ask for product in current time step " << std::endl;
    std::cout << "4: max product bid/ask -> find maximum bid or ask for product in current time step " << std::endl;
    std::cout << "5: avg product ask/bid timesteps -> compute average ask or bid for the sent product over the sent number of time steps " << std::endl;
    std::cout << "6: predict max/min product ask/bid -> predict max or min ask or bid for the sent product for the next time step " << std::endl;
    std::cout << "7: time -> state current time in dataset, i.e. which timeframe are we looking at " << std::endl;
    std::cout << "8: step -> move to next time step " << std::endl;
    std::cout << "9: wallet -> prints wallet " << std::endl;
    std::cout << "10: quiz -> starts a quiz that can make you win or lose your BTC! " << std::endl;
    blankLine();
}

void ExchangeMain::printHelp()
{
    std::cout << "Help - your aim is to make money. Analyse the market and make bids and offers. " << std::endl;
}

void ExchangeMain::printMarketStats()
{
    for (std::string const& p : orderBook.getKnownProducts())
    {
        std::cout << "Product: " << p << std::endl;
        std::vector<OrderBookEntry> entries = orderBook.getOrders(OrderBookType::ask, p, currentTime);
        std::cout << "Asks seen: " << entries.size() << std::endl;
        std::cout << "Max ask: " << OrderBook::getHighPrice(entries) << std::endl;
        std::cout << "Min ask: " << OrderBook::getLowPrice(entries) << std::endl;
    }
}
void ExchangeMain::printAvailableProducts()
{
    std::cout << "advisorbot> list of products:" << std::endl;
    blankLine();
    for (std::string const& p : orderBook.getKnownProducts())
    {
        std::cout << "Product: " << p << std::endl;
    }
    blankLine();
}

void ExchangeMain::enterAsk()
{
    std::cout << "Make an ask - enter the amount: product,price, amount, eg  ETH/BTC,200,0.5" << std::endl;
    std::string input;
    std::getline(std::cin, input);

    std::vector<std::string> tokens = CSVReader::tokenise(input, ',');
    if (tokens.size() != 3)
    {
        std::cout << "ExchangeMain::enterAsk Bad input! " << input << std::endl;
    }
    else {
        try {
            OrderBookEntry obe = CSVReader::stringsToOBE(
                tokens[1],
                tokens[2], 
                currentTime, 
                tokens[0], 
                OrderBookType::ask 
            );
            obe.username = "simuser";
            if (wallet.canFulfillOrder(obe))
            {
                std::cout << "Wallet looks good. " << std::endl;
                orderBook.insertOrder(obe);
            }
            else {
                std::cout << "Wallet has insufficient funds . " << std::endl;
            }
        }catch (const std::exception& e)
        {
            std::cout << " ExchangeMain::enterAsk Bad input " << std::endl;
        }   
    }
}

void ExchangeMain::enterBid()
{
    std::cout << "Make an bid - enter the amount: product,price, amount, eg  ETH/BTC,200,0.5" << std::endl;
    std::string input;
    std::getline(std::cin, input);

    std::vector<std::string> tokens = CSVReader::tokenise(input, ',');
    if (tokens.size() != 3)
    {
        std::cout << "ExchangeMain::enterBid Bad input! " << input << std::endl;
    }
    else {
        try {
            OrderBookEntry obe = CSVReader::stringsToOBE(
                tokens[1],
                tokens[2], 
                currentTime, 
                tokens[0], 
                OrderBookType::bid 
            );
            obe.username = "simuser";

            if (wallet.canFulfillOrder(obe))
            {
                std::cout << "Wallet looks good. " << std::endl;
                orderBook.insertOrder(obe);
            }
            else {
                std::cout << "Wallet has insufficient funds . " << std::endl;
            }
        }catch (const std::exception& e)
        {
            std::cout << " ExchangeMain::enterBid Bad input " << std::endl;
        }   
    }
}

void ExchangeMain::printWallet()
{
    std::cout << wallet.toString() << std::endl;
}
        
void ExchangeMain::gotoNextTimeframe()
{
    std::cout << "Going to next time frame. " << std::endl;
    for (std::string p : orderBook.getKnownProducts())
    {
        std::cout << "matching " << p << std::endl;
        std::vector<OrderBookEntry> sales =  orderBook.matchAsksToBids(p, currentTime);
        std::cout << "Sales: " << sales.size() << std::endl;
        for (OrderBookEntry& sale : sales)
        {
            std::cout << "Sale price: " << sale.price << " amount " << sale.amount << std::endl; 
            if (sale.username == "simuser")
            {
                // update the wallet
                wallet.processSale(sale);
            }
        }
        
    }

    currentTime = orderBook.getNextTime(currentTime);
}

// function to create a quiz
void ExchangeMain::quiz()
{
    std::cout << "advisorbot> answer the question below to win 1 BTC... or to lose it! only lowercase please:" << std::endl;
    blankLine();
    // string to check if the user wants to keep playing
    std::string keepPlaying = "Y";
    std::string answer = "";
    while(keepPlaying == "Y")
    {
        // generating a random number that determines which question will show up
        int randomNumber = (rand() % 10) + 1;
        if(randomNumber == 1)
        {
            std::cout << "advisorbot> What is the name of the underlying technology of Bitcoin and many cryptocurrencies?" << std::endl;
            std::cout << "user> ";
            getline(std::cin, answer);
            if(answer == "blockchain")
            {
                std::cout << "advisorbot> YOU WON 1 BTC! this is your balance at this moment:" << std::endl;
                blankLine();
                // if the answer is correct, 1 BTC is added to the balance
                wallet.insertCurrency("BTC", 1);
                printWallet();
                std::cout << "advisorbot> Do you want to keep playing? Y: keep playing - Anything else: stops playing" << std::endl;
                std::cout << "user> ";
                getline(std::cin, keepPlaying);
            }
            else
            {
                std::cout << "advisorbot> Oh no! Unfortunately that wasn't the answer, therefore you have lost 1 BTC - this is your balance at this moment:" << std::endl;
                blankLine();
                // if the answer is incorrect, 1 BTC is removed to the balance
                wallet.removeCurrency("BTC", 1);
                printWallet();
                std::cout << "advisorbot> Do you want to keep playing? Y: keep playing - Anything else: stops playing" << std::endl;
                std::cout << "user> ";
                getline(std::cin, keepPlaying);
                blankLine();
            }
        }
        if(randomNumber == 2)
        {
            std::cout << "advisorbot> Which cryptography algorithm is used for hasting new blocks and running the network to create Bitcoins?" << std::endl;
            std::cout << "user> ";
            getline(std::cin, answer);
            if(answer == "sha256")
            {
                std::cout << "advisorbot> YOU WON 1 BTC! this is your balance at this moment:" << std::endl;
                blankLine();
                wallet.insertCurrency("BTC", 1);
                printWallet();
                std::cout << "advisorbot> Do you want to keep playing? Y: keep playing - Anything else: stops playing" << std::endl;
                std::cout << "user> ";
                getline(std::cin, keepPlaying);
            }
            else
            {
                std::cout << "advisorbot> Oh no! Unfortunately that wasn't the answer, therefore you have lost 1 BTC - this is your balance at this moment:" << std::endl;
                blankLine();
                wallet.removeCurrency("BTC", 1);
                printWallet();
                std::cout << "advisorbot> Do you want to keep playing? Y: keep playing - Anything else: stops playing" << std::endl;
                std::cout << "user> ";
                getline(std::cin, keepPlaying);
                blankLine();
            }
        }
        if(randomNumber == 3)
        {
            std::cout << "advisorbot> What is the estimated year when the last Bitcoin is mined?" << std::endl;
            std::cout << "user> ";
            getline(std::cin, answer);
            if(answer == "2140")
            {
                std::cout << "advisorbot> YOU WON 1 BTC! this is your balance at this moment:" << std::endl;
                blankLine();
                wallet.insertCurrency("BTC", 1);
                printWallet();
                std::cout << "advisorbot> Do you want to keep playing? Y: keep playing - Anything else: stops playing" << std::endl;
                std::cout << "user> ";
                getline(std::cin, keepPlaying);
            }
            else
            {
                std::cout << "advisorbot> Oh no! Unfortunately that wasn't the answer, therefore you have lost 1 BTC - this is your balance at this moment:" << std::endl;
                blankLine();
                wallet.removeCurrency("BTC", 1);
                printWallet();
                std::cout << "advisorbot> Do you want to keep playing? Y: keep playing - Anything else: stops playing" << std::endl;
                std::cout << "user> ";
                getline(std::cin, keepPlaying);
                blankLine();
            }
        }
        if(randomNumber == 4)
        {
            std::cout << "advisorbot> Which cryptocurrency term refers to entities or individuals that hold large amounts of Bitcoin?" << std::endl;
            std::cout << "user> ";
            getline(std::cin, answer);
            if(answer == "whale")
            {
                std::cout << "advisorbot> YOU WON 1 BTC! this is your balance at this moment:" << std::endl;
                blankLine();
                wallet.insertCurrency("BTC", 1);
                printWallet();
                std::cout << "advisorbot> Do you want to keep playing? Y: keep playing - Anything else: stops playing" << std::endl;
                std::cout << "user> ";
                getline(std::cin, keepPlaying);
            }
            else
            {
                std::cout << "advisorbot> Oh no! Unfortunately that wasn't the answer, therefore you have lost 1 BTC - this is your balance at this moment:" << std::endl;
                blankLine();
                wallet.removeCurrency("BTC", 1);
                printWallet();
                std::cout << "advisorbot> Do you want to keep playing? Y: keep playing - Anything else: stops playing" << std::endl;
                std::cout << "user> ";
                getline(std::cin, keepPlaying);
                blankLine();
            }
        }
        if(randomNumber == 5)
        {
            std::cout << "advisorbot> Which was the first publicly-traded company to purchase Bitcoin in 2020?" << std::endl;
            std::cout << "user> ";
            getline(std::cin, answer);
            if(answer == "microstrategy")
            {
                std::cout << "advisorbot> YOU WON 1 BTC! this is your balance at this moment:" << std::endl;
                blankLine();
                wallet.insertCurrency("BTC", 1);
                printWallet();
                std::cout << "advisorbot> Do you want to keep playing? Y: keep playing - Anything else: stops playing" << std::endl;
                std::cout << "user> ";
                getline(std::cin, keepPlaying);
            }
            else
            {
                std::cout << "advisorbot> Oh no! Unfortunately that wasn't the answer, therefore you have lost 1 BTC - this is your balance at this moment:" << std::endl;
                blankLine();
                wallet.removeCurrency("BTC", 1);
                printWallet();
                std::cout << "advisorbot> Do you want to keep playing? Y: keep playing - Anything else: stops playing" << std::endl;
                std::cout << "user> ";
                getline(std::cin, keepPlaying);
                blankLine();
            }
        }
        if(randomNumber == 6)
        {
            std::cout << "advisorbot> Which one doesn't apply to BTC transactions? permisionless - anonymous - private - reversible " << std::endl;
            std::cout << "user> ";
            getline(std::cin, answer);
            if(answer == "reversible")
            {
                std::cout << "advisorbot> YOU WON 1 BTC! this is your balance at this moment:" << std::endl;
                blankLine();
                wallet.insertCurrency("BTC", 1);
                printWallet();
                std::cout << "advisorbot> Do you want to keep playing? Y: keep playing - Anything else: stops playing" << std::endl;
                std::cout << "user> ";
                getline(std::cin, keepPlaying);
            }
            else
            {
                std::cout << "advisorbot> Oh no! Unfortunately that wasn't the answer, therefore you have lost 1 BTC - this is your balance at this moment:" << std::endl;
                blankLine();
                wallet.removeCurrency("BTC", 1);
                printWallet();
                std::cout << "advisorbot> Do you want to keep playing? Y: keep playing - Anything else: stops playing" << std::endl;
                std::cout << "user> ";
                getline(std::cin, keepPlaying);
                blankLine();
            }
        }
        if(randomNumber == 7)
        {
            std::cout << "advisorbot> What is the smallest unit of BTC called" << std::endl;
            std::cout << "user> ";
            getline(std::cin, answer);
            if(answer == "satoshi")
            {
                std::cout << "advisorbot> YOU WON 1 BTC! this is your balance at this moment:" << std::endl;
                blankLine();
                wallet.insertCurrency("BTC", 1);
                printWallet();
                std::cout << "advisorbot> Do you want to keep playing? Y: keep playing - Anything else: stops playing" << std::endl;
                std::cout << "user> ";
                getline(std::cin, keepPlaying);
            }
            else
            {
                std::cout << "advisorbot> Oh no! Unfortunately that wasn't the answer, therefore you have lost 1 BTC - this is your balance at this moment:" << std::endl;
                blankLine();
                wallet.removeCurrency("BTC", 1);
                printWallet();
                std::cout << "advisorbot> Do you want to keep playing? Y: keep playing - Anything else: stops playing" << std::endl;
                std::cout << "user> ";
                getline(std::cin, keepPlaying);
                blankLine();
            }
        }
        if(randomNumber == 8)
        {
            std::cout << "advisorbot> Guessing at the secret number (nonce) of the blockchain is called this..." << std::endl;
            std::cout << "user> ";
            getline(std::cin, answer);
            if(answer == "hasing")
            {
                std::cout << "advisorbot> YOU WON 1 BTC! this is your balance at this moment:" << std::endl;
                blankLine();
                wallet.insertCurrency("BTC", 1);
                printWallet();
                std::cout << "advisorbot> Do you want to keep playing? Y: keep playing - Anything else: stops playing" << std::endl;
                std::cout << "user> ";
                getline(std::cin, keepPlaying);
            }
            else
            {
                std::cout << "advisorbot> Oh no! Unfortunately that wasn't the answer, therefore you have lost 1 BTC - this is your balance at this moment:" << std::endl;
                blankLine();
                wallet.removeCurrency("BTC", 1);
                printWallet();
                std::cout << "advisorbot> Do you want to keep playing? Y: keep playing - Anything else: stops playing" << std::endl;
                std::cout << "user> ";
                getline(std::cin, keepPlaying);
                blankLine();
            }
        }
        if(randomNumber == 9)
        {
            std::cout << "advisorbot> Which one of these BTC hacks lead to the largest loss so far? bitfinex - mt. gox collapse - coincheck hack - govex collapse" << std::endl;
            std::cout << "user> ";
            getline(std::cin, answer);
            if(answer == "mt. gox collapse")
            {
                std::cout << "advisorbot> YOU WON 1 BTC! this is your balance at this moment:" << std::endl;
                blankLine();
                wallet.insertCurrency("BTC", 1);
                printWallet();
                std::cout << "advisorbot> Do you want to keep playing? Y: keep playing - Anything else: stops playing" << std::endl;
                std::cout << "user> ";
                getline(std::cin, keepPlaying);
            }
            else
            {
                std::cout << "advisorbot> Oh no! Unfortunately that wasn't the answer, therefore you have lost 1 BTC - this is your balance at this moment:" << std::endl;
                blankLine();
                wallet.removeCurrency("BTC", 1);
                printWallet();
                std::cout << "advisorbot> Do you want to keep playing? Y: keep playing - Anything else: stops playing" << std::endl;
                std::cout << "user> ";
                getline(std::cin, keepPlaying);
                blankLine();
            }
        }
        if(randomNumber == 10)
        {
            std::cout << "advisorbot> Which one was the first country to adopt BTC as a legal tender?" << std::endl;
            std::cout << "user> ";
            getline(std::cin, answer);
            if(answer == "el salvador")
            {
                std::cout << "advisorbot> YOU WON 1 BTC! this is your balance at this moment:" << std::endl;
                blankLine();
                wallet.insertCurrency("BTC", 1);
                printWallet();
                std::cout << "advisorbot> Do you want to keep playing? Y: keep playing - Anything else: stops playing" << std::endl;
                std::cout << "user> ";
                getline(std::cin, keepPlaying);
            }
            else
            {
                std::cout << "advisorbot> Oh no! Unfortunately that wasn't the answer, therefore you have lost 1 BTC - this is your balance at this moment:" << std::endl;
                blankLine();
                wallet.removeCurrency("BTC", 1);
                printWallet();
                std::cout << "advisorbot> Do you want to keep playing? Y: keep playing - Anything else: stops playing" << std::endl;
                std::cout << "user> ";
                getline(std::cin, keepPlaying);
                blankLine();
            }
        }
    } 
    
}

// https://www.javatpoint.com/how-to-split-strings-in-cpp

// The logic in the parsing function is based on spliting the string introduced by the user and analysing the words one by one
// The spliting functionality was based on the code that can be found on the link above

void ExchangeMain::parsingUserOption()
{
    std::string command;
    std::string strings[10];
    std::cout << "user> ";
    getline(std::cin, command);

    if(command == "prod") 
    {
        printAvailableProducts();
    } 

    if(command == "stats")
    {
        printMarketStats();
    }  

    if(command == "time")
    {
        std::cout << "advisorbot> current time is: " << currentTime << std::endl;
        blankLine();
    }     

    if(command == "step")
    {
        currentTime = orderBook.getNextTime(currentTime);
        std::cout << "advisorbot> now at: " << currentTime << std::endl;
        blankLine();
    } 

    if(command == "wallet")
    {
        printWallet();
    }

    if(command == "quiz")
    {
        quiz();
    }

    else
    {
        int length = 0;  
        for (int i = 0; command[i] != '\0'; i++)  
        {  
            length++;  
        }      

        int currIndex = 0, i = 0;  
        int startIndex = 0, endIndex = 0;  
        while (i <= length)  
        {  
            if (command[i] == ' ' || i == length)  
            {  
                endIndex = i;  
                std::string subStr = "";  
                subStr.append(command, startIndex, endIndex - startIndex);  
                strings[currIndex] = subStr;
                currIndex += 1;  
                startIndex = endIndex + 1;  
            }  
            i++;  
        }  

        // https://www.codespeedy.com/count-the-number-of-null-elements-in-an-array-in-c/
        int count = 0;
        for(int i=0; i<10; i++)
        {
            if(strings[i]!="")
            count++;   // Counting non-null elements in the string array
        }

        if(strings[0] == "help") 
        {
            // printing menu if command == help only 1 word
            if(count == 1)
            {
                printMenu();
            }
            else
            {
                if(count>2)
                {
                    std::cout << "advisorbot> syntax error: this help command is not recognised - please try again " << std::endl; // raising an error if help <cmd> contains more than 2 words
                    blankLine();
                }
                else
                {
                    if(strings[1] == "prod")
                    {
                        std::cout << "advisorbot> list available products " << std::endl;
                        blankLine();
                    }
                    else
                    {
                        if(strings[1] == "min")
                        {
                            std::cout << "advisorbot> syntax: min product bid/ask, eg. min ETH/BTC bid 10 -> min ETH/BTC bid over last 10 time steps " << std::endl;
                            blankLine();
                        }
                        else
                        {
                            if(strings[1] == "max")
                            {
                                std::cout << "advisorbot> syntax: max product bid/ask, eg. max ETH/BTC bid 10 -> max ETH/BTC bid over last 10 time steps " << std::endl;
                                blankLine();
                            }
                            else
                            {
                                if(strings[1] == "avg")
                                {
                                    std::cout << "advisorbot> syntax: avg product bid/ask, eg. avg ETH/BTC bid 10 -> avg ETH/BTC bid over last 10 time steps " << std::endl;
                                    blankLine();
                                }
                                else
                                {
                                    if(strings[1] == "predict")
                                    {
                                        std::cout << "advisorbot> syntax: predict max/min product ask/bid, eg. predict max ETH/BTC bid -> The average ETH/BTC ask price over the last 10 timesteps was 1.0  " << std::endl;
                                        blankLine();
                                    }
                                    else
                                    {
                                        if(strings[1] == "time")
                                        {
                                            std::cout << "advisorbot> syntax: time -> state current time in dataset, i.e. which timeframe are we looking at " << std::endl;
                                            blankLine();
                                        }
                                        else
                                        {
                                            if(strings[1] == "step")
                                            {
                                                std::cout << "advisorbot> syntax: step -> move to next time step " << std::endl;
                                                blankLine();
                                            }
                                            else
                                            {
                                                if(strings[1] == "wallet")
                                                {
                                                    std::cout << "advisorbot> syntax: wallet -> prints wallet on console with current holdings " << std::endl;
                                                    blankLine();
                                                }
                                                else
                                                {
                                                    if(strings[1] == "quiz")
                                                    {
                                                        std::cout << "advisorbot> syntax: quiz -> starts a quiz that can make you win or lose your BTC! " << std::endl;
                                                        blankLine();
                                                    }
                                                    else
                                                    {
                                                        std::cout << "advisorbot> syntax error: this help command is not recognised - please try again " << std::endl;
                                                        blankLine();
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        // logic when the first word of command is min
        if(strings[0] == "min")
        {
            // if more than 3 words print an error
            if(count>3)
            {
                std::cout << "advisorbot> syntax error: this help command is not recognised - please try again " << std::endl; // raising an error if help <cmd> contains more than 2 words
                blankLine();
            }
            else
            {
                // check if the second word is contained in the list of products
                bool prodFound = false;
                for (std::string const& p : orderBook.getKnownProducts())
                {
                    if(strings[1] == p)
                    {
                    prodFound = true;
                        // ask or bid command on third word
                        if(strings[2] == "ask")
                        {
                            std::vector<OrderBookEntry> entries = orderBook.getOrders(OrderBookType::ask, p, currentTime);
                            std::cout << "advisorbot> the min ask for " << p << " is " << OrderBook::getLowPrice(entries) << std::endl;
                            blankLine();
                            break;
                        }
                        else
                        {
                            if(strings[2] == "bid")
                            {
                                std::vector<OrderBookEntry> entries = orderBook.getOrders(OrderBookType::bid, p, currentTime);
                                std::cout << "advisorbot> the min bid for " << p << " is " << OrderBook::getLowPrice(entries) << std::endl;
                                blankLine();
                                break;
                            }
                            else
                            {
                                std::cout << "advisorbot> syntax error: the command and product are recognised but not the action - only bid/ask allowed - please try again " << std::endl;
                                blankLine();
                                break;
                            }
                        }
                    }
                }
                if (prodFound == false)
                {
                    std::cout << "advisorbot> syntax error: product not recognised - please use prod command to see a list of products available " << std::endl;
                    blankLine();
                }
            }
        }
        // logic when the first word of command is max
        if(strings[0] == "max")
        {
            // if more than 3 words print an error
            if(count>3)
            {
                std::cout << "advisorbot> syntax error: this help command is not recognised - please try again " << std::endl; // raising an error if help <cmd> contains more than 2 words
                blankLine();
            }
            else
            {
                // check if the second word is contained in the list of products
                bool prodFound = false;
                for (std::string const& p : orderBook.getKnownProducts())
                {
                    if(strings[1] == p)
                    {
                        prodFound = true;
                        // ask or bid command on third word
                        if(strings[2] == "ask")
                        {
                            std::vector<OrderBookEntry> entries = orderBook.getOrders(OrderBookType::ask, p, currentTime);
                            std::cout << "advisorbot> the max ask for " << p << " is " << OrderBook::getHighPrice(entries) << std::endl;
                            blankLine();
                            break;
                        }
                        else
                        {
                            if(strings[2] == "bid")
                            {
                                std::vector<OrderBookEntry> entries = orderBook.getOrders(OrderBookType::bid, p, currentTime);
                                std::cout << "advisorbot> the max bid for " << p << " is " << OrderBook::getHighPrice(entries) << std::endl;
                                blankLine();
                                break;
                            }
                            else
                            {
                                std::cout << "advisorbot> syntax error: the command and product are recognised but not the action - only bid/ask allowed - please try again " << std::endl;
                                blankLine();
                                break;
                            }
                        }
                    }
                }
                if (prodFound == false)
                {
                    std::cout << "advisorbot> syntax error: product not recognised - please use prod command to see a list of products available " << std::endl;
                    blankLine();
                }
            }
        }

        if(strings[0] == "avg")
        {
            // if more than 3 words print an error
            if(count>4)
            {
                std::cout << "advisorbot> syntax error: this help command is not recognised - please try again " << std::endl; // raising an error if help <cmd> contains more than 2 words
                blankLine();
            }
            else
            {
                 // check if the second word is contained in the list of products
                bool prodFound = false;
                for (std::string const& p : orderBook.getKnownProducts())
                {
                    if(strings[1] == p)
                    {
                        prodFound = true;
                        // ask or bid command on third word
                        if(strings[2] == "ask")
                        {
                            int sampleSize = 0;
                            double totalAmount = 0;
                            // converting last character into int for the number of steps
                            int steps = stoi(strings[3]);
                            std::string currentTime_ = currentTime;
                            for(int i=0; i<steps; i++)
                            {
                                std::vector<OrderBookEntry> entries = orderBook.getOrders(OrderBookType::ask, p, currentTime_);
                                sampleSize = sampleSize + entries.size();
                                totalAmount = totalAmount + orderBook.getSumPrice(entries);
                                currentTime_ = orderBook.getPreviousTime(currentTime_);
                            }
                            std::cout << "advisorbot> the average " << p << " ask price over the last " << steps << " timesteps was " << totalAmount/sampleSize << std::endl;
                            blankLine();
                        }
                        else
                        {
                            if(strings[2] == "bid")
                            {
                                int sampleSize = 0;
                                double totalAmount = 0;
                                // converting last character into int for the number of steps
                                int steps = stoi(strings[3]);
                                std::string currentTime_ = currentTime;
                                for(int i=0; i<steps; i++)
                                {
                                    std::vector<OrderBookEntry> entries = orderBook.getOrders(OrderBookType::bid, p, currentTime_);
                                    sampleSize = sampleSize + entries.size();
                                    totalAmount = totalAmount + orderBook.getSumPrice(entries);
                                    currentTime_ = orderBook.getPreviousTime(currentTime_);
                                }
                                std::cout << "advisorbot> the average " << p << " bid price over the last " << steps << " timesteps was " << totalAmount/sampleSize << std::endl;
                                blankLine();
                            }
                        }
                    }
                }
                if (prodFound == false)
                {
                    std::cout << "advisorbot> syntax error: product not recognised - please use prod command to see a list of products available " << std::endl;
                    blankLine();
                }
            }
        }
    }
}
