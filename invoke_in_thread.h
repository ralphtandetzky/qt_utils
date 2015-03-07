/// @file
///
/// @author Ralph Tandetzky
/// @date 14 Feb 2014

#pragma once

#include <../cpp_utils/std_make_unique.h>

#include <QApplication>
#include <QObject>
#include <QThread>
#include <future>
#include <functional>

namespace qu
{

namespace detail
{

    class InvokeInThreadHelper
        : public QObject
    {
        Q_OBJECT
    public:
        template <typename F>
        InvokeInThreadHelper( F && f )
            : f( std::forward<F>(f) )
        {
        }

    public slots:
        void runAndDeleteMe()
        {
            f();
            delete this;
        }

    private:
        std::function<void()> f;
    };

} // namespace detail


/// \brief This functions spawns a functor in the event loop of a @c QThread.
///
/// The given thread must run a Qt event loop in order to dispatch the
/// given functor. The asynchroneous result can be retrieved through
/// the returned @c std::future. The connection type should not be
/// @c Qt::DirectConnection. The default value @c Qt::AutoConnection
/// will arrange the call to be synchroneous, iff @c thread is the
/// thread from which the function is called. If @c Qt::QueuedConnection
/// is used, then the execution will always be pushed to the end of the
/// event queue of @c thread.
template <typename F>
auto invokeInThread( QThread * thread, F && f,
                     Qt::ConnectionType connectionType = Qt::AutoConnection )
    -> std::future<decltype(f())>
{
    using RetType = decltype(f());
    auto task = std::make_shared<std::packaged_task<RetType()>>( std::forward<F>(f) );
    auto future = task->get_future();
    auto invoker = std::make_unique<detail::InvokeInThreadHelper>(
                [=]() { (*task)(); } );
    invoker->moveToThread( thread );
    QMetaObject::invokeMethod( invoker.get(), "runAndDeleteMe", connectionType );
    invoker.release();
    return future;
}

/// \brief Spawns a functor in the event loop of the Qt gui thread.
///
/// This is especially helpful to perform actions that can only be done
/// in the gui thread such as creating and accessing QWidgets or derived
/// classes.
template <typename F>
auto invokeInGuiThread( F && f,
                        Qt::ConnectionType connectionType = Qt::AutoConnection )
    -> std::future<decltype(f())>
{
    return invokeInThread( QApplication::instance()->thread(),
                           std::forward<F>(f),
                           std::move(connectionType) );
}

/// Like @c invokeInThread(), but with a @c Qt::QueuedConnection.
template <typename F>
auto invokeInThreadAsync( QThread * thread, F && f )
    -> std::future<decltype(f())>
{
    return invokeInThread( thread,
                           std::forward<F>(f),
                           Qt::QueuedConnection );
}

/// Like @c InvokeInGuiThread(), but with a @c Qt::QueuedConnection.
template <typename F>
auto invokeInGuiThreadAsync( F && f )
    -> std::future<decltype(f())>
{
    return invokeInGuiThread( std::forward<F>(f),
                              Qt::QueuedConnection );
}

/// Calls a functor in a thread, waits for it to finish and returns its result.
template <typename F>
auto invokeInThreadSync( QThread * thread, F && f )
    -> decltype(f())
{
    return invokeInThread( thread,
                           std::forward<F>(f),
                           Qt::AutoConnection ).get();
}

/// Like @c invokeInThreadSync(), but passing a pointer to the Qt gui thread.
template <typename F>
auto invokeInGuiThreadSync( F && f )
    -> decltype(f())
{
    return invokeInGuiThread( std::forward<F>(f),
                              Qt::AutoConnection ).get();
}


} // namespace qu
