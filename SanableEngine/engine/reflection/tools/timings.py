import time

global timings
timings = {}

global curTask
curTask = None
global curTaskBegin
curTaskBegin = 0
def switchTask(newTask):
    global curTask
    global curTaskBegin
    if curTask != None:
        elapsed = time.perf_counter()-curTaskBegin
        if curTask in timings.keys(): timings[curTask] += elapsed
        else: timings[curTask] = elapsed

    prevTask = curTask
    curTask = newTask
    curTaskBegin = time.perf_counter()
    
    return prevTask


def timeScoped(_callable, subtask):
    prevTask = switchTask(subtask)
    rv = _callable()
    switchTask(prevTask)
    return rv


TASK_ID_INIT = "Initial setup and load"
TASK_ID_DISCOVER = "Source project indexing"
TASK_ID_EXT_FILE_IO = "File I/O"
TASK_ID_CLANG_PARSE = "Clang lexing and parsing"
TASK_ID_WALK_AST_INTERNAL = "AST walking (python-side)"
TASK_ID_WALK_AST_EXTERNAL = "AST walking (Clang-side)"
TASK_ID_FINALIZE = "Finalizing"
