// import QtQuick 2.15
// import QtQuick.Controls 2.15

// ApplicationWindow {
//     visible: true
//     width: 400
//     height: 600
//     title: "Clock"

//     Rectangle {
//         anchors.fill: parent

//         Image {
//             anchors.fill: parent
//             source: "./images/background.png"
//             fillMode: Image.PreserveAspectCrop
//         }

//         Rectangle {
//             anchors.fill: parent
//             color: "transparent"

//             Text {
//                 text: "16:38:33"
//                 font.pixelSize: 24
//                 color: "white"
//             }

//         }

//     }

// }

import QtQuick 2.12
import QtQuick.Window 2.12
import QtQuick.Dialogs 1.3
import QtQuick.Controls 1.4
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
        }
        Menu {
            title: qsTr("File")
            MenuItem {
                text: qsTr("Select Image Stack")
                onTriggered: fileDialog.open()
            }
            MenuItem {
                text: qsTr("Redo")
                onTriggered: Qt.quit();
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

// Window {
//         id: mainWindow
//         visible: true

//         width: 700
//         height: 500

//         // FileDialog
//         FileDialog {
//             id: fileDialog
//             title: "Please choose a file"
//             folder: shortcuts.home
//             selectMultiple: true
//             nameFilters: [
//                 "Image files (*.NEF *.ARW *.SRF *.SR2 *.MEF *.ORF *.SRW *.ERF *.KDC *.DCS *.RW2 *.RAF *.DCR *.DNG *.PEF *.CRW *.CHDK *.IIQ *.3FR *.NRW *.NEF *.MOS *.CR2 *.ARI)",
//             ]
//             onAccepted: {
//                 console.log("You chose: " + fileDialog.fileUrls)
//                 //acceptDialog();
//             }
//             onRejected: {
//                 console.log("rejected")
//                 //rejectDialog();
//             }
//             Component.onCompleted: visible = true
//         }
// }
