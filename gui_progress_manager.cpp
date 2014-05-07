#include "gui_progress_manager.h"

#include "gui_progress_widget.h"

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
        : shared()
    {
        // Special care has to be taken in the case that the @c progressWidget
        // is destroyed before @c this. In that case the widget's
        // at-exit-function shall set the internal @c progressWidget pointer
        // to null. If @c this is destroyed first, then the destructor will
        // restore the old at-exit-function, so the at-exit function does not
        // access a destroyed object.
        shared( [=]( Shared & d )
        {
            d.progressWidget = progressWidget;
            const auto tmpFunc = d.progressWidget->getAtExitFunction();
            d.oldAtExitFunc = [=]{
                this->shared( []( Shared & d )
                {
                    d.progressWidget = nullptr;
                });
                tmpFunc();
            };
            d.progressWidget->swapAtExitFunction( d.oldAtExitFunc );
        });
    }

    virtual ~ProgressWidgetProgress()
    {
        // See comments in the destructor for an explanation.
        shared( [=]( Shared & d )
        {
            if ( d.progressWidget )
                d.progressWidget->swapAtExitFunction( d.oldAtExitFunc );
        });
    }

    virtual void setProgress( double progress ) override
    {
        // no op, if the associated ProgressWidget has been destroyed.
        shared( [=]( Shared & d ){
            if ( d.progressWidget )
                d.progressWidget->getProgressInterface().setProgress( progress );
        } );
    }

    virtual bool shallAbort() const override
    {
        // returns true, if the associated ProgressWidget has been destroyed.
        return shared( []( const Shared & d ){
            return d.progressWidget ?
                    d.progressWidget->getProgressInterface().shallAbort() :
                    true;
        } );
    }

private:
    struct Shared
    {
        ProgressWidget * progressWidget = nullptr;
        std::function<void()> oldAtExitFunc;
    };

    cu::Monitor<Shared> shared;
};

} // unnamed namespace

struct ProgressWidgetContainer::Impl : ProgressManagerInterface
{
    Impl( QWidget * containerWidget )
        : containerWidget(containerWidget)
    {
        auto layout = std::make_unique<QVBoxLayout>( containerWidget );
        containerWidget->setLayout( layout.release() );
    }

    virtual std::unique_ptr<cu::ProgressInterface> createProgress(
            const QString & operationName ) override
    {
        auto progressWidget = std::make_unique<ProgressWidget>();
        progressWidget->setOperationName( operationName );
        auto progress = std::make_unique<ProgressWidgetProgress>( progressWidget.get() );
        containerWidget->layout()->addWidget( progressWidget.release() );
        return std::move(progress);
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
