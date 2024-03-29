#include "../includes/LeaderboardPage.h"

LeaderboardPage::LeaderboardPage(PageController* controller, QWidget* parent)
{
	layout = new QVBoxLayout(this);
	boardPadding = new QWidget;
	topLeftLayout = new QHBoxLayout;
	bottomLeftLayout = new QVBoxLayout;
	middleLayout = new QVBoxLayout;
	isGameFinished = false;
	title = new QLabel("Leaderboard",this);
	index = 0;
	StyleElements();
	PlaceElements();
	connect(sharedElements.GetReturnButton(), &QPushButton::clicked, controller, [=]() {
		isGameFinished = false;
		ClearPlayerProfiles();
		controller->ShowPage("MainMenu");
		});
}

void LeaderboardPage::PlaceElements()
{
	setLayout(layout);

	topLeftLayout->addWidget(sharedElements.GetImageLabel());
	topLeftLayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);

	bottomLeftLayout->addWidget(sharedElements.GetReturnButton());
	bottomLeftLayout->setAlignment(Qt::AlignLeft | Qt::AlignBottom);

	boardPadding->setFixedSize(400, 350);
	middleLayout->addWidget(title);
	middleLayout->addWidget(boardPadding);
	middleLayout->setAlignment(Qt::AlignCenter);

	layout->addLayout(topLeftLayout);
	layout->addLayout(middleLayout);
	layout->addLayout(bottomLeftLayout);
}

void LeaderboardPage::StyleElements()
{
	boardPadding->setAccessibleName("mainPadding");
	title->setAccessibleName("title");
	QFile styleFile("resources//style.css");
	styleFile.open(QFile::ReadOnly | QFile::Text);
	QString styleSheet = styleFile.readAll();
	setStyleSheet(styleSheet);
}


void LeaderboardPage::UpdateData()
{
	auto responsePlayers = cpr::Get(
		cpr::Url{ "http://localhost:18080/getplayersdatafromgame" }
	);
	auto responseEnd = cpr::Get(
		cpr::Url{ "http://localhost:18080/requestend" });
	if (responseEnd.status_code == 200)
	{
		auto responseEnd = cpr::Get(
			cpr::Url{ "http://localhost:18080/deletegame" });
	}
	auto players = nlohmann::json::parse(responsePlayers.text);
	for (const auto& player : players)
	{
		AddPlayer(QString::fromUtf8(player["username"].get<std::string>()), QString::fromUtf8(player["score"].get<std::string>()));
	}
}

void LeaderboardPage::showEvent(QShowEvent* event)
{
	if (!isGameFinished)
	{
		QMessageBox::StandardButton reply;
		reply = QMessageBox::information(this, "Game", "The game has ended.", QMessageBox::Ok);
		if (reply == QMessageBox::Ok)
		{
			UpdateData();
		}
		isGameFinished = true;
	}
	QWidget::showEvent(event);
}

void LeaderboardPage::AddPlayer(const QString& playerName, const QString& playerScore)
{
	QWidget* newProfilePadding = new QWidget(boardPadding);
	newProfilePadding->setAccessibleName("profilePadding");
	newProfilePadding->setFixedSize(380, 70);
	newProfilePadding->setGeometry(10, 20 + 80 * index, 0, 0);

	QHBoxLayout* newProfileLayout = new QHBoxLayout(newProfilePadding);

	QLabel* newProfileImage = new QLabel;
	newProfileImage->setPixmap(QPixmap("resources//[PNG] App_icon.png"));
	newProfileImage->setFixedSize(40, 40);
	newProfileImage->setScaledContents(true);

	QLabel* newProfileName = new QLabel;
	newProfileName->setText(playerName);

	QLabel* newProfileScore = new QLabel;
	newProfileScore->setText(playerScore);

	newProfileLayout->addWidget(newProfileImage, Qt::AlignLeft);
	newProfileLayout->addWidget(newProfileName);
	newProfileLayout->addWidget(newProfileScore, 0, Qt::AlignRight);
	newProfileLayout->setAlignment(Qt::AlignCenter);

	newProfilePadding->setVisible(true);
	newProfileImage->setVisible(true);
	newProfileName->setVisible(true);
	newProfileScore->setVisible(true);

	profilePaddings.append(newProfilePadding);
	profileLayouts.append(newProfileLayout);
	profileImages.append(newProfileImage);
	profileNames.append(newProfileName);
	profileScores.append(newProfileScore);

	index++;
}

void LeaderboardPage::ClearPlayerProfiles()
{
	for (int index = 0; index < profilePaddings.size(); index++)
	{
		delete profilePaddings[index];
	}
	profilePaddings.clear();
	profileLayouts.clear();
	profileNames.clear();
	this->index = 0;
}

LeaderboardPage::~LeaderboardPage()
{
	/*empty*/
}
