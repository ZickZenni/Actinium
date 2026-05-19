#pragma once

#include <QComboBox>
#include <QDialog>
#include <QDialogButtonBox>
#include <QLineEdit>
#include <QVBoxLayout>

namespace Actinium
{
    class CreateInstanceDialog : public QDialog
    {
    public:
        explicit CreateInstanceDialog(QWidget *parent = nullptr);

        [[nodiscard]] QString GetNameValue() const
        {
            return m_name_input->text();
        }

        [[nodiscard]] QString GetGameValue() const
        {
            return m_game_combo->currentText();
        }

    private:
        QVBoxLayout *m_central_layout;
        QDialogButtonBox* m_button_box;
        QLineEdit *m_name_input;
        QComboBox *m_game_combo;

        void ValidateInput() const;
    };
}