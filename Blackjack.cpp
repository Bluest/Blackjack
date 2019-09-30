#include <iostream>
#include <ctime>

struct Card
{
	char m_name[6];
	char m_suit[9];
	int m_value;
};

struct Player
{
	char m_name[20];
	int m_cardsInHand;
	int m_handValue;
	int m_bank;
	int m_bet;
	bool m_turn;
	Card m_hand[12];
};

void PlayerSetup(Player& _player, bool& _acesHigh);
void InitialiseDeck(Card _deck[], bool acesHigh);
void ShuffleDeck(Card _deck[]);
void NewRound(Player& _player, Player& _dealer, Card _deck[], int& _nextCard);
void PlaceBet(Player& _player);
void ClearHand(Player& _thisPlayer);
void DealCard(Player& _thisPlayer, Card _deck[], int& _nextCard);
int CalculateHandValue(Player _thisPlayer);
void Display(Player _player, Player _dealer);
void PlayersTurn(Player& _player, Player _dealer, Card _deck[], int& _nextCard);
char GetChoice();
bool Hit(Player& _player, Card _deck[], int& _nextCard);
void DealersTurn(Player& _player, Player& _dealer, Card _deck[], int& _nextCard);
void RevealHoleCard(Player _player, Player& _dealer);
bool DealerHit(Player& _player, Player& _dealer, Card _deck[], int& _nextCard);
void ResolveRound(Player& _player, Player _dealer);
bool PlayAgain();

int main()
{
	// Set up players
	Player player = { "Player", 0, 0, 500, 0, false };
	Player dealer = { "Dealer", 0, 0, 0, 0, false };
	bool acesHigh;
	PlayerSetup(player, acesHigh);

	// Create and shuffle deck
	Card deck[52];
	int nextCard = 0;
	InitialiseDeck(deck, acesHigh);
	ShuffleDeck(deck);

	// Play game
	bool playGame = true;
	while (playGame == true)
	{
		// Start round
		NewRound(player, dealer, deck, nextCard);
		PlayersTurn(player, dealer, deck, nextCard);
		Display(player, dealer);
		// Player's turn is over

		// If they didn't bust, it's the dealer's turn
		if (player.m_handValue <= 21)
		{
			DealersTurn(player, dealer, deck, nextCard);
		}
		else
		{
			std::cout << " You bust! You lost your bet of $" << player.m_bet << "." << std::endl << std::endl;
		}
		// The round is over

		// If the player still has money, they will be prompted to play again
		if (player.m_bank > 0)
		{
			playGame = PlayAgain();
		}
		else
		{
			std::cout << " You're out of money!" << std::endl;
			playGame = false;
		}
		// If the player doesn't play again, or runs out of money, the game is over
	}

	// Display final score and exit
	std::cout << std::endl << " ~ Thanks for playing! Your final score is $" << player.m_bank << " ~" << std::endl << std::endl << " ";
	system("PAUSE");
	return 0;
}

void PrintHeader()
{
	// Clear the screen and print the header
	system("CLS");
	std::cout << std::endl << " --- BLACKJACK ---" << std::endl << std::endl;
}

void PlayerSetup(Player& _player, bool& _acesHigh)
{
	// Set the player's name
	PrintHeader();
	std::cout << " Enter your name: ";
	std::cin >> _player.m_name;

	// Ask if the player wants to play with aces high
	char choice;
	bool valid = false;
	while (valid == false)
	{
		PrintHeader();
		std::cout << " Aces high? (y/n): ";
		std::cin >> choice;
		if ((choice == 'Y') || (choice == 'y'))
		{
			_acesHigh = true;
			valid = true;
		}
		if ((choice == 'N') || (choice == 'n'))
		{
			_acesHigh = false;
			valid = true;
		}
	}
}

void InitialiseDeck(Card _deck[], bool _acesHigh)
{
	// Loop through each suit
	for (int suitID = 0; suitID < 4; suitID++)
	{
		char thisSuit[9];
		switch (suitID)
		{
		case 0: strcpy_s(thisSuit, "Spades"); break;
		case 1: strcpy_s(thisSuit, "Hearts"); break;
		case 2: strcpy_s(thisSuit, "Diamonds"); break;
		case 3: strcpy_s(thisSuit, "Clubs"); break;
		}

		// Create all 13 cards
		for (int nameID = 1; nameID <= 13; nameID++)
		{
			char thisName[6];
			int thisValue = nameID;
			switch (nameID)
			{
				case 1:
				{
					strcpy_s(thisName, "Ace");
					// Set aces high or low
					if (_acesHigh == true)
					{
						thisValue = 11;
					}
					else
					{
						thisValue = 1;
					}
					break;
				}
				case 2: { strcpy_s(thisName, "Two"); break; }
				case 3: { strcpy_s(thisName, "Three"); break; }
				case 4: { strcpy_s(thisName, "Four"); break; }
				case 5: { strcpy_s(thisName, "Five"); break; }
				case 6: { strcpy_s(thisName, "Six"); break; }
				case 7: { strcpy_s(thisName, "Seven"); break; }
				case 8: { strcpy_s(thisName, "Eight"); break; }
				case 9: { strcpy_s(thisName, "Nine"); break; }
				case 10: { strcpy_s(thisName, "Ten"); break; }
				// Face cards' values are manually assigned
				case 11:
				{
					strcpy_s(thisName, "Jack");
					thisValue = 10;
					break;
				}
				case 12:
				{
					strcpy_s(thisName, "Queen");
					thisValue = 10;
					break;
				}
				case 13:
				{
					strcpy_s(thisName, "King");
					thisValue = 10;
					break;
				}
			}

			// Put this card in the deck (0 to 51)
			int thisCard = suitID * 13 + nameID - 1;
			strcpy_s(_deck[thisCard].m_name, thisName);
			strcpy_s(_deck[thisCard].m_suit, thisSuit);
			_deck[thisCard].m_value = thisValue;
		}
	}
}

void ShuffleDeck(Card _deck[])
{
	srand(time(NULL));
	// Create a temporary slot to hold the card being shuffled
	char holdName[6];
	char holdSuit[9];
	int holdValue;

	// Loop through each card in the deck
	for (int shuffle1 = 0; shuffle1 < 52; shuffle1++)
	{
		// Randomly select a second card to swap with
		int shuffle2 = rand() % 52;

		// Hold the first card
		strcpy_s(holdName, _deck[shuffle1].m_name);
		strcpy_s(holdSuit, _deck[shuffle1].m_suit);
		holdValue = _deck[shuffle1].m_value;

		// Replace the first card with the second card
		strcpy_s(_deck[shuffle1].m_name, _deck[shuffle2].m_name);
		strcpy_s(_deck[shuffle1].m_suit, _deck[shuffle2].m_suit);
		_deck[shuffle1].m_value = _deck[shuffle2].m_value;

		// Replace the second card with the held first card
		strcpy_s(_deck[shuffle2].m_name, holdName);
		strcpy_s(_deck[shuffle2].m_suit, holdSuit);
		_deck[shuffle2].m_value = holdValue;
	}
}

void NewRound(Player& _player, Player& _dealer, Card _deck[], int& _nextCard)
{
	PlaceBet(_player);

	// Deal player's hand
	ClearHand(_player);
	DealCard(_player, _deck, _nextCard);
	DealCard(_player, _deck, _nextCard);

	// Deal dealer's hand
	ClearHand(_dealer);
	DealCard(_dealer, _deck, _nextCard);
	DealCard(_dealer, _deck, _nextCard);

	// Set to player's turn
	_player.m_turn = true;
}

void PlaceBet(Player& _player)
{
	// Get a valid bet
	bool valid = false;
	while (valid == false)
	{
		PrintHeader();
		std::cout << " Bank: " << _player.m_bank << std::endl << std::endl << " How much do you want to bet?: ";
		std::cin >> _player.m_bet;
		// Bet must be affordable
		if (_player.m_bet > _player.m_bank)
		{
			std::cout << std::endl << " You cannot afford that bet!" << std::endl << std::endl << " ";
			system("PAUSE");
		}
		// Bet must not be lower than 1
		else if (_player.m_bet < 1)
		{
			std::cout << std::endl << " You cannot bet nothing!" << std::endl << std::endl << " ";
			system("PAUSE");
		}
		else
		{
			valid = true;
		}
	}
	// When a valid bet is entered, take it out of the bank amount
	_player.m_bank -= _player.m_bet;
}

void ClearHand(Player& _thisPlayer)
{
	// Clear each slot in this player's hand
	for (int i = 0; i < 12; i++)
	{
		strcpy_s(_thisPlayer.m_hand[i].m_name, "");
		strcpy_s(_thisPlayer.m_hand[i].m_suit, "");
		_thisPlayer.m_hand[i].m_value = 0;
	}
	// No cards in hand
	_thisPlayer.m_cardsInHand = 0;
}

void DealCard(Player& _thisPlayer, Card _deck[], int& _nextCard)
{
	// Next card in the deck is assigned to the next slot in this player's hand
	_thisPlayer.m_hand[_thisPlayer.m_cardsInHand] = _deck[_nextCard];
	_thisPlayer.m_cardsInHand++;
	// Update hand value
	_thisPlayer.m_handValue = CalculateHandValue(_thisPlayer);

	// Point to the next card in the deck
	_nextCard++;
	// If the deck is out of cards, re-shuffle
	if (_nextCard > 51)
	{
		ShuffleDeck(_deck);
		_nextCard = 0;
	}
}

int CalculateHandValue(Player _thisPlayer)
{
	int handValue = 0;
	// Loop through the hand, totalling the values
	for (int i = 0; i < _thisPlayer.m_cardsInHand; i++)
	{
		handValue += _thisPlayer.m_hand[i].m_value;
	}
	return handValue;
}

void PrintBank(Player _player)
{
	// Display current bank and bet values
	std::cout << " Bank: $" << _player.m_bank << " - Bet: $" << _player.m_bet << std::endl << std::endl;
}

void PrintDealersHand(Player _dealer)
{
	std::cout << " - " << _dealer.m_name << " -" << std::endl;
	if (_dealer.m_turn == true)
	{
		// If it's the dealer's turn, reveal the hole card and hand value
		std::cout << " Hand value: " << _dealer.m_handValue << std::endl << std::endl;
		std::cout << " " << _dealer.m_hand[0].m_name << " of " << _dealer.m_hand[0].m_suit << std::endl;
	}
	else
	{
		// If it's the player's turn, hide the hole card and hand value
		std::cout << " Hand value: ?" << std::endl << std::endl;
		std::cout << " Hole card" << std::endl;
	}

	// Display the rest of the cards in the dealer's hand
	for (int i = 1; i < _dealer.m_cardsInHand; i++)
	{
		std::cout << " " << _dealer.m_hand[i].m_name << " of " << _dealer.m_hand[i].m_suit << std::endl;
	}
	std::cout << std::endl;
}

void PrintPlayersHand(Player _player)
{
	// Display player name and hand value
	std::cout << " - " << _player.m_name << " -" << std::endl;
	std::cout << " Hand value: " << _player.m_handValue << std::endl << std::endl;

	// Display the player's hand
	for (int i = 0; i < _player.m_cardsInHand; i++)
	{
		std::cout << " " << _player.m_hand[i].m_name << " of " << _player.m_hand[i].m_suit << std::endl;
	}
	std::cout << std::endl;
}

void Display(Player _player, Player _dealer)
{
	// Print each part of the interface
	PrintHeader();
	PrintBank(_player);
	PrintDealersHand(_dealer);
	PrintPlayersHand(_player);
}

void PlayersTurn(Player& _player, Player _dealer, Card _deck[], int& _nextCard)
{
	// As long as the player isn't bust or standing, loop the choice prompt
	while (_player.m_turn == true)
	{
		// Refresh the display each time
		Display(_player, _dealer);
		char choice = GetChoice();
		if (choice == '1')
		{
			_player.m_turn = Hit(_player, _deck, _nextCard);
		}
		if (choice == '2')
		{
			_player.m_turn = false;
		}
	}
}

char GetChoice()
{
	// Prompt the user to hit or stand
	std::cout << " [1] Hit" << std::endl << " [2] Stand" << std::endl << std::endl;
	char choice;
	bool valid = false;
	while (valid == false)
	{
		std::cout << " Choice: ";
		std::cin >> choice;
		if ((choice == '1') || (choice == '2'))
		{
			valid = true;
		}
	}
	return choice;
}

bool Hit(Player& _player, Card _deck[], int& _nextCard)
{
	// Deal the player a card
	DealCard(_player, _deck, _nextCard);

	// If they bust, return false for the player's turn loop, ending their turn and losing their bet
	if (_player.m_handValue > 21)
	{
		return false;
	}
	else
	{
		return true;
	}
}

void DealersTurn(Player& _player, Player& _dealer, Card _deck[], int& _nextCard)
{
	// It is now the dealer's turn
	_dealer.m_turn = true;
	std::cout << " You stand. Dealer's turn..." << std::endl << std::endl << " ";
	system("PAUSE");
	
	RevealHoleCard(_player, _dealer);

	// Dealer's turn logic
	while (_dealer.m_turn == true)
	{
		if (_dealer.m_handValue < 17)
		{
			// If the dealer's hand value is less than 17, they will hit
			_dealer.m_turn = DealerHit(_player, _dealer, _deck, _nextCard);
		}
		else
		{
			// Otherwise, they will stand, and the winner is determined
			Display(_player, _dealer);
			std::cout << " The dealer stands." << std::endl << std::endl;
			ResolveRound(_player, _dealer);
			_dealer.m_turn = false;
		}
	}
}

void RevealHoleCard(Player _player, Player& _dealer)
{
	// Refresh display on the dealer's turn, revealing the hole card
	Display(_player, _dealer);
	if (_dealer.m_handValue == 21)
	{
		// If the dealer has blackjack, they end their turn
		std::cout << " The dealer reveals the hole card. Blackjack!" << std::endl << std::endl;
		ResolveRound(_player, _dealer);
		_dealer.m_turn = false;
	}
	else
	{
		// Otherwise, they take their turn as normal
		std::cout << " The dealer reveals the hole card." << std::endl << std::endl << " ";
		system("PAUSE");
	}
}

bool DealerHit(Player& _player, Player& _dealer, Card _deck[], int& _nextCard)
{
	// Deal a card and update the display
	DealCard(_dealer, _deck, _nextCard);
	Display(_player, _dealer);

	if (_dealer.m_handValue > 21)
	{
		// If the dealer busts, the player wins double their bet
		int winnings = _player.m_bet * 2;
		_player.m_bank += winnings;
		std::cout << " The dealer busts! You won $" << winnings << "." << std::endl << std::endl;
		return false;
	}
	else
	{
		// Otherwise, the round continues
		std::cout << " The dealer hits." << std::endl << std::endl << " ";
		system("PAUSE");
		return true;
	}
}

void ResolveRound(Player& _player, Player _dealer)
{
	if (_player.m_handValue > _dealer.m_handValue)
	{
		// If the player has the higher hand value, they win double their bet
		int winnings = _player.m_bet * 2;
		_player.m_bank += winnings;
		std::cout << " You win the round! You won $" << winnings << "." << std::endl << std::endl;
	}
	else if (_player.m_handValue < _dealer.m_handValue)
	{
		// If the dealer has the higher hand value, the player loses their bet
		std::cout << " You lose the round. You lost your bet of $" << _player.m_bet << "." << std::endl << std::endl;
	}
	else
	{
		// If the hand values are tied, return the player's bet
		_player.m_bank += _player.m_bet;
		std::cout << " The round is tied. Your bet of $" << _player.m_bet << " is returned." << std::endl << std::endl;
	}
}

bool PlayAgain()
{
	char playAgain;
	// Loop until y or n are entered
	while (1)
	{
		std::cout << " Play again? (y/n): ";
		std::cin >> playAgain;
		if ((playAgain == 'Y') || (playAgain == 'y'))
		{
			return true;
		}
		if ((playAgain == 'N') || (playAgain == 'n'))
		{
			return false;
		}
	}
}