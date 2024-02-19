import sys
from PySide6.QtCore import QObject, Slot
from PySide6.QtGui import QGuiApplication
from PySide6.QtQml import QQmlApplicationEngine
import glob

class Engine(QQmlApplicationEngine):
    def __init__(self):
        super().__init__()

    @Slot(None, result=None)
    def reload(self) -> None:
        self.clearComponentCache()
        qml = glob.glob("**/main.qml", recursive=True)[0]
        self.load(qml)
        
def main() -> None:
    app = QGuiApplication(sys.argv)
    engine = Engine()
    engine.rootContext().setContextProperty("Engine", engine);
    qml = glob.glob("**/main.qml", recursive=True)[0]
    engine.load(qml)
    if not engine.rootObjects():
        sys.exit(-1)
    engine.quit.connect(app.quit)
    sys.exit(app.exec())   

if __name__ == "__main__":
    main()
