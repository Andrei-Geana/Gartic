#include "ProfilePage.h"

ProfilePage::ProfilePage(PageController* controller, QWidget* parent)
{
	layout = new QVBoxLayout(this);
	imageLabel = new QLabel(this);
	returnButton = new QPushButton(this);
	mainPadding = new QWidget;
	username = new QLabel;
	userImage = new QLabel;
	averageScore = new QLabel("Average Score: ");
	matchHistory = new QTextEdit("Match History");

	SetSize();
	StyleElements();
	PlaceElements();
	connect(returnButton, &QPushButton::clicked, controller, [controller]() {
		controller->ShowPage("MainMenu");
		});
}

void ProfilePage::PlaceElements()
{
	setLayout(layout);
	QHBoxLayout* topLeftLayout = new QHBoxLayout;
	QPixmap image("Images/Game_Name.png");
	imageLabel->setPixmap(image);
	topLeftLayout->addWidget(imageLabel);
	topLeftLayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);

	QVBoxLayout* bottomLeftLayout = new QVBoxLayout;
	returnButton->setIconSize(QSize(50, 50));
	returnButton->setFixedSize(40, 40);
	bottomLeftLayout->addWidget(returnButton);
	bottomLeftLayout->setAlignment(Qt::AlignLeft | Qt::AlignBottom);

	QVBoxLayout* middleLayout = new QVBoxLayout;
	middleLayout->addWidget(mainPadding);
	middleLayout->setAlignment(Qt::AlignCenter);

	QHBoxLayout* mainPaddingLayout = new QHBoxLayout(mainPadding);
	
	QVBoxLayout* leftSideLayout = new QVBoxLayout;
	leftSideLayout->addWidget(userImage);
	leftSideLayout->addWidget(username);
	leftSideLayout->addWidget(averageScore);


	mainPaddingLayout->addLayout(leftSideLayout);
	mainPaddingLayout->addWidget(matchHistory);

	layout->addLayout(topLeftLayout);
	layout->addLayout(middleLayout);
	layout->addLayout(bottomLeftLayout);
	}

void ProfilePage::StyleElements()
{
	returnButton->setAccessibleName("returnButton");
	mainPadding->setAccessibleName("mainPadding");
	matchHistory->setAccessibleName("matchHistory");

	userImage->setPixmap(QPixmap("Images/[PNG] App_icon.png"));
	userImage->setFixedSize(150, 150);
	userImage->setScaledContents(true);
	QRegion* region = new QRegion(0, 0, userImage->width(), userImage->height(), QRegion::Ellipse);
	userImage->setMask(*region);

	username->setText("Username");

	QFile styleFile("style.css");
	styleFile.open(QFile::ReadOnly | QFile::Text);
	QString styleSheet = styleFile.readAll();
	setStyleSheet(styleSheet);
}

void ProfilePage::SetSize()
{
	returnButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	mainPadding->setFixedSize(600, 500);
	matchHistory->setFixedSize(300,400);
}

ProfilePage::~ProfilePage()
{
	delete  layout, imageLabel, username, userImage, mainPadding,
		averageScore, matchHistory, returnButton;
}
