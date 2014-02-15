/// \file This header file provides functionality for serializing properties
///     of @c QWidgets. This helps to set up the same parameters in the gui
///     that were present when the gui last closed down.
///
/// \author Ralph Tandetzky
/// \date 30 Aug 2013

#pragma once

#include <algorithm>
#include <memory>
#include <ostream>
#include <istream>

#include <QString>

// forward declarations
class QObject;
class QCheckBox;
class QSpinBox;
class QDoubleSpinBox;
class QComboBox;
class QLineEdit;
class QPlainTextEdit;
class QTabWidget;

namespace qu {

/// \brief An interface which helps to serialize properties of \c QObjects.
///
/// The value being serialized should be written without any line-breaking
/// characters, so that it is possible to skip reading a value by
/// \c std::getline(stream,string), if a \c '\n' character is written after
/// the serialized value.
class PropertySerializer
{
public:
    virtual ~PropertySerializer() {}
    virtual QObject * getObject() const = 0;
    virtual void read (std::istream&) = 0;
    virtual void write(std::ostream&) = 0;
    virtual std::unique_ptr<PropertySerializer> clone() const = 0;
};

/// \brief Factory function which creates a serializer for a \c QWidget.
///
/// The input value of the widget can be loaded and stored by the
/// returned \c PropertySerializer.
std::unique_ptr<PropertySerializer> createPropertySerializer( QCheckBox      * obj );
std::unique_ptr<PropertySerializer> createPropertySerializer( QSpinBox       * obj );
std::unique_ptr<PropertySerializer> createPropertySerializer( QDoubleSpinBox * obj );
std::unique_ptr<PropertySerializer> createPropertySerializer( QComboBox      * obj );
std::unique_ptr<PropertySerializer> createPropertySerializer( QLineEdit      * obj );
std::unique_ptr<PropertySerializer> createPropertySerializer( QPlainTextEdit * obj );
std::unique_ptr<PropertySerializer> createPropertySerializer( QTabWidget     * obj );

/// \brief Creates \c PropertySerializers for the elements of a container.
///
/// \param orig This must be an STL-compatible container providing the
/// \c cbegin() and \c cend() member functions. Furthermore, the value type
/// of the container must be a parameter type of one of the overloads of
/// createPropertySerializer.
///
/// \param out This is where the created \c PropertySerializers will be
/// written to. It must be an \c OutputIterator with the value type
/// \c std::unique_ptr<PropertySerializer>.
///
/// \example The typical use of this function is the following inside the
/// constructor of a class derived from \c QWidget.
/// \code
///     std::vector<std::unique_ptr<PropertySerializer>> serializers;
///     qu::appendPropertySerializers(
///         this->findChildren<QCheckBox*>(),
///         std::back_inserter( m->serializers ) );
///     qu::appendPropertySerializers(
///         this->findChildren<QSpinBox*>(),
///         std::back_inserter( m->serializers ) );
/// \endcode
template <typename Container, typename OutputIterator>
void createPropertySerializers(
        const Container & orig,
        OutputIterator out )
{
    auto first = orig.begin();
    const auto last = orig.end();
    while ( first != last )
    {
        *out = createPropertySerializer( *first );
        ++first;
        ++out;
    }
}

/// \brief Writes a container of \c PropertySerializer pointers to a stream.
///
/// The pointer type can be raw pointers, \c std::unique_ptrs,
/// \c std::shared_ptrs or other smart pointers implementing the
/// \c operator->() function.
///
/// @note All objects whose object name is @c "qt_spinbox_lineedit" will
/// not be serialized, since Qt uses this name for @c QLineEdits that
/// are used internally to build @c QSpinBoxes and @c QDoubleSpinBoxes.
/// The widgets with this name are skipped on purpose, since these
/// @c QSpinBoxes and @c QDoubleSpinBoxes can easily be serialized
/// and it is normally not the users intention to serialize the internal
/// @c QLineEdits of these widgets. If you still want to serialize these
/// widgets consider to rename them.
template <typename Container>
void writeProperties( std::ostream & stream, const Container & container )
{
    for ( const auto & p : container )
    {
        auto name = p->getObject()->objectName().toStdString();
        // skip QLineEdits that are used internally by Qt for QSpinBoxes
        if ( name == "qt_spinbox_lineedit" )
            continue;
        stream << name << ' ';
        p->write( stream );
        stream << std::endl;
    }
}

template <typename Container>
void readProperties( std::istream & stream, const Container & container )
{
    while ( stream.good() )
    {
        std::string name;
        stream >> name;
        const auto qName = QString::fromStdString(name);
        bool read = false;
        for ( const auto & p : container )
        {
            if ( p->getObject()->objectName() == qName )
            {
                p->read( stream );
                read = true;
                break;
            }
        }
        if ( !read )
            std::getline( stream, name );
    }

}

} // namespace qu
