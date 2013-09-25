#include "gui_user_parameter.h"
#include "cpp_utils/user_parameter.h"
#include "cpp_utils/std_make_unique.h"
#include "cpp_utils/scope_guard.h"

#include <QDoubleSpinBox>
#include <QSpinBox>
#include <QCheckBox>
#include <memory>

using namespace cu;

namespace qu
{

namespace {
    class CreateControlVisitor final
            : public ConstUserParameterVisitor
    {
    public:
        CreateControlVisitor( QWidget * parent )
            : parent(parent)
        {
        }

        std::unique_ptr<QWidget> getResult()
        {
            return std::move(result);
        }

    protected:
        virtual void visit( const RealUserParameter & param )
        {
            auto w = std::make_unique<QDoubleSpinBox>(parent);
            SCOPE_SUCCESS { result = std::move(w); };
            w->setMinimum( param.getLowerBound() );
            w->setMaximum( param.getUpperBound() );
            w->setSingleStep( param.getStepSize() );
            w->setDecimals( param.getNDecimals() );
            w->setSuffix( QString::fromStdString(param.getSuffix()) );
            w->setValue( param.getValue() );
            w->setToolTip( QString::fromStdString(
                               param.getDescription() ) );
        }

        virtual void visit( const IntUserParameter & param )
        {
            auto w = std::make_unique<QSpinBox>(parent);
            SCOPE_SUCCESS { result = std::move(w); };
            w->setMinimum( param.getLowerBound() );
            w->setMaximum( param.getUpperBound() );
            w->setSingleStep( param.getStepSize() );
            w->setValue( param.getValue() );
            w->setToolTip( QString::fromStdString(
                               param.getDescription() ) );
        }

        virtual void visit( const BoolUserParameter & param )
        {
            auto w = std::make_unique<QCheckBox>(parent);
            SCOPE_SUCCESS { result = std::move(w); };
            w->setChecked( param.getValue() );
            w->setToolTip( QString::fromStdString(
                               param.getDescription() ) );
        }

    private:
        std::unique_ptr<QWidget> result;
        QWidget * parent;
    };
} // unnamed namespace

std::unique_ptr<QWidget> createControl(
        const UserParameter & param
        , QWidget * parent
        )
{
    CreateControlVisitor v( parent );
    param.accept( v );
    return v.getResult();
}


namespace {
    class ReadFromControlVisitor final
            : public UserParameterVisitor
    {
    public:
        ReadFromControlVisitor( const QWidget & control )
            : control(control)
        {
        }

    protected:
        virtual void visit( RealUserParameter & param )
        {
            const auto & w =
                    dynamic_cast<const QDoubleSpinBox &>(control);
            param.setValue( w.value() );
        }

        virtual void visit( IntUserParameter & param )
        {
            const auto & w =
                    dynamic_cast<const QSpinBox &>(control);
            param.setValue( w.value() );
        }

        virtual void visit( BoolUserParameter & param )
        {
            const auto & w =
                    dynamic_cast<const QCheckBox &>(control);
            param.setValue( w.isChecked() );
        }

    private:
        const QWidget & control;
    };
} // unnamed namespace

void readFromControl(
        const QWidget & control
        , cu::UserParameter & param
        )
{
    ReadFromControlVisitor v( control );
    param.accept( v );
}


namespace {
    class WriteToControlVisitor final
            : public ConstUserParameterVisitor
    {
    public:
        WriteToControlVisitor( QWidget & control )
            : control(control)
        {
        }

    protected:
        virtual void visit( const RealUserParameter & param )
        {
            auto & w = dynamic_cast<QDoubleSpinBox &>(control);
            w.setValue( param.getValue() );
        }

        virtual void visit( const IntUserParameter & param )
        {
            auto & w = dynamic_cast<QSpinBox &>(control);
            w.setValue( param.getValue() );
        }

        virtual void visit( const BoolUserParameter & param )
        {
            auto & w = dynamic_cast<QCheckBox &>(control);
            w.setChecked( param.getValue() );
        }

    private:
        QWidget & control;
    };
} // unnamed namespace

void writeToControl(
        const cu::UserParameter & param
        , QWidget & control
        )
{
    WriteToControlVisitor v( control );
    param.accept( v );
}

} // namespace qu
