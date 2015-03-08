/** @file
  @author Ralph Tandetzky
  @date 08 Mar 2015
*/

#pragma once

#include <QGraphicsItem>
#include <functional>

namespace qu
{

template <typename T>
class EventHandlingGraphicsItem
        : public T
{
public:
    using T::T;

#define IMPLEMENT_EVENT_HANDLER( EventName, eventName, EventType ) \
public: \
    std::function<void(EventHandlingGraphicsItem*,EventType*)> get##EventName##Handler() \
    { \
        return eventName##Handler; \
    } \
\
    template <typename F> \
    void set##EventName##Handler( F && f ) \
    { \
        eventName##Handler = std::forward<F>(f); \
    }\
\
protected: \
    virtual void eventName(EventType * event) override \
    { \
        get##EventName##Handler()(this,event); \
    } \
\
private: \
    std::function<void(EventHandlingGraphicsItem*,EventType*)> eventName##Handler \
        = getDefault##EventName##Handler(); \
\
    std::function<void(EventHandlingGraphicsItem*,EventType*)> \
        getDefault##EventName##Handler() \
    { \
        return [](EventHandlingGraphicsItem * item, EventType * event) \
            { item->callBase_##eventName(event); }; \
    } \
\
    void callBase_##eventName(EventType * event) \
    { \
        T::eventName( event ); \
    }

    IMPLEMENT_EVENT_HANDLER( DragEnterEvent       , dragEnterEvent       , QGraphicsSceneDragDropEvent )
    IMPLEMENT_EVENT_HANDLER( DragLeaveEvent       , dragLeaveEvent       , QGraphicsSceneDragDropEvent )
    IMPLEMENT_EVENT_HANDLER( DragMoveEvent        , dragMoveEvent        , QGraphicsSceneDragDropEvent )
    IMPLEMENT_EVENT_HANDLER( DropEvent            , dropEvent            , QGraphicsSceneDragDropEvent )
    IMPLEMENT_EVENT_HANDLER( FocusInEvent         , focusInEvent         , QFocusEvent                 )
    IMPLEMENT_EVENT_HANDLER( FocusOutEvent        , focusOutEvent        , QFocusEvent                 )
    IMPLEMENT_EVENT_HANDLER( HoverEnterEvent      , hoverEnterEvent      , QGraphicsSceneHoverEvent    )
    IMPLEMENT_EVENT_HANDLER( HoverLeaveEvent      , hoverLeaveEvent      , QGraphicsSceneHoverEvent    )
    IMPLEMENT_EVENT_HANDLER( HoverMoveEvent       , hoverMoveEvent       , QGraphicsSceneHoverEvent    )
    IMPLEMENT_EVENT_HANDLER( InputMethodEvent     , inputMethodEvent     , QInputMethodEvent           )
    IMPLEMENT_EVENT_HANDLER( KeyPressEvent        , keyPressEvent        , QKeyEvent                   )
    IMPLEMENT_EVENT_HANDLER( KeyReleaseEvent      , keyReleaseEvent      , QKeyEvent                   )
    IMPLEMENT_EVENT_HANDLER( MouseDoubleClickEvent, mouseDoubleClickEvent, QGraphicsSceneMouseEvent    )
    IMPLEMENT_EVENT_HANDLER( MouseMoveEvent       , mouseMoveEvent       , QGraphicsSceneMouseEvent    )
    IMPLEMENT_EVENT_HANDLER( MousePressEvent      , mousePressEvent      , QGraphicsSceneMouseEvent    )
    IMPLEMENT_EVENT_HANDLER( MouseReleaseEvent    , mouseReleaseEvent    , QGraphicsSceneMouseEvent    )
    IMPLEMENT_EVENT_HANDLER( WheelEvent           , wheelEvent           , QGraphicsSceneWheelEvent    )

#undef IMPLEMENT_EVENT_HANDLER
};

} // namespace qu
