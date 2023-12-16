#include "PageController.h"

PageController::PageController(QWidget* parent):QStackedWidget(parent)
{
	/*empty*/
}

void PageController::AddPage(QWidget* page, const QString& name)
{
	pagesMap[name] = page;
	QStackedWidget::addWidget(page);
}

void PageController::ShowPage(const QString& name)
{
	if (pagesMap.contains(name))
	{
		QWidget* page = pagesMap[name];
		setCurrentWidget(page);
	}
}

void PageController::SetStartupPage(const QString& name)
{
	if (pagesMap.contains(name))
	{
		ShowPage(name);
	}
}

bool PageController::VerifyLogin(const QString& enteredUsername, const QString& enteredPassword)
{
	std::string username = enteredUsername.toUtf8().constData();
	std::string password = enteredPassword.toUtf8().constData();
	if (player.VerifyLogin(username, password))
	{
		auto responseVerify = cpr::Get(
			cpr::Url{ "http://localhost:18080/login" },
			cpr::Parameters{
					{ "username", username},
					{ "password", password},
			}
		);
		if (responseVerify.status_code == 202)
			return true;
	}
	return false;
}

bool PageController::VerifyRegister(const QString& enteredUsername, const QString& enteredEmail, const QString& enteredPassword)
{
	return player.VerifyRegister(enteredUsername.toStdString(), enteredEmail.toStdString(), enteredPassword.toStdString());
}