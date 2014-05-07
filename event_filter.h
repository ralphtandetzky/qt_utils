/// @file
///
/// @author Ralph Tandetzky
/// @date 22 Jan 2014

#pragma once

#include <QObject>

#include <functional>
#include <memory>

#include "cpp_utils/std_make_unique.h"


class QEvent;

namespace qu
{

class GenericEventFilter;

/// @brief Creates an event filter with @c f as filter function.
///
/// The functor @f must have the signature @c bool(QObject*,QEvent*) just
/// as the function @c QObject::eventFilter(). A pointer to an object will
/// be returned which executes the moved or copied functor whenever
/// @c QObject::eventFilter() is called. 
template <typename F>
std::unique_ptr<QObject> makeEventFilter( F && f )
{
    return std::make_unique<GenericEventFilter>( std::forward<F>(f) );
}


/// @brief Creates and installs an event filter.
/// 
/// The function creates an event filter with @c makeEventFilter and then
/// installs it on the given object @c obj. The functor @f must have the
/// signature @c bool(QObject*,QEvent*) just as the function
/// @c QObject::eventFilter().The event filter will have @c obj as
/// parent, if successful. The strong exception guarantee is provided.
template <typename F>
void installEventFilter( QObject * obj, F && f )
{
    auto filter = makeEventFilter( std::forward<F>(f) );
    filter->setParent( obj );
    obj->installEventFilter( filter.get() );
    filter.release();
}


/// @brief Event filter class with customizable event filter function.
/// 
/// At construction a functor is passed which is moved or copied into 
/// the @c GenericEventFilter. When @c @QObject::eventFilter() is called
/// then its arguments will be forwarded to the moved or copied functor.
class GenericEventFilter 
    : public QObject
{
    Q_OBJECT
public:
    template <typename F>
    GenericEventFilter( F && f )
        : f(std::forward<F>(f))
    {
    }

    virtual bool eventFilter( QObject * receiver, QEvent * event )
    {
        return f(receiver,event);
    }

private:
    std::function<bool(QObject*,QEvent*)> f;
};

} // namespace qu
