#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <map>
#include <string>
#include <chrono>
#include <stdexcept>
#include <iostream>
#include <fstream>
#include <ostream>
#include <set>

using std::string;
using std::vector;

class Game
{
	class Board
	{
		class Button
		{
			protected:
				sf::Sprite box;
				std::map<string, sf::Sprite> sprites = { {"box", box} };

			public:
				Button() {};
				Button(sf::Texture& data, const float& x, const float& y);
				void drawSprite(sf::RenderWindow& window, bool isPaused = false);
				bool inSprite(const sf::Vector2f& cords);
				void setSprite(const string& name, sf::Texture& data);
				void addSprite(const sf::Texture& data, const string& id, const float& x = 0, const float& y = 0);
				void delSprite(const string& name);
		};

		class Tile : public Button
		{
			bool isMine = false, hasFlag = false, isRevealed = false;
			int danger = 0;
			vector<Tile*> tileList;
			vector<Tile*> safeList;

			public:
				Tile(sf::Texture& data, const int& x, const int& y);
				void setPtr(Tile* adj);
				bool setMine();
				void setSafe(sf::Texture& data);
				void reveal(const std::map<string, sf::Texture>& map, int& revCount, std::set<Tile*> adjTiles = {}); 
				bool isSeen();
				bool isBomb();
				void updateSafe();
				bool toggleFlag(sf::Texture& data, bool gameWon = false);
				sf::Vector2f getPos();
		};
		
		string playerName = "";
		string* recentWin = nullptr; //For delete the * between games.
		int width = 25, height = 16, mineCount = 50, safeTiles = 0; //Static Data after inital load.
		int flagCount = 0, revTiles = 0, timer = 0; //Data that is changed.
		bool showMines = false, gameOver = false, isPaused = false, cntNeg = true, inDebug = false;

		//Memory for Leaderboard
		sf::RenderWindow* ldrWin;
		sf::Text* winTxt;
		vector<string> ldrNames;

		vector<vector<Tile>> gameBoard; //Memory for tiles.
		vector<Tile*> mineTiles; //For ease of access to mines if player clicks on one.
		vector<sf::Sprite> mineField; //Since mines are always shown all at the same time can be stored together instead on the tiles themselves.
		std::map<string, Button> buttons; //Memory for non-tile buttons.
		std::time_t timeInit = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now()); //Memory for Clock

		//Initalize where the textures will be stored.
		//SFML creates pointers to the texures. Needs long term storage.
		sf::Texture one, two, three, four, five, six, seven, eight; 
		sf::Texture flag, mine, hide, rev, happy, lose, win, ldr, pause, play, debug, digits;
		sf::Texture d0, d1, d2, d3, d4, d5, d6, d7, d8, d9, dn; 
		//Map to retrieve the textures easily.
		std::map<string, sf::Texture> txtList = {
			{"flag", flag},{"mine", mine},{"tile_hidden", hide},{"tile_revealed", rev}, //Reused Textures for Tiles
			{"face_happy", happy}, {"face_lose", lose},{"face_win", win}, {"leaderboard", ldr}, 
			{"pause", pause}, {"play", play}, {"debug", debug}
		};

		void buildBoard(); //Used Onced
		void addMines();
		void resetGame();
		void setTextures(std::map<int, sf::Texture>& m);
		void setTextures(std::map<string, sf::Texture>& m);
		void setTextures(vector <std::pair<string, sf::Texture>>& v);
		void mkButtons(const vector<std::pair<string, std::pair<int, int>>>& v);
		void updateCnt();
		void readLdr();
		void writeLdr();
		void addLdr();
		void updateLdr();

		public:
			Board(const string& name, const int& wide, const int& tall, const int& total, sf::RenderWindow& ldrWin, sf::Text& bestTimes);
			void drawTiles(sf::RenderWindow& window);
			void btnClk(const sf::Vector2f& cords);
			void flagClk(const sf::Vector2f& cords);
			void updateTime();
			void updateTime(const std::time_t& time);
			void toggleHide(bool hide = false);
	};

	void Field(const std::string& userName, const int& wide, const int& tall, const int& mineCount);
	sf::Text mkmidTxt(const std::string& txt, const int& size, const float& x, const float& y, const sf::Font& font, bool isUL = false);
	void cntTxtPos(sf::Text& words, const float& x, const float& y);

	public:
		Game(); 
};