#include "qt_utils/serialize_props.h"

#include "cpp_utils/std_make_unique.h"

#include <QCheckBox>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QSpinBox>

#include <ostream>
#include <istream>

namespace qu {


template <typename Object, typename Reader, typename Writer>
static std::unique_ptr<PropertySerializer> createPropertySerializer(
        Object * obj, Reader r, Writer w )
{
    struct S final : PropertySerializer
    {
        S( Object * obj, Reader && r, Writer && w )
            : obj(obj), r(std::move(r)), w(std::move(w)) {}

        virtual QObject * getObject() const override
        {
            return obj;
        }

        virtual void read ( std::istream & stream ) override
        {
            r( stream, obj );
        }

        virtual void write( std::ostream & stream ) override
        {
            w( stream, obj );
        }

        virtual std::unique_ptr<PropertySerializer> clone() const override
        {
            return std::make_unique<S>(*this);
        }

        Object * obj;
        Reader r;
        Writer w;
    };

    return std::make_unique<S>( obj, std::move(r), std::move(w) );
}


std::unique_ptr<PropertySerializer> createPropertySerializer( QCheckBox * obj )
{
    return createPropertySerializer( obj,
        []( std::istream & stream, QCheckBox * obj )
    {
        bool b;
        stream >> b;
        obj->setChecked( b );
    },
        []( std::ostream & stream, const QCheckBox * obj )
    {
        stream << obj->isChecked();
    } );
}

std::unique_ptr<PropertySerializer> createPropertySerializer( QSpinBox * obj )
{
    return createPropertySerializer( obj,
        []( std::istream & stream, QSpinBox * obj )
    {
        int i;
        stream >> i;
        obj->setValue( i );
    },
        []( std::ostream & stream, const QSpinBox * obj )
    {
        stream << obj->value();
    } );
}

std::unique_ptr<PropertySerializer> createPropertySerializer( QDoubleSpinBox * obj )
{
    return createPropertySerializer( obj,
        []( std::istream & stream, QDoubleSpinBox * obj )
    {
        double d;
        stream >> d;
        obj->setValue( d );
    },
        []( std::ostream & stream, const QDoubleSpinBox * obj )
    {
        stream << obj->value();
    } );
}

std::unique_ptr<PropertySerializer> createPropertySerializer( QComboBox * obj )
{
    return createPropertySerializer( obj,
        []( std::istream & stream, QComboBox * obj )
    {
        int index;
        stream >> index;
        obj->setCurrentIndex( index );
    },
        []( std::ostream & stream, const QComboBox * obj )
    {
        stream << obj->currentIndex();
    } );
}

} // namespace qu
