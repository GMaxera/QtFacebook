#include <QGuiApplication>
#include <QQmlApplicationEngine>
//#include <QtWebView>
#include "qfacebook.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    qmlRegisterSingletonType<QFacebook>("org.qtproject.example", 1, 0, "Facebook", QFacebook::qFacebookProvider);

    QQmlApplicationEngine engine;
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));

    return app.exec();
}
