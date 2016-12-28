#include "widget.h"
#include "ui_widget.h"

#define NAME_LEN 20
#define MAX_LEN 1000

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget){
    ui->setupUi(this);
    if((usrName = (new login())->execute()).isEmpty())   exit(0);
    setupPanel();
}

Widget::~Widget(){
    updateSock->write(QByteArray("-").append(usrName.toLatin1()));	//to tell server current user has logout
    updateSock->flush();
    updateSock->disconnectFromHost();
    updateSock->close();
    delete ui;
}

void Widget::setupPanel(){
    //layout configuration
}

void Widget::sendName(){updateSock->write(QByteArray("+").append(usrName.toLatin1()));}
//I just forget what's aim of these two...
void Widget::sendName2(){chatSock->write(usrName.toLatin1());}

void Widget::updateOnline(){	//update online user list
    char logInfo[NAME_LEN];
    while(updateSock->read(logInfo, NAME_LEN) > 0){
        QString changedName = QString(logInfo+1);
        if(logInfo[0] == '+'){
            QListWidgetItem *tempItem = new QListWidgetItem(changedName);
            friendItems.insert(tempItem, changedName);
            usrList->addItem(tempItem);
            //if new user has login, add a new item to the user list
        }else if(logInfo[0] == '-'){
            QListWidgetItem *tempItem = friendItems.keys(changedName)[0];
            usrList->removeItemWidget(tempItem);
            friendItems.remove(tempItem);
            delete tempItem;
            //else delete the logout user
        }
    }
}

void Widget::openChatWindow(QListWidgetItem *frd){
    QString name = friendItems[frd];
    if(openedList.contains(name))    return;
    if(frd->text().startsWith(">>> "))  frd->setText(name);
    openedList.append(name);
    chatwindow *chatWindow = new chatwindow(name);
    chatWindow->show();
    pushRcd(recorder[name]);
}

void Widget::sendMsg(QString msg){	//send message through the tcp socket
    chatSock->write(msg.toLatin1());
    chatSock->flush();
}

void Widget::recvMsg(QString sender, QString msg){emit sendToWindow(sender, msg);}

void Widget::clearRcd(QString name){recorder[name].clear();}
//clear the history record

void Widget::storeRcd(){
//if someone is chatting with you, however you haven't open the corresponding window, this function can store the chat history until you open the chat window.
    QString origin = chatSock->read(MAX_LEN), msg;
    QString sender = firstBound(origin, msg);
    if(openedList.contains(sender) || (sender[0] == '#' && openedList.contains("ALL"))) recvMsg(sender, msg);
    //service for single chat
    else{
        if(sender[0] != '#'){
            recorder[sender].append(msg);
            moveToTop(sender);
        }else   recorder["ALL"].append(origin);
    }//service for group chat
}

void Widget::moveToTop(QString name){	//if a coming chat, the user sending that message will be move to the top of the list and marked
    QListWidgetItem *item = friendItems.keys(name)[0];
    usrList->removeItemWidget(item);
    delete item;
    item = new QListWidgetItem(">>> "+name+" <<<");
    usrList->insertItem(1, item);
}

void Widget::removeName(QString name){openedList.removeAt(openedList.indexOf(name));}
//if someone logout, his/her name will be remove from the list

QString Widget::firstBound(QString src, QString& msg){
    int i = -1;
    while(src[++i] != '>');
    msg = src.mid(i+1);
    return src.mid(0, i);
}//split the message string
