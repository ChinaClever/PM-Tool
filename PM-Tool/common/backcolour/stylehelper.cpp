#include "stylehelper.h"

void StyleHelper::setLightBlueButton(QPushButton* button) {
    button->setStyleSheet(R"(
        QPushButton {
            background-color: #d0ebff;
            color: #003366;
            border: 1px solid #90caf9;
            border-radius: 6px;
            padding: 6px 12px;
        }
        QPushButton:hover {
            background-color: #a5d8ff;
        }
        QPushButton:pressed {
            background-color: #74c0fc;
        }
    )");
}
