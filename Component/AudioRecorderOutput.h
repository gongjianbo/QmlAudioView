#ifndef AUDIORECORDEROUTPUT_H
#define AUDIORECORDEROUTPUT_H

#include <QObject>

class AudioRecorderOutput : public QObject
{
    Q_OBJECT
public:
    explicit AudioRecorderOutput(QObject *parent = nullptr);

signals:

public slots:
};

#endif // AUDIORECORDEROUTPUT_H
