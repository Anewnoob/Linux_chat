#include "chatwindow.h"

chatwindow::chatwindow(QString name, QWidget *parent) : QWidget(parent), peerName(name){
    //set layout
}

chatwindow::~chatwindow(){
    //close subwindows
}

void chatwindow::setChatLayout(QString name){
    //layout configuration
}

void chatwindow::sendToSock(){
    QString msg = input->toPlainText();	//get input
    if(msg.isEmpty())   return;	//check if empty
    output->append("Me:"+msg);
    sendMsg(peerName+">"+msg);	//send message to father window
    input->clear();
}

void chatwindow::fetchRcd(QStringList record){
    if(record.isEmpty())    return;
    foreach(QString msg, record){
        if(msg[0] == '#'){
            QString contain, name = firstBound(msg, contain);
            //get message sender and context
            showOnScreen(name, contain);
        }else   showOnScreen(peerName, msg);
    }
}

void chatwindow::showOnScreen(QString sender, QString msg){
    if(sender == peerName)  output->append(peerName+':'+msg);
    else if(sender[0] == '#' && peerName == "ALL")  output->append(QString(sender.mid(1))+':'+msg);
}

QString chatwindow::firstBound(QString src, QString& msg){
    int i = -1;
    while(src[++i] != '>');
    msg = src.mid(i+1);
    return src.mid(0, i);
}
