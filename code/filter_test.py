import os.path
import time
import glob
import pathlib
from datetime import datetime


#경로의 파일 리스트 생성 - 경로입력
file = glob.glob(r'C:\Users\HOME\Desktop\Tarkov\**')
# 파일명 입력
fname = 'sss'
# 확장자명 입력
sufname = '.xlsx'

# 리스트 요소 개수
x = len(file)

#파일 리스트 출력
print ("전체 파일 리스트 :", file)

# 입력받아 확인할 확장자명
file_suffix = [sufname]

# 현재 시간
now_time = datetime.now()




# 리스트 index 변수 - file[y]
y = 0
a = 0
b = 0
c = 0
d = 0

# 중요도 점수에 따라 파일을 넣을 리스트
A = [] # 0점
B = [] # 1점
C = [] # 2점
D = [] # 3점

while y < x:

    print("파일명 :", file[y])

    # path 변수 선언 - 아래에서 사용
    path = pathlib.Path(file[y])

    # 입력받아 확인할 파일명, find 를 사용하여 파일명이 있는지 확인
    # path.name 에 입력받은 파일명이 없으면 val = -1
    val = path.name.find(fname)

    # 리스트에 있는 파일의 생성 시간
    file_ctime = os.path.getctime(file[y])
    # 파일 시간의 타임 스탬프를 실제 시간으로 변경
    file_time = datetime.fromtimestamp(file_ctime)
    # 현재 시간과 파일의 생성 시간의 차이
    time_diff = now_time - file_time

    # 중요도 점수 변수
    n = 0

    # 파일명 확인
    if val == -1:
        print("no")
    else:
        print("yes")
        n = n + 1

    # 확장자명 확인
    if file_suffix == path.suffixes:
        print("yes")
        n = n + 1
    else:
        print("no")

    # 파일 생성이 1년이 넘었는지 확인
    # 파일 생성 1년 이내 1점
    # 숫자 비교를 위해 time_diff.days 사용
    if time_diff.days < 365:
        print("yes")
        n = n + 1
    else:
        print("no")



    print(n)


    if n == 0:
        A.insert(a, file[y])
        a = a + 1

    if n == 1:
        B.insert(b, file[y])
        b = b + 1

    if n == 2:
        C.insert(c, file[y])
        c = c + 1

    if n == 3:
        D.insert(d, file[y])
        d = d + 1

    y = y + 1

print("중요도 점수별 리스트")
print("0점 :", A)
print("1점 :", B)
print("2점 :", C)
print("3점 :", D)