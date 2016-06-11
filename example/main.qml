import QtQuick 2.6
import QtQuick.Window 2.2
import org.qtproject.example 1.0
import QtQuick.Controls 1.4
import "."

Window {
    property var facebook : Facebook
    width: 400
    height: 600
    visible: true

    /*Facebook{
        id: facebook
    }*/

    Loader{
        id: screenLoader
        source: "LoginScreen.qml"
        anchors.fill: parent
    }

    Connections {
        target: screenLoader.item
        onFacebookButtonClicked: {
            if(screenName === "LoginScreen"){
                facebook.login()
                console.log("Facebook Login Button clicked")
            }
            else if(screenName === "MainScreen"){
                facebook.close()
                console.log("Facebook Logout Button clicked")
            }
        }
    }

    Connections {
        target: facebook
        onConnectedChanged: {
            //connected, switch to MainScreen
            if(facebook.connected){
                console.log("Sucessfully connected...loading data for MainScreen")
                console.log("Granted permissions " +facebook.getGrantedPermissions())
                facebook.addRequestPermission("public_profile")
                facebook.requestMe()
            }
            else{
                console.log("Sucessfully disconnected...switching to LoginScreen")
                screenLoader.source = "LoginScreen.qml"
            }
        }

        onOperationDone: {
            if(operation === "requestMe"){
                console.log("operation done " +JSON.stringify(data))
            }
        }
    }


    /*MouseArea {
        anchors.fill: parent
        onClicked: {
            console.log("login1");
            facebook.login()
        }
    }*/
}
