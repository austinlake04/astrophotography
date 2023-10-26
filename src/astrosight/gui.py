"""Astrosight GUI tooling."""

import sys

from PyQt5.QtGui import QGuiApplication
from PyQt5.QtQml import QQmlApplicationEngine
from PyQt5.QtWidgets import QFileDialog, QWidget


class App(QWidget):
    """Astrosight GUI."""

    def __init__(self):
        """GUI initialization."""
        super().__init__()
        self.title = "PyQt5 file dialogs - pythonspot.com"
        self.left = 10
        self.top = 10
        self.width = 640
        self.height = 480
        self.setWindowTitle(self.title)
        self.setGeometry(self.left, self.top, self.width, self.height)
        self.openFileNameDialog()
        self.openFileNamesDialog()
        self.saveFileDialog()
        self.show()

    def openFileNamesDialog(self):
        """Opens file selection window."""
        options = QFileDialog.Options()
        # options |= QFileDialog.DontUseNativeDialog
        files, _ = QFileDialog.getOpenFileNames(
            self,
            "QFileDialog.getOpenFileNames()",
            "",
            "All Files (*);;Python Files (*.py)",
            options=options,
        )
        if files:
            print(files)

    def saveFileDialog(self):
        """Opens file save window."""
        options = QFileDialog.Options()
        # options |= QFileDialog.DontUseNativeDialog
        fileName, _ = QFileDialog.getSaveFileName(
            self,
            "QFileDialog.getSaveFileName()",
            "",
            "All Files (*);;Text Files (*.txt)",
            options=options,
        )
        if fileName:
            print(fileName)


def main():
    """Main GUI function."""
    app = QGuiApplication(sys.argv)
    engine = QQmlApplicationEngine()
    engine.quit.connect(app.quit)
    engine.load("main.qml")
    sys.exit(app.exec())


if __name__ == "__main__":
    main()
