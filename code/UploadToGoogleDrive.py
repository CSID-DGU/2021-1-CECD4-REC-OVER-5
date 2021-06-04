from __future__ import print_function
from googleapiclient.discovery import build
from httplib2 import Http
from oauth2client import file, client, tools

#import cryption

def uploadToGoogleDrive():
    try :
        import argparse
        flags = argparse.ArgumentParser(parents=[tools.argparser]).parse_args()
    except ImportError:
        flags = None

    SCOPES = 'https://www.googleapis.com/auth/drive.file'
    store = file.Storage('storage.json')
    creds = store.get()

    if not creds or creds.invalid:
        print("make new storage data file ")
        #oauth json파일
        flow = client.flow_from_clientsecrets('C:\\test\\client_secret_828580371975-fvfcc5vutfr2cb3h7tckt2kgimh8ltub.apps.googleusercontent.com.json', SCOPES)
        creds = tools.run_flow(flow, store, flags) if flags else tools.run(flow, store)

    DRIVE = build('drive', 'v3', http=creds.authorize(Http()))

    FILES = (
        ('C:\\test\\hello.txt'), #업로드할파일
    )

    folder_id = '15ND8PEYFwXOttX8NaltOQHtFEFP9kIul' #구글드라이브 폴더이름

    for file_title in FILES :

        print(file_title)
        with open(file_title, 'rb') as p: #바이너리 데이터로 파일 읽어옴
            data = p.read()
            data = data.decode('utf-8');
            print("data: ", data)

        #encrypted_data = cryption.Crypt().encrypt_str(data) #데이터 암호화
        #print("encrypted_data: ", encrypted_data)

        #새 파일에 암호화된 내용 저장
        #with open("C:\\test\\newFile.txt", 'w') as nf:
        #    nf.write(encrypted_data)

        file_name = file_title
        metadata = {'name': '잘되냐', #이 이름으로 업로드
                    'parents' : [folder_id],
                    'mimeType': None
                    }

        res = DRIVE.files().create(body=metadata, media_body=file_name).execute()
        if res:
            print('Uploaded "%s" (%s)' % (file_name, res['mimeType']))
    print("구글드라이브 업로드 완료")

if __name__ == "__main__":
    print("파이선 메인함수 실행")
    #uploadToGoogleDrive()
