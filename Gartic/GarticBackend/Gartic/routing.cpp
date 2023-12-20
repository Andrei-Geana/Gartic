#include "routing.h"

import lobby;
using namespace gartic;

void Routing::Run(GarticDatabase& db, std::unique_ptr<Game>& game, std::unique_ptr<Lobby>& lobby)
{
	//IF MULTIPLE LOBBIES ARE ALLOWED -> lobby SHOULD BE CHANGED TO vector of LOBBY AND ALL RESPECTIVE ROUTES
	//getelement => send element from server to client
	//sendelement => send element from client to server
	CROW_ROUTE(m_app, "/")([]() {
		return "Server is running.This is the main branch";
			});

	CROW_ROUTE(m_app, "/logincredentials")([&db]() {
		std::vector<crow::json::wvalue> logincredentials_json;
		auto players = db.GetPlayers();
		for (const auto& logincredential : players)
		{
			logincredentials_json.push_back(crow::json::wvalue{
				{"username", logincredential.GetUsername()},
				{"password", logincredential.GetPassword()},
				{"email", logincredential.GetEmail()}
				});
		}
		return crow::json::wvalue{ logincredentials_json };
		});

	CROW_ROUTE(m_app, "/register")
		.methods(crow::HTTPMethod::PUT)([&db](const crow::request& req)
			{
				std::string receivedUsername = req.url_params.get("username");
				std::string receivedPassword = req.url_params.get("password");
				std::string receivedEmail = req.url_params.get("email");

				if (receivedUsername.empty() || receivedEmail.empty() || receivedPassword.empty())
					return crow::response(400);

				if (!db.PlayerIsInDatabase(receivedUsername, receivedPassword, receivedEmail))
				{
					if (db.AddPlayerToDatabase(receivedUsername, receivedEmail, receivedPassword))
						return crow::response(201);
				}
				else return crow::response(406);

			});

	CROW_ROUTE(m_app, "/verifyuser")
		.methods(crow::HTTPMethod::GET)([&db](const crow::request& req)
			{
				std::string receivedUsername = req.url_params.get("username");
				std::string receivedPassword = req.url_params.get("password");
				std::string receivedEmail = req.url_params.get("email");

				if (receivedPassword.empty() || receivedUsername.empty() || receivedEmail.empty())
					return crow::response(400);

				if (db.PlayerIsInDatabase(receivedUsername, receivedPassword, receivedEmail))
					return crow::response(302);

				return crow::response(404);
			});

	CROW_ROUTE(m_app, "/login")
		.methods(crow::HTTPMethod::GET)([&db, &lobby, &game](const crow::request& req)
			{
				std::string receivedUsername = req.url_params.get("username");
				std::string receivedPassword = req.url_params.get("password");

				if (receivedPassword.empty() || receivedUsername.empty())
					return crow::response(400);

				if (db.PlayerIsInDatabase(receivedUsername, receivedPassword))
					return crow::response(404);

				//CHECK IF USERNAME IS IN LOBBY OR GAME
				//IF USER ALREADY LOGGED IN, BUT NOT JOINED OR CREATED LOBBY, THEN IT WILL STILL LET IT PASS
				//CAN BE FIXED BY HAVING A VECTOR OF LOGGED IN PLAYER USERNAMES?
				if (lobby) 
				{
					if (lobby->IsInLobby(receivedUsername))
						return crow::response(409);
				}
				if (game)
				{
					if (game->IsPlayerInGame(receivedUsername))
						return crow::response(409);
				}
				return crow::response(202);
			});

	CROW_ROUTE(m_app, "/deleteuser")
		.methods(crow::HTTPMethod::GET)([&db](const crow::request& req)
			{
				std::string receivedUsername = req.url_params.get("username");

				if (receivedUsername.empty())
					return crow::response(400);

				else {
					db.DeletePlayerFromDatabase(receivedUsername);
					return crow::response(202);
				}
				return crow::response(404);
			});

	CROW_ROUTE(m_app, "/createlobby")
		.methods(crow::HTTPMethod::GET)([&lobby](const crow::request& req)
			{
				//IF LOBBY ALREADY EXISTS
				if (lobby)
					return crow::response(409);
				std::string receivedUsername = req.url_params.get("username");
				if (receivedUsername.empty())
					return crow::response(204);
				lobby = std::make_unique<Lobby>();
				std::unique_ptr<Player> player = std::make_unique<Player>(receivedUsername);
				lobby->AddPlayer(player);
				return crow::response(201);
			});

	CROW_ROUTE(m_app, "/join")
		.methods(crow::HTTPMethod::GET)([&lobby](const crow::request& req)
			{
				std::string receivedLobbyCode = req.url_params.get("lobbycode");
				std::string receivedUsername = req.url_params.get("username");

				//CHECK DATA
				if (receivedLobbyCode.empty() || receivedUsername.empty())
					return crow::response(400);

				//CHECK IF THERE IS A LOBBY
				if (!lobby)
					return crow::response(404);

				if (lobby->GetLobbyCode() == receivedLobbyCode)
				{
					std::unique_ptr<Player> player = std::make_unique<Player>(receivedUsername);
					try {
						lobby->AddPlayer(player);
						return crow::response(200);
					}
					catch (...)
					{
						return crow::response(400);
					}
				}
				return crow::response(404);
			});

	CROW_ROUTE(m_app, "/getlobbystatus")
		.methods(crow::HTTPMethod::GET)([&lobby](const crow::request& req)
			{
				return crow::json::wvalue{ lobby->GetStatusOfLobby() };
			});
	
	CROW_ROUTE(m_app, "/getlobbycode")
		.methods(crow::HTTPMethod::GET)([&lobby](const crow::request& req)
			{
				return crow::json::wvalue{ lobby->GetLobbyCode()};
			});

	/*CROW_ROUTE(m_app, "/senddifficulty")
		.methods(crow::HTTPMethod::PUT)([&game](const crow::request& req)
			{
				try
				{
					int difficulty = std::stoi(req.url_params.get("difficulty"));
					game->ChangeDifficulty(difficulty);
					return crow::response(200);
				}
				catch(...)
				{
					return crow::response(400);
				}
			});*/

	CROW_ROUTE(m_app, "/getusernamesfromlobby")
		.methods(crow::HTTPMethod::GET)([&lobby]()
			{
				std::vector<crow::json::wvalue> gameData_json;
				auto playersUsername = lobby->GetUsernamesOfPlayers();
				for (const auto& username : playersUsername)
				{
					gameData_json.push_back(crow::json::wvalue{
						{"username", username}
					});
				}
				return crow::json::wvalue{ gameData_json };
		});
	
	CROW_ROUTE(m_app, "/startgame")
		.methods(crow::HTTPMethod::GET)([&lobby, &game](const crow::request& req)
			{
				if (game)
					return crow::response(400);
				game = std::make_unique<Game>();
				//SET DIFFICULTY OF GAME
				try
				{
					//WILL BREAK SERVER IF DIFFICULTY IS NOT SENT
					int difficulty = std::stoi(req.url_params.get("difficulty"));
					game->ChangeDifficulty(difficulty);
				}
				catch (...)
				{
					return crow::response(400);
				}
				//MOVE PLAYERS FROM LOBBY TO GAME
				lobby->MovePlayersToGame(*(game.get()));
				game->StartAnotherRound();
				return crow::response(200);
		});

	//ROUTES RELATED TO GAME
		
	CROW_ROUTE(m_app, "/getplayersdatafromgame")
		.methods(crow::HTTPMethod::GET)([&game]() {
				std::vector<crow::json::wvalue> gameData_json;
				auto players = game->GetPlayers();
				for (const auto& player : players)
				{
					gameData_json.push_back(crow::json::wvalue{
						{"username", player->GetUsername()},
						{"score", std::to_string(player->GetScore())}
						});
				}
				return crow::json::wvalue{ gameData_json };
			});

	CROW_ROUTE(m_app, "/getroundnumber")
		.methods(crow::HTTPMethod::GET)([&game](const crow::request& req)
			{
				return crow::json::wvalue{ game->GetRoundNumber()};
			});

	CROW_ROUTE(m_app, "/gettimer")
		.methods(crow::HTTPMethod::GET)([&game](const crow::request& req)
			{
				return crow::json::wvalue{ game->GetTimer() };
			});
	
	CROW_ROUTE(m_app, "/getboard")
		.methods(crow::HTTPMethod::GET)([&game](const crow::request& req)
			{
				auto board = game->GetBoard();
				std::string boardToSend;
				for (size_t i = 0; i < board.size(); ++i) 
				{
					boardToSend.push_back(char(board[i] + '0'));
				}
				return crow::json::wvalue{ {"board", boardToSend} };
			});
	
	CROW_ROUTE(m_app, "/sendboard")
		.methods(crow::HTTPMethod::PUT)([&game](const crow::request& req)
			{
				std::string stringBoard = req.url_params.get("board");
				if (stringBoard.size() != Game::kSize)
					return crow::response(400);
				std::array<uint16_t, Game::kSize> newBoard;
				for (size_t i = 0; i < stringBoard.size(); ++i)
				{
					newBoard[i] = static_cast<uint16_t>(stringBoard[i]-'0');
				}
				game->UpdateBoard(newBoard);
				return crow::response(200);
			});
}

crow::SimpleApp& Routing::GetApp()
{
	return m_app;
}
