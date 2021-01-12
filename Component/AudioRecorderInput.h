#ifndef AUDIORECORDERINPUT_H
#define AUDIORECORDERINPUT_H

#include <QObject>

class AudioRecorderInput : public QObject
{
    Q_OBJECT
public:
    explicit AudioRecorderInput(QObject *parent = nullptr);

signals:

public slots:
};

#endif // AUDIORECORDERINPUT_H
