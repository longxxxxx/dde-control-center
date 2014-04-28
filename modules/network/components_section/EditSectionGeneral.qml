import QtQuick 2.1
import Deepin.Widgets 1.0
import "../components"

BaseEditSection{
    id: sectionGeneral
    section: "general"
    
    header.sourceComponent: EditDownArrowHeader{
        text: dsTr("General")
    }

    content.sourceComponent: Column {
        EditLineSwitchButton{
            key: "autoconnect"
            text: dsTr("Automatically connect")
        }
    }
}