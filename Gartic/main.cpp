#include "MainWindow.h"
#include <QtWidgets/QApplication>
#include <QIcon>

#include "Player.h"
//import round;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QIcon appIcon("Images/App_icon.ico");
    a.setWindowIcon(appIcon);
    MainWindow w;
    w.show();

    //Added console interface
    AllocConsole();
    if(!freopen("CONIN$", "r", stdin))
        throw(std::exception("input flux not open."));
    if (!freopen("CONOUT$", "w", stdout))
        throw(std::exception("output flux not open."));
    QTextStream input(stdin);
    QTextStream output(stdout);
    //Added console interface
    return a.exec();
}
