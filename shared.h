#pragma once

#include <QMessageBox>
#include <QString>
#include <QWidget>

inline constexpr auto SQLITE_CONSTRAINT_UNIQUE = "2067";

class Shared {
public:
    Shared() = delete;
    static inline void handle_error(QWidget* parent, const QString& code)
    {
        if (code == SQLITE_CONSTRAINT_UNIQUE) {
            QMessageBox::warning(parent, "Error", "Field must be unique. That one already exists in database.");
        } else {
            QMessageBox::warning(parent, "Error", "Unspecified error.");
        }
    }
};
