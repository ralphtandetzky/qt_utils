#include "gui_progress_manager.h"

#include "gui_progress_widget.h"
#include "invoke_in_thread.h"

#include "../cpp_utils/locking.h"
#include "../cpp_utils/progress.h"
#include "../cpp_utils/std_make_unique.h"

#include <QBoxLayout>
#include <atomic>
#include <memory>


namespace qu {

namespace { // unnamed

class ProgressWidgetProgress
    : public cu::ProgressInterface
{
public:
    ProgressWidgetProgress( ProgressWidget * progressWidget )
        : widget(std::make_shared<cu::Monitor<ProgressWidget*>>(progressWidget))
    {
        // Special care has to be taken in the case that the @c progressWidget
        // is destroyed before @c this. In that case the widget's
        // at-exit-function shall set the internal @c ProgressWidget pointer
        // to null.
        const auto sharedWidget = widget;
        const auto f = progressWidget->getAtExitFunction();
        progressWidget->setAtExitFunction( [sharedWidget,f]() mutable {
            (*sharedWidget)( []( ProgressWidget *& ptr ){ ptr = nullptr; } );
            if ( f )
                f();
        } );
    }

    virtual void setProgress( double progress ) override
    {
        // no op, if the associated ProgressWidget has been destroyed.
        (*widget)( [=]( ProgressWidget * const ptr ) {
            if ( ptr ) // widget still lives?
                ptr->getProgressInterface().setProgress( progress );
        } );
    }

    virtual bool shallAbort() const override
    {
        // returns true, if the associated ProgressWidget has been destroyed.
        return (*widget)( [=]( ProgressWidget * const ptr ) -> bool {
            if ( ptr ) // widget still lives?
                return ptr->getProgressInterface().shallAbort();
            return true;
        } );

    }

private:
    std::shared_ptr<cu::Monitor<ProgressWidget *>> widget;
};

} // unnamed namespace

struct ProgressWidgetContainer::Impl : ProgressManagerInterface
{
    Impl( QWidget * containerWidget )
        : containerWidget(containerWidget)
    {
        auto layout = std::make_unique<QVBoxLayout>( containerWidget );
        layout->addStretch(1);
        containerWidget->setLayout( layout.release() );
    }

    virtual std::unique_ptr<cu::ProgressInterface> createProgress(
            const QString & operationName ) override
    {
        return invokeInGuiThreadSync( [&]{
            auto progressWidget = std::make_unique<ProgressWidget>();
            progressWidget->setOperationName( operationName );
            auto progress = std::make_unique<ProgressWidgetProgress>( progressWidget.get() );
            containerWidget->layout()->addWidget( progressWidget.release() );
            return std::move(progress);
        } );
    }

    QWidget * containerWidget;
};

ProgressWidgetContainer::ProgressWidgetContainer( QWidget * parent )
    : QWidget(parent)
    , m( std::make_unique<Impl>(this) )
{
}

ProgressWidgetContainer::~ProgressWidgetContainer()
{
}

ProgressManagerInterface & ProgressWidgetContainer::getProgressManagerInterface()
{
    return *m;
}

const ProgressManagerInterface & ProgressWidgetContainer::getProgressManagerInterface() const
{
    return *m;
}


static std::atomic<ProgressManagerInterface *> globalProgressManager{ nullptr };

void setGlobalProgressManager( ProgressManagerInterface * manager )
{
    globalProgressManager = manager;
}

ProgressManagerInterface * getGlobalProgressManager()
{
    return globalProgressManager.load();
}

std::unique_ptr<cu::ProgressInterface> createProgress(const QString & operationName)
{
    const auto progressManager = getGlobalProgressManager();
    return progressManager ?
                progressManager->createProgress( operationName ) :
                std::make_unique<cu::ProgressForwarder>(nullptr);
}

} // namespace qu
