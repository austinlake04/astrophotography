import QtQuick;
import QtQuick.Window
import QtQuick.Dialogs
import QtQuick.Controls
import QtQml.Models


ApplicationWindow {
	width: 1280
	height: 720
	minimumWidth: 620
	minimumHeight: 480
    visible: true
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
                text: "Issue"
                onClicked: Qt.openUrlExternally("https://github.com/austinlucaslake/astrosight/issues")
            }
        }
    }

	Row {
		visible: true
		spacing: 10

/*
		TreeView {
			// based on QtQuick TreeView example
			// (https://doc.qt.io/qt-6/qml-qtquick-treeview.html)
			width: 400
			height: 400
			id: treeView
			visible: true
			model: Model
			selectionModel: ItemSelectionModel {}

			delegate: Item {
				implicitWidth: padding + label.x + label.implicitWidth + padding
				implicitHeight: label.implicitHeight * 1.5

				readonly property real indentation: 20
				readonly property real padding: 5

				required property TreeView treeView
				required property bool isTreeNode
				required property bool expanded
				required property int hasChildren
				required property int depth
				required property int row
				required property int column
				required property bool current

				property Animation indicatorAnimation: NumberAnimation {
					target: indicator
					property: "rotation"
					from: expanded ? 0 : 90
					to: expanded ? 90 : 0
					duration: 100
					easing.type: Easing.OutQuart
				}
				TableView.onPooled: indicatorAnimation.complete()
				TableView.onReused: if (current) indicatorAnimation.start()
				onExpandedChanged: indicator.rotation = expanded ? 90 : 0

				Rectangle {
					id: background
					anchors.fill: parent
					color: row === treeView.currentRow ? palette.highlight : "black"
					opacity: (treeView.alternatingRows && row % 2 !== 0) ? 0.1 : 0.3
				}

				Label {
					id: indicator
					x: padding + (depth * indentation)
					anchors.verticalCenter: parent.verticalCenter
					visible: isTreeNode && hasChildren
					text: "▶"

					TapHandler {
						onSingleTapped: {
							let index = treeView.index(row, column)
							treeView.selectionModel.setCurrentIndex(index, ItemSelectionModel.NoUpdate)
							treeView.toggleExpanded(row)
						}
					}
				}

				Label {
					id: label
					x: padding + (isTreeNode ? (depth + 1) * indentation : 0)
					anchors.verticalCenter: parent.verticalCenter
					width: parent.width - padding - x
					clip: true
					text: model.data
				}
			}
		}
*/


		Image {
			id: preview
			width: 620
			height: 480
			visible: true
			fillMode: Image.PreserveAspectFit
			cache: false
			source: "image://preview/0"
		}
		
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

    Shortcut {
        sequence: "F5"
        onActivated: Engine.reload()
    }

}
