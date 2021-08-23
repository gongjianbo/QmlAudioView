#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQuickWindow>
#include "ComponentRegister.h"

int main(int argc, char* argv[])
{
	QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
	//默认字体设置为微软雅黑
	QFont ft("Microsoft YaHei");
	ft.setPixelSize(14);
	QGuiApplication::setFont(ft);
	//文本渲染样式设置为native
	QQuickWindow::setTextRenderType(QQuickWindow::NativeTextRendering);
	QGuiApplication app(argc, argv);
	QQmlApplicationEngine engine;

	//组件注册到qml
	ComponentRegister::registerType(engine.rootContext());

	const QUrl url(QStringLiteral("qrc:/main.qml"));
	QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
		&app, [url](QObject* obj, const QUrl& objUrl) {
		if (!obj && url == objUrl)
			QCoreApplication::exit(-1);
	}, Qt::QueuedConnection);
	engine.load(url);

	return app.exec();
}
