// ReSharper disable CppDFAMemoryLeak
#include "create_instance_dialog.h"

#include <QDialogButtonBox>
#include <QVBoxLayout>

namespace Actinium
{
    CreateInstanceDialog::CreateInstanceDialog(QWidget* parent)
        : QDialog(parent)
    {
        setObjectName("CreateInstanceDialog");

        resize(320, 240);

        auto* button_box = new QDialogButtonBox(
            QDialogButtonBox::StandardButton::Ok | QDialogButtonBox::StandardButton::Cancel, this);
        button_box->setObjectName("ButtonBox");

        auto* main_layout = new QVBoxLayout(this);
        main_layout->addStretch();
        main_layout->addWidget(button_box, 0, Qt::AlignmentFlag::AlignRight);

        connect(button_box, &QDialogButtonBox::accepted, this, qOverload<>(&QDialog::accept));
        connect(button_box, &QDialogButtonBox::rejected, this, qOverload<>(&QDialog::reject));

        QMetaObject::connectSlotsByName(this);
    }
}
// ReSharper restore CppDFAMemoryLeak
