from subprocess import Popen, PIPE
import os
import time


def run_prog(inp):
    p = Popen("/home/Yurec/repos/CLionProjects/CompressAlgHuffman/cmake-build-debug/CompressAlgHuffman", stdin=PIPE, stdout=PIPE)
    out, _ = p.communicate(inp.encode(), timeout=100)
    return p.returncode, str(out)

def test_1():
    dir_files = "/home/Yurec/repos/testing"
    for root, dirs, files in os.walk(dir_files):
        for file in files:
            size = os.path.getsize(os.path.join(root,file))
            start_time = time.time()
            code, answers_prog = run_prog(os.path.join(root,file))
            finish_time = time.time()
            if code != 0:
                print(code)
                print(inp)
                break
            print(file + "(" + str(round((size/1048576),2)) + "MBytes) Compress rate: "+answers_prog + " TIME:"+" %s sec" % (finish_time - start_time) )




test_1()
