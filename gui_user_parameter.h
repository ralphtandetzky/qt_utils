/// @file
///
/// @author Ralph Tandetzky
/// @date 25 Sep 2013

#pragma once

#include <memory>

// forward declaration
namespace cu { class UserParameter; }
class QWidget;
class QLabel;

namespace qu
{


std::unique_ptr<QWidget> createControl(
        const cu::UserParameter & param
        , QWidget * parent
        );

std::unique_ptr<QLabel> createLabel(
        const cu::UserParameter & param
        , QWidget * parent
        );

void readFromControl(
        const QWidget & control
        , cu::UserParameter & param
        );

void writeToControl(
        const cu::UserParameter & param
        , QWidget & control
        );


} // namespace qu
