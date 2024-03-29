#pragma once

#include <filesystem>
#include <memory>
#include <sqlite_orm/sqlite_orm.h>
namespace sql = sqlite_orm;

import player;
import word;
import gamescore;

import <fstream>;
import <vector>;
import <random>;

namespace gartic
{
	inline auto CreateStorage(const std::string& filename)
	{
		return sql::make_storage(
			filename,
			sql::make_table(
				"Players",
				sql::make_column("username", &Player::SetUsername, &Player::GetUsername, sql::unique()),
				sql::make_column("email", &Player::SetEmail, &Player::GetEmail, sql::unique()),
				sql::make_column("password", &Player::SetPassword, &Player::GetPassword)
			),
			sql::make_table(
				"Words",
				sql::make_column("word", &Word::SetWord, &Word::GetWord, sql::unique()),
				sql::make_column("difficulty", &Word::SetDifficulty, &Word::GetDifficulty)
			),
			sql::make_table(
				"GameScores",
				sql::make_column("gameid", &GameScore::SetGameId, &GameScore::GetGameId),
				sql::make_column("username", &GameScore::SetUsername, &GameScore::GetUsername),
				sql::make_column("score", &GameScore::SetScore, &GameScore::GetScore)
			)
		);
	}

	using Storage = decltype(CreateStorage(""));


	class GarticDatabase
	{
	public:
		bool Initialize();

		bool AddPlayerToDatabase(const std::string& username, const std::string& email, const std::string& password);
		bool DeletePlayerFromDatabase(const std::string& username);
		bool PlayerIsInDatabase(const std::string& username, const std::string& password, const std::string& email = "");
		std::vector<Player> GetPlayers();

		std::vector<Word> GetWords();
		Word GetRandomWordWithDifficulty(int difficulty);
		void AddWordToDatabase(const std::string& word, int difficulty);

		void AddScoreToDatabase(int gameID, const std::string& username, float score);
		std::vector<GameScore> GetScoresOfPlayer(const std::string& username);

		int GetNextGameID();

	private:
		void PopulatePlayerStorage();
		void PopulateWordsStorage();
		int GenerateRandomNumber(int max);

	private:
		const std::string kDbFile{ "resources//database.sqlite" };
		const std::string kWordFile{ "resources//words.txt" };
		const std::string kCredentialsFile{ "resources//credentials.txt" };
		const int kNumberOfWordsOfADifficulty{ 200 };

	private:
		Storage m_db = CreateStorage(kDbFile);
	};
}