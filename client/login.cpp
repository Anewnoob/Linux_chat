#include "login.h"

login::login(QDialog *parent) : QDialog(parent){
    isVaild = false;
    tcp = new QTcpSocket();
    initLayout();
}

login::~login(){
    tcp->close();
}

void login::initLayout(){
    //layout configuration
}

QString login::execute(){
    this->exec();
    tcp->disconnectFromHost();
    if(isVaild) return usrName->text();
    return "";
}

void login::log(){
    if(tcp->state() == QAbstractSocket::ConnectedState) tcp->disconnectFromHost();
    tcp->connectToHost(AIM_IP, PORT_LOGIN);
}

void login::sign(){
    if(tcp->state() == QAbstractSocket::ConnectedState) tcp->disconnectFromHost();
    tcp->connectToHost(AIM_IP, PORT_SIGNUP);
}

void login::check(){	//check if login or signup successfully
    char response[2];
    tcp->read(response, 2);
    if(tcp->peerPort() == PORT_LOGIN){
        if(response[0] == 'Y'){isVaild = true;this->close();}
        else    showDialog("Invaild user or password\n");
    }else{
        if(response[0] == 'Y')  showDialog("Sign up successfully");
        else    showDialog("Sign up failed");
    }
}

void login::showDialog(QString hint){	//warning box
    msgBox->setWindowTitle("Warning");
    msgBox->setText(hint);
    msgBox->show();
}

void login::sendUsrInfo(){	//send user's infomation to father object
    QString name = usrName->text(), pswd = passwd->text();
    if(name.isEmpty()){
        showDialog("user name cannot be empty");
        return;
    }else if(pswd.isEmpty()){
        showDialog("password cannot be empty");
        return;
    }
    name.append("&").append(pswd);
    if(tcp->state() != QAbstractSocket::ConnectedState)    showDialog("please check your network");
    else{
        tcp->write(name.toLatin1());
        tcp->flush();
    }
}
