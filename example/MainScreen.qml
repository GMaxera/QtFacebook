import QtQuick 2.6
import QtQuick.Window 2.2
import QtQuick.Controls 1.4

Rectangle{
    id: mainScreen
    objectName: "MainScreen"
    //anchors.fill: parent
    signal facebookButtonClicked(string screenName)
    signal loadData()

    onLoadData: {
        console.log("load data")
    }

    Button{
        id: facebookLogoutButton
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.verticalCenter: parent.verticalCenter
        anchors.leftMargin: 5
        anchors.rightMargin: 5

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
