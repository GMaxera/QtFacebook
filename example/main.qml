import QtQuick 2.6
import QtQuick.Window 2.2
import org.qtproject.example 1.0
import QtQuick.Controls 1.4
import "."

/*
 * This is a sample project, that demonstrates how to use QtFacebook.
 * Currently, only the android target is supported.
 *
 * In order to use that sample project, change app_name and app_id in android/res/values/strings.xml
 * accordingly.
 */

Window {
    property var facebook : Facebook
    width: 400
    height: 600
    visible: true

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
            if(facebook.connected){
                //connected, switch to MainScreen
                console.log("Sucessfully connected...loading data for MainScreen")
                facebook.requestMe()
            }
            else{
                //disconnected, switch to LoginScreen
                console.log("Sucessfully disconnected...switching to LoginScreen")
                screenLoader.source = "LoginScreen.qml"
            }
        }

        onOperationDone: {
            if(operation === "requestMe"){
                console.log("Loaded data for MainScreen...switching to MainScreen")
                screenLoader.source = "MainScreen.qml"
                screenLoader.item.firstname = data["first_name"];
                screenLoader.item.lastname = data["last_name"];
            }
        }
    }
}
