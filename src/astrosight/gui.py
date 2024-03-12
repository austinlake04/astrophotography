import sys
from PySide6.QtCore import QObject, Slot
from PySide6.QtGui import QGuiApplication
from PySide6.QtQml import QQmlApplicationEngine
import glob
from astrosight.backend import Backend
import numpy as np

def main() -> None:
    app = QGuiApplication(sys.argv)
    engine = QQmlApplicationEngine()
    backend = Backend()
    engine.addImageProvider("preview", backend)
    engine.rootContext().setContextProperty("Model", backend.model)
    qml = glob.glob("**/main.qml", recursive=True)[0]
    engine.load(qml)
    if not engine.rootObjects():
        sys.exit(-1)
    engine.quit.connect(app.quit)
    sys.exit(app.exec())   

if __name__ == "__main__":
    main()
