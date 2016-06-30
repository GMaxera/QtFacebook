import QtQuick 2.6
import QtQuick.Window 2.2
import QtQuick.Controls 1.4

Rectangle{
    id: mainScreen
    objectName: "MainScreen"
    property string firstname: "";
    property string lastname: "";

    signal facebookButtonClicked(string screenName)

    Label{
        id: greeting
        text: "Hello " + firstname + " " + lastname
        anchors.top: parent.top
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.topMargin: 100
    }

    Button{
        id: facebookLogoutButton
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.verticalCenter: parent.verticalCenter
        anchors.leftMargin: 5
        anchors.rightMargin: 5
        anchors.topMargin: 50

        Text{
            anchors.fill: parent
            text: "Logout"
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignHCenter
            MouseArea{
                anchors.fill: parent
                onClicked: mainScreen.facebookButtonClicked(mainScreen.objectName) //emit signal
            }
        }
    }
}
