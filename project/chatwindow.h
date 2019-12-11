#ifndef CHATWINDOW_H
#define CHATWINDOW_H

#include <QWidget>
#include <QAbstractSocket>
class ChatClient;
class QStandardItemModel;
namespace Ui { class ChatWindow; }
class ChatWindow : public QWidget
{
    Q_OBJECT
    Q_DISABLE_COPY(ChatWindow)
public:
    explicit ChatWindow(QWidget *parent = nullptr);
    ~ChatWindow() override;
private:
    Ui::ChatWindow *ui_m;
    ChatClient *chat_client_m;
    QStandardItemModel *chat_model_m;
    QString last_username_m;
private slots:
    void attempt_connection();
    void connected_to_server();
    void attempt_login(const QString &user_name, const QString &room_name);
    void logged_in();
    void message_received(const QString &sender, const QString &text);
    void send_message();
    void user_joined(const QString &username);
    void user_left(const QString &username);
    void error();
};

#endif // CHATWINDOW_H
