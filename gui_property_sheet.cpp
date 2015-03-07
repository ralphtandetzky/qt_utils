#include "gui_property_sheet.h"

#include "gui_user_parameter.h"

#include "../cpp_utils/std_make_unique.h"
#include "../cpp_utils/user_parameter.h"
#include "../cpp_utils/user_parameter_container.h"

#include <QWidget>
#include <QLabel>
#include <QGridLayout>

namespace qu {

std::unique_ptr<QWidget> createPropertySheet(
        const cu::UserParameterContainer & cont
        , QWidget * parent
        )
{
    auto result = std::make_unique<QWidget>(parent);
    auto layout = std::make_unique<QGridLayout>();
    const auto nParams = cont.getNParameters();
    for ( size_t row = 0; row != nParams; ++row )
    {
        const auto param = cont.getParameter( row );
        auto label   = createLabel  ( *param, result.get() );
        auto control = createControl( *param, result.get() );
        layout->addWidget( label  .release(), row, 0 );
        layout->addWidget( control.release(), row, 1 );
    }
    result->setLayout( layout.release() );
    return result;
}


void readFromPropertySheet(
        const QWidget & sheet
        , cu::UserParameterContainer & cont
        )
{
    const auto layout =
        dynamic_cast<QGridLayout*>( sheet.layout() );
    assert( layout );
    const auto nParams = cont.getNParameters();
    for ( size_t row = 0; row != nParams; ++row )
    {
        const auto item = layout->itemAtPosition( row, 1 );
        assert( item );
        const auto control = item->widget();
        assert( control );
        const auto param = cont.getParameter( row );
        assert( param );
        readFromControl( *control, *param );
        cont.setParameter( *param );
    }
}


void writeToPropertySheet(
        const cu::UserParameterContainer & cont
        , QWidget & sheet
        )
{
    const auto layout =
        dynamic_cast<QGridLayout*>( sheet.layout() );
    assert( layout );
    const auto nParams = cont.getNParameters();
    for ( size_t row = 0; row != nParams; ++row )
    {
        const auto item = layout->itemAtPosition( row, 1 );
        assert( item );
        const auto control = item->widget();
        assert( control );
        const auto param = cont.getParameter( row );
        assert( param );
        writeToControl( *param, *control );
    }
}

} // namespace cu
