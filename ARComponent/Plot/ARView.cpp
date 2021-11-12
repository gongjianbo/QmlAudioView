#include "ARView.h"

#include <cmath>
#include <algorithm>

#include <QGuiApplication>
#include <QCursor>
#include <QMouseEvent>
#include <QHoverEvent>
#include <QPainter>
#include <QFileInfo>
#include <QFile>
#include <QDir>
#include <QTime>
#include <QDebug>

ARView::ARView(QQuickItem *parent)
    : QQuickPaintedItem(parent)
{
    setAcceptedMouseButtons(Qt::LeftButton | Qt::RightButton);
    setAcceptHoverEvents(true);

    init();
}

ARView::~ARView()
{
    free();
}

void ARView::paint(QPainter *painter)
{
    painter->fillRect(0, 0, width(), height(), Qt::gray);
}

void ARView::init()
{
    player = new ARPlayer(&source, this);
    recorder = new ARRecorder(&source, this);
}

void ARView::free()
{

}
