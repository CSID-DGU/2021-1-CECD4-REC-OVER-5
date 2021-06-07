import os.path
import os
import time
import glob
import pathlib
from datetime import datetime
import csv

# csv 파일 리스트로 받는 함수
def csv2list(filename):

    file_csv = open(filename, "r", encoding='utf-8-sig')
    csvfile = csv.reader(file_csv)
    lists = []
    for item in csvfile:
        lists.append(item)
    return lists

# 확장자명 리스트
suf_csv = csv2list("C:\\Users\\HOME\\Desktop\\suffix.csv") # 확장자명 csv 파일 경로
# 파일명 리스트
stem_csv = csv2list("C:\\Users\\HOME\\Desktop\\stem.csv") # 파일명 csv 파일 경로



#경로의 파일 리스트 생성
file_path = r'C:\Users\HOME\Desktop\Tarkov' # 필터에 넣을 파일들이 들어있는 폴더 경로
file_name = os.listdir(file_path) # 파일명만해서 리스트 정리할 때 사용

file = glob.glob(r'C:\Users\HOME\Desktop\Tarkov\**') # 경로부터 파일명까지, 조건 확인할 때 사용


# 리스트 요소 개수
x = len(file)

#파일 리스트 출력
print ("전체 파일 리스트 :", file_name)

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

    print("파일명 :", file_name[y])

    # path 변수 선언 - 아래에서 사용
    path = pathlib.Path(file[y])


    # 리스트에 있는 파일의 최종 수정 시간
    file_mtime = os.path.getmtime(file[y])
    # 파일 최종 수정 시간의 타임 스탬프를 실제 시간으로 변경
    file_time = datetime.fromtimestamp(file_mtime)
    # 현재 시간과 파일의 최종 수정 시간의 차이
    time_diff = now_time - file_time

    # 중요도 점수 변수
    n = 0

    # 파일명 확인
    if path.stem in stem_csv[0]:
        print("yes")
        n = n + 1
    else :
        print ("no")


    # 확장자명 확인
    if path.suffixes[0] in suf_csv[0]:
        print("yes")
        n = n + 1
    else:
        print("no")


    # 파일 수정일이 1년이 넘었는지 확인
    # 파일 수정 1년 이내 1점
    # 숫자 비교를 위해 time_diff.days 사용
    if time_diff.days < 365:
        print("yes")
        n = n + 1
    else:
        print("no")



    print(n)


    if n == 0:
        A.insert(a, file_name[y])
        a = a + 1

    if n == 1:
        B.insert(b, file_name[y])
        b = b + 1

    if n == 2:
        C.insert(c, file_name[y])
        c = c + 1

    if n == 3:
        D.insert(d, file_name[y])
        d = d + 1

    y = y + 1

print("중요도 점수별 리스트")
print("0점 :", A)
print("1점 :", B)
print("2점 :", C)
print("3점 :", D)
