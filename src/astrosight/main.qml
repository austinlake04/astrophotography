import QtQuick;
import QtQuick.Window
import QtQuick.Dialogs
import QtQuick.Controls
import QtQml.Models


ApplicationWindow {
    visible: true
    width: 960
    height: 720
    title: "Astrosight" 

    menuBar: MenuBar {
        Menu {
            title: "Astrosight"

            MenuItem {
                text: "About Astrosight"
            }

            Rectangle {
                width: parent.width
                height: 1
            }

            MenuItem {
                text: "Settings"
                onClicked: fileDialog.open()
            }

            Rectangle {
                width: parent.width
                height: 1
            }

            MenuItem {
                text: "Hide Astrosight"
                onClicked: showMinimized()
            }
            MenuItem {
                text: "Exit Astrosight"
                onClicked: Qt.quit();
            }
        }

        Menu {
            title: "File"

            MenuItem {
                text: "Select image"
                onClicked: fileDialog.open()
            }
        }

        Menu {
            title: "Edit"
            
            MenuItem {
                text: "Undo"
            }
            MenuItem {
                text: "Redo"
            }

            Rectangle {
                width: parent.width
                height: 1
            }
            
            MenuItem {
                text: "Cut"
            }
            MenuItem {
                text: "Copy"
            }
            MenuItem {
                text: "Paste"
            }
            MenuItem {
                text: "Delete"
            }

            Rectangle {
                width: parent.width
                height: 1
            }

            MenuItem {
                text: "Select all"
            }
        }

        Menu {
            title: "Help"

            MenuItem {
                text: "Select image"
                onClicked: fileDialog.open()
            }
        }
    }

    Shortcut {
        sequence: "F5"
        onActivated: Engine.reload()
    }

    FileDialog {
        id: fileDialog;
        title: "Please choose a file";
        nameFilters: ["Image Files (*.jpg *.png *.gif)"];
        visible: false
        onAccepted: {
            console.log("User has selected " + dialogFile.folder);
            fileDialog.close()
        }
    }

}
