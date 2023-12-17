import QtQuick 6.6.1
import QtQuick.Window 6.6.1
import QtQuick.Dialogs 6.6.1
import QtQuick.Controls 6.6.1
import QtQml.Models 2.2


ApplicationWindow {
    visible: true
    width: 640
    height: 480
    title: qsTr("File System")

    menuBar: MenuBar {
        Menu {
            title: qsTr("Astrosight")
            MenuItem {
                text: qsTr("Settings")
                onTriggered: fileDialog.open()
            }
            MenuItem {
                text: qsTr("Exit Astrosight")
                onTriggered: Qt.quit();
            }
			MenuItem {
				text: qsTr("Hide Astrosight")
			}
        }

        Menu {
            title: qsTr("File")
            MenuItem {
                text: qsTr("Select Image Stack")
                onTriggered: fileDialog.open()
            }
            MenuItem {
                text: qsTr("Edit")
                onTriggered: Qt.quit();
            }
        }
    }

    Row {
        id: row
        anchors.top: parent.top
        anchors.topMargin: 12
        anchors.horizontalCenter: parent.horizontalCenter

        ExclusiveGroup {
            id: eg
        }

        Repeater {
            model: [ "File", "Edit", "View", "Help" ]
            Button {
                text: modelData
                exclusiveGroup: eg
                checkable: true
                checked: index === 1
                onClicked: view.selectionMode = index
            }
        }
    }

    ItemSelectionModel {
        id: sel
        model: fileSystemModel
        onSelectionChanged: {
            console.log("selected", selected)
            console.log("deselected", deselected)
            fileManagement.printFileNames(model, selectedIndexes)
        }
        onCurrentChanged: console.log("current", current)
    }

    TreeView {
        id: view
        anchors.fill: parent
        anchors.margins: 2 * 12 + row.height
        model: fileSystemModel
        selection: sel

        onCurrentIndexChanged: console.log("current index", currentIndex)

        TableViewColumn {
            title: "Name"
            role: "fileName"
            resizable: true
        }

        TableViewColumn {
            title: "Permissions"
            role: "filePermissions"
            resizable: true
        }

        onClicked: {
            console.log("clicked", index)
            fileManagement.printPath(index.model, index)
        }
        onDoubleClicked: isExpanded(index) ? collapse(index) : expand(index)
    }

    Component.onCompleted: fileManagement.test()
}
