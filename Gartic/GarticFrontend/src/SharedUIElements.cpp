#include "../includes/SharedUIElements.h"

SharedUIElements::SharedUIElements()
{
	returnButton = new QPushButton;
	imageLabel = new QLabel;

	StyleElements();
}

void SharedUIElements::StyleElements()
{
	QPixmap image("resources//Game_Name.png");
	imageLabel->setPixmap(image);

	returnButton->setIconSize(QSize(50, 50));
	returnButton->setFixedSize(40, 40);

	returnButton->setAccessibleName("returnButton");
	returnButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

	QFile styleFile("resources//style.css");
	styleFile.open(QFile::ReadOnly | QFile::Text);
	QString styleSheet = styleFile.readAll();
	setStyleSheet(styleSheet);
}

void SharedUIElements::SetImagePhoto(QPixmap image)
{
	imageLabel->setPixmap(image);
}

QPushButton* SharedUIElements::GetReturnButton()
{
	return returnButton;
}

QLabel* SharedUIElements::GetImageLabel()
{
	return imageLabel;
}

SharedUIElements::~SharedUIElements()
{
	/*empty*/
}
