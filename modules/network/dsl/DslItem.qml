import QtQuick 2.1
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import Deepin.Widgets 1.0
import "../widgets"

Item {
    width: parent.width
    height: childrenRect.height
    Behavior on height {
        PropertyAnimation { duration: 100 }
    }

    property int connectionStatus: 100

    property var infos: dslConnections[index]
    property var activeConnectionInfo: getActiveConnectionInfo(infos.Uuid)

    property bool isConnected: activeConnectionInfo && activeConnectionInfo.dsl && activeConnectionInfo.State == nmActiveConnectionStateActivated

    function goToEditConnection(){
        stackView.push({
            "item": stackViewPages["connectionPropertiesPage"],
            "properties": { "uuid": infos.Uuid, "connectionPath": infos.Path, "devicePath": "/" },
            "destroyOnPop": true
        })
        stackView.currentItemId = "connectionPropertiesPage"
    }

    function activateThisConnection(){
        dbusNetwork.ActivateConnection(infos.Uuid, "/")
    }

    DBaseLine {
        id: lineBox
        property bool hovered: false
        property bool selected: false
        color: dconstants.contentBgColor

        MouseArea{
            z:-1
            anchors.fill:parent
            hoverEnabled: true
            onEntered: parent.hovered = true
            onExited: parent.hovered = false

            onClicked: {
                if (isConnected){
                    goToEditConnection()
                }
                else{
                    activateThisConnection()
                }
            }
        }

        leftLoader.sourceComponent: Item {
            width: parent.width
            height: parent.height

            DImageButton {
                anchors.verticalCenter: parent.verticalCenter
                normal_image: "../images/connected.png"
                hover_image: "../images/disconnect.png"
                visible: isConnected
                onClicked: {
                    dbusNetwork.DeactivateConnection(infos.Uuid)
                }
            }

            WaitingImage {
                anchors.left: parent.left
                anchors.verticalCenter: parent.verticalCenter
                on: activeConnectionInfo && activeConnectionInfo.Uuid == infos.Uuid && activeConnectionInfo.dsl && activeConnectionInfo.State == nmActiveConnectionStateActivating
            }

            DLabel {
                anchors.left: parent.left
                anchors.leftMargin: 24
                anchors.verticalCenter: parent.verticalCenter
                text: infos.Id
                font.pixelSize: 12
                color: {
                    if(lineBox.selected){
                        return dconstants.activeColor
                    }
                    else if(lineBox.hovered){
                        return dconstants.hoverColor
                    }
                    else{
                        return dconstants.fgColor
                    }
                }
            }
        }

        rightLoader.sourceComponent: DArrowButton {
            onClicked: {
                goToEditConnection()
            }
        }
    }

    PasswordBox {
        id: passwordArea
        anchors.top: lineBox.bottom
        anchors.topMargin: 0 - arrowHeight
        uuid: infos.Uuid
        path: infos.Path
    }
}