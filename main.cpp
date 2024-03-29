/*
    //  Copyright © 2018 Zihan Chen. All rights reserved.
*/


#include "game.h"
#include "rules.h"
#include "player.h"
#include "rewardDeck.h"
#include "derivedGame.h"
#include "derivedBoard.h"
#include "derivedRules.h"

#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <chrono>
#include <thread>
#include <exception>

using std::cout;
using std::endl;
using std::cin;
using std::stringstream;

using std::vector;
using std::map;

using namespace std::this_thread; // sleep_for, sleep_until
using namespace std::chrono; // nanoseconds, system_clock, seconds

//class Player;
void buildLetter(std::map<char,Game::Letter> &l);
void buildNumber(std::map<char, Game::Number> &n);
bool sortReward(int i,int j) { return (i<j); }
bool allFaceup(map<char,Game::Letter> letter,map<char,Game::Number> number,const Board& board);
int main(){
    /*
    Now this program only focuses on Base mode. eXpert mode to be done
    */
   //Game::Letter letter;
   //Game::Number number;
   map<char,Game::Letter> letter;
   map<char,Game::Number> number;
    



   buildLetter(letter);
   buildNumber(number);
   string response = "";

   int numberOfPlayer = 0;  
   vector<string> nameOfPlayer;
   vector<Reward*> reward;
   string mode;
   //char mode = {0};
    cout<<"Please Select a Game Mode"<<endl;
    cout<<"1:    Standard Game Mode"<<endl;
    cout<<"2:    Expert Game Mode"<<endl;
    cout<<"3:    Expert Desplay With Standard Rules"<<endl;
    cout<<"4:    Standard Desplay With Expert Rules"<<endl;
     
   while(true){
       getline(cin,response);
       if(response=="1" || response == "2" || response == "3"|| response == "4"){
           mode = response;
           break;
       }
       cout<< "Invalid input, please enter 1, 2, 3, or 4"<<endl;
   }
   cout<<"Your choice is: "<<mode<<endl;
   while(true){
       cout<<"Please Enter Number Of Players (2-4) "<<endl;
       getline(cin,response);
       stringstream ss(response);
       if(ss>>numberOfPlayer){
           if((numberOfPlayer>=2)&&(numberOfPlayer<=4))
                break;
       }
       cout<<"Invalid Entry, please try again"<<endl;
   }
   cout<<numberOfPlayer<<" Players are chosen"<<endl;
   int num = numberOfPlayer;
   while(num!=0){
       cout<<"Please enter the Names for players one by one"<<endl;
       getline(cin,response);
       nameOfPlayer.push_back(response);
       num--;
   }
   for(int i=1;i<nameOfPlayer.size()+1;++i){
       cout<<"Player"<<i<<" name is: "<<nameOfPlayer[i-1]<<endl;
   }
   cout<<"------------------------------------"<<endl;
   /*
    Now building game logic
   */
    //Board board;
    //Game game(board,mode);
    try{
        
        DerivedBoard board;
        DerivedGame game(board,mode);
    
    vector<Player> Tplayers; // 
    for(int i=0;i<numberOfPlayer;++i){ // adding players to game


        Tplayers.emplace_back(nameOfPlayer[i],0);
        if(i==0){
            Tplayers[i].setSide(Player::Side::top);
        }else if(i==1){
            Tplayers[i].setSide(Player::Side::bottom);
        }else if(i==2){
            Tplayers[i].setSide(Player::Side::right);
        }else if(i==3){
            Tplayers[i].setSide(Player::Side::left);
        }
        game.addPlayer(Tplayers[i]);
    }
    cout<<game<<endl;

    for(int i=0;i<Tplayers.size();++i){  // this for loop is revealing carsd to players
        cout<<"------------------------------------"<<endl;
        cout<<"Player "<<nameOfPlayer[i]<<" it's your turn to check 3 cards in front of you"<<endl;
        if(Tplayers[i].getSide() == Player::Side::top){
            game.getCard(Game::Letter::A,Game::Number::Number2);
            game.getCard(Game::Letter::A,Game::Number::Number3);
            game.getCard(Game::Letter::A,Game::Number::Number4);
            sleep_for(nanoseconds(10));
            sleep_until(system_clock::now() + seconds(1));
            
        }else if(Tplayers[i].getSide() == Player::Side::bottom){
            game.getCard(Game::Letter::E,Game::Number::Number2);
            game.getCard(Game::Letter::E,Game::Number::Number3);
            game.getCard(Game::Letter::E,Game::Number::Number4);
            sleep_for(nanoseconds(10));
            sleep_until(system_clock::now() + seconds(1));
        }else if(Tplayers[i].getSide() == Player::Side::right){
            game.getCard(Game::Letter::B,Game::Number::Number5);
            game.getCard(Game::Letter::C,Game::Number::Number5);
            game.getCard(Game::Letter::D,Game::Number::Number5);
            sleep_for(nanoseconds(10));
            sleep_until(system_clock::now() + seconds(1));
        }else if(Tplayers[i].getSide() == Player::Side::left){
            game.getCard(Game::Letter::B,Game::Number::Number1);
            game.getCard(Game::Letter::C,Game::Number::Number1);
            game.getCard(Game::Letter::D,Game::Number::Number1);
            sleep_for(nanoseconds(10));
            sleep_until(system_clock::now() + seconds(1));
        }

        cout<<game<<endl;
        board.reset();
    }
    

    RewardDeck &rd = RewardDeck::make_RewardDeck();
    while(!rd.isEmpty()){
        reward.push_back(rd.getNext());
    }

    /*
        Create rule
    */

    DerivedRules rules(Tplayers);
    /*
        Game body starts
    */
    while(!rules.gameOver(game)){
        for(int i=0;i<Tplayers.size();++i){
            Tplayers[i].setAcive(true);
            game.getPlayer(Tplayers[i].getSide()).setAcive(true);
        }
        
        game.setCurrentCard(nullptr); // current/prev cards should be nullptr when nobody is playing
        int ind = 0;
        int help = 0;
        int win = 0;
        string blockCard = "";
        
        cout<<"Now Startig Game: "<<endl;
        cout<<game<<endl;
        while(!rules.roundOver(game)){
            bool crab=false;
            if(ind == Tplayers.size()){ind = 0;}
             
            /*
            # next active player takes a turn
            */
           while(!rules.getNextPlayer(game).isActive()){
               ind++;
               if(ind >= Tplayers.size()){ind = 0;}
           }
            
            cout<<"---------------------------------------------------------------------"<<endl;
            cout<<"---------------------------------------------------------------------"<<endl;

            cout<<"ROUND: "<<game.getRound()<<"-----------------------------------------"<<endl;
            string input = "";   
            cout<<"Player "<<nameOfPlayer[ind]<<" now choose one card"<<endl;
            getline(cin,input);

            while((!letter.count(input[0]) || !number.count(input[1])) || input==""|| (input[0] == 'C' && input[1] == '3')|| (board.isFaceUp(letter.find(input[0])->second,number.find(input[1])->second))){
                cout<<"Letter is " << input[0] <<" number is "<<input[1]<<endl;
                cout<<"Invalid Input, Please choose one card"<<endl;
        
                getline(cin,input);
            }

            cout<<"Letter is " << input[0] <<" number is "<<input[1]<<endl;
            
            while(game.getCard(letter.find(input[0])->second,number.find(input[1])->second) == nullptr){
                cout<<"Sorry this card is blocked, please try other cards: "<<endl;
                getline(cin,input);
            }
            /*
                try catch
            */

            game.setCurrentCard(game.getCard(letter.find(input[0])->second,number.find(input[1])->second));

            if(!rules.isValid(game) || allFaceup(letter,number,board)){
                /*
                    set current player as inactive
                */
                game.getPlayer(Tplayers[ind].getSide()).setAcive(false);
                Tplayers[ind].setAcive(false);
                //ind++;
                }
            /*
            In expert mode, animal spacial ability is triggered iff player[ind] is actived
            */
            if(blockCard != "")
                    game.setBlock(false,letter.find(blockCard[0])->second,number.find(blockCard[1])->second);//unblock the card
            if((mode == "2" || mode =="4") && game.getPlayer(Tplayers[ind].getSide()).isActive()) {
                //if(blockCard != "")
                   // game.setBlock(false,letter.find(blockCard[0])->second,number.find(blockCard[1])->second);//unblock the card
                string animal = rules.triggerAbility(game,ind);
                blockCard = "";
                if(animal == "TURTLE"){ //THIS BLOCK IS FOR TURTLE
                    cout<<"Player "+nameOfPlayer[ind]+" is skipped !!!! "<<endl;
                }
                else if(animal == "WALRUS"){ // THIS BLOCK IS FOR WALRUS ABILITY
                    if(!rules.roundOver(game)){
                        cout<<"Please enter the position in which the card will be blocked for next player: "<<endl;
                        getline(cin,blockCard);
                        while((!letter.count(input[0]) || !number.count(input[1])) || (blockCard[0] == 'C' && blockCard[1] == '3')|| (board.isFaceUp(letter.find(blockCard[0])->second,number.find(blockCard[1])->second))){
                            cout<<"Invalid Input, Please choose one card"<<endl;
                            getline(cin,blockCard);
                        }
                        game.setBlock(true,letter.find(blockCard[0])->second,number.find(blockCard[1])->second);
                    }
                }
                else if(animal == "CRAB" ){// THIS BLOCK IS FOR CRAB
                if(!rules.roundOver(game)){
                    cout<<"Turn over another card-------------->"<<endl;
                        crab = true;
                        ind--;
                    }
                }
                else if(animal == "PENGUIN"){// THIS BLOCK IS FOR PENGUIN
                    if(!rules.roundOver(game)){
                        if(game.getNFUCard()>2){ // PENGUIN IS NOT FIRST CARD
                             cout<<"Turn one card facedown: "<<endl;
                            string toBeFD;
                            getline(cin,toBeFD);
                            while((!letter.count(input[0]) || !number.count(input[1])) || (toBeFD[0] == 'C' && toBeFD[1] == '3') || !(board.isFaceUp(letter.find(toBeFD[0])->second,number.find(toBeFD[1])->second))){
                                cout<<"Invalid Input, Please choose one card"<<endl;
                                getline(cin,toBeFD);
                            }
                            board.turnFaceDown(letter.find(toBeFD[0])->second,number.find(toBeFD[1])->second);
                        }
                    }
                }
                else if(animal == "OCTOPUS"){// THIS BLOCK IS FOR OCTOPUS
                    if(!rules.roundOver(game)){
                        cout<<"Please select one of the neighbours"<<endl;
                        vector<string> neighbours = game.getNeighbour(letter.find(input[0])->second,number.find(input[1])->second);
                        for(int i=0;i<neighbours.size();++i){
                            cout<<neighbours[i]<<" ";
                        }
                        cout<<endl;
                        string n;
                        getline(cin,n);
                        while(true){
                            bool ok = false;
                            for(int i=0;i<neighbours.size();++i){
                                if(n==neighbours[i]){
                                    ok = true;
                                }
                            }
                            if(ok)
                                break;
                            cout<<"Invalid Input, Please choose neighbour"<<endl;
                            getline(cin,n);
                        }
                        //get the octopus
                        Card* c = game.getCard(letter.find(input[0])->second,number.find(input[1])->second);
                        // check if card to be exchanged is faceup/down
                        Card* c_ex;
                        if(!board.isFaceUp(letter.find(n[0])->second,number.find(n[1])->second)){
                            c_ex = game.getCard(letter.find(n[0])->second,number.find(n[1])->second);
                            board.turnFaceDown(letter.find(n[0])->second,number.find(n[1])->second);
                        }
                        else{
                            c_ex = game.getCard(letter.find(n[0])->second,number.find(n[1])->second);
                        }
                        game.setCard(letter.find(n[0])->second,number.find(n[1])->second,c);
                         
                        game.setCard(letter.find(input[0])->second,number.find(input[1])->second,c_ex);
                        
                        // now two cards are exchanged --> input[0],[1] is now chosen card (origanl is octopus)
                        if(mode == "2")
                            game.updatePosition(letter.find(n[0])->second,number.find(n[1])->second,letter.find(input[0])->second,number.find(input[1])->second);
                        else if(mode =="4"){
                            /*
                                if the non-octopus is Fd, faceup first then turn down to trigger updateboard 
                            */
                            if(!board.isFaceUp(letter.find(input[0])->second,number.find(input[1])->second)){
                                board.turnFaceUp(letter.find(input[0])->second,number.find(input[1])->second);
                                board.turnFaceDown(letter.find(input[0])->second,number.find(input[1])->second);
                            }
                            // this is for octopus to trigger updateboard
                            board.turnFaceDown(letter.find(n[0])->second,number.find(n[1])->second);
                            board.turnFaceUp(letter.find(n[0])->second,number.find(n[1])->second);
                        
                        }
                    }
                }
            }           
            ind++;
            if(rules.roundOver(game)){
                for(int i=0;i<Tplayers.size();++i){
                    if(Tplayers[i].isActive()){
                        win = i;
                        break;
                    }
                }
            }
           // cout<<"ind: "<<ind<<"-------------"<<endl;
           //if(!crab ||rules.roundOver(game)){
            cout<<game<<endl;
            sleep_for(nanoseconds(10));
            sleep_until(system_clock::now() + seconds(1));
           //}
        }
        // remaining player gets reward
        cout<<"Winner for this round is :"+nameOfPlayer[win]<<endl;
        cout<<"---------------------------------------------------------------------"<<endl;
        cout<<endl;
        game.getPlayer(Tplayers[win].getSide()).addReward(*reward[game.getRound()]);
        
        game.nextRound();

    
    }
   // cout<<game<<endl;
    cout<<"End Of Game------------------------>"<<endl;
    for(int i=0;i<Tplayers.size();++i){   // set display Mode(end of game) to true
          game.getPlayer(Tplayers[i].getSide()).setDisplayMode(true); 
    }
    cout<<game<<endl;

    map<int,string> totalList;
    vector<int> finalReward;
    for(int i=0;i<Tplayers.size();++i){
        totalList.insert(std::pair<int,string>
            (game.getPlayer(Tplayers[i].getSide()).getNRubies(),
            game.getPlayer(Tplayers[i].getSide()).getName()));
        finalReward.push_back(game.getPlayer(Tplayers[i].getSide()).getNRubies());      
    }
    
    std::sort(finalReward.begin(),finalReward.end(),sortReward); // sort players based on rewards received
    for(int i=0;i<Tplayers.size();++i){
        cout<<"Player "<<totalList.find(finalReward[i])->second<<
        " you got "<<finalReward[i]<<" Rubies in this game"<<endl;
    }
    cout<<"------------------------------------"<<endl;
    cout<<"Congratulations to Player "<<
        totalList.find(finalReward.back())->second<<
        " for winning "<<finalReward.back()<<" rubies!!!!"<<endl;

//  END OF WHILE GAME OVER IS FALSE
    }catch (const char* msg){
        std::cerr<<msg<<endl;
        exit(0);
    }catch(const std::out_of_range& oor){
        std::cerr<<"out of range: "<<oor.what()<<endl;
        exit(0);
    }

    return 0;
}

void buildLetter(std::map<char,Game::Letter> &l){
    l.insert(std::pair<char,Game::Letter>('A',Game::Letter::A));
    l.insert(std::pair<char,Game::Letter>('B',Game::Letter::B));
    l.insert(std::pair<char,Game::Letter>('C',Game::Letter::C));
    l.insert(std::pair<char,Game::Letter>('D',Game::Letter::D));
    l.insert(std::pair<char,Game::Letter>('E',Game::Letter::E));
}
void buildNumber(std::map<char,Game::Number> &n){
    n.insert(std::pair<char,Game::Number>('1',Game::Number::Number1));
    n.insert(std::pair<char,Game::Number>('2',Game::Number::Number2));
    n.insert(std::pair<char,Game::Number>('3',Game::Number::Number3));
    n.insert(std::pair<char,Game::Number>('4',Game::Number::Number4));
    n.insert(std::pair<char,Game::Number>('5',Game::Number::Number5));
}

bool allFaceup(map<char,Game::Letter> l,map<char,Game::Number> n,const Board& board){
    for(auto const& x:l){
        for(auto const& y:n){
            if(board.isFaceUp(x.second,y.second) == false){
                return false;
            }
        }
        
    }
    return true;

    
}
