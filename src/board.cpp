#include "gameboard.h"

Game::Board::Board(const string& name, const int& wide, const int& tall, const int& total, sf::RenderWindow& ldrWin, sf::Text& bestTimes)
{
	this->width = wide;
	this->height = tall;
	this->mineCount = total;
	this->safeTiles = wide * tall - total;
	this->playerName = name;
	this->flagCount = 0;
	this->ldrWin = &ldrWin; //Window Object Itself
	this->winTxt = &bestTimes; //Vector to hold text for window.
	srand((unsigned)time(NULL) + width + height + mineCount + playerName.length());

	setTextures(txtList);
	std::map<int, sf::Texture> numList = { {1, one},{2, two},{3, three},{4, four},{5, five},{6, six},{7, seven},{8, eight} }; //Temporary 
	vector <std::pair<string, sf::Texture>> tmpList = { {"d0", d0},{"d1", d1},{"d2", d2},{"d3", d3},{"d4", d4},{"d5", d5},{"d6", d6},{"d7", d7},{"d8", d8},{"d9", d9},{"dn", dn} };
	setTextures(numList);
	setTextures(tmpList);
	buildBoard();
	mkButtons({
		{"face_happy",{(width / 2.0) * 32 - 32,32 * (height + 0.5f)}},
		{"debug",{width * 32 - 304,32 * (height + 0.5f)}},
		{"pause",{width * 32 - 240,32 * (height + 0.5f)}},
		{"leaderboard",{width * 32 - 176,32 * (height + 0.5f)}}
		});

	readLdr();
	updateLdr();
	updateCnt();
}

void Game::Board::buildBoard()
{
	//Reset
	gameBoard.clear();

	//Construct Data
	for (int i = 0; i < height; i++)
	{
		vector<Tile> newLine;

		for (int j = 0; j < width; j++)
		{
			Tile newTile(txtList.at("tile_hidden"), j, i);
			newLine.push_back(newTile);
		}

		gameBoard.push_back(newLine);
	}

	//Establish relationships.
	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			if (j + 1 != width) gameBoard.at(i).at(j).setPtr(&gameBoard.at(i).at(j + 1)); //To the Right
			if (i > 0) { //Row below exists. No rows above will exist yet.
				gameBoard.at(i).at(j).setPtr(&gameBoard.at(i - 1).at(j)); //Below
				if (j + 1 != width) gameBoard.at(i).at(j).setPtr(&gameBoard.at(i - 1).at(j + 1));	//Below and Right
				if (j > 0) gameBoard.at(i).at(j).setPtr(&gameBoard.at(i - 1).at(j - 1)); //Below and Left
			}
		}
	}

	addMines();
}

void Game::Board::addMines()
{
	int count = 0, randRow = 0, randCol = 0;
	while (count < mineCount)
	{
		//Random indexes.
		randRow = rand() % gameBoard.size();
		randCol = rand() % gameBoard[0].size();

		//Element has not already been chosen.
		if (gameBoard.at(randRow).at(randCol).setMine()) {
			count += 1;

			sf::Sprite mine;
			mineField.push_back(mine);
			mineField.back().setTexture(txtList.at("mine"));
			mineField.back().setPosition(gameBoard.at(randRow).at(randCol).getPos());
			mineTiles.push_back(&gameBoard.at(randRow).at(randCol));
		}
	}

	for (auto& line : gameBoard)
	{
		for (auto& tile : line)
		{
			tile.updateSafe();
		}
	}
}

void Game::Board::resetGame()
{
	mineField.clear();
	mineTiles.clear();
	if (!inDebug) showMines = false;
	gameOver = false;
	isPaused = false;
	flagCount = 0;
	revTiles = 0;
	timer = 0;
	updateTime(std::chrono::system_clock::to_time_t(std::chrono::system_clock::now()));
	buttons.at("clock").setSprite("timML", txtList.at("d0"));
	buttons.at("clock").setSprite("timMR", txtList.at("d0"));
	buttons.at("clock").setSprite("timSL", txtList.at("d0"));
	buttons.at("clock").setSprite("timSR", txtList.at("d0"));
	buttons.at("face_happy").setSprite("box", txtList.at("face_happy"));

	for (auto& line : gameBoard)
	{
		for (auto& tile : line)
		{
			tile.setSafe(txtList.at("tile_hidden"));
		}
	}

	updateCnt();
	addMines();
}

void Game::Board::setTextures(std::map<int, sf::Texture>& m)
{
	string filename = "";
	std::map<int, sf::Texture>::iterator iter;
	for (iter = m.begin(); iter != m.end(); iter++)
	{
		filename = "files/images/number_" + std::to_string(iter->first) + ".png";
		iter->second.loadFromFile(filename);
		txtList.insert({ std::to_string(iter->first), iter->second });
	}
}

void Game::Board::setTextures(std::map<string, sf::Texture>& m)
{
	string filename = "";
	std::map<string, sf::Texture>::iterator iter;
	for (iter = m.begin(); iter != m.end(); iter++)
	{
		filename = "files/images/" + iter->first + ".png";
		iter->second.loadFromFile(filename);
	}
}

void Game::Board::setTextures(vector <std::pair<string, sf::Texture>>& v)
{
	for (int i = 0; i < v.size(); i++)
	{
		v.at(i).second.loadFromFile("files/images/digits.png", sf::IntRect(21 * i, 0, 21, 32));
		txtList.insert({ v.at(i).first, v.at(i).second });
	}
}

void Game::Board::mkButtons(const vector<std::pair<string, std::pair<int, int>>>& v)
{
	for (auto& para : v)
	{
		Button but(txtList.at(para.first), para.second.first, para.second.second);
		buttons.emplace(para.first, but);
	}

	Button cnt(txtList.at("dn"), 12, 32 * (height + 0.5f) + 16);
	buttons.emplace("count", cnt);
	buttons.at("count").addSprite(txtList.at("d0"), "cntH", 33, 32 * (height + 0.5f) + 16);
	buttons.at("count").addSprite(txtList.at("d0"), "cntT", 54, 32 * (height + 0.5f) + 16);
	buttons.at("count").addSprite(txtList.at("d0"), "cntO", 75, 32 * (height + 0.5f) + 16);

	Button clk(txtList.at("d0"), 0, 0);
	buttons.emplace("clock", clk);
	buttons.at("clock").delSprite("box");
	buttons.at("clock").addSprite(txtList.at("d0"), "timML", width * 32 - 97, 32 * (height + 0.5f) + 16);
	buttons.at("clock").addSprite(txtList.at("d0"), "timMR", width * 32 - 76, 32 * (height + 0.5f) + 16);
	buttons.at("clock").addSprite(txtList.at("d0"), "timSL", width * 32 - 54, 32 * (height + 0.5f) + 16);
	buttons.at("clock").addSprite(txtList.at("d0"), "timSR", width * 32 - 33, 32 * (height + 0.5f) + 16);
}

void Game::Board::toggleHide(bool hide)
{
	for (auto& line : gameBoard)
	{
		for (auto& tile : line)
		{
			if (isPaused || hide) tile.setSprite("box", txtList.at("tile_revealed"));
			else if (!tile.isSeen()) tile.setSprite("box", txtList.at("tile_hidden"));
		}
	}
}

void Game::Board::updateCnt()
{
	int count = mineCount - flagCount;
	if (count >= 0 && cntNeg) { buttons.at("count").delSprite("box"); cntNeg = false; }
	else if (count < 0 && !cntNeg) { buttons.at("count").addSprite(txtList.at("dn"), "box", 12, 32 * (height + 0.5f) + 16); cntNeg = true; }

	count = std::abs(count);
	buttons.at("count").setSprite("cntH", txtList.at("d" + std::to_string(count / 100 % 10)));
	buttons.at("count").setSprite("cntT", txtList.at("d" + std::to_string(count / 10 % 10)));
	buttons.at("count").setSprite("cntO", txtList.at("d" + std::to_string(count % 10)));
}

void Game::Board::readLdr()
{
	std::ifstream file("files/leaderboard.txt");
	if (file.is_open())
	{
		string line;
		while (std::getline(file, line)) {
			ldrNames.push_back(line);
		}
	}
	std::sort(ldrNames.begin(), ldrNames.end());
	file.close();
}

void Game::Board::writeLdr()
{
	std::ofstream file;
	file.open("files/leaderboard.txt", std::ofstream::out | std::ofstream::trunc);
	if (file.is_open())
		for (auto& name : ldrNames) {
			file << name;
			if (name != ldrNames.at(ldrNames.size() - 1))
				file << "\n";
		}

	file.close();
	*recentWin += "*";
}

void Game::Board::addLdr()
{
	int minutes = timer / 60;
	int seconds = timer - (minutes * 60);
	string mL = std::to_string(minutes / 10 % 10);
	string mR = std::to_string(minutes % 10);
	string sL = std::to_string(seconds / 10 % 10);
	string sR = std::to_string(seconds % 10);
	string newTime = mL + mR + ":" + sL + sR + ',' + playerName;

	if (ldrNames.size() == 0) { 
		ldrNames.push_back(newTime);
		recentWin = &ldrNames.at(0);
		writeLdr();
	}
	else {
		for (int i = 0; i < ldrNames.size(); i++) {
			if (newTime < ldrNames.at(i)) {
				if (recentWin != nullptr) recentWin->pop_back(); 	//Last Winner gets * removed;
				ldrNames.insert(ldrNames.begin() + i, newTime);
				if (ldrNames.size() > 5)ldrNames.pop_back(); //Last Place gets removes.
				recentWin = &ldrNames.at(i);
				writeLdr();
				return;
			}
		}

		if (ldrNames.size() < 5) {
			if (recentWin != nullptr) recentWin->pop_back();
			ldrNames.push_back(newTime);
			recentWin = &ldrNames.at(ldrNames.size() - 1);
			writeLdr();
		}
	}
}

void Game::Board::updateLdr()
{
	string output = "";
	for (int i = 0; i < ldrNames.size(); i++)
	{
		output += std::to_string(i + 1) + ".\t" + ldrNames.at(i).substr(0, 5) + "\t" + ldrNames.at(i).substr(6);
		if (i != ldrNames.size()) output += "\n\n";
	}

	winTxt->setString(output);
	sf::FloatRect box = winTxt->getLocalBounds();
	winTxt->setOrigin(box.left + box.width / 2.0f, box.top + box.height / 2.0f);
	winTxt->setPosition(16 * width / 2.0f, 16 * height / 2.0f + 20);
}

void Game::Board::drawTiles(sf::RenderWindow& window)
{
	bool hideTiles = false;
	if (isPaused || (ldrWin->isOpen() && !gameOver)) hideTiles = true;
	for (auto& line : gameBoard)
	{
		for (auto& tile : line)
		{
			tile.drawSprite(window, hideTiles);
		}
	}

	std::map<string, Button>::iterator iter;
	for (iter = buttons.begin(); iter != buttons.end(); iter++) { iter->second.drawSprite(window); }

	if (showMines && !hideTiles) for (auto& mine : mineField) { window.draw(mine); }
}

void Game::Board::btnClk(const sf::Vector2f& cords)
{
	std::map<string, Button>::iterator iter;
	for (iter = buttons.begin(); iter != buttons.end(); iter++)
	{
		if (iter->second.inSprite(cords))
		{
			if (iter->first == "face_happy") resetGame();
			if (iter->first == "pause" && !gameOver) {
				isPaused = !isPaused; //Toggle
				if (isPaused) {
					iter->second.setSprite("box", txtList.at("play"));
					toggleHide();
				}
				else
				{
					iter->second.setSprite("box", txtList.at("pause"));
					toggleHide();
					updateTime(std::chrono::system_clock::to_time_t(std::chrono::system_clock::now()));
				}
			}
			if (iter->first == "debug" && !isPaused && !gameOver) { inDebug = !inDebug; showMines = !showMines; } //Toggle
				
				
			if (iter->first == "leaderboard") {
				ldrWin->create(sf::VideoMode(16 * width, 16 * height + 50), "Minesweeper");
				if (!gameOver) toggleHide(true);
			}
		}
	}

	if (!gameOver && !isPaused)
	{
		
		for (auto& line : gameBoard)
		{
			for (auto& tile : line)
			{
				if (!tile.isSeen() && tile.inSprite(cords)) { //If click is within the bounds of a tile.
					if (tile.isBomb()) { //And that tile is a mine.
						gameOver = true;
						showMines = true;
						for (auto mine : mineTiles) 
							mine->setSprite("box", txtList.at("tile_revealed"));
						buttons.at("face_happy").setSprite("box", txtList.at("face_lose"));
					}
					else
					{
						std::set<Tile*> adjTiles;
						tile.reveal(txtList, revTiles, adjTiles);
						if (revTiles == safeTiles)
						{
							gameOver = true;
							buttons.at("face_happy").setSprite("box", txtList.at("face_win"));
							flagCount = mineCount;
							updateCnt();
							addLdr();
							updateLdr();
							for (auto& mine : mineTiles) mine->toggleFlag(txtList.at("flag"), true);
							ldrWin->create(sf::VideoMode(16 * width, 16 * height + 50), "Minesweeper");
						}
					}
				}
			}
		}
	}
}

void Game::Board::flagClk(const sf::Vector2f& cords)
{
	if (!isPaused && !gameOver)
	{
		for (auto& line : gameBoard)
		{
			for (auto& tile : line)
			{
				if (tile.inSprite(cords)) //If click is within the bounds of a tile.
					if (!tile.isSeen()) { //And the tile is not already revealed.
						if (tile.toggleFlag(txtList.at("flag"))) flagCount += 1; //Returns whether a flag was added or removed.
						else flagCount -= 1;
						updateCnt();
					}
			}
		}
	}
}

void Game::Board::updateTime(const std::time_t& time) { timeInit = time; } //For pausing/ldrboard.
void Game::Board::updateTime()
{
	if (!ldrWin->isOpen() && !isPaused && !gameOver) {
		std::time_t timeNow = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
		double diff = difftime(timeNow, timeInit);
		if (diff >= 1) {
			timeInit = timeNow;
			timer += 1;
			if (timer >= 6000) timer = 0; //Loops clock back to 0 if it goes past 99:59 to prevent crash. Funny thing would be to force a gameover though.

			int minutes = timer / 60;
			int seconds = timer - (minutes * 60);
			buttons.at("clock").setSprite("timML", txtList.at("d" + std::to_string(minutes / 10 % 10)));
			buttons.at("clock").setSprite("timMR", txtList.at("d" + std::to_string(minutes % 10)));
			buttons.at("clock").setSprite("timSL", txtList.at("d" + std::to_string(seconds / 10 % 10)));
			buttons.at("clock").setSprite("timSR", txtList.at("d" + std::to_string(seconds % 10)));
		}
	}
}