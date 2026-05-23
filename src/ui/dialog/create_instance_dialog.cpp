// ReSharper disable CppDFAMemoryLeak
#include "create_instance_dialog.h"

#include "core/application.h"

#include <QLabel>
#include <QPushButton>

namespace Actinium
{
    CreateInstanceDialog::CreateInstanceDialog(QWidget* parent)
        : QDialog(parent)
        , m_central_layout(nullptr)
        , m_button_box(nullptr)
        , m_name_input(nullptr)
        , m_game_combo(nullptr)
    {
        setObjectName("CreateInstanceDialog");

        m_central_layout = new QVBoxLayout(this);
        m_central_layout->setObjectName("CentralLayout");

        {
            m_name_input = new QLineEdit();
            m_name_input->setObjectName("NameField");
            m_name_input->setPlaceholderText("New Instance");

            auto* name_label = new QLabel("Name:");
            name_label->setObjectName("NameLabel");

            auto* name_layout = new QHBoxLayout();
            name_layout->setObjectName("NameLayout");
            name_layout->addWidget(name_label);
            name_layout->addWidget(m_name_input);

            connect(m_name_input, &QLineEdit::textChanged, this, &CreateInstanceDialog::ValidateInput);

            m_central_layout->addLayout(name_layout);
        }

        {
            m_game_combo = new QComboBox();
            m_game_combo->setObjectName("GameField");
            m_game_combo->setPlaceholderText("Choose a Game");

            for (const auto& game : GAMES)
            {
                const auto name = QString::fromStdString(std::string(game.GetName()));
                const auto id = QString::fromStdString(std::string(game.GetId()));

                m_game_combo->addItem(name, id);
            }

            auto* game_label = new QLabel("Game:");
            game_label->setObjectName("GameLabel");

            auto* game_layout = new QHBoxLayout();
            game_layout->setObjectName("GameLayout");
            game_layout->addWidget(game_label);
            game_layout->addWidget(m_game_combo, 1);

            connect(m_game_combo, &QComboBox::currentIndexChanged, this, &CreateInstanceDialog::ValidateInput);

            m_central_layout->addLayout(game_layout);
        }

        {
            m_button_box = new QDialogButtonBox(
                QDialogButtonBox::StandardButton::Ok | QDialogButtonBox::StandardButton::Cancel, this);
            m_button_box->setObjectName("ButtonBox");
            m_button_box->button(QDialogButtonBox::StandardButton::Ok)->setText("Create");

            connect(m_button_box, &QDialogButtonBox::accepted, this, qOverload<>(&QDialog::accept));
            connect(m_button_box, &QDialogButtonBox::rejected, this, qOverload<>(&QDialog::reject));

            auto* button_box_layout = new QVBoxLayout();
            button_box_layout->setObjectName("ButtonBoxLayout");
            button_box_layout->addStretch();
            button_box_layout->addWidget(m_button_box, 0, Qt::AlignmentFlag::AlignRight);

            m_central_layout->addLayout(button_box_layout);
        }

        ValidateInput();
        QMetaObject::connectSlotsByName(this);
    }

    void CreateInstanceDialog::ValidateInput() const
    {
        if (m_button_box == nullptr || m_name_input == nullptr || m_game_combo == nullptr)
        {
            return;
        }

        const auto button = m_button_box->button(QDialogButtonBox::StandardButton::Ok);
        const auto name_input = m_name_input->text();
        const auto game_input = m_game_combo->currentText();

        button->setEnabled(!name_input.trimmed().isEmpty() && !game_input.trimmed().isEmpty());
    }
}
// ReSharper restore CppDFAMemoryLeak
