import QtQuick 2.1
import QtQuick.Window 2.0

Item {
    id: root
    property int trayWidth: 48
    property int trayHeight: trayWidth
    property color defaultBackgroundColor: "#252627"
    property bool trayIconShowAll: false

    property int viewHoverPadding: 100

    property int displayState: viewState.allHide

    function dsTr(s){
        return qtgettext.qsTr(s)
    }

    QtObject { 
        // enumeration for root view state
        id: viewState
        property int allHide: 0
        property int trayShow: 1
        property int rightBoxShow: 2
        property int allShow: 3
    }

    function outerAreaClicked(mousex, mousey){
        if ((root.displayState == viewState.trayShow) && (
            mousex <= screenSize.width - trayWidth)) {
            hideTrayIcon()
        }
        else if ((root.displayState == viewState.allShow) && (
            mousex <= screenSize.width - root.width)) {
            hideTrayIcon()
        }
    }

    function hideTrayIcon(){
        if (!hidingRoot.running){
            hidingRoot.restart()
        }
    }

    function displayTrayIcon(){
        if (!showingTrayIconBox.running){
            showingTrayIconBox.restart()
        }
    }

    function showRightBox(trayIconId) {
        if (trayIconId == 'shutdown'){
            Qt.quit()
            //hideTrayIcon()
            //windowView.shutdown()
        }
        else if (trayIconId == 'dss'){
            expandHideTrayIcon()
        }
        else{
            if (!showingRightBox.running && !showingTrayIconBox.running){
                showingRightBox.restart()
            }
            rightBoxLoaderItem.iconId = trayIconId
        }
    }

    function initTrayIcon() {
        var icon_path_array = modulesId.common_ids()
        var new_tray_height = screenSize.height/(icon_path_array.length+1.0)
        if (new_tray_height < trayWidth){
            trayHeight = new_tray_height
        }
        else{
            trayHeight = trayWidth
        }
        trayIconTabArea.clear()
        for (var i in icon_path_array){
            trayIconTabArea.append({
                'iconId': icon_path_array[i]
            })
        }
        trayIconTabList.height = trayHeight * trayIconTabArea.count
        trayIconShowAll = false
    }

    function expandHideTrayIcon() {
        trayIconTabArea.remove(modulesId.common_ids().length - 2)
        trayIconTabList.currentIndex = -1
        var newIds = modulesId.hide_ids()
        for (var i in newIds){
            var index = trayIconTabArea.count - 1
            trayIconTabArea.insert(index, {'iconId': newIds[i]})
        }
        var new_tray_height = screenSize.height/(trayIconTabArea.count)
        if (new_tray_height < trayWidth){
            trayHeight = new_tray_height
        }
        else{
            trayHeight = trayWidth
        }

        trayIconTabList.height = trayHeight * trayIconTabArea.count
        trayIconShowAll = true
    }

    function trayIconHoverHandler(module_id, index) {
        //console.log(module_id + ": "  + index)
        var tipDisplayHeight
        tipDisplayHeight = Math.abs(trayHeight - trayIconTip.height)/2 + trayHeight * index
        if (trayHeight == trayWidth) {
            tipDisplayHeight = (screenSize.height - trayHeight * trayIconTabArea.count)/2 + tipDisplayHeight
        }
        trayIconTip.y = tipDisplayHeight
        trayIconTipText.text = modulesId.module_names()[module_id]
        trayIconTip.visible = true
    }

    Timer {
        // init something
        running: true
        interval: 100
        repeat: false
        onTriggered: {
            initTrayIcon()
            trayIconTabList.positionViewAtEnd()
        }
    }

    PropertyAnimation {
        id: showingRightBox
        alwaysRunToEnd: true
        target: windowView
        property: "x"
        to: screenSize.width - 360
        duration: 300
        easing.type: Easing.OutQuad

        /***
        onStarted: {
            windowView.x = 0
        }
        ***/

        onStopped: {
            displayState = viewState.allShow
        }
    }

    PropertyAnimation {
        id: hidingRoot
        alwaysRunToEnd: true
        target: windowView
        property: "x"
        to: screenSize.width
        duration: 300
        easing.type: Easing.OutQuad
        onStopped: {
            //windowView.x = screenSize.width - 2
            trayIconTabList.currentIndex = -1
            displayState = viewState.allHide
            initTrayIcon()
            windowView.hide()
        }
    }

    PropertyAnimation {
        id: showingTrayIconBox
        alwaysRunToEnd: true
        target: windowView
        property: "x"
        to: screenSize.width - 48
        duration: 300
        easing.type: Easing.OutQuad

        onStarted: {
            windowView.show()
        }

        onStopped: {
            displayState = viewState.trayShow
        }
    }
    // animation for root frame

    Rectangle {
        id: frame
        color: Qt.rgba(0, 0, 0, 0)
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.bottom: parent.bottom
		width: 360
		height: root.height
    }

    Window {
        id: trayIconTip
        width: trayIconTipText.width + 52
        height: 44
        x: windowView.x - width
        visible: false
        flags: Qt.Popup

        color: Qt.rgba(0, 0, 0, 0)

        RectWithCorner {
            id: trayIconTipArrowRect
            x: 0
            y: 0
            cornerDirection: "right"
            rectWidth: parent.width
            rectHeight: parent.height
            cornerPos: rectHeight/2
            cornerWidth: 12
            cornerHeight: 8
        }

        Text {
            id: trayIconTipText
            anchors.verticalCenter: trayIconTipArrowRect.verticalCenter
            anchors.horizontalCenter: trayIconTipArrowRect.horizontalCenter
            anchors.horizontalCenterOffset: - 4
            font.pixelSize: 13
            color: "white"
        }
    }

    Rectangle {
        id: trayFrame
        width: trayWidth
        anchors.left: frame.left
        anchors.top: frame.top
        anchors.bottom: frame.bottom
        color: defaultBackgroundColor
        //visible: false

        Rectangle {
            color: defaultBackgroundColor
            width: parent.width
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            anchors.left: parent.left

            ListView {
                property QtObject currentHoverItem

                id: trayIconTabList
                width: parent.width
                anchors.centerIn: parent

                delegate: TabButtonDelegate{width: trayWidth; height: trayHeight}
                model: ListModel {id: trayIconTabArea}
                currentIndex: -1
                onCurrentIndexChanged: {
                    if (currentIndex != -1){
                        showRightBox(currentItem.trayIconId)
                    }
                }
                highlight: Rectangle { color: Qt.rgba(255, 255, 255, 0.1); radius: 3; }
                highlightMoveVelocity: 800
                highlightFollowsCurrentItem: true
                maximumFlickVelocity: 0
                Behavior on height {
                    NumberAnimation { duration: 300 }
                }
            }
        }
    }

    // separator lines for trayicon and rightbox
    Rectangle {
        width: 1
        anchors.fill: frame
        anchors.leftMargin: trayWidth
        color: "#0b0809"
    }
    Rectangle {
        width: 1
        anchors.fill: frame
        anchors.leftMargin: trayWidth + 1
        color: "#3b3b3d"
    }
    // separator lines for trayicon and rightbox

    Rectangle {
        id: rightBox
        anchors.fill: frame
        anchors.leftMargin: trayWidth + 2
        color: defaultBackgroundColor

        Item {
            id: rightBoxLoaderItem

            property string iconId

            visible: false
            clip: true
            onIconIdChanged: {
                rightBoxLoaderItem.visible = (iconId == '' ? false : true)
                rightBoxLoader.iconId = iconId
                if (iconId){
                    rightBoxLoader.source = '../modules/' + iconId + '/main.qml'
                }
                else{
                    rightBoxLoader.source = ''
                }
            }
            anchors.fill: parent

            MouseArea{
                anchors.fill: parent
                enabled: rightBoxLoaderItem.visible
                //Eats mouse events
            }

            Loader{
                id: rightBoxLoader
                property string iconId
                property color childColor: defaultBackgroundColor
                focus: true
                source: ''
                anchors.fill: parent
            }
        }
    }
}
