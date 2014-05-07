/** @file
  @author Ralph Tandetzky
  @date
*/

#pragma once

#include <QWidget>
#include <memory>

namespace cu { class ProgressInterface; }

namespace qu {

class ProgressWidget : public QWidget
{
    Q_OBJECT
    
public:
    explicit ProgressWidget(QWidget *parent = nullptr);
    ~ProgressWidget();

    void setOperationName( const QString & name );
    void setPauseButtonVisible( bool val = true );
    void setCancelButtonVisible( bool val = true );
    cu::ProgressInterface * getProgressInterface() const;
    std::function<void()> getAtExitFunction() const;
    void setAtExitFunction( std::function<void()> f );

private slots:
    void pause( bool shallPause );
    void cancel();

private:
    struct Impl;
    std::unique_ptr<Impl> m;
};


} // namespace qu
