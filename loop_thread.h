/// @file
///
/// @author Ralph Tandetzky
/// @date 14 Feb 2014

#pragma once

#include <QThread>

namespace qu
{

class LoopThread
        : public QThread
{
    Q_OBJECT
public:
    explicit LoopThread(
            QObject * parent = nullptr
            , QThread::Priority priority = QThread::InheritPriority )
        : QThread( parent )
    {
        start( priority );
    }

    virtual ~LoopThread()
    {
        quit();
        wait();
    }
};

} // namespace qu
