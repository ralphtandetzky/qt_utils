#pragma once

#include <QWidget>
#include <memory>

namespace cu { class ProgressInterface; }

namespace qu {

class ProgressManagerInterface
{
public:
    virtual ~ProgressManagerInterface() {}

    /// This function must be implemented thread-safely by all
    /// implementation classes.
    virtual std::unique_ptr<cu::ProgressInterface> createProgress(
            const QString & operationName ) = 0;
};

class ProgressWidgetContainer : public QWidget
{
    Q_OBJECT

public:
    ProgressWidgetContainer( QWidget * parent = nullptr );
    ~ProgressWidgetContainer();

    ProgressManagerInterface & getProgressManagerInterface();
    const ProgressManagerInterface & getProgressManagerInterface() const;

private:
    struct Impl;
    std::unique_ptr<Impl> m;
};

void setGlobalProgressManager( ProgressManagerInterface * manager );
ProgressManagerInterface * getGlobalProgressManager();

std::unique_ptr<cu::ProgressInterface> createProgress(
        const QString & operationName);

} // namespace qu
