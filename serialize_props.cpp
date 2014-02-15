#include "qt_utils/serialize_props.h"

#include "cpp_utils/std_make_unique.h"

#include <QCheckBox>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QSpinBox>
#include <QLineEdit>
#include <QPlainTextEdit>

#include <cassert>
#include <istream>
#include <ostream>

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

static std::string unescapeAllWhiteSpaces( const std::string & s )
{
    auto result = std::string();
    if ( s == "\\0" )
        return result;
    auto it = begin(s);
    while ( it != s.end() )
    {
        auto c = *it;
        ++it;
        if ( c!='\\' )
        {
            result.push_back(c);
            continue;
        }
        c = *it;
        ++it;
        switch ( c )
        {
        case 't' : result.push_back( '\t' ); break;
        case 'n' : result.push_back( '\n' ); break;
        case 'v' : result.push_back( '\v' ); break;
        case 'f' : result.push_back( '\f' ); break;
        case 'r' : result.push_back( '\r' ); break;
        case '\\': result.push_back( '\\' ); break;
        case 's' : result.push_back( ' '  ); break;
        default:
            assert(!"Unknown whitespace character in string.");
            result.push_back(c);
            break;
        }
    }
    return result;
}

static std::string escapeAllWhiteSpaces( const std::string & s )
{
    if ( s.empty() )
        return "\\0";
    auto result = std::string();
    for ( auto c : s )
    {
        if ( !isspace(c) && c!='\\' )
        {
            result.push_back(c);
            continue;
        }
        result.push_back('\\');
        switch ( c )
        {
        case '\t': result.push_back( 't' ); break;
        case '\n': result.push_back( 'n' ); break;
        case '\v': result.push_back( 'v' ); break;
        case '\f': result.push_back( 'f' ); break;
        case '\r': result.push_back( 'r' ); break;
        case '\\': result.push_back( '\\'); break;
        case ' ' : result.push_back( 's' ); break;
        default:
            assert(!"Unknown whitespace character in string.");
            result.push_back(c);
            break;
        }
    }
    return result;
}

std::unique_ptr<PropertySerializer> createPropertySerializer( QLineEdit * obj )
{
    return createPropertySerializer( obj,
        []( std::istream & stream, QLineEdit * obj )
    {
        std::string s;
        stream >> s;
        obj->setText( QString::fromStdString(
            unescapeAllWhiteSpaces(s)) );
    },
        []( std::ostream & stream, const QLineEdit * obj )
    {
        stream << escapeAllWhiteSpaces(
            obj->text().toStdString() );
    } );
}

std::unique_ptr<PropertySerializer> createPropertySerializer( QPlainTextEdit *obj )
{
    return createPropertySerializer( obj,
        []( std::istream & stream, QPlainTextEdit * obj )
    {
        std::string s;
        stream >> s;
        obj->setPlainText( QString::fromStdString(
            unescapeAllWhiteSpaces(s)) );
    },
        []( std::ostream & stream, const QPlainTextEdit * obj )
    {
        stream << escapeAllWhiteSpaces(
            obj->toPlainText().toStdString() );
    } );
}

std::unique_ptr<PropertySerializer> createPropertySerializer( QTabWidget *obj )
{
    return createPropertySerializer( obj,
        []( std::istream & stream, QTabWidget * obj )
    {
        int index;
        stream >> index;
        if ( index < obj->count() )
            obj->setCurrentIndex( index );
    },
        []( std::ostream & stream, const QTabWidget * obj )
    {
        stream << obj->currentIndex();
    } );
}

} // namespace qu
