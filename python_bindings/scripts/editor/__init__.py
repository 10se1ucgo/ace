from ace.editor import MapEditor

def init(editor: MapEditor):
    from .scene import EditorScript
    return EditorScript(editor)
