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


#파일 리스트 출력
print (file)
#extension 변수 선언 - 사용 X
root, extension = os.path.splitext(file[1])
#path 변수 선언 - 아래에서 사용
path=pathlib.Path(file[0])


#파일 생성시간 출력
print (time.ctime(os.path.getmtime(file[0])))
print (time.ctime(os.path.getmtime(file[1])))

#파일 확장자명 출력 extension 변수 사용
#print('extension:', extension)

#파일 확장자명 출력 path 변수 선언 사용
print('Suffix(es):', path.suffixes)

#파일명 출력 path 변수 선언 사용
print('Filename:', path.name)

# 중요도 점수 변수
n = 0

# 입력받아 확인할 파일명, find 를 사용하여 파일명이 있는지 확인
# path.name 에 입력받은 파일명이 없으면 val = -1
val = path.name.find(fname)

# 입력받아 확인할 확장자명
file_suffix = [sufname]

# 리스트에 있는 파일의 생성 시간
file_ctime = os.path.getctime(file[0])
# 현재 시간
now_time = datetime.now()
# 파일 시간의 타임 스탬프를 실제 시간으로 변경
file_time = datetime.fromtimestamp(file_ctime)
#print(now_time)
#print(file_time)
# 현재 시간과 파일의 생성 시간의 차이
time_diff = now_time - file_time
print ("차이 :", time_diff, ", Type :", type(time_diff))




if val == -1:
    print("no")
else:
    print ("yes")
    n = n+1

# 확장자명 확인
if file_suffix == path.suffixes:
    print("yes")
    n = n+1
else:
    print ("no")

# 파일 생성이 1년이 넘었는지 확인
# 파일 생성 1년 이내 1점
# 숫자 비교를 위해 time_diff.days 사용
if time_diff.days < 365:
    print("yes")
    n = n+1
else:
    print("no")


print(n)