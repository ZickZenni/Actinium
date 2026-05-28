#pragma once

#include <QComboBox>
#include <QDialog>
#include <QDialogButtonBox>
#include <QLineEdit>

namespace Actinium
{
    class CreateInstanceDialog : public QDialog
    {
        Q_OBJECT

    public:
        explicit CreateInstanceDialog(QWidget *parent = nullptr);

        /**
         * Retrieves the instance name the user inputed.
         */
        [[nodiscard]] QString GetInstanceName() const
        {
            return m_name_input->text().trimmed();
        }

        /**
         * Retrieves the id of the game the user has chosen.
         */
        [[nodiscard]] QString GetGameId() const
        {
            return m_game_combo->currentData().toString();
        }

    private:
        QDialogButtonBox *m_button_box;
        QLineEdit *m_name_input;
        QComboBox *m_game_combo;

        /**
         * Validates the user given input.
         */
        void ValidateInput() const;
    };
}