import QtQuick 2.6
import QtQuick.Window 2.2
import QtQuick.Controls 1.4

Rectangle{
    id: loginScreen
    objectName: "LoginScreen"
    signal facebookButtonClicked(string screenName)
    //anchors.fill: parent
    /*TextField{
        id: usernameInput
        placeholderText: "username"
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.leftMargin: 5
        anchors.rightMargin: 5
        anchors.topMargin: 100
    }
    TextField{
        id: passwordInput
        placeholderText: "password"
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: usernameInput.bottom
        anchors.topMargin: 10
        anchors.leftMargin: 5
        anchors.rightMargin: 5
    }*/

    Button{
        id: facebookLoginButton
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.verticalCenter: parent.verticalCenter
        //anchors.top: passwordInput.bottom
        //anchors.topMargin: 100
        anchors.leftMargin: 5
        anchors.rightMargin: 5

        Text{
            anchors.fill: parent
            text: "Login with Facebook"
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignHCenter
            MouseArea{
                anchors.fill: parent
                onClicked: loginScreen.facebookButtonClicked(loginScreen.objectName) //emit signal
            }
        }
    }

}
