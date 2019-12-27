/********************************************************************************
** Form generated from reading UI file 'chatwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.12.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CHATWINDOW_H
#define UI_CHATWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QListView>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_ChatWindow
{
public:
    QGridLayout *gridLayout;
    QPushButton *connectButton;
    QListView *chatView;
    QLineEdit *messageEdit;
    QPushButton *sendButton;

    void setupUi(QWidget *ChatWindow)
    {
        if (ChatWindow->objectName().isEmpty())
            ChatWindow->setObjectName(QString::fromUtf8("ChatWindow"));
        ChatWindow->resize(400, 300);
        gridLayout = new QGridLayout(ChatWindow);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        connectButton = new QPushButton(ChatWindow);
        connectButton->setObjectName(QString::fromUtf8("connectButton"));

        gridLayout->addWidget(connectButton, 0, 0, 1, 2);

        chatView = new QListView(ChatWindow);
        chatView->setObjectName(QString::fromUtf8("chatView"));
        chatView->setEnabled(false);
        chatView->setEditTriggers(QAbstractItemView::NoEditTriggers);

        gridLayout->addWidget(chatView, 1, 0, 1, 2);

        messageEdit = new QLineEdit(ChatWindow);
        messageEdit->setObjectName(QString::fromUtf8("messageEdit"));
        messageEdit->setEnabled(false);

        gridLayout->addWidget(messageEdit, 2, 0, 1, 1);

        sendButton = new QPushButton(ChatWindow);
        sendButton->setObjectName(QString::fromUtf8("sendButton"));
        sendButton->setEnabled(false);

        gridLayout->addWidget(sendButton, 2, 1, 1, 1);


        retranslateUi(ChatWindow);

        sendButton->setDefault(true);


        QMetaObject::connectSlotsByName(ChatWindow);
    } // setupUi

    void retranslateUi(QWidget *ChatWindow)
    {
        ChatWindow->setWindowTitle(QApplication::translate("ChatWindow", "Capy Chat", nullptr));
        connectButton->setText(QApplication::translate("ChatWindow", "Connect to new room", nullptr));
        sendButton->setText(QApplication::translate("ChatWindow", "Send", nullptr));
    } // retranslateUi

};

namespace Ui {
    class ChatWindow: public Ui_ChatWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CHATWINDOW_H
