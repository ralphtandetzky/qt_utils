/// @file
///
/// @author Ralph Tandetzky
/// @date 16 Oct 2013

#pragma once

namespace qu {

#define QU_HANDLE_ALL_EXCEPTIONS_FROM \
    ::qu::exception_detail::ExceptionHandlerImpl() += [&]

void handleException();

namespace exception_detail
{
    struct ExceptionHandlerImpl
    {
        template <typename F>
        void operator+=( F && f ) const
        {
            try
            {
                f();
            }
            catch( ... )
            {
                handleException();
            }
        }
    };
}

}
