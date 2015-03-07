#include "exception_handling.h"

#include "invoke_in_thread.h"
#include "../cpp_utils/exception_handling.h"

#include <exception>
#include <QMessageBox>

namespace qu {

void handleException()
{
    auto p = std::current_exception();
    assert( p != std::exception_ptr() );
    const auto chain = cu::getExceptionChain( p );

    if ( cu::hasUserCancelledException(chain) )
        return;

    auto whatChain = cu::getWhatChain( chain );
    assert( !whatChain.empty() );

    whatChain.erase( std::remove(
            begin(whatChain), end(whatChain), "" )
        , end(whatChain) );

    std::string mainMessage;
    std::string reasons;
    switch ( whatChain.size() )
    {
    case 0:
        mainMessage =
            "A problem of unknown source occurred.";
        break;
    case 1:
        mainMessage =
            "A problem occured:";
        reasons = move(whatChain.front());
        break;
    default:
        mainMessage =
            "A problem occurred with the "
            "following chain of reasons:";
        {
            int i=0;
            for ( const auto & what : whatChain )
                reasons += "\t" + std::to_string(++i)
                        + ". " + what + '\n';
            reasons.pop_back();
        }
    }

    invokeInGuiThread( [=]()
    {
        QMessageBox msgBox;
        msgBox.setText( QString::fromStdString( mainMessage ) );
        msgBox.setInformativeText( QString::fromStdString( reasons ) );
        msgBox.exec();
    } );
}

}
