#include "gameboard.h"

Game::Board::Button::Button(sf::Texture& data, const float& x, const float& y)
{
	sprites.at("box").setTexture(data);
	sprites.at("box").setPosition(x, y);
}

void Game::Board::Button::drawSprite(sf::RenderWindow& window, bool isPaused)
{
	if (!isPaused)
	{
		std::map<string, sf::Sprite>::iterator iter;
		for (iter = sprites.begin(); iter != sprites.end(); iter++)
		{
			window.draw(iter->second);
		}
	}
	else
	{
		window.draw(sprites.at("box"));
	}
}

bool Game::Board::Button::inSprite(const sf::Vector2f& cords) {
	if (sprites.find("box") != sprites.end())
		return sprites.at("box").getGlobalBounds().contains(cords);
	else return false;
}

void Game::Board::Button::setSprite(const string& name, sf::Texture& data) { sprites.at(name).setTexture(data); }
void Game::Board::Button::delSprite(const string& name) { sprites.erase(name); }
void Game::Board::Button::addSprite(const sf::Texture& data, const string& id, const float& x, const float& y) {
	sf::Sprite tmp;
	sprites.insert({ id, tmp });
	sprites.at(id).setTexture(data);
	if (x > 0 || y > 0) sprites.at(id).setPosition(x, y);
	else sprites.at(id).setPosition(sprites.at("box").getPosition());
}

#include "gameboard.h"

Game::Board::Tile::Tile(sf::Texture& data, const int& x, const int& y)
{
	sprites.at("box").setTexture(data);
	sprites.at("box").setPosition(x * 32, y * 32);
}

void Game::Board::Tile::setPtr(Tile* adj)
{
	this->tileList.push_back(adj);
	adj->tileList.push_back(this);
}

bool Game::Board::Tile::setMine()
{
	if (!isMine) {
		isMine = true;

		for (auto& t : tileList)
		{
			t->danger += 1;
		}

		return true;
	}
	return false;
}

void Game::Board::Tile::setSafe(sf::Texture& data)
{
	hasFlag = false;
	isRevealed = false;
	isMine = false;
	danger = 0;
	sprites = { {"box", sprites.at("box")} };
	sprites.at("box").setTexture(data);
}

void Game::Board::Tile::reveal(const std::map<string, sf::Texture>& map, int& revCount, std::set<Tile*> adjTiles)
{
	if (adjTiles.size() == 0) adjTiles.emplace(this);
	for (auto& adj : adjTiles) {
		if (adj->isRevealed || adj->hasFlag) return;
		adj->isRevealed = true;
		revCount += 1;
		adj->sprites.at("box").setTexture(map.at("tile_revealed"));

		if (adj->isMine) return;
		if (adj->danger > 0) adj->addSprite(map.at(std::to_string(adj->danger)), "num");
	}

	std::set<Tile*> newList;
	for (auto& adj : adjTiles) {
		adj->updateSafe();
		if (adj->danger == 0) for (auto& safe : adj->safeList)
			newList.emplace(safe);
	}
	reveal(map, revCount, newList);
}

bool Game::Board::Tile::isSeen() { return isRevealed; }
bool Game::Board::Tile::isBomb() { return isMine && !hasFlag; }

bool Game::Board::Tile::toggleFlag(sf::Texture& data, bool gameWon)
{
	hasFlag = !hasFlag;
	if (hasFlag || gameWon) addSprite(data, "flag");
	else sprites.erase("flag");
	return hasFlag;
}

void Game::Board::Tile::updateSafe()
{
	safeList.clear();
	for (auto& adj : tileList)
		if (!adj->isMine && !adj->isRevealed && !adj->hasFlag) safeList.push_back(adj);
}

sf::Vector2f Game::Board::Tile::getPos() { return sprites.at("box").getPosition(); }