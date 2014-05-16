#include "gui_progress_widget.h"
#include "ui_gui_progress_widget.h"

#include "../cpp_utils/locking.h"
#include "../cpp_utils/progress_interface.h"
#include "../cpp_utils/std_make_unique.h"

#include <condition_variable>

static const int maxProgressValue = 10000;

namespace qu {

struct ProgressWidget::Impl final : cu::ProgressInterface
{
    ////////////////////////////////////////////////
    // Implementation of @c cu::ProgressInterface //
    ////////////////////////////////////////////////
    virtual void setProgress( double progress ) override
    {
        int val = maxProgressValue * progress;
        QMetaObject::invokeMethod( ui.progressBar, "setValue", Q_ARG(int,val) );
    }

    virtual bool shallAbort() const override
    {
        return shared.withUniqueLock( []( const Shared & shared
            , std::unique_lock<std::mutex> lock )
        {
            shared.cv.wait( lock, [&shared]{ return !shared.isPaused; } );
            return shared.isCancelled;
        } );
    }

    //////////
    // data //
    //////////

    std::function<void()> atExitFunc;
    Ui::ProgressWidget ui;

    struct Shared
    {
        mutable std::condition_variable_any cv;
        bool isCancelled = false;
        bool isPaused = false;
    };

    cu::Monitor<Shared> shared;
};


ProgressWidget::ProgressWidget(QWidget *parent) :
    QWidget(parent),
    m( std::make_unique<Impl>() )
{
    m->ui.setupUi(this);
    m->ui.progressBar->setMaximum( maxProgressValue );
    setOperationName({});
}

ProgressWidget::~ProgressWidget()
{
    if ( m->atExitFunc )
        m->atExitFunc();
}

void ProgressWidget::setOperationName( const QString & name )
{
    m->ui.progressBar->setFormat(
        name.isEmpty() ? "%p %" : name + "  -  %p %" );
}

void ProgressWidget::setPauseButtonVisible( bool val )
{
    m->ui.pauseButton->setVisible( val );
}

void ProgressWidget::setCancelButtonVisible( bool val )
{
    m->ui.cancelButton->setVisible( val );
}

cu::ProgressInterface & ProgressWidget::getProgressInterface() const
{
    return *m;
}

std::function<void()> ProgressWidget::getAtExitFunction() const
{
    return m->atExitFunc;
}

void ProgressWidget::setAtExitFunction( std::function<void()> f )
{
    swapAtExitFunction( f );
}

void ProgressWidget::swapAtExitFunction( std::function<void()> & f ) noexcept
{
    m->atExitFunc.swap(f);
}

void ProgressWidget::pause( bool shallPause )
{
    m->shared( [=]( Impl::Shared & shared ) {
        shared.isPaused = shallPause;
        shared.cv.notify_all();
    } );
}

void ProgressWidget::cancel()
{
    m->shared( [=]( Impl::Shared & shared ) {
        shared.isPaused = false;
        shared.isCancelled = true;
        shared.cv.notify_all();
    } );
}

} // namespace qu
