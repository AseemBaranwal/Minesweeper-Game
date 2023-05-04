#include "gameboard.h"
//Window Handling
Game::Game()
{
	int wide = 40, tall = 30, mineCount = 100;
	std::ifstream file("files/board_config.cfg");
	if (file.is_open())
	{
		try {
			string num;
			std::getline(file, num);
			wide = stoi(num);
			if (wide < 22) wide = 22; //Minimum for buttons to fit.
			if (wide < 22) wide = 22; //Minimum for buttons to fit.
			if (wide > 255) wide = 255; //Window breaks at 8k pixels.
			std::getline(file, num);
			tall = stoi(num);
			if (tall < 16) tall = 16;
			if (tall > 250) tall = 250;
			std::getline(file, num);
			mineCount = stoi(num);
			if (mineCount > wide * tall) mineCount = wide * tall;
		}
		catch (std::invalid_argument) 
		{
			std::cout << "Values in Config are not ints." << std::endl;
		}
	}
	file.close();

	//readConfig(); //For info.
	sf::RenderWindow welcome(sf::VideoMode(32 * wide, 32 * tall + 100), "Minesweeper");
	welcome.setFramerateLimit(60);
	sf::Font font;
	font.loadFromFile("files/font.ttf");

	sf::Text greet = mkmidTxt("WELCOME TO MINESWEEPER!", 24, 32 * wide, 32 * tall / 2.0f - 150, font, true);
	sf::Text ask = mkmidTxt("Enter your name:", 20, 32 * wide, 32 * tall / 2.0f - 75, font);
	sf::Text name = mkmidTxt("", 18, 32 * wide, 32 * tall / 2.0f - 45, font);
	name.setFillColor(sf::Color::Yellow);
	std::string userName = "";

	while (welcome.isOpen())
	{
		sf::Event event;
		while (welcome.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				welcome.close();

			if (event.type == sf::Event::TextEntered && userName.size() < 10)
			{
				char letter = '0';
				if (event.text.unicode < 128) //https://www.sfml-dev.org/tutorials/2.1/window-events.php
					letter = static_cast<char>(event.text.unicode);

				if (isalpha(letter)) {
					if (userName.length() == 0) userName += toupper(letter);
					else  userName += tolower(letter);
				}
					
			}

			if (event.type == sf::Event::KeyPressed && userName.size() > 0)
			{
				if (event.key.code == sf::Keyboard::Backspace)
					userName.pop_back();

				else if (event.key.code == sf::Keyboard::Enter) {
					welcome.close();
					Field(userName, wide, tall, mineCount);
				}
			}
		}

		welcome.clear(sf::Color(0, 0, 255));
		welcome.draw(greet);
		welcome.draw(ask);
		name.setString(userName + "|");
		cntTxtPos(name, 32 * wide, 32 * tall / 2.0f - 45);
		welcome.draw(name);
		welcome.display();
	}
}

void Game::Field(const std::string& userName, const int& wide, const int& tall, const int& mineCount)
{
	sf::RenderWindow game(sf::VideoMode(32 * wide, 32 * tall + 100), "Minesweeper");
	game.setFramerateLimit(60);
	sf::RenderWindow ldrBrd;
	ldrBrd.setFramerateLimit(60);
	sf::Font font;
	font.loadFromFile("files/font.ttf");
	sf::Text leader = mkmidTxt("LEADERBOARD", 20, 16 * wide, 16 * tall / 2.0f - 120, font, true);
	sf::Text bestTimes = mkmidTxt("", 18, 16 * wide, 16 * tall / 2.0f + 20, font);
	Board board(userName, wide, tall, mineCount, ldrBrd, bestTimes);

	while (game.isOpen())
	{
		sf::Event event;
		while (game.pollEvent(event) && !ldrBrd.isOpen())
		{
			if (event.type == sf::Event::Closed) game.close();
			if (event.type == sf::Event::MouseButtonPressed) //Don't take mouse input when leaderboard is open.
				if (event.mouseButton.button == sf::Mouse::Left)
					board.btnClk(game.mapPixelToCoords(sf::Mouse::getPosition(game))); //Convert Pixel to World Cords]
				else if (event.mouseButton.button == sf::Mouse::Right)
					board.flagClk(game.mapPixelToCoords(sf::Mouse::getPosition(game)));
		}

		game.clear(sf::Color(255, 255, 255));
		board.updateTime();
		board.drawTiles(game);
		game.display();

		if (ldrBrd.isOpen())
		{
			while (ldrBrd.pollEvent(event))
			{
				if (event.type == sf::Event::Closed) {
					board.updateTime(std::chrono::system_clock::to_time_t(std::chrono::system_clock::now())); //So time doesn't 'skip' when unpausing.
					board.toggleHide();
					ldrBrd.close();
				}
			}

			ldrBrd.clear(sf::Color(0, 0, 255));
			ldrBrd.draw(leader);
			ldrBrd.draw(bestTimes);
			ldrBrd.display();
		}
	}
}

sf::Text Game::mkmidTxt(const std::string& txt, const int& size, const float& x, const float& y, const sf::Font& font, bool isUL)
{
	sf::Text text;
	text.setFont(font);
	text.setString(txt);
	text.setCharacterSize(size);
	if (isUL) text.setStyle(sf::Text::Bold | sf::Text::Underlined);
	else text.setStyle(sf::Text::Bold);
	//sf::FloatRect box = text.getLocalBounds();
	//text.setOrigin(box.left + box.width / 2.0f, box.top + box.height / 2.0f);
	cntTxtPos(text, x, y);

	return text;
}

void Game::cntTxtPos(sf::Text& words, const float& x, const float& y)
{
	words.setOrigin(words.getLocalBounds().width / 2, words.getLocalBounds().height / 2);
	words.setPosition(x / 2.0f, y);
}