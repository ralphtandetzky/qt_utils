#pragma once

#include <memory>

class QWidget;
namespace cu { class UserParameterContainer; }


namespace qu {

std::unique_ptr<QWidget> createPropertySheet(
        const cu::UserParameterContainer & cont
        , QWidget * parent
        );

void readFromPropertySheet(
        const QWidget & sheet
        , cu::UserParameterContainer & cont
        );

void writeToPropertySheet(
        const cu::UserParameterContainer & cont
        , QWidget & sheet
        );

} // namespace cu
