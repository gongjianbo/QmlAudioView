#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQuickWindow>
#include <QFont>
#include "ARRegister.h"

int main(int argc, char *argv[])
{
	QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
	QFont ft("Microsoft YaHei");
	ft.setPixelSize(14);
	QGuiApplication::setFont(ft);
	//QQuickWindow::setTextRenderType(QQuickWindow::NativeTextRendering);
	QGuiApplication app(argc, argv);
	QQmlApplicationEngine engine;

    ARRegister::registerComponent(&engine);

	const QUrl url(QStringLiteral("qrc:/main.qml"));
	QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
        &app, [url](QObject *obj, const QUrl &objUrl) {
		if (!obj && url == objUrl)
			QCoreApplication::exit(-1);
	}, Qt::QueuedConnection);
	engine.load(url);

	return app.exec();
}
