/// @file
///
/// @author Ralph Tandetzky
/// @date 16 Oct 2013

#pragma once

#include "exception_handling.h"
#include <QApplication>

namespace qu {

class ExceptionHandlingApplication
        : public QApplication
{
    Q_OBJECT
public:
    template <typename...Ts>
    ExceptionHandlingApplication( Ts&&...ts )
        : QApplication( std::forward<Ts>(ts)... )
    {
    }

    bool notify(QObject * receiver, QEvent * e) override
    {
        bool result{};
        QU_HANDLE_ALL_EXCEPTIONS_FROM
        {
            result = QApplication::notify( receiver, e );
        };
        return result;
    }
};

}
