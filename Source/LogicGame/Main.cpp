#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <stdlib.h>
#include <Windows.h>
#include <algorithm>
#include <vector>
#include <iostream>
#include <random>

using namespace std; 
using namespace sf; 

int widthX = VideoMode::getDesktopMode().width;
int heightY = VideoMode::getDesktopMode().height;

RenderWindow window(VideoMode(widthX, heightY), "Booleo", Style::Fullscreen);
RectangleShape background(Vector2f(widthX, heightY));


const Vector2f cardSize(widthX/16, heightY/6.75);
struct Card
{
    Card() 
	{
        card.setSize(cardSize);
		card.setOrigin(Vector2f(cardSize.x/2, cardSize.y/2));
    }
    RectangleShape card;
    bool selected = false, result = false, locked = false;
	Vector2f lastPos;
	Texture texture;
	string cardValue = "";
	bool Operation(bool a, bool b, string op)
	{
		if (op == "or") { return a | b; }
		if (op == "and") { return a & b; }
		return a ^ b; 
	}
};

struct Slot
{
	Slot()
	{
		slot.setSize(cardSize);
	}
	Card* currentCard = nullptr;
	RectangleShape slot;
	Texture texture;
	int index = 0, level = 0, cardsIn = 0;
	bool pHand = false, full = false, locked = false, deck = false, discard = false;

};

void mainMenu();
////////////////////////////////////////PVP/////////////////////////////////////////////////////////
void PVP()
{
	int seed = time(NULL);
	srand(seed);
	bool playerTurn = 0;
	Texture BGtexture;
	BGtexture.loadFromFile("../../Images/BGtexture.png");
	BGtexture.setSmooth(true);
	background.setTexture(&BGtexture);

	RectangleShape BackButton(Vector2f(widthX / 20, heightY / 22));
	BackButton.setFillColor(Color::Red);
	BackButton.setPosition(Vector2f(widthX / 50, heightY / 40));

	Texture backButtonTexture;
	backButtonTexture.loadFromFile("../../Images/backButton.png");
	backButtonTexture.setSmooth(1);
	BackButton.setTexture(&backButtonTexture);

	RectangleShape TTButton(Vector2f(widthX / 20, heightY / 22));
	TTButton.setFillColor(Color::White);
	TTButton.setPosition(Vector2f(widthX-100, 20));

	Texture TTButtonTexture;
	TTButtonTexture.loadFromFile("../../Images/helpButton.png");
	TTButtonTexture.setSmooth(1);
	TTButton.setTexture(&TTButtonTexture);

	RectangleShape TruthTable(Vector2f(widthX / 5, heightY / 10));
	TruthTable.setFillColor(Color::White);
	TruthTable.setPosition(widthX+100, heightY / 2);
	TruthTable.setOrigin(Vector2f(widthX / 10, heightY / 20));


	Texture TruthTableTexture;
	TruthTableTexture.loadFromFile("../../Images/truthTable.png");
	TruthTable.setTexture(&TruthTableTexture);

	Font font;
	font.loadFromFile("Fonts/arial.ttf");
	Text playerTurnT;
	playerTurnT.setFont(font);
	playerTurnT.setCharacterSize(widthX/30);
	playerTurnT.setFillColor(Color::Black);
	playerTurnT.setPosition(Vector2f(widthX/ 1.37, heightY/ 1.1));
	playerTurnT.setString("Player One's turn");
	
	//Cards vector
	Card* currentCard = nullptr;
	vector<vector<Card>> cards;
	cards.push_back({});
	cards.push_back({});
	bool dragging = false;
	//Slots vector
	vector<vector<Slot>> slots;
	slots.push_back({});
	slots.push_back({});
	float startW = widthX / 2 - (3.0 * cardSize.x + 2.5 * widthX / 113);
	int lock = 0, level = 0;

	//Generate slots
	auto deck = new Slot;
	deck->cardsIn = 48;
	deck->deck = 1;
	deck->locked = 1;
	deck->slot.setPosition(Vector2f(widthX - 100.f - cardSize.x / 2, heightY / 2 - cardSize.y / 2));
	slots[playerTurn].push_back(*deck);
	slots[!playerTurn].push_back(*deck);
	for (int i = 0; i < 6; i++)
	{
		auto card = new Card;
		card->result = rand() % 2;
		card->texture.loadFromFile("../../Images/1-0Card.png");
		card->texture.setSmooth(1);
		card->locked = true;
		card->card.setTexture(&card->texture);
		if (!card->result)
		{
			card->card.setRotation(180);
		}
		cards[playerTurn].push_back(*card);
		cards[!playerTurn].push_back(*card);
	}
	for (float i = 0, Y = heightY / 63.5; i < 7; i++, Y += cardSize.y + heightY / 63.5)
	{
		for (float j = 0, X = startW; j < 6 - i; j++, X += cardSize.x + widthX / 113, lock++)
		{
			auto slot = new Slot;
			slot->slot.setPosition(Vector2f(X, Y));
			slot->index = int(i);

			if (lock < 6)
			{
				slot->locked = true;
				slot->currentCard = &cards[playerTurn][lock];
				slot->currentCard->card.setPosition(slot->slot.getPosition().x + cardSize.x / 2, slot->slot.getPosition().y + cardSize.y / 2);
				slot->currentCard->lastPos = slot->slot.getPosition();
				slot->currentCard->locked = true;
				slot->full = true;
			}
			slot->level = level;
			slots[playerTurn].push_back(*slot);
			if (lock < 6)
			{
				slot->locked = true;
				slot->currentCard = &cards[!playerTurn][lock];
				slot->currentCard->card.setPosition(slot->slot.getPosition().x + cardSize.x / 2, slot->slot.getPosition().y + cardSize.y / 2);
				slot->currentCard->lastPos = slot->slot.getPosition();
				(slot->currentCard->card.getRotation()==0) ? slot->currentCard->card.setRotation(180) : slot->currentCard->card.setRotation(0);
				slot->currentCard->locked = true;
				slot->full = true;
			}
			slots[!playerTurn].push_back(*slot);
		}
		level++;
		startW += (cardSize.x + widthX / 113) / 2;
	}
	auto discard = new Slot;
	discard->cardsIn = 0;
	discard->discard = 1;
	discard->slot.setPosition(Vector2f(widthX - 100 - cardSize.x / 2, heightY / 2 - cardSize.y / 2+cardSize.y+ heightY / 63.5));
	slots[playerTurn].push_back(*discard);
	slots[!playerTurn].push_back(*discard);
	// Generate cards
	vector<string> typesOfCards = { "1or", "0or", "1and", "0and", "1xor", "0xor" };
	int count[6] = {0,0,0,0,0,0}, cardRandomiser = 6, curCount, i = 0;
	string temp;
	int index = 0;
	while (i < 48) {
		auto card = new Card;
		card->card.setPosition(Vector2f(widthX - 100.f, heightY / 2));
		card->lastPos = Vector2f(card->card.getPosition().x - cardSize.x/2-1, card->card.getPosition().y - cardSize.y / 2-1);

		int r = rand() % cardRandomiser;
		temp = typesOfCards[r];

		if (temp == "1or") {
			curCount = 0;
			if (count[curCount] < 8){count[curCount]++; card->cardValue = temp;}
		}
		else if (temp == "0or") {
			curCount = 1;
			if (count[curCount] < 8) { count[curCount]++; card->cardValue = temp; }
		}
		else if (temp == "1and") {
			curCount = 2;
			if (count[curCount] < 8) { count[curCount]++; card->cardValue = temp; }
		}
		else if (temp == "0and") {
			curCount = 3;
			if (count[curCount] < 8) { count[curCount]++; card->cardValue = temp; }
		}
		else if (temp == "1xor") {
			curCount = 4;
			if (count[curCount] < 8) { count[curCount]++; card->cardValue = temp; }
		}
		else {
			curCount = 5;
			if (count[curCount] < 8) { count[curCount]++; card->cardValue = temp; }
		}

		(card->cardValue != "") ? deck->currentCard = card : 0;
		if (card->cardValue.size() != 0)
		{
			cards[playerTurn].push_back(*card);
			cards[!playerTurn].push_back(*card);
			i++;
		}
	}
	for (int i = 6; i < cards[playerTurn].size(); i++)
	{
		cards[playerTurn][i].result = (cards[playerTurn][i].cardValue.substr(0, 1) == '1');
		cards[playerTurn][i].texture.loadFromFile("../../Images/" + cards[playerTurn][i].cardValue + ".png");
		cards[playerTurn][i].texture.setSmooth(1);
		cards[playerTurn][i].card.setTexture(&cards[playerTurn][i].texture);
		cards[!playerTurn][i].result = (cards[playerTurn][i].cardValue.substr(0, 1) == '1');
		cards[!playerTurn][i].texture.loadFromFile("../../Images/" + cards[playerTurn][i].cardValue + ".png");
		cards[!playerTurn][i].texture.setSmooth(1);
		cards[!playerTurn][i].card.setTexture(&cards[playerTurn][i].texture);
	}
	default_random_engine eng(seed);
	shuffle(cards[playerTurn].begin() + 6, cards[playerTurn].end(), eng);

	for (float i = 0, Y = heightY - (heightY / 63.5 + cardSize.y); i < 5; i++, Y-=cardSize.y + heightY/63.5)
	{
		auto hand = new Slot;
		hand->pHand = 1;
		hand->slot.setPosition(widthX/113,Y);
		slots[playerTurn].push_back(*hand);
		slots[!playerTurn].push_back(*hand);
	}
	for (int i = 0; i < slots[playerTurn].size(); i++)
	{
		slots[playerTurn][i].texture.loadFromFile("../../Images/BGslot.png");
		slots[!playerTurn][i].texture.loadFromFile("../../Images/BGslot.png");
		slots[playerTurn][i].slot.setTexture(&slots[playerTurn][i].texture);
		slots[!playerTurn][i].slot.setTexture(&slots[!playerTurn][i].texture);
	}
	int collected[8] = { 0, 0, 0, 0, 0, 0, 0 };
	for (int i = 0; i < 4; i++)
	{
		int r = rand() % 41 + 6;
		for (int s = 0; s < 8; s++)
		{
			for (int h = 0; h < 8; h++)
			{
				while (r == collected[h]) {
					r = rand() % 41 + 6;
				}
			}
		}
		collected[i] = r;
		slots[playerTurn][i + 23].currentCard = &cards[playerTurn][r];
		slots[playerTurn][i + 23].full = 1;
		slots[playerTurn][i + 23].cardsIn = 1;
		slots[playerTurn][i + 23].currentCard->card.setPosition(slots[playerTurn][i + 23].slot.getPosition().x + cardSize.x / 2,
																slots[playerTurn][i + 23].slot.getPosition().y + cardSize.y / 2);
		slots[playerTurn][i + 23].currentCard->lastPos = Vector2f(slots[playerTurn][i + 23].slot.getPosition().x,
																  slots[playerTurn][i + 23].slot.getPosition().y);
		r = rand() % 41 + 6;
		for (int s = 0; s < 8; s++)
		{
			for (int h = 0; h < 8; h++)
			{
				while (r == collected[h]) {
					r = rand() % 41 + 6;
				}
			}
		}
		collected[i+4] = r;
		slots[!playerTurn][i + 23].currentCard = &cards[!playerTurn][r];
		slots[!playerTurn][i + 23].full = 1;
		slots[!playerTurn][i + 23].cardsIn = 1;
		slots[!playerTurn][i + 23].currentCard->card.setPosition(slots[!playerTurn][i + 23].slot.getPosition().x + cardSize.x / 2,
																 slots[!playerTurn][i + 23].slot.getPosition().y + cardSize.y / 2);
		slots[!playerTurn][i + 23].currentCard->lastPos = Vector2f(slots[!playerTurn][i + 23].slot.getPosition().x,
																   slots[!playerTurn][i + 23].slot.getPosition().y);
		
	}
	while (window.isOpen())
	{
		Event event;
		while (window.pollEvent(event)){}
		if (Keyboard::isKeyPressed(Keyboard::Escape)) {mainMenu(); }

		auto mpos = window.mapPixelToCoords(Mouse::getPosition(window));
		if (Mouse::isButtonPressed(Mouse::Left) && !dragging && currentCard == nullptr)
		{
			dragging = true;
			for (auto& it : slots[playerTurn]) {
				if (it.slot.getGlobalBounds().contains(mpos) && it.full && !it.currentCard->locked)
				{
					it.full = false;
					it.cardsIn--;
					currentCard = it.currentCard;
					it.currentCard = nullptr;
					break;
				}
			}
			for (auto& it : cards[playerTurn]) {
				if (it.card.getGlobalBounds().contains(mpos) && !it.locked){
					it.selected = true;
					currentCard = &it;
					break;
				}
			}
		}
		else if (!(Mouse::isButtonPressed(Mouse::Left))) {

			if (dragging && currentCard != nullptr)
			{
				for (int i = 0; i < slots[playerTurn].size(); i++)
				{
					if (slots[playerTurn][i].slot.getGlobalBounds().contains(mpos) && !slots[playerTurn][i].full && !slots[playerTurn][i].locked)
					{
						currentCard->card.setPosition(slots[playerTurn][i].slot.getPosition().x + cardSize.x / 2,
													  slots[playerTurn][i].slot.getPosition().y + cardSize.y / 2);
						slots[playerTurn][i].cardsIn++;
						if (!slots[playerTurn][i].deck && !slots[playerTurn][i].discard)
						{
							slots[playerTurn][i].full = 1;
						}
						else if (slots[playerTurn][i].cardsIn == 48)
						{
							slots[playerTurn][i].full = 1;
						}
						if (!slots[playerTurn][i].pHand)
						{
							currentCard->locked = 1;
							for (int l = 0; l < cards[playerTurn].size(); l++)
							{
								cards[playerTurn][l].card.setFillColor(Color::Transparent);
							}
							(playerTurn) ? playerTurnT.setString("Player One's turn") : playerTurnT.setString("Player Two's turn");
							playerTurn = !playerTurn;
							for (int l = 0; l < cards[playerTurn].size(); l++)
							{
								cards[playerTurn][l].card.setFillColor(Color::White);
							}
						}
						slots[playerTurn][i].currentCard = currentCard;
						currentCard->lastPos = Vector2f(currentCard->card.getGlobalBounds().left,
							currentCard->card.getGlobalBounds().top);
						break;
						}
				}
				currentCard->card.setPosition(currentCard->lastPos.x + cardSize.x/2, currentCard->lastPos.y + cardSize.y/2);
			}
			dragging = false;
			if (currentCard)
			{
				currentCard->selected = false;
				currentCard = nullptr;
			}
		}

		if (dragging && currentCard != nullptr)
		{
			currentCard->card.setPosition(mpos);
		}
		if (Mouse::isButtonPressed(Mouse::Left) && BackButton.getGlobalBounds().contains(mpos))
		{
			mainMenu();

		}

		if (Mouse::isButtonPressed(Mouse::Left) && TTButton.getGlobalBounds().contains(mpos))
		{
			TruthTable.setPosition(widthX/2, heightY/2);
			
		}
		//sf::Event::MouseButtonReleased
		if (!Mouse::isButtonPressed(Mouse::Left) )
		{
			TruthTable.setPosition(widthX +200, heightY / 2);

		}

		window.clear();
		window.draw(background);
		window.draw(BackButton);
		window.draw(TTButton); 
		window.draw(playerTurnT);
		for (size_t i = 0; i < slots[playerTurn].size(); i++) {
			window.draw(slots[playerTurn][i].slot);
		}

		//Draw the cards
		for (auto i = cards[playerTurn].size() - 1; i != -1; i--) {
			window.draw(cards[playerTurn][i].card);
		}
		window.draw(TruthTable);
		window.display();
	}
}
////////////////////////////////////////PVC/////////////////////////////////////////////////////////

void PVC()
{
	int seed = time(NULL);
	srand(seed);
	bool playerTurn = 0;
	Texture BGtexture;
	BGtexture.loadFromFile("../../Images/BGtexture.png");
	BGtexture.setSmooth(true);
	background.setTexture(&BGtexture);

	RectangleShape BackButton(Vector2f(widthX / 20, heightY / 22));
	BackButton.setFillColor(Color::Red);
	BackButton.setPosition(Vector2f(widthX / 50, heightY / 40));

	Texture backButtonTexture;
	backButtonTexture.loadFromFile("../../Images/backButton.png");
	backButtonTexture.setSmooth(1);
	BackButton.setTexture(&backButtonTexture);

	RectangleShape TTButton(Vector2f(widthX / 20, heightY / 22));
	TTButton.setFillColor(Color::White);
	TTButton.setPosition(Vector2f(widthX - 100, 20));

	Texture TTButtonTexture;
	TTButtonTexture.loadFromFile("../../Images/helpButton.png");
	TTButtonTexture.setSmooth(1);
	TTButton.setTexture(&TTButtonTexture);

	RectangleShape TruthTable(Vector2f(widthX / 5, heightY / 10));
	TruthTable.setFillColor(Color::White);
	TruthTable.setPosition(widthX + 100, heightY / 2);
	TruthTable.setOrigin(Vector2f(widthX / 10, heightY / 20));


	Texture TruthTableTexture;
	TruthTableTexture.loadFromFile("../../Images/truthTable.png");
	TruthTable.setTexture(&TruthTableTexture);

	Font font;
	font.loadFromFile("Fonts/arial.ttf");
	Text playerTurnT;
	playerTurnT.setFont(font);
	playerTurnT.setCharacterSize(widthX / 30);
	playerTurnT.setFillColor(Color::Black);
	playerTurnT.setPosition(Vector2f(widthX / 1.37, heightY / 1.1));
	playerTurnT.setString("Player One's turn");

	//Cards vector
	Card* currentCard = nullptr;
	vector<vector<Card>> cards;
	cards.push_back({});
	cards.push_back({});
	bool dragging = false;
	//Slots vector
	vector<vector<Slot>> slots;
	slots.push_back({});
	slots.push_back({});
	float startW = widthX / 2 - (3.0 * cardSize.x + 2.5 * widthX / 113);
	int lock = 0, level = 0;

	//Generate slots
	auto deck = new Slot;
	deck->cardsIn = 48;
	deck->deck = 1;
	deck->locked = 1;
	deck->slot.setPosition(Vector2f(widthX - 100.f - cardSize.x / 2, heightY / 2 - cardSize.y / 2));
	slots[playerTurn].push_back(*deck);
	slots[!playerTurn].push_back(*deck);
	for (int i = 0; i < 6; i++)
	{
		auto card = new Card;
		card->result = rand() % 2;
		card->texture.loadFromFile("../../Images/1-0Card.png");
		card->texture.setSmooth(1);
		card->locked = true;
		card->card.setTexture(&card->texture);
		if (!card->result)
		{
			card->card.setRotation(180);
		}
		cards[playerTurn].push_back(*card);
		cards[!playerTurn].push_back(*card);
	}
	for (float i = 0, Y = heightY / 63.5; i < 7; i++, Y += cardSize.y + heightY / 63.5)
	{
		for (float j = 0, X = startW; j < 6 - i; j++, X += cardSize.x + widthX / 113, lock++)
		{
			auto slot = new Slot;
			slot->slot.setPosition(Vector2f(X, Y));
			slot->index = int(i);

			if (lock < 6)
			{
				slot->locked = true;
				slot->currentCard = &cards[playerTurn][lock];
				slot->currentCard->card.setPosition(slot->slot.getPosition().x + cardSize.x / 2, slot->slot.getPosition().y + cardSize.y / 2);
				slot->currentCard->lastPos = slot->slot.getPosition();
				slot->currentCard->locked = true;
				slot->full = true;
			}
			slot->level = level;
			slots[playerTurn].push_back(*slot);
			if (lock < 6)
			{
				slot->locked = true;
				slot->currentCard = &cards[!playerTurn][lock];
				slot->currentCard->card.setPosition(slot->slot.getPosition().x + cardSize.x / 2, slot->slot.getPosition().y + cardSize.y / 2);
				slot->currentCard->lastPos = slot->slot.getPosition();
				(slot->currentCard->card.getRotation() == 0) ? slot->currentCard->card.setRotation(180) : slot->currentCard->card.setRotation(0);
				slot->currentCard->locked = true;
				slot->full = true;
			}
			slots[!playerTurn].push_back(*slot);
		}
		level++;
		startW += (cardSize.x + widthX / 113) / 2;
	}
	auto discard = new Slot;
	discard->cardsIn = 0;
	discard->discard = 1;
	discard->slot.setPosition(Vector2f(widthX - 100 - cardSize.x / 2, heightY / 2 - cardSize.y / 2 + cardSize.y + heightY / 63.5));
	slots[playerTurn].push_back(*discard);
	slots[!playerTurn].push_back(*discard);
	// Generate cards
	vector<string> typesOfCards = { "1or", "0or", "1and", "0and", "1xor", "0xor" };
	int count[6] = { 0,0,0,0,0,0 }, cardRandomiser = 6, curCount, i = 0;
	string temp;
	int index = 0;
	while (i < 48) {
		auto card = new Card;
		card->card.setPosition(Vector2f(widthX - 100.f, heightY / 2));
		card->lastPos = Vector2f(card->card.getPosition().x - cardSize.x / 2 - 1, card->card.getPosition().y - cardSize.y / 2 - 1);

		int r = rand() % cardRandomiser;
		temp = typesOfCards[r];

		if (temp == "1or") {
			curCount = 0;
			if (count[curCount] < 8) { count[curCount]++; card->cardValue = temp; }
		}
		else if (temp == "0or") {
			curCount = 1;
			if (count[curCount] < 8) { count[curCount]++; card->cardValue = temp; }
		}
		else if (temp == "1and") {
			curCount = 2;
			if (count[curCount] < 8) { count[curCount]++; card->cardValue = temp; }
		}
		else if (temp == "0and") {
			curCount = 3;
			if (count[curCount] < 8) { count[curCount]++; card->cardValue = temp; }
		}
		else if (temp == "1xor") {
			curCount = 4;
			if (count[curCount] < 8) { count[curCount]++; card->cardValue = temp; }
		}
		else {
			curCount = 5;
			if (count[curCount] < 8) { count[curCount]++; card->cardValue = temp; }
		}

		(card->cardValue != "") ? deck->currentCard = card : 0;
		if (card->cardValue.size() != 0)
		{
			cards[playerTurn].push_back(*card);
			cards[!playerTurn].push_back(*card);
			i++;
		}
	}
	for (int i = 6; i < cards[playerTurn].size(); i++)
	{
		cards[playerTurn][i].result = (cards[playerTurn][i].cardValue.substr(0, 1) == '1');
		cards[playerTurn][i].texture.loadFromFile("../../Images/" + cards[playerTurn][i].cardValue + ".png");
		cards[playerTurn][i].texture.setSmooth(1);
		cards[playerTurn][i].card.setTexture(&cards[playerTurn][i].texture);
		cards[!playerTurn][i].result = (cards[playerTurn][i].cardValue.substr(0, 1) == '1');
		cards[!playerTurn][i].texture.loadFromFile("../../Images/" + cards[playerTurn][i].cardValue + ".png");
		cards[!playerTurn][i].texture.setSmooth(1);
		cards[!playerTurn][i].card.setTexture(&cards[playerTurn][i].texture);
	}
	default_random_engine eng(seed);
	shuffle(cards[playerTurn].begin() + 6, cards[playerTurn].end(), eng);

	for (float i = 0, Y = heightY - (heightY / 63.5 + cardSize.y); i < 5; i++, Y -= cardSize.y + heightY / 63.5)
	{
		auto hand = new Slot;
		hand->pHand = 1;
		hand->slot.setPosition(widthX / 113, Y);
		slots[playerTurn].push_back(*hand);
		slots[!playerTurn].push_back(*hand);
	}
	for (int i = 0; i < slots[playerTurn].size(); i++)
	{
		slots[playerTurn][i].texture.loadFromFile("../../Images/BGslot.png");
		slots[!playerTurn][i].texture.loadFromFile("../../Images/BGslot.png");
		slots[playerTurn][i].slot.setTexture(&slots[playerTurn][i].texture);
		slots[!playerTurn][i].slot.setTexture(&slots[!playerTurn][i].texture);
	}
	int collected[8] = { 0, 0, 0, 0, 0, 0, 0 };
	for (int i = 0; i < 4; i++)
	{
		int r = rand() % 41 + 6;
		for (int s = 0; s < 8; s++)
		{
			for (int h = 0; h < 8; h++)
			{
				while (r == collected[h]) {
					r = rand() % 41 + 6;
				}
			}
		}
		collected[i] = r;
		slots[playerTurn][i + 23].currentCard = &cards[playerTurn][r];
		slots[playerTurn][i + 23].full = 1;
		slots[playerTurn][i + 23].cardsIn = 1;
		slots[playerTurn][i + 23].currentCard->card.setPosition(slots[playerTurn][i + 23].slot.getPosition().x + cardSize.x / 2,
			slots[playerTurn][i + 23].slot.getPosition().y + cardSize.y / 2);
		slots[playerTurn][i + 23].currentCard->lastPos = Vector2f(slots[playerTurn][i + 23].slot.getPosition().x,
			slots[playerTurn][i + 23].slot.getPosition().y);
		r = rand() % 41 + 6;
		for (int s = 0; s < 8; s++)
		{
			for (int h = 0; h < 8; h++)
			{
				while (r == collected[h]) {
					r = rand() % 41 + 6;
				}
			}
		}
		collected[i + 4] = r;
		slots[!playerTurn][i + 23].currentCard = &cards[!playerTurn][r];
		slots[!playerTurn][i + 23].full = 1;
		slots[!playerTurn][i + 23].cardsIn = 1;
		slots[!playerTurn][i + 23].currentCard->card.setPosition(slots[!playerTurn][i + 23].slot.getPosition().x + cardSize.x / 2,
			slots[!playerTurn][i + 23].slot.getPosition().y + cardSize.y / 2);
		slots[!playerTurn][i + 23].currentCard->lastPos = Vector2f(slots[!playerTurn][i + 23].slot.getPosition().x,
			slots[!playerTurn][i + 23].slot.getPosition().y);

	}
	while (window.isOpen())
	{
		Event event;
		while (window.pollEvent(event)) {}
		if (Keyboard::isKeyPressed(Keyboard::Escape)) { mainMenu(); }

		auto mpos = window.mapPixelToCoords(Mouse::getPosition(window));
		if (Mouse::isButtonPressed(Mouse::Left) && !dragging && currentCard == nullptr)
		{
			dragging = true;
			for (auto& it : slots[playerTurn]) {
				if (it.slot.getGlobalBounds().contains(mpos) && it.full && !it.currentCard->locked)
				{
					it.full = false;
					it.cardsIn--;
					currentCard = it.currentCard;
					it.currentCard = nullptr;
					break;
				}
			}
			for (auto& it : cards[playerTurn]) {
				if (it.card.getGlobalBounds().contains(mpos) && !it.locked) {
					it.selected = true;
					currentCard = &it;
					break;
				}
			}
		}
		else if (!(Mouse::isButtonPressed(Mouse::Left))) {

			if (dragging && currentCard != nullptr)
			{
				for (int i = 0; i < slots[playerTurn].size(); i++)
				{
					if (slots[playerTurn][i].slot.getGlobalBounds().contains(mpos) && !slots[playerTurn][i].full && !slots[playerTurn][i].locked)
					{
						currentCard->card.setPosition(slots[playerTurn][i].slot.getPosition().x + cardSize.x / 2,
							slots[playerTurn][i].slot.getPosition().y + cardSize.y / 2);
						slots[playerTurn][i].cardsIn++;
						if (!slots[playerTurn][i].deck && !slots[playerTurn][i].discard)
						{
							slots[playerTurn][i].full = 1;
						}
						else if (slots[playerTurn][i].cardsIn == 48)
						{
							slots[playerTurn][i].full = 1;
						}
						if (!slots[playerTurn][i].pHand)
						{
							currentCard->locked = 1;
							for (int l = 0; l < cards[playerTurn].size(); l++)
							{
								cards[playerTurn][l].card.setFillColor(Color::Transparent);
							}
							(playerTurn) ? playerTurnT.setString("Player One's turn") : playerTurnT.setString("Player Two's turn");
							playerTurn = !playerTurn;
							for (int l = 0; l < cards[playerTurn].size(); l++)
							{
								cards[playerTurn][l].card.setFillColor(Color::White);
							}
						}
						slots[playerTurn][i].currentCard = currentCard;
						currentCard->lastPos = Vector2f(currentCard->card.getGlobalBounds().left,
							currentCard->card.getGlobalBounds().top);
						break;
					}
				}
				currentCard->card.setPosition(currentCard->lastPos.x + cardSize.x / 2, currentCard->lastPos.y + cardSize.y / 2);
			}
			dragging = false;
			if (currentCard)
			{
				currentCard->selected = false;
				currentCard = nullptr;
			}
		}

		if (dragging && currentCard != nullptr)
		{
			currentCard->card.setPosition(mpos);
		}
		if (Mouse::isButtonPressed(Mouse::Left) && BackButton.getGlobalBounds().contains(mpos))
		{
			mainMenu();

		}

		if (Mouse::isButtonPressed(Mouse::Left) && TTButton.getGlobalBounds().contains(mpos))
		{
			TruthTable.setPosition(widthX / 2, heightY / 2);

		}
		//sf::Event::MouseButtonReleased
		if (!Mouse::isButtonPressed(Mouse::Left))
		{
			TruthTable.setPosition(widthX + 200, heightY / 2);

		}

		window.clear();
		window.draw(background);
		window.draw(BackButton);
		window.draw(TTButton);
		window.draw(playerTurnT);
		for (size_t i = 0; i < slots[playerTurn].size(); i++) {
			window.draw(slots[playerTurn][i].slot);
		}

		//Draw the cards
		for (auto i = cards[playerTurn].size() - 1; i != -1; i--) {
			window.draw(cards[playerTurn][i].card);
		}
		window.draw(TruthTable);
		window.display();
	}
}

////////////////////////////////////////H2P/////////////////////////////////////////////////////////

void  H2P()
{
	RectangleShape content(Vector2f(widthX /2, heightY ));
	content.setFillColor(Color::White);
	content.setOrigin(Vector2f(widthX  /2/ 2, heightY / 2));
	content.setPosition(widthX / 2, heightY /2);
	content.setOutlineThickness(3.f);
	content.setOutlineColor(Color::Black);

	Texture h2pContent;
	h2pContent.loadFromFile("../../Images/h2pContent.png");
	h2pContent.setSmooth(1);

	content.setTexture(&h2pContent);
	content.setPosition(Vector2f(widthX / 2, heightY / 2));
	content.setOrigin(Vector2f(widthX / 4, heightY / 2));

	RectangleShape BackButton(Vector2f(widthX / 20, heightY / 22));
	BackButton.setFillColor(Color::Red);
	BackButton.setPosition(Vector2f(widthX / 50, heightY / 40));

	Texture backButtonTexture;
	backButtonTexture.loadFromFile("../../Images/backButton.png");
	backButtonTexture.setSmooth(1);
	BackButton.setTexture(&backButtonTexture);

	while (window.isOpen())
	{
		auto mpos = window.mapPixelToCoords(Mouse::getPosition(window));
		Event event;
		while (window.pollEvent(event))
		{
			if (Keyboard::isKeyPressed(Keyboard::Escape))
				mainMenu();
		}
		if (Mouse::isButtonPressed(Mouse::Left) && BackButton.getGlobalBounds().contains(mpos))
		{
			mainMenu();

		}
		window.clear();
		window.draw(background);
		window.draw(content);
		window.draw(BackButton);
		window.display();
	}


}
//////////////////////////////////////mainMenu//////////////////////////////////////////////////////
void mainMenu()
{
	Texture BGtexture;
	BGtexture.loadFromFile("../../Images/BGtexture.png");
	BGtexture.setSmooth(true);
	background.setTexture(&BGtexture);


	RectangleShape PVPbutton(Vector2f(widthX/8,heightY/10));
	RectangleShape PVCbutton(Vector2f(widthX / 8, heightY / 10));
	RectangleShape H2Pbutton(Vector2f(widthX / 10, heightY / 12));
	RectangleShape Exitbutton(Vector2f(widthX / 10, heightY / 12));
	Texture pvp, pvc, h2p, exit;


	pvp.loadFromFile("../../Images/pvpButton.png");
	pvc.loadFromFile("../../Images/pvcButton.png");
	h2p.loadFromFile("../../Images/h2pButton.png");
	exit.loadFromFile("../../Images/exitButton.png");
	pvp.setSmooth(1);
	pvc.setSmooth(1);
	h2p.setSmooth(1);
	exit.setSmooth(1);


	PVPbutton.setTexture(&pvp);
	PVPbutton.setPosition(Vector2f(widthX/2,heightY/2-150));
	PVPbutton.setOrigin(Vector2f(widthX / 8 / 2, heightY / 8 / 2));

	PVCbutton.setTexture(&pvc);
	PVCbutton.setPosition(Vector2f(widthX / 2, heightY / 2-35));
	PVCbutton.setOrigin(Vector2f(widthX / 8 / 2, heightY / 8 / 2));

	H2Pbutton.setTexture(&h2p);
	H2Pbutton.setPosition(Vector2f(widthX / 2, heightY / 2 +55));
	H2Pbutton.setOrigin(Vector2f(widthX / 10 / 2, heightY / 12 / 2));

	Exitbutton.setTexture(&exit);
	Exitbutton.setPosition(Vector2f(widthX / 2, heightY / 2 + 155));
	Exitbutton.setOrigin(Vector2f(widthX / 10 / 2, heightY / 12 / 2));

	while (window.isOpen())
	{
		Event event;
		while (window.pollEvent(event)){}
		Vector2f mpos = window.mapPixelToCoords(Mouse::getPosition(window));

		if (Mouse::isButtonPressed(Mouse::Left) && PVPbutton.getGlobalBounds().contains(mpos))
		{
			PVP();
		}
		if (Mouse::isButtonPressed(Mouse::Left) && PVCbutton.getGlobalBounds().contains(mpos))
		{
			PVC();
		}
		if (Mouse::isButtonPressed(Mouse::Left) && H2Pbutton.getGlobalBounds().contains(mpos))
		{
			H2P();
		}
		if (Mouse::isButtonPressed(Mouse::Left) && Exitbutton.getGlobalBounds().contains(mpos))
		{
			window.close(); 
		}

		window.clear();
		window.draw(background);
		
		window.draw(PVPbutton);
		window.draw(PVCbutton);
		window.draw(H2Pbutton); 
		window.draw(Exitbutton);

		window.display(); 
	}

}

int main()
{	
	Image icon;
	icon.loadFromFile("../../Images/icon.png");
	window.setIcon(icon.getSize().x, icon.getSize().y, icon.getPixelsPtr());
	mainMenu();
}