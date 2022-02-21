#include "application.h"
#include "composer/documenthandler.h"

#include <QApplication>
#include <QQmlApplicationEngine>
#include <QUrl>
#include <QtQml>
#include <QtWebView/QtWebView>

int main(int argc, char *argv[])
{
    QtWebView::initialize();
    QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication app(argc, argv);
    QQmlApplicationEngine engine;

    //    qmlRegisterType<DocumentHandler>("mail.widgets", 1, 0, "DocumentHandler");

    Application *application = new Application();
    engine.rootContext()->setContextProperty("session", application);

    engine.rootContext()->setContextObject(&engine);
    engine.load(QUrl(QStringLiteral("qrc:///qml/main.qml")));

    if (engine.rootObjects().isEmpty()) {
        return -1;
    }

    return app.exec();
}
