import subprocess
import sys

def test_pass():
    assert True;

def test_fail():
    assert False;

def test_1():
    rc = subprocess.run(['test_driver','1'], capture_output=True, text=True)
    print(rc.stdout)
    assert rc.returncode == 0

def test_1v():
    rc = subprocess.run(['test_driver','1','v'], capture_output=True, text=True)
    print(rc.stdout)
    assert rc.returncode == 0

def test_1d():
    rc = subprocess.run(['test_driver','1','d'], capture_output=True, text=True)
    print(rc.stdout)
    assert rc.returncode == 0

def test_2():
    rc = subprocess.run(['test_driver','2'], capture_output=True, text=True)
    print(rc.stdout)
    assert rc.returncode == 0

def test_2v():
    rc = subprocess.run(['test_driver','2','v'], capture_output=True, text=True)
    print(rc.stdout)
    assert rc.returncode == 0

def test_2d():
    rc = subprocess.run(['test_driver','2','d'], capture_output=True, text=True)
    print(rc.stdout)
    assert rc.returncode == 0

def test_3():
    rc = subprocess.run(['test_driver','3'], capture_output=True, text=True)
    print(rc.stdout)
    assert rc.returncode == 0

def test_3v():
    rc = subprocess.run(['test_driver','3','v'], capture_output=True, text=True)
    print(rc.stdout)
    assert rc.returncode == 0

def test_3d():
    rc = subprocess.run(['test_driver','3','d'], capture_output=True, text=True)
    print(rc.stdout)
    assert rc.returncode == 0

def test_6():
    rc = subprocess.run(['test_driver','6'], capture_output=True, text=True)
    print(rc.stdout)
    assert rc.returncode == 0

def test_6v():
    rc = subprocess.run(['test_driver','6','v'], capture_output=True, text=True)
    print(rc.stdout)
    assert rc.returncode == 0

def test_6d():
    rc = subprocess.run(['test_driver','6','d'], capture_output=True, text=True)
    print(rc.stdout)
    assert rc.returncode == 0
