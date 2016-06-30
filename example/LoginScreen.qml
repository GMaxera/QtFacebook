import QtQuick 2.6
import QtQuick.Window 2.2
import QtQuick.Controls 1.4

Rectangle{
    id: loginScreen
    objectName: "LoginScreen"
    signal facebookButtonClicked(string screenName)

    Button{
        id: facebookLoginButton
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.verticalCenter: parent.verticalCenter
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
