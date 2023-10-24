import argparse
import core
import glob
import matplotlib.pyplot as plt
from treelib import Node, Tree
from PyQt5.QtWidgets import QFileDialog
import rawpy

def select_files() -> list[str]:
    dialog = QFileDialog()
    dialog.setDirectory(".")
    dialog.setFileMode(QFileDialog.FileMode.ExistingFiles)
    dialog.setNameFilter("Image files (*.NEF *.ARW *.SRF *.SR2 *.MEF *.ORF *.SRW *.ERF *.KDC *.DCS *.RW2 *.RAF *.DCR *.DNG *.PEF *.CRW *.CHDK *.IIQ *.3FR *.NRW *.NEF *.MOS *.CR2 *.ARI)")
    dialog.setViewMode(QFileDialog.ViewMode.List)
    if dialog.exec():
        filenames = dialog.selectedFiles()
        return filenames

def create_tree(quiet: bool = False) -> dict[list[str]]:
    """Creates file tree of astrophotography images

    Parameters
    ----------
    quiet : bool
        Hides text printed to the terminal.

    Returns
    -------
    file_tree : dict[list[str]]
        File tree containing light, dark, bias, and flat images
        {'light': [...], 'dark': [...], 'bias': [...], 'flat': [...]}
    """
    file_tree = {}
    print("Select core images...")

    file_tree["Light"] = glob.glob("./test_data/Light/*")#select_files()
    file_tree["Bias"] = glob.glob("./test_data/Bias/*")#select_files()
    file_tree["Dark"] = glob.glob("./test_data/Dark/*")#select_files()
    file_tree["Flat"] = glob.glob("./test_data/Flat/*")#select_files()
    file_tree["Dark Flat"] = glob.glob("test_data/DarkFlat/*")#select_files()
    if not quiet:
        print(f"{len(file_tree['Light'])} light frames selected")
        print(f"{len(file_tree['Bias'])} bias frames selected")
        print(f"{len(file_tree['Dark'])} dark frames selected")
        print(f"{len(file_tree['Flat'])} flat frames selected")
        print(f"{len(file_tree['Dark Flat'])} dark flat frames selected")
        # print("Current image tree")
        # show_tree(files)    
    return file_tree

    
def add_children(files, tree):
    for key in files.keys:
        tree.create_node(files[key], files[key], parent=key)
        add_children(files, tree, files[key])

def show_tree(files):
    tree = Tree()
    root = "image tree"
    tree.create_node(root, root)
    add_children(files, tree, files)
    tree.show()


def main():
    parser = argparse.ArgumentParser(description="Astrosight. A astrophotography image processing software.")
    parser.add_argument("-q", "--quiet",
                        action="store_true", help="Hides text printed to the terminal.")
    parser.add_argument("--feature-detector", choices=["ORB", "SIFT", "AKAZE"], default="ORB",
                        help="Feature detector to locate stars in image")
    parser.add_argument("-f", "--filename", help="Filename for final, stacked image.")
    parser.add_argument("-s", "--save", action="store_true", help="Automatically saves final, stacked image.")
    arguments = parser.parse_args()

    file_tree = create_tree(arguments)
    stacked_image = core.stack(file_tree=file_tree,
                               feature_detector=arguments.feature_detector,
                               filename=arguments.filename,
                               save=arguments.save,
                               quiet=arguments.quiet)

if __name__ == "__main__":
    main()
