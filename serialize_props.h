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

namespace qu {

class PropertySerializer
{
public:
    virtual ~PropertySerializer() {}
    virtual QObject * getObject() const = 0;
    virtual void read (std::istream&) = 0;
    virtual void write(std::ostream&) = 0;
    virtual std::unique_ptr<PropertySerializer> clone() const = 0;
};


std::unique_ptr<PropertySerializer> createPropertySerializer( QCheckBox      * obj );
std::unique_ptr<PropertySerializer> createPropertySerializer( QSpinBox       * obj );
std::unique_ptr<PropertySerializer> createPropertySerializer( QDoubleSpinBox * obj );
std::unique_ptr<PropertySerializer> createPropertySerializer( QComboBox      * obj );

template <typename Container, typename OutputIterator>
void appendPropertySerializers(
        const Container & orig,
        OutputIterator out )
{
    auto first = orig.cbegin();
    const auto last = orig.cend();
    while ( first != last )
    {
        *out = createPropertySerializer( *first );
        ++first;
        ++out;
    }
}

template <typename Container>
void writeProperties( std::ostream & stream, const Container & container )
{
    for ( const auto & p : container )
    {
        stream << p->getObject()->objectName().toStdString() << ' ';
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
